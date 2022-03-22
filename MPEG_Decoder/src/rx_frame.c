//----------------------------------------------------------------------------
// Description       "receive" data and demultiplex it
// Author:           Kim Radmacher
// Created on:
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
#include <stdio.h>
#include "init.h"

//#define DEBUG
#ifdef DEBUG
#include <assert.h>
#endif

#define MAX_ANC_BIT_SIZE    (32*32) // max number of bits for ancillary data

static int32_t readBits(FILE* in_file, uint8_t* pBitstream, const uint32_t numberOfBits, int32_t *retBits);

static uint32_t bitNdx = 0;
static const uint32_t bufSize = 16*1024*1024; // 16 MB max
static uint32_t validBits = 0;  // valid bits for current frame
static uint32_t start_found = 0;
const uint32_t syncWords[2] = {0xCCCCAAAA, 0xAAAAF0F0};

int32_t rx_frame(FILE *in_file, Mp1Decoder *mp1dec)
{
    short n_band, sample, N;
    tot_bits_rx = 0;
    cnt_FRAME_read = 0;

    int syncFound = 0, timeout = 0;  
    mp1dec->mph.mpeg_header_bitwise.sync = 0;
    while(syncFound == 0)
    {
        int32_t tmp;
        if(readBits(in_file, pFRAME1, 1, &tmp))
        {
            return -1;
        }
        
        mp1dec->mph.mpeg_header_bitwise.sync = (mp1dec->mph.mpeg_header_bitwise.sync>>1) | ((tmp & 0x1) << 11);
        if(mp1dec->mph.mpeg_header_bitwise.sync == 0xFFF)
        {
            timeout = 0;
            if(readBits(in_file, pFRAME1, 32-12, &tmp))
            {
                return -1;
            }
            mp1dec->mph.mpeg_header_word |= tmp;
#ifdef DEBUG
            printf("Preamble/sync found\n");
            printf("mp1dec->mph.mpeg_header_bitwise.sync = 0x%x\n", mp1dec->mph.mpeg_header_bitwise.sync);
            printf("mp1dec->mph.mpeg_header_bitwise.mpeg_version = 0x%x\n", mp1dec->mph.mpeg_header_bitwise.mpeg_version);
            printf("mp1dec->mph.mpeg_header_bitwise.layer = 0x%x\n", mp1dec->mph.mpeg_header_bitwise.layer);
            printf("mp1dec->mph.mpeg_header_bitwise.protection = 0x%x\n", mp1dec->mph.mpeg_header_bitwise.protection);
            printf("mp1dec->mph.mpeg_header_bitwise.bitrate = 0x%x\n", mp1dec->mph.mpeg_header_bitwise.bitrate);
            printf("mp1dec->mph.mpeg_header_bitwise.samplerate = 0x%x\n", mp1dec->mph.mpeg_header_bitwise.samplerate);
            printf("mp1dec->mph.mpeg_header_bitwise.padding = 0x%x\n", mp1dec->mph.mpeg_header_bitwise.padding);
            printf("mp1dec->mph.mpeg_header_bitwise.priv = 0x%x\n", mp1dec->mph.mpeg_header_bitwise.priv);
            printf("mp1dec->mph.mpeg_header_bitwise.channel_mode = 0x%x\n", mp1dec->mph.mpeg_header_bitwise.channel_mode);
            printf("mp1dec->mph.mpeg_header_bitwise.mode_ext = 0x%x\n", mp1dec->mph.mpeg_header_bitwise.mode_ext);
            printf("mp1dec->mph.mpeg_header_bitwise.copyright = 0x%x\n", mp1dec->mph.mpeg_header_bitwise.copyright);
            printf("mp1dec->mph.mpeg_header_bitwise.original = 0x%x\n", mp1dec->mph.mpeg_header_bitwise.original);
            printf("mp1dec->mph.mpeg_header_bitwise.emphasis = 0x%x\n", mp1dec->mph.mpeg_header_bitwise.emphasis);
#endif
            switch(mp1dec->mph.mpeg_header_bitwise.channel_mode)
            {
                case 0 : mp1dec->channel = 2; break;
                case 1 : mp1dec->channel = 2; break;
                case 2 : mp1dec->channel = 2; break;
                case 3 : mp1dec->channel = 1; break;
                default : fprintf(stderr, "ERROR: could not find syncwords\n");
                          return -1;
            }
            switch(mp1dec->mph.mpeg_header_bitwise.samplerate)
            {
                case 0 : mp1dec->sample_rate = 44100; break;
                case 1 : mp1dec->sample_rate = 48000; break;
                case 2 : mp1dec->sample_rate = 32000; break;
                case 3 : // no break - falltrough
                default : fprintf(stderr, "ERROR: could not find syncwords\n");
                          return -1;
            }

            syncFound = 1;
        }
        else
        {
            if(++timeout >= MAX_ANC_BIT_SIZE)
            {
                fprintf(stderr, "ERROR: could not find syncwords\n");
                return -1;
            }
        }
    }

    if(mp1dec->mph.mpeg_header_bitwise.protection = 0)
    {
        // read crc
        int32_t crc;
        if(readBits(in_file, pFRAME1, 16, &crc))
        {
#ifdef DEBUG
            printf("Need to check crc\n");
#endif            
            return -1;
        }
    }

    // read bit allocations
    // read 32 * 4 bits for bit alloc of each subband
    for(n_band=0; n_band<BANDSIZE; n_band++)
    {
        for(int ch=0; ch < mp1dec->channel; ch++)
        {
            int32_t bspl;
            const uint32_t mask = (1<<4)-1;
            if(readBits(in_file, pFRAME1, 4, &bspl))
            {
                return -1;
            }
            BSPL_rx[ch][n_band] = (uint32_t)bspl & mask;

            tot_bits_rx += 4;
            scf_rx[ch][n_band] = 0;    // reset scf_rx
        }
    }

    // read scale factors
    for(n_band=0; n_band<BANDSIZE; n_band++)
    {
        for(int ch=0; ch < mp1dec->channel; ch++)
        {
            N = BSPL_rx[ch][n_band];
            if(N > 0)
            {
                // read the index of the (6 bit) scale factor
                int32_t scfIndex;
                const uint32_t mask = (1<<6)-1;
                if(readBits(in_file, pFRAME1, 6, &scfIndex))
                {
                    return -1;
                }
                
                scf_rx[ch][n_band] = table_scf[scfIndex & mask];   // look into scf table
                tot_bits_rx += 6;
            }
        }
    }

    // read max. 12*32 = 384 samples
    for(sample=0; sample < 12; sample++) // according to ISO, samples before bands
    {
        for(n_band=0; n_band < BANDSIZE; n_band++)
        {
            for(int ch=0; ch < mp1dec->channel; ch++)
            {
                y_rx[ch][sample][n_band] = 0; // init to 0
                N = BSPL_rx[ch][n_band];
                if(N > 0)
                {
                    tot_bits_rx += N;
                    int32_t y;
                    //const uint32_t mask = (1<<N)-1; // apply mask on y results in flip of sign
                    if(readBits(in_file, pFRAME1, N, &y))
                    {
                        return -1;
                    }

                    y_rx[ch][sample][n_band] = (y * scf_rx[ch][n_band]/(1<<(N-1)));
                }
            }
        }
    }
    
    return (tot_bits_rx);
}

