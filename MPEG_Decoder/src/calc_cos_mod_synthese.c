//----------------------------------------------------------------------------
// Description       create global masking threshold
// Author:           Kim Radmacher
// Created on:       18.12.2014
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

void calc_cos_mod_synthese()
{
    short band_i=0, polyph=0, tw_sam=0;
     // do the cosine-modulation (multiply-accu or dot-product)
     for(tw_sam=0; tw_sam<12; tw_sam++)
     {
         for(polyph=0; polyph < 64; polyph++)
         {
             pOut1[polyph+tw_sam*64]=0.0;
             for(band_i=0; band_i<BANDSIZE; band_i++)
             {
                 pOut1[polyph+tw_sam*64] += T[band_i][polyph] * y_rx[tw_sam][band_i];    // for own coeff. use factor of 2 -> own coeffs already in T[][]
             }
         }
     }
}
