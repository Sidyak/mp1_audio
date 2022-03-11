
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

void quantization_and_tx_frame(void)
{
    short n_band,scf_ind,sample,N;
    short number;

    total_bit_leng=0;    // reset for counter for total Bits they have to transmit
    cnt_FRAME_fill=4;    // first data starts right behind the preamble (frame sync sequence of 4*16 bit)
    
    // first 32 frame positions are the number of bits for each subband
    for(n_band=0; n_band < BANDSIZE; n_band++)
    {
        pFRAME1[cnt_FRAME_fill++] = BSPL[n_band];
        total_bit_leng += 4;
    }

    // indices of scalefactor table
    for(n_band=0; n_band < BANDSIZE; n_band++)
    {
        scf_ind=62;
        if(BSCF[n_band] > 0)
        {
            // Store the scf index only
            while(scf[n_band] != table_scf[scf_ind])
            {
                scf_ind--;
            }
            pFRAME1[cnt_FRAME_fill++] = scf_ind;
            total_bit_leng += 6;
        }
    }

    // following frame positions own the bits of quantized subbands
    for(sample=0; sample<12; sample++)
    {
        for(n_band=0; n_band < BANDSIZE; n_band++)
        {
            N=BSPL[n_band];   // determine number of required bits in subband
#ifdef DEBUG
                printf("%d bits used in subband %d\n", N, n_band);
#endif
            if(N > 0)
            {   // quantize if bits are available 
                number = floor( (S[n_band][sample]/(scf[n_band]*exp2LUT[BSPL[n_band]-2])) + 0.5 );
                pFRAME1[cnt_FRAME_fill++] = number;
                total_bit_leng += N;
            }
        }
    }
}
