//----------------------------------------------------------------------------
// Description         log/dB-level calculation
// Author:             Kim Radmacher
// Created on:
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
