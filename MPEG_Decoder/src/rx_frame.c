//----------------------------------------------------------------------------
// Description       "receive" data and demultiplex it
// Author:           Kim Radmacher
// Created on:
//----------------------------------------------------------------------------

#include "init.h"

void rx_frame(void)
{
    short n_band, sample,N;
    tot_bits_rx = 0;
    cnt_FRAME_read = 0;

    // read bit allocations
    // read 32 * 4 bits for bit alloc of each subband
    for(n_band=0; n_band<BANDSIZE; n_band++)
    {
        BSPL_rx[n_band]=pFRAME1[cnt_FRAME_read++];
        tot_bits_rx+=4;
        scf_rx[n_band] = 0;    // reset scf_rx
    }

    // read scale factors
    for(n_band=0; n_band<BANDSIZE; n_band++)
    {
        N=BSPL_rx[n_band];
        if(N>0)
        {
            tot_bits_rx+=6;
            // read the index of the (6 bit) scale factor
            scf_rx[n_band] = table_scf[pFRAME1[cnt_FRAME_read++]];   // look into scf table
        }
    }

    // read max. 12*32 = 384 samples
    for(sample=0; sample<12; sample++)
    {
        for(n_band=0; n_band<BANDSIZE; n_band++)
        {
            y_rx[sample][n_band] = 0.0;    // init to 0
            N = BSPL_rx[n_band];
            if(N > 0)
            {
                tot_bits_rx += N;
                y_rx[sample][n_band] = (pFRAME1[cnt_FRAME_read++]*scf_rx[n_band])/(1<<(N-1) ) ;
            }
        }
    }
}
