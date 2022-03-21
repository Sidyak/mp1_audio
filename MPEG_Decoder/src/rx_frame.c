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

static int32_t readBits(FILE* in_file, uint8_t* pBitstream, const uint32_t numberOfBits, int32_t *retBits);

static uint32_t bitNdx = 0;
static const uint32_t bufSize = 16*1024*1024; // 16 MB max
static uint32_t validBits = 0;  // valid bits for current frame
#ifdef FIX_FOR_REAL_BITRATE_REDUCTION
static uint32_t start_found = 0;
#endif
const uint32_t syncWords[2] = {0xCCCCAAAA, 0xAAAAF0F0};

int32_t rx_frame(FILE *in_file)
{
    short n_band, sample, N;
    tot_bits_rx = 0;
    cnt_FRAME_read = 0;

#ifdef FIX_FOR_REAL_BITRATE_REDUCTION
#if 0
    int32_t sW[2];
    
    while(start_found == 0)
    {
        if(readBits(in_file, pFRAME1, 32, &sW[0]))
        {
            return -1;
        }
        if(readBits(in_file, pFRAME1, 32, &sW[1]))
        {
            return -1;
        }
        
        if((sW[0] == syncWords[0]) && (sW[1] == syncWords[1]))
        {
#ifdef DEBUG
            printf("Preamble for COMPRESSED found\n");
#endif
            start_found = 1; // start sequence for compressed data
        }
        else
        {
#ifdef DEBUG
            fprintf(stderr, "ERROR: could not find syncwords\n");
            printf("sW[0] = 0x%x (0x%x)\n", sW[0], syncWords[0]);
            printf("sW[1] = 0x%x (0x%x)\n", sW[1], syncWords[1]);
#endif
            return -1;
        }
    }
#else
        // mpeg conform header is attached to each frame
        union mpeg_header
        {
            struct
            {
#if 0
                uint16_t sync :12;
                uint8_t mpeg_version :1;
                uint8_t layer :2;
                uint8_t protection :1;     
                uint8_t bitrate :4;
                uint8_t samplerate :2;
                uint8_t padding :1;
                uint8_t priv :1;
                uint8_t channel_mode :2;
                uint8_t mode_ext :2;
                uint8_t copyright :1;
                uint8_t original :1;
                uint8_t emphasis :2;
#else
                uint8_t emphasis :2;
                uint8_t original :1;
                uint8_t copyright :1;
                uint8_t mode_ext :2;
                uint8_t channel_mode :2;
                uint8_t priv :1;
                uint8_t padding :1;
                uint8_t samplerate :2;
                uint8_t bitrate :4;
                uint8_t protection :1;
                uint8_t layer :2;
                uint8_t mpeg_version :1;
                uint16_t sync :12;
#endif
            } mpeg_header_bitwise; // 32 bit

            uint32_t mpeg_header_word;
        };

    mpeg_header mph;
    
    //while(start_found == 0)
    {
        if(readBits(in_file, pFRAME1, 32, (int32_t*)&mph.mpeg_header_word))
        {
            return -1;
        }
        if(mph.mpeg_header_bitwise.sync == 0xFFF)
        {
#ifdef DEBUG
            printf("Preamble/sync found\n");
            printf("mph.mpeg_header_bitwise.sync = 0x%x\n", mph.mpeg_header_bitwise.sync);
            printf("mph.mpeg_header_bitwise.mpeg_version = 0x%x\n", mph.mpeg_header_bitwise.mpeg_version);
            printf("mph.mpeg_header_bitwise.layer = 0x%x\n", mph.mpeg_header_bitwise.layer);
            printf("mph.mpeg_header_bitwise.protection = 0x%x\n", mph.mpeg_header_bitwise.protection);
            printf("mph.mpeg_header_bitwise.bitrate = 0x%x\n", mph.mpeg_header_bitwise.bitrate);
            printf("mph.mpeg_header_bitwise.samplerate = 0x%x\n", mph.mpeg_header_bitwise.samplerate);
            printf("mph.mpeg_header_bitwise.padding = 0x%x\n", mph.mpeg_header_bitwise.padding);
            printf("mph.mpeg_header_bitwise.priv = 0x%x\n", mph.mpeg_header_bitwise.priv);
            printf("mph.mpeg_header_bitwise.channel_mode = 0x%x\n", mph.mpeg_header_bitwise.channel_mode);
            printf("mph.mpeg_header_bitwise.mode_ext = 0x%x\n", mph.mpeg_header_bitwise.mode_ext);
            printf("mph.mpeg_header_bitwise.copyright = 0x%x\n", mph.mpeg_header_bitwise.copyright);
            printf("mph.mpeg_header_bitwise.original = 0x%x\n", mph.mpeg_header_bitwise.original);
            printf("mph.mpeg_header_bitwise.emphasis = 0x%x\n", mph.mpeg_header_bitwise.emphasis);
#endif
            start_found = 1;
        }
        else
        {
#ifdef DEBUG
            fprintf(stderr, "ERROR: could not find syncwords\n");
            printf("mph.mpeg_header_bitwise.sync = 0x%x\n", mph.mpeg_header_bitwise.sync);
            printf("mph.mpeg_header_bitwise.mpeg_version = 0x%x\n", mph.mpeg_header_bitwise.mpeg_version);
            printf("mph.mpeg_header_bitwise.layer = 0x%x\n", mph.mpeg_header_bitwise.layer);
            printf("mph.mpeg_header_bitwise.protection = 0x%x\n", mph.mpeg_header_bitwise.protection);
            printf("mph.mpeg_header_bitwise.bitrate = 0x%x\n", mph.mpeg_header_bitwise.bitrate);
            printf("mph.mpeg_header_bitwise.samplerate = 0x%x\n", mph.mpeg_header_bitwise.samplerate);
            printf("mph.mpeg_header_bitwise.padding = 0x%x\n", mph.mpeg_header_bitwise.padding);
            printf("mph.mpeg_header_bitwise.priv = 0x%x\n", mph.mpeg_header_bitwise.priv);
            printf("mph.mpeg_header_bitwise.channel_mode = 0x%x\n", mph.mpeg_header_bitwise.channel_mode);
            printf("mph.mpeg_header_bitwise.mode_ext = 0x%x\n", mph.mpeg_header_bitwise.mode_ext);
            printf("mph.mpeg_header_bitwise.copyright = 0x%x\n", mph.mpeg_header_bitwise.copyright);
            printf("mph.mpeg_header_bitwise.original = 0x%x\n", mph.mpeg_header_bitwise.original);
            printf("mph.mpeg_header_bitwise.emphasis = 0x%x\n", mph.mpeg_header_bitwise.emphasis);
#endif
            return -1;
        }
        if(mph.mpeg_header_bitwise.protection = 0)
        {
            // read crc
            int32_t crc;
            if(readBits(in_file, pFRAME1, 16, &crc))
            {
                return -1;
            }
        }
    }
#endif
#endif

    // read bit allocations
    // read 32 * 4 bits for bit alloc of each subband
    for(n_band=0; n_band<BANDSIZE; n_band++)
    {
#ifdef FIX_FOR_REAL_BITRATE_REDUCTION
        int32_t bspl;
        const uint32_t mask = (1<<4)-1;
        if(readBits(in_file, pFRAME1, 4, &bspl))
        {
            return -1;
        }
        BSPL_rx[n_band] = (uint32_t)bspl & mask;
#else    
        BSPL_rx[n_band] = pFRAME1[cnt_FRAME_read++];
#endif

        tot_bits_rx += 4;
        scf_rx[n_band] = 0;    // reset scf_rx
    }

    // read scale factors
    for(n_band=0; n_band<BANDSIZE; n_band++)
    {
        N = BSPL_rx[n_band];
        if(N > 0)
        {
            // read the index of the (6 bit) scale factor
#ifdef FIX_FOR_REAL_BITRATE_REDUCTION
            int32_t scfIndex;
            const uint32_t mask = (1<<6)-1;
            if(readBits(in_file, pFRAME1, 6, &scfIndex))
            {
                return -1;
            }
            
            scf_rx[n_band] = table_scf[scfIndex & mask];   // look into scf table
#else
            scf_rx[n_band] = table_scf[pFRAME1[cnt_FRAME_read++]];   // look into scf table
#endif
            tot_bits_rx += 6;
        }
    }

    // read max. 12*32 = 384 samples
    for(sample=0; sample < 12; sample++)
    {
        // TODO: switch for loops for less computational complexity -> needs to be switched at decoder as well. what does the MPEG ISO do?
        for(n_band=0; n_band < BANDSIZE; n_band++)
        {
            y_rx[sample][n_band] = 0; // init to 0
            N = BSPL_rx[n_band];
            if(N > 0)
            {
                tot_bits_rx += N;
#ifdef FIX_FOR_REAL_BITRATE_REDUCTION
                int32_t y;
                //const uint32_t mask = (1<<N)-1; // apply mask on y results in flip of sign
                if(readBits(in_file, pFRAME1, N, &y))
                {
                    return -1;
                }

                y_rx[sample][n_band] = (y * scf_rx[n_band]/(1<<(N-1)));
#else
                y_rx[sample][n_band] = (pFRAME1[cnt_FRAME_read++] * scf_rx[n_band])/(1<<(N-1));
#endif
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

    *retBits = (pBitstream[byteOffset & byteMask] << 24) |
                       (pBitstream[(byteOffset + 1) & byteMask] << 16) |
                       (pBitstream[(byteOffset + 2) & byteMask] << 8) |
                        pBitstream[(byteOffset + 3) & byteMask];

    if (bitOffset)
    {
        *retBits <<= bitOffset;
        *retBits |= pBitstream[(byteOffset + 4) & byteMask] >> (8 - bitOffset);
    }

    validBits -= numberOfBits;

    *retBits = (*retBits >> (32 - numberOfBits));

    return 0;
}
