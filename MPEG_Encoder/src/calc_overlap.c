//----------------------------------------------------------------------------
// Description         determine fft overlap and hann windowing
// Author:             Kim Radmacher
// Created on:         21.01.2015
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

void calc_overlap(void)
{
    for (ind_calc_overlap=0;ind_calc_overlap<128;ind_calc_overlap++)
    {
        pxFFT[ind_calc_overlap].re = pxFFT_old[ind_calc_overlap].re * 32767 * hanning[ind_calc_overlap];    // 64 samples (OVERLAP)
        pxFFT[ind_calc_overlap].im = 0;     // set to 0 since to overwrite last fft bins (in-place calc.)
        pxFFT_old[ind_calc_overlap].re = pWork_fft[127-ind_calc_overlap];     // store latest 128 samples
    }

    // last 128 samples are calc_overlap stored in fist platec in reversed order
    for (ind_calc_overlap=128;ind_calc_overlap<NFFT;ind_calc_overlap++)
    {
        pxFFT[ind_calc_overlap].re = pWork_fft[383-ind_calc_overlap+128] * 32767 * hanning[ind_calc_overlap];    // 384 new samples (12*32=384 and 128 overlap)
        pxFFT[ind_calc_overlap].im = 0;
    }
}