int32_t readBits(FILE* in_file, uint8_t* pBitstream, const uint32_t numberOfBits, int32_t* retBits)
{
    uint32_t byteOffset = bitNdx >> 3;
    uint32_t bitOffset = bitNdx & 0x07;
    
    bitNdx = (bitNdx + numberOfBits) & (bufSize*8 - 1);

    uint32_t byteMask = bufSize - 1;

    if(validBits < numberOfBits)
    {
        int readBytes = fread(pBitstream, sizeof(uint8_t), bufSize, in_file);
#ifdef DEBUG
        printf("\nread %d bits and got %d valid bits", readBytes*8, validBits);
#endif
        validBits += readBytes*8;
#ifdef DEBUG
        printf(" and got new %d valid bits\n", validBits);
#endif        
        if(validBits <= 15 /*0*/)
        {
#ifdef DEBUG
            printf("INFO: running out of valid bits\n");
#endif
            return -1;
        }
    }

#if 1
    *retBits = (pBitstream[byteOffset & byteMask] << 24) |
                       (pBitstream[(byteOffset + 1) & byteMask] << 16) |
                       (pBitstream[(byteOffset + 2) & byteMask] << 8) |
                        pBitstream[(byteOffset + 3) & byteMask];
#else
    *retBits = (pBitstream[byteOffset & byteMask] << 0) |
                       (pBitstream[(byteOffset + 1) & byteMask] << 8) |
                       (pBitstream[(byteOffset + 2) & byteMask] << 16) |
                       (pBitstream[(byteOffset + 3) & byteMask] << 24) ;
#endif
    if (bitOffset)
    {
        *retBits <<= bitOffset;
        *retBits |= pBitstream[(byteOffset + 4) & byteMask] >> (8 - bitOffset);
    }

    validBits -= numberOfBits;

    *retBits = (*retBits >> (32 - numberOfBits));

    return 0;
}
