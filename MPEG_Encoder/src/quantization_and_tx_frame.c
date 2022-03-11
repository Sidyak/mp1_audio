
//----------------------------------------------------------------------------
// Description       quantization and production of frame to store
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

#include "init.h"
#include <math.h>

#define CACHE_BITS  32

static void putBits(uint8_t* pBitBuf, uint32_t value, const uint32_t numberOfBits);
static uint8_t writeBits(uint8_t *pBitBuf, uint32_t value, const uint32_t numberOfBits);


const uint32_t bitMask[32 + 1] = 
{
    0x0,        0x1,        0x3,       0x7,       0xf,       0x1f,
    0x3f,       0x7f,       0xff,      0x1ff,     0x3ff,     0x7ff,
    0xfff,      0x1fff,     0x3fff,    0x7fff,    0xffff,    0x1ffff,
    0x3ffff,    0x7ffff,    0xfffff,   0x1fffff,  0x3fffff,  0x7fffff,
    0xffffff,   0x1ffffff,  0x3ffffff, 0x7ffffff, 0xfffffff, 0x1fffffff,
    0x3fffffff, 0x7fffffff, 0xffffffff
};

static uint32_t bitsInCache = 0;
static uint32_t cacheWord = 0;
static uint32_t bitNdx = 0;
static uint32_t validBits = 0; // valid bits for current frame 
static uint32_t bufSize = 0;
static uint32_t bufBits = 0;

int quantization_and_tx_frame(void)
{
    uint8_t n_band, scf_ind, sample, N;
    short number;

    uint32_t total_bit_leng = 0;    // total bits planed to use in current frame
    cnt_FRAME_fill = 4;    // first data starts right behind the preamble (frame sync sequence of 4*16 bit)

    validBits = 0;

    // first 32 frame positions are the number of bits for each subband
    for(n_band=0; n_band < BANDSIZE; n_band+=2)
    {
#ifdef FIX_FOR_REAL_BITRATE_REDUCTION
        writeBits(pFRAME1, BSPL[n_band], 4);
        total_bit_leng += 4;
        writeBits(pFRAME1, BSPL[n_band+1], 4);
        total_bit_leng += 4;
#else
        pFRAME1[cnt_FRAME_fill++] = BSPL[n_band];
        total_bit_leng += 4;
        pFRAME1[cnt_FRAME_fill++] = BSPL[n_band+1];
        total_bit_leng += 4;
#endif
    }

    // indices of scalefactor table
    for(n_band=0; n_band < BANDSIZE; n_band++)
    {
        scf_ind = 62;
        if(BSCF[n_band] > 0)
        {
            // Store the scf index only
            while(scf[n_band] != table_scf[scf_ind])
            {
                scf_ind--;
            }
#ifdef FIX_FOR_REAL_BITRATE_REDUCTION
            writeBits(pFRAME1, scf_ind, 6);
#else
            pFRAME1[cnt_FRAME_fill++] = scf_ind;
#endif
            total_bit_leng += 6;
        }
    }

    // following frame positions own the bits of quantized subbands
    for(sample=0; sample < 12; sample++)
    {
        for(n_band=0; n_band < BANDSIZE; n_band++)
        {
            N = BSPL[n_band];   // determine number of required bits in subband
#ifdef DEBUG
                printf("%d bits used in subband %d\n", N, n_band);
#endif
            if(N > 0)
            {   // quantize if bits are available 
                number = floor( (S[n_band][sample]/(scf[n_band]*exp2LUT[BSPL[n_band]-2])) + 0.5 );
#ifdef FIX_FOR_REAL_BITRATE_REDUCTION
                writeBits(pFRAME1, number, N);
#else
                pFRAME1[cnt_FRAME_fill++] = number;
#endif
               total_bit_leng += N;
            }
        }
    }

    return validBits;
}

/**
 * \brief WriteBits Function. This function writes numberOfBits of value into
 * bitstream.
 *
 * \param pBitBuf       pointer to bitstream buffer
 * \param value         The data to be written
 * \param numberOfBits  The number of bits to be written
 * \return              Number of bits written
 */
uint8_t writeBits(uint8_t *pBitBuf, uint32_t value, const uint32_t numberOfBits)
{
    const uint32_t validMask = bitMask[numberOfBits];

    if ((bitsInCache + numberOfBits) < CACHE_BITS)
    {
        bitsInCache += numberOfBits;
        cacheWord = (cacheWord << numberOfBits) | (value & validMask);
    }
    else
    {
        /* Put always 32 bits into memory             */
        /* - fill cache's LSBits with MSBits of value */
        /* - store 32 bits in memory using subroutine */
        /* - fill remaining bits into cache's LSBits  */
        /* - upper bits in cache are don't care       */

        /* Compute number of bits to be filled into cache */
        int missing_bits = CACHE_BITS - bitsInCache;
        int remaining_bits = numberOfBits - missing_bits;
        value = value & validMask;
        /* Avoid shift left by 32 positions */
        uint32_t cacheWord = (missing_bits == 32) ? 0 : (cacheWord << missing_bits);
        cacheWord |= (value >> (remaining_bits));

        putBits(pBitBuf, cacheWord, 32);

        cacheWord = value;
        bitsInCache = remaining_bits;
      }

     return validBits;
}

void putBits(uint8_t* pBitBuf, uint32_t value, const uint32_t numberOfBits)
{
    if (numberOfBits != 0)
    {
       uint32_t byteOffset0 = bitNdx >> 3;
       uint32_t bitOffset = bitNdx & 0x7;

        bitNdx = (bitNdx + numberOfBits) & (bufBits - 1);
        validBits += numberOfBits;

        uint32_t byteMask = bufSize - 1;

        uint32_t byteOffset1 = (byteOffset0 + 1) & byteMask;
        uint32_t byteOffset2 = (byteOffset0 + 2) & byteMask;
        uint32_t byteOffset3 = (byteOffset0 + 3) & byteMask;

        // Create tmp containing free bits at the left border followed by bits to
        // write, LSB's are cleared, if available Create mask to apply upon all
        // buffer bytes
        uint32_t tmp = (value << (32 - numberOfBits)) >> bitOffset;
        uint32_t mask = ~((bitMask[numberOfBits] << (32 - numberOfBits)) >> bitOffset);

        // read all 4 bytes from buffer and create a 32-bit cache
        uint32_t cache = (((uint32_t)pBitBuf[byteOffset0]) << 24) |
                    (((uint32_t)pBitBuf[byteOffset1]) << 16) |
                    (((uint32_t)pBitBuf[byteOffset2]) << 8) |
                    (((uint32_t)pBitBuf[byteOffset3]) << 0);

        cache = (cache & mask) | tmp;
        pBitBuf[byteOffset0] = (uint8_t)(cache >> 24);
        pBitBuf[byteOffset1] = (uint8_t)(cache >> 16);
        pBitBuf[byteOffset2] = (uint8_t)(cache >> 8);
        pBitBuf[byteOffset3] = (uint8_t)(cache >> 0);

        pBitBuf += 4; // move to next 32 bits

        if ((bitOffset + numberOfBits) > 32)
        {
            uint32_t byteOffset4 = (byteOffset0 + 4) & byteMask;
            // remaining bits: in range 1..7
            // replace MSBits of next byte in buffer by LSBits of "value"
            int bits = (bitOffset + numberOfBits) & 7;
            cache = (uint32_t)pBitBuf[byteOffset4] & (~(bitMask[bits] << (8 - bits)));
            cache |= value << (8 - bits);
            pBitBuf[byteOffset4] = (uint8_t)cache;
        }
    }
}
