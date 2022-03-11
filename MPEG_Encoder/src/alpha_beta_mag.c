//----------------------------------------------------------------------------
// Description        magnitude estimation
//                    magnitude ~= alpha * max(|I|, |Q|) + beta * min(|I|, |Q|)
// Author:            Kim Radmacher
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

#define ALPHA 0.960433870103
#define BETA 0.397824734759

void alpha_beta_mag(void)
{
    float mag_max;
    short ifor;
    mag_max=MIN_POWER;

    for(ifor=0; ifor < 256; ifor++)
    {
        abs_inphase = fabs(pxFFT[ifor].re);
        abs_quadrature = fabs(pxFFT[ifor].im);

        if(abs_inphase > abs_quadrature)
            magnitude[ifor] = ((ALPHA * abs_inphase + BETA * abs_quadrature)/(256));
        else
            magnitude[ifor] = ((ALPHA * abs_quadrature + BETA * abs_inphase)/(256));
        if(magnitude[ifor] > mag_max)
            mag_max=magnitude[ifor];
    }
    // normalize to 96 dB -> 10^(96/20)=3981100000
    for (ifor=0; ifor<256; ifor++)
        magnitude[ifor]=(magnitude[ifor]/mag_max)*3981100000;
}
