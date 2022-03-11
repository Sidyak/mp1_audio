//----------------------------------------------------------------------------
// Description         determine fft overlap and hann windowing
// Author:             Kim Radmacher
// Created on:         21.01.2015
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
