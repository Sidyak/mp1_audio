//-----------------------------------------------------------------------
// Description:  MPEG-1 Layer I Decoder
//
// Author:       Kim Radmacher
// Date:         
//               19-Feb-15
// Date of final version:
//               23-Jun-15
// Date of porting to x86 and refactoring:
//               06-Mae-22
//
// Copyright (C) 2022 Kim Radmacher
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
// express or implied.
// See the License for the specific language governing permissions
// and limitations under the License.
//----------------------------------------------------------------------------

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "analysis_coeffs_float.h"  // filterbank coef
#include "mpeg_tables.h"            // mpeg-1 tables
#include "wavwriter.h"

#if defined(_MSC_VER)
#include <getopt.h>
#else
#include <unistd.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif
#include "wavwriter.h"
#ifdef __cplusplus
}
#endif

//#define DEBUG

#define BUFLEN          (12*32) // buffer length: 12 samples and 32 subbands = 384
#define PI              3.14159265358979
#define BANDSIZE        32    // 32 filter subbands
#define COMPRESSED      1
#define ORIGINAL        2

const uint32_t syncWords[2] = {0xAAAACCCC, 0xF0F0AAAA};

unsigned int table_Xmt[BUFLEN];    // EDMA Xmt buffer
unsigned int table_Rcv[BUFLEN];    // EDMA Rcv buffer
short direction=-1;                // EDMA Interrupt Rcv=0, Xmt=1, default=-1
short cnt_samp=0;
short count_fb=0, count=0, count_12=0, cnt_out=0, count_12_synthese=0;    // counter for filterbank and polayphase rotating switch

// some control variables 
int i_m = 0,k_m = 0;

// Filterbank variables
float M[32][64],T[32][64], S[32][12]={0};
float teta=0;
float INT_y,INT_y1,INT_y2;    // current polyphase outputs
float out_delay[64];          // polyphase component outputs
float /*int32_t*/ y_rx[12][BANDSIZE];     // demultiplexed subbands
float Out1[768];              // 64 polyphases * 12 samples=768
float *pOut1;                 // pointer reference

// Psychoacoustic Model variables 
#ifdef FIX_FOR_REAL_BITRATE_REDUCTION
uint8_t FRAME1[16*1024*1024] = {0}; // Rcv Frame
uint8_t *pFRAME1;              // pointer reference
#else
short FRAME1[448] = {0};        // Rcv Frame
short *pFRAME1;                 // pointer reference
#endif
uint8_t BSPL_rx[BANDSIZE];        // received bit values for subbands
float scf_rx[BANDSIZE];         // received bit values for scalefactors
short tot_bits_rx;              // number of received bits
short cnt_FRAME_read = 0;       // array index for received data
short buffer[BUFLEN] = {0};     // McBSP buffer
#ifndef FIX_FOR_REAL_BITRATE_REDUCTION
short start_frame_offset = 0;   // start sequence to data offset
short start_found = 0;          // flag for correct star sequence found
#endif

void init_table(void);
float fir_filter(float delays[], float coe[], short N_delays, float x_n);
void calc_cos_mod_synthese(void);
void calc_polyphase_synthese_fb(void);
int32_t rx_frame(FILE *in);

void usage(const char* name)
{
    fprintf(stderr, "%s in.mp1 out.wav\n", name);
}

