//----------------------------------------------------------------------------
// Description         calc signal-mask-ratio
// Author:             Kim Radmacher
// Created on:
//----------------------------------------------------------------------------

#include "init.h"

void calc_SMR(void)
{
    short n_band = 0;

    for(n_band=0; n_band < BANDSIZE; n_band++)
    {
         SMR[n_band] = Ls[n_band]-LTmin[n_band];
    }
}
