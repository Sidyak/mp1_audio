//----------------------------------------------------------------------------
// Description       create global masking threshold
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

void global_mask_th(void)
{
    short mask_ind, ton_ind, nton_ind;
    float temp;
    
    /* modified method */
    for(mask_ind=0;mask_ind < 102; mask_ind++)
    {
       temp = LTq_dB[mask_ind];  // abs. hearing threshold non-log.
       for(ton_ind=0;ton_ind < 25;ton_ind++)
       {
            // if list of tonals not empty
            if(LTtm[ton_ind][mask_ind] > temp)
            {
                // take tonal mask threshold at masker
                temp = LTtm[ton_ind][mask_ind];
            }
       }

        for(nton_ind=0;nton_ind < 25;nton_ind++)
        {
            // if list of non-tonals not empty    
            if(LTnm[nton_ind][mask_ind] > temp)
            {   
                // take non-tonal mask threshold at masker
                temp = LTnm[nton_ind][mask_ind];
            }
       }

       LTg[mask_ind]=temp;   // determine globale masking threshold

    }
}
