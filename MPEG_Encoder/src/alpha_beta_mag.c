//----------------------------------------------------------------------------
// Description        magnitude estimation
//                    magnitude ~= alpha * max(|I|, |Q|) + beta * min(|I|, |Q|)
// Author:            Kim Radmacher
// Created on:
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
