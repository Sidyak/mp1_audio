//----------------------------------------------------------------------------
// Description       create global masking threshold
// Author:           Kim Radmacher
// Created on:       18.12.2014
//----------------------------------------------------------------------------

#include "init.h"

void calc_cos_mod_synthese()
{
    short band_i=0, polyph=0, tw_sam=0;
     // do the cosine-modulation (multiply-accu or dot-product)
     for(tw_sam=0; tw_sam<12; tw_sam++)
     {
         for(polyph=0; polyph < 64; polyph++)
         {
             pOut1[polyph+tw_sam*64]=0.0;
             for(band_i=0; band_i<BANDSIZE; band_i++)
             {
                 pOut1[polyph+tw_sam*64] += T[band_i][polyph] * y_rx[tw_sam][band_i];    // for own coeff. use factor of 2 -> own coeffs already in T[][]
             }
         }
     }
}
