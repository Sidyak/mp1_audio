//----------------------------------------------------------------------------
// Description         cosinus modulation
// Author:             Kim Radmacher
// Created on:         18.12.2014
//----------------------------------------------------------------------------

#include "init.h"

void calc_cos_mod()
{
    short band_i=0, polyph=0, tw_sam=0;
     // do the cosine-modulation (multiply-accu bzw. dot-product)
    for(tw_sam=0;tw_sam<12;tw_sam++)
    {
        for(band_i=0;band_i<BANDSIZE;band_i++)
        {
            S[band_i][tw_sam]=0.0;
            for(polyph=0;polyph < 64;polyph++)
            {
                S[band_i][tw_sam] += M[band_i][polyph] * pWork_fb[polyph+tw_sam*64];
            }
        }
    }
}
