//----------------------------------------------------------------------------
// Description         log/dB-level calculation
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

#include "init.h"

void calc_mag_log(void)
{
    short cnt_cm=0;
    float manti_fft,logM_fft;
    short exponent_fft;
    int tmp_INT_fft;

    for (cnt_cm=0; cnt_cm<256; cnt_cm++)
    {
        tmp_INT_fft = (*(int*)&magnitude[cnt_cm]);
        manti_fft = (tmp_INT_fft & 0x007fffff)/8388608.0;
        logM_fft = logLuT[(short)(((manti_fft*1000)))];
        exponent_fft = ((tmp_INT_fft & 0x7f800000)>>23);
        magnitude_dB[cnt_cm] = ((logM_fft + (float)((exponent_fft-127)*log2value)) * constant_factor_log_to_10log10);
    }
}
