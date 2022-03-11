
//----------------------------------------------------------------------------
// Description       quantization and production of frame to store
// Author:           Kim Radmacher
// Created on:       
//----------------------------------------------------------------------------

#include "init.h"
#include <math.h>

void quantization_and_tx_frame(void)
{
    short n_band,scf_ind,sample,N;
    short number;

    total_bit_leng=0;    // reset for counter for total Bits they have to transmit
    cnt_FRAME_fill=4;    // first data starts right behind the preamble (frame sync sequence of 4*16 bit)
    
    // first 32 frame positions are the number of bits for each subband
    for(n_band=0; n_band < BANDSIZE; n_band++)
    {
        pFRAME1[cnt_FRAME_fill++] = BSPL[n_band];
        total_bit_leng += 4;
    }

    // indices of scalefactor table
    for(n_band=0; n_band < BANDSIZE; n_band++)
    {
        scf_ind=62;
        if(BSCF[n_band] > 0)
        {
            // Store the scf index only
            while(scf[n_band] != table_scf[scf_ind])
            {
                scf_ind--;
            }
            pFRAME1[cnt_FRAME_fill++] = scf_ind;
            total_bit_leng += 6;
        }
    }

    // following frame positions own the bits of quantized subbands
    for(sample=0; sample<12; sample++)
    {
        for(n_band=0; n_band < BANDSIZE; n_band++)
        {
            N=BSPL[n_band];   // determine number of required bits in subband
#ifdef DEBUG
                printf("%d bits used in subband %d\n", N, n_band);
#endif
            if(N > 0)
            {   // quantize if bits are available 
                number = floor( (S[n_band][sample]/(scf[n_band]*exp2LUT[BSPL[n_band]-2])) + 0.5 );
                pFRAME1[cnt_FRAME_fill++] = number;
                total_bit_leng += N;
            }
        }
    }
}
