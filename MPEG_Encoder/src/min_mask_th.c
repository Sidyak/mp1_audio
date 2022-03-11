
//----------------------------------------------------------------------------
// Description       determination of minimum masking threshold
// Author:           Kim Radmacher
// Created on:       
//----------------------------------------------------------------------------

#include "init.h"

void min_mask_th(void)
{

    short Subband_size=(NFFT/2)/BANDSIZE;   // = 8, so 32 bands in steps of 8 to 512/2
    short n_sample, n_band;
    float band_min;

    // use global masking threshold to determine the minimal masking threshold
    // by transform all 32 subbands which are equivilent to 512 fft (relevent for 256 or 24 kHz)
    // to 25 freq. groups accorting to Zwicker which are converted into 102 values with respect to
    // fft up to 216 or 20.05 kHz. 
    for(n_band=0; n_band < BANDSIZE; n_band++)
    {
        band_min=96.0;    // init to max possible dB
        for(n_sample=0; n_sample < Subband_size; n_sample++)
        {
            if(LTg[Map[n_band*Subband_size+n_sample]-1] < band_min)
            {
                band_min=LTg[Map[n_band*Subband_size+n_sample]-1];
            }
        }
        LTmin[n_band]=band_min;
    }
}
