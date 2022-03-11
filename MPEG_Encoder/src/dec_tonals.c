//----------------------------------------------------------------------------
// Description         reduce tonal and non-tonal masker
// Author:             Kim Radmacher
// Created on:
//----------------------------------------------------------------------------

#include "init.h"
#include <math.h>

void dec_tonals(int bitrate){
    // 5. Reduction of maskter (also computational reduction)
    // only determine the global masking threshold in case maskter >= LTq_dB
    // also apply abs. hearing threshold

    if(bitrate >= 96)
    {
        for(ton_ind=0; ton_ind < Ton_list_leng; ton_ind++)
        {
            // masker remains only if abs. hearing thresh. is reached otherwise set to min. power
            if (Ton_list.power[ton_ind] < LTq_dB[Map[Ton_list.index[ton_ind]]-1])
            {
                Ton_list.power[ton_ind] = MIN_POWER;
            }
        }

        for(nton_ind=0 ;nton_ind < 25; nton_ind++)
        {
            // masker remains only if abs. hearing thresh. is reached otherwise set to min. power
            if (nTon_list.power[nton_ind] < LTq_dB[Map[nTon_list.index[nton_ind]]-1])
            {
                nTon_list.power[nton_ind]=MIN_POWER;
            }
        }
    }
    else
    {
        for(ton_ind=0; ton_ind < Ton_list_leng; ton_ind++)
        {
            // masker remains only if abs. hearing thresh. is reached otherwise set to min. power
            if (Ton_list.power[ton_ind] < (LTq_dB[Map[Ton_list.index[ton_ind]]-1]) +12)
            {
                Ton_list.power[ton_ind] = MIN_POWER;
            }
        }

        for(nton_ind=0; nton_ind < 25; nton_ind++)
        {
            if (nTon_list.power[nton_ind] < (LTq_dB[Map[nTon_list.index[nton_ind]]-1]) +12)
            {
                nTon_list.power[nton_ind] = MIN_POWER;
            }
        }
    }

    // get rid of weaker tonal masker of two masker which are close together (< 0.5 bark)
    if (Ton_list_leng > 1)
    {
        for(ton_ind=0; ton_ind <= (Ton_list_leng-1); ton_ind++)
        {
            if ( (LTq_B[Map[Ton_list.index[ton_ind+1]]-1] - LTq_B[Map[Ton_list.index[ton_ind]]-1]) < 0.5)
            {     
                if (Ton_list.power[ton_ind] < Ton_list.power[ton_ind+1])
                { 
                    Ton_list.power[ton_ind] = MIN_POWER;    // indices & power
                }
                else
                {
                    Ton_list.power[ton_ind+1] = MIN_POWER;    // indices & power
                }
            }
        }
    }
}
