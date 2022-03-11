//----------------------------------------------------------------------------
// Description       create global masking threshold
// Author:           Kim Radmacher
// Created on:
//----------------------------------------------------------------------------

#include "init.h"
#include <math.h>

void global_mask_th(void)
{
    short mask_ind, ton_ind, nton_ind;
    float temp;
    
    /* modified method */
    for(mask_ind=0;mask_ind < 102; mask_ind++)
    {
       temp = LTq_dB[mask_ind];  // abs. hearing threshold non-log.
       for(ton_ind=0;ton_ind < 25;ton_ind++)
       {
            // if list of tonals not empty
            if(LTtm[ton_ind][mask_ind] > temp)
            {
                // take tonal mask threshold at masker
                temp = LTtm[ton_ind][mask_ind];
            }
       }

        for(nton_ind=0;nton_ind < 25;nton_ind++)
        {
            // if list of non-tonals not empty    
            if(LTnm[nton_ind][mask_ind] > temp)
            {   
                // take non-tonal mask threshold at masker
                temp = LTnm[nton_ind][mask_ind];
            }
       }

       LTg[mask_ind]=temp;   // determine globale masking threshold

    }
}
