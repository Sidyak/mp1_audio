//----------------------------------------------------------------------------
// Description         Scalefactor calculation
// Author:             Kim Radmacher
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

#include <math.h>
#include "init.h"

void scalefactor(void)
{
    float s_band_max, Sabs;
    short table_len = 63, ji, ind, sample;

    // take maximum of 12 samples in band 'ind' and set as minimum
    for (ind=0; ind<BANDSIZE; ind++)
    {
        s_band_max = 0.0;
        for(sample=0; sample<12; sample++)
        {
            Sabs = fabs(S[ind][sample]);
            if(Sabs > s_band_max)
                s_band_max = Sabs;
        }
        if(s_band_max > table_scf[0])
        {
            scf[ind] = table_scf[0];
        }
        else 
        {
            ji = 0;
            // find next matching scalefactor to max of band i from table
            while(ji < table_len && s_band_max > table_scf[table_len - ji - 1])
            {
                ji++;
            }
            scf[ind] = table_scf[table_len - ji - 1];
       }
    }
}
