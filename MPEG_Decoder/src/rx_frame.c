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

#include "init.h"

void rx_frame(void)
{
    short n_band, sample,N;
    tot_bits_rx = 0;
    cnt_FRAME_read = 0;

    // read bit allocations
    // read 32 * 4 bits for bit alloc of each subband
    for(n_band=0; n_band<BANDSIZE; n_band++)
    {
        BSPL_rx[n_band]=pFRAME1[cnt_FRAME_read++];
        tot_bits_rx+=4;
        scf_rx[n_band] = 0;    // reset scf_rx
    }

    // read scale factors
    for(n_band=0; n_band<BANDSIZE; n_band++)
    {
        N=BSPL_rx[n_band];
        if(N>0)
        {
            tot_bits_rx+=6;
            // read the index of the (6 bit) scale factor
            scf_rx[n_band] = table_scf[pFRAME1[cnt_FRAME_read++]];   // look into scf table
        }
    }

    // read max. 12*32 = 384 samples
    for(sample=0; sample<12; sample++)
    {
        for(n_band=0; n_band<BANDSIZE; n_band++)
        {
            y_rx[sample][n_band] = 0.0;    // init to 0
            N = BSPL_rx[n_band];
            if(N > 0)
            {
                tot_bits_rx += N;
                y_rx[sample][n_band] = (pFRAME1[cnt_FRAME_read++]*scf_rx[n_band])/(1<<(N-1) ) ;
            }
        }
    }
}
