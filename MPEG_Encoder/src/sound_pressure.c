//----------------------------------------------------------------------------
// Description       Sound pressure calculation
// Author:           Kim Radmacher
// Created on:
//----------------------------------------------------------------------------

#include "init.h"

void sound_pressure(void)
{
    short ind,band_k;
    float Xmax,Ls_scf, scf_32768;
    float manti2,logM2;
    short exponent2;
    int tmp_INT2;

    for (band_k=0; band_k < BANDSIZE; band_k++)
    {
        Xmax = 0;
        for(ind=0; ind < 8; ind++)
        {    
            // search for max of (512/2)/32 = 8 values in subband
            if(Xmax < magnitude_dB[band_k*8+ind])
                Xmax = magnitude_dB[band_k*8+ind];
        }
        // take greatest value
        scf_32768 = scf[band_k]*32768;
        tmp_INT2 = (*(int*)&scf_32768);
        manti2 = (tmp_INT2 & 0x007fffff)/8388608.0;
        logM2 = logLuT[(short)(((manti2*1000)))];
        exponent2 = ((tmp_INT2 & 0x7f800000)>>23);
        Ls_scf = ((logM2 + (float)((exponent2-127)*log2value)) * constant_factor_log_to_20log10);

        if(Xmax > (Ls_scf-10))    // 3.16227766016838 = db2mag(10) for non logarithm
            Ls[band_k] = Xmax;
        else
            Ls[band_k] = (Ls_scf-10);
    }
}
