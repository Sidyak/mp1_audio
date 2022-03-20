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

#include <assert.h>

static int32_t readBits(FILE* in_file, uint8_t* pBitstream, const uint32_t numberOfBits, int32_t *retBits);

static uint32_t bitNdx = 0;
static const uint32_t bufSize = 16*1024*1024; // 16 MB max
static uint32_t validBits = 0;  // valid bits for current frame
#ifdef FIX_FOR_REAL_BITRATE_REDUCTION
static uint32_t start_found = 0;
#endif
const uint32_t syncWords[2] = {0xCCCCAAAA, 0xAAAAF0F0};  // original

int32_t rx_frame(FILE *in_file)
{
    short n_band, sample, N;
    tot_bits_rx = 0;
    cnt_FRAME_read = 0;

#ifdef FIX_FOR_REAL_BITRATE_REDUCTION
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
            printf("Preamble for COMPRESSED found\n");
            start_found = 1; // start sequence for compressed data
        }
        else
        {
            fprintf(stderr, "ERROR: could not find syncwords\n");
            printf("sW[0] = 0x%x (0x%x)\n", sW[0], syncWords[0]);
            printf("sW[1] = 0x%x (0x%x)\n", sW[1], syncWords[1]);
            
            return -1;
        }
    }
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
//printf("BSPL[%d] = %d\n", n_band, BSPL_rx[n_band]);

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
//printf("scfIndex[%d] = %d\t", n_band ,scfIndex);
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
        for(n_band=0; n_band<BANDSIZE; n_band++)
        {
            y_rx[sample][n_band] = 0; // init to 0
            N = BSPL_rx[n_band]; // TODO: move out of for loop and swap for loops to improve performance
            if(N > 0)
            {
                tot_bits_rx += N;
#ifdef FIX_FOR_REAL_BITRATE_REDUCTION
                int32_t y;
                const uint32_t mask = (1<<N)-1;
                if(readBits(in_file, pFRAME1, N, &y))
                {
                    return -1;
                }
//                if(y) printf("y[%d][%d] = %d (%d bits)\t", sample, n_band, y, N);
// TODO: sign sometimes switches -> problem was: e.g. an 8 with 4 bits signed is -8. 
//       round down instead of round up fixed the issue:
//       floor( (S[n_band][sample]/(scf[n_band]*exp2LUT[BSPL[n_band]-2])) /*+ 0.5*/ ); 
                y_rx[sample][n_band] = (y * scf_rx[n_band]/(1<<(N-1)));
#else
                y_rx[sample][n_band] = (pFRAME1[cnt_FRAME_read++] * scf_rx[n_band])/(1<<(N-1) ) ;
#endif
            }
        }
    }
//printf("\n");
    return (tot_bits_rx);
}

int32_t readBits(FILE* in_file, uint8_t* pBitstream, const uint32_t numberOfBits, int32_t* retBits)
{
    uint32_t byteOffset = bitNdx >> 3;
    uint32_t bitOffset = bitNdx & 0x07;
//printf("got %d valid bits\n", validBits);
    bitNdx = (bitNdx + numberOfBits) & (bufSize*8 - 1);

    uint32_t byteMask = bufSize - 1;

    if(validBits < numberOfBits)
    {
        int readBytes = fread(pBitstream, sizeof(uint8_t), bufSize, in_file);
        printf("\nread %d bits and got %d valid bits", readBytes*8, validBits);
        validBits += readBytes*8;
        printf(" and got new %d valid bits\n", validBits);
        if(validBits <= 15 /*0*/)
        {
            printf("INFO: running out of valid bits\n");
            return -1;
        }
    }

//printf(" pBitstream[%d] = 0x%x\n", byteOffset, pBitstream[byteOffset]);
//printf(" pBitstream[%d] = 0x%x\n", byteOffset+1, pBitstream[byteOffset+1]);
//printf(" pBitstream[%d] = 0x%x\n", byteOffset+2, pBitstream[byteOffset+2]);
//printf(" pBitstream[%d] = 0x%x\n", byteOffset+3, pBitstream[byteOffset+3]);
#if 1
    *retBits = (pBitstream[byteOffset & byteMask] << 24) |
                       (pBitstream[(byteOffset + 1) & byteMask] << 16) |
                       (pBitstream[(byteOffset + 2) & byteMask] << 8) |
                        pBitstream[(byteOffset + 3) & byteMask];
#else
    *retBits = (pBitstream[byteOffset & byteMask]) |
               (pBitstream[(byteOffset + 1) & byteMask] << 8) |
               (pBitstream[(byteOffset + 2) & byteMask] << 16) |
                pBitstream[(byteOffset + 3) & byteMask] << 24;
#endif
//printf(" *retBits = 0x%x\n", *retBits);
    if (bitOffset)
    {
        *retBits <<= bitOffset;
        *retBits |= pBitstream[(byteOffset + 4) & byteMask] >> (8 - bitOffset);
    }

    validBits -= numberOfBits;

    *retBits = (*retBits >> (32 - numberOfBits));
//    printf("read uint32_t 0x%x\n", *retBits);

    return 0;
}