int main(int argc, char *argv[])
{
    const char *infile, *outfile;
    FILE *in;
    void *wavOut;
    int format, sample_rate, channels, bits_per_sample;
    uint32_t data_length;
    int input_size;
    uint8_t* input_buf;
    int16_t* convert_buf;
    
    if (argc - optind < 2)
    {
        fprintf(stderr, "Error: not enough parameter provided\n");
        usage(argv[0]);
        return 1;
    }
    
    infile = argv[optind];
    outfile = argv[optind + 1];

    in = fopen(infile, "rb");
    if (!in) {
        perror(infile);
        return 1;
    }

    sample_rate = 48000;//44100;
    bits_per_sample = 16;
    channels = 1;
    wavOut = wav_write_open(outfile, sample_rate, bits_per_sample, channels);

    if (!wavOut)
    {
        fprintf(stderr, "Unable to open wav file for writing %s\n", outfile);
        return 1;
    }

    // create cos-mod-matrix
    for(i_m=0; i_m<32; i_m++)
    {
        for(k_m=0; k_m<64; k_m++)
        {
            teta = (pow(-1,i_m))*PI/4;
            M[i_m][k_m] = 2*(cos((i_m+0.5)*(k_m-511/2)*PI/32+teta));
            T[i_m][k_m] = 2*(cos((i_m+0.5)*(k_m-511/2)*PI/32-teta));
        }
    }
   
    // pointer references
    pOut1 = Out1; // used for synthesis fb
    pFRAME1 = FRAME1;

    // clear frame buffer
#ifdef FIX_FOR_REAL_BITRATE_REDUCTION
    memset(pFRAME1, 0, sizeof(FRAME1));
#else
    for(i_m=0; i_m<(448); i_m++)
    {
        pFRAME1[i_m] = 0;    // init value - Transmission takes 8 ms at 192 kbps
    }
#endif

    init_table();     // init Tables
#ifndef FIX_FOR_REAL_BITRATE_REDUCTION
    start_frame_offset = 0;
#endif

    uint32_t nFrame = 0; // frame counter

    while(1)
    {
#ifndef FIX_FOR_REAL_BITRATE_REDUCTION
        int readBytes = fread(table_Rcv, 1, BUFLEN/**channels*/*sizeof(int32_t), in);
        if(readBytes == 0)
        {
            printf("File seems to end\n");
            break;    
        }

        // find start sequence and the offset to data
        if(start_found == 0)
        {
            start_frame_offset = 0;
            while(start_found == 0)
            {
                if( (table_Rcv[start_frame_offset] == 0xCCCCAAAA) && (table_Rcv[start_frame_offset+1] == 0xAAAAF0F0) )
                {
                    printf("Preamble for COMPRESSED found\n");
                    start_found=COMPRESSED; // start sequence for compressed data
                    //start_frame_offset += 2; // two int32 offset -> done below in copy table_Rcv to pFRAME1
                }
                else if( (table_Rcv[start_frame_offset]==0xAAAAC0C0) && (table_Rcv[start_frame_offset+1]==0xF0F0AAAA) )
                {
                    printf("Preamble for ORIGINAL found\n");
                    start_found=ORIGINAL;    // start sequence for original data samples
                    printf("ORIGINAL is no use case\n");
                    break;
                }
                else
                {
                    printf("start_frame_offset = %d\n", start_frame_offset);
                    start_frame_offset++;    // increment offset

                    if(start_frame_offset > 4)
                    {
                        fprintf(stderr, "ERROR: could not find syncwords\n");
                        break;
                    }
                }
            }

            if( (start_frame_offset > 157) )
            {    //    if offset > 157, data is too long to fit into table_Rcv array buffer
                start_frame_offset=0;            // reset start offset
                start_found=0;                    // reset start found flag
                fprintf(stderr, "start_frame_offset = %d\n", start_frame_offset);
                break;
            }
        }

        for(i_m=0; i_m < (224); i_m++)
        {
            pFRAME1[i_m*2] = (short)(0x0000FFFF & table_Rcv[start_frame_offset+2+i_m]);            // lower 16 Bit from rcv 32 Bit
            pFRAME1[i_m*2+1] = (short)((0xFFFF0000 & table_Rcv[start_frame_offset+2+i_m])>>16);    // upper 16 Bit from rcv 32 Bit
        }

        if(start_found==COMPRESSED)
#endif // #ifndef FIX_FOR_REAL_BITRATE_REDUCTION
        {
            int32_t bitsReadDone;
            /* Receive data and demultiplex 384 subband samples */
            // TODO: pass file pointer to read from
            bitsReadDone = rx_frame(in);
            if(bitsReadDone < 0)
            {
                //fprintf(stderr, "ERROR: something went wrong while rx frame\n");
                //return -1;
                printf("done decoding\n");
                break;
            }
#ifdef DEBUG
//            printf("%d bits read\n", bitsReadDone);
#endif
            /* Synthesis FILTERBANK */
            calc_cos_mod_synthese();        // cosinus modulation
            count_12_synthese=0;
            calc_polyphase_synthese_fb();    // polyphase filterbank    
#ifndef FIX_FOR_REAL_BITRATE_REDUCTION
            // TODO: start decoding and write to wav file here (before overwriting buffer in rx_frame)
            if(start_found == COMPRESSED)
#endif
            {
                for(i_m=0; i_m<BUFLEN; i_m++)
                {
                    //table_Xmt[i_m] = ((int32_t)buffer[i_m]<<16) & (int32_t)buffer[i_m];

                    // TODO: fix for stereo
                    int16_t oL = (int16_t)buffer[i_m];
                    //int16_t oR = (int16_t)buffer[i_m];
                    wav_write_data(wavOut, (unsigned char*)&oL, 2);
                    //wav_write_data(wavOut, (unsigned char*)&oR, 2);
                }
            }
#ifndef FIX_FOR_REAL_BITRATE_REDUCTION
            else
            {
                fprintf(stderr, "ERROR: something went wrong while decoding\n");
                break;
            }
#endif
            nFrame++;
#ifdef FIX_FOR_REAL_BITRATE_REDUCTION
            //printf("\r[%d]", nFrame);
#else
            //printf("\r[%d|%d]", nFrame, readBytes);
#endif
        }
    } // end while(1)

    wav_write_close(wavOut);
    fclose(in);

    return 0;
}

/* variables initialization for Xmt and Rcv */
void init_table(void)
{
    short ind=0;
    for(ind=0; ind < BUFLEN; ind++)
    {
        table_Xmt[ind] = 0;
        table_Rcv[ind] = 0;
        buffer[ind] = 0;
    }
}
