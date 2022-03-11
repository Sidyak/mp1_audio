//----------------------------------------------------------------------------
// Description       create individual masking threshold
// Author:           Kim Radmacher
// Created on:       28.01.2015
//----------------------------------------------------------------------------

#include "init.h"

void indiv_mask_th(void)
{
    // 6. calculation of local threshold LTtm, LTnm
    short mask_ind, PL_i;
    float dz, zj, av_tm, av_nm, vf, PL;

    for(mask_ind=0;mask_ind < 102;mask_ind++)
    {
        for(ton_ind=0;ton_ind < 25;ton_ind++)
        {
            LTtm[ton_ind][mask_ind] = MIN_POWER;
            LTnm[ton_ind][mask_ind] = MIN_POWER;
        }
    }

    // for all (102) freq. indices each 24 bark bands
    for (ton_ind=0; ton_ind<25; ton_ind++)
    {
        if(Ton_list.power[ton_ind] > MIN_POWER)
        {
            zj = LTq_B[Map[Ton_list.index[ton_ind]]-1]; // crit. band in order to freq. or tonal index k
            av_tm = -1.525-0.275 * zj - 4.5;
            for (mask_ind=0; mask_ind < 102; mask_ind++)
            {
                dz = LTq_B[mask_ind] - zj;       // diff. of all bark bands to the masker at position j
                if (dz >= -3 && dz < -1)
                {
                    vf = 17*(dz+1)-(0.4*magnitude_dB[Ton_list.index[ton_ind]]+6); // calc. mask function vf (corresponds to matrix B with slopes S1,S2...example U. U. Zoelzer)
                    LTtm[ton_ind][mask_ind] = Ton_list.power[ton_ind] + av_tm + vf;
                }
                else if (dz >= -1 && dz < 0)
                {
                    vf = (0.4*magnitude_dB[Ton_list.index[ton_ind]]+6)*dz;// calc. mask function vf (corresponds to matrix B with slopes S1,S2...example U. Zoelzer)
                    LTtm[ton_ind][mask_ind] = Ton_list.power[ton_ind] + av_tm + vf;
                }
                else if  (dz >= 0 && dz < 1)
                {
                    vf = -17*dz;// calc. mask function vf (corresponds to matrix B with slopes S1,S2...example U. Zoelzer)
                    LTtm[ton_ind][mask_ind] = Ton_list.power[ton_ind] + av_tm + vf;
                }
                else if  (dz >= 1 && dz < 8)
                {
                    vf = -(dz -1)*(17-0.15*magnitude_dB[Ton_list.index[ton_ind]])-17;// calc. mask function vf (corresponds to matrix B with slopes S1,S2...example U. Zoelzer)
                    LTtm[ton_ind][mask_ind] = Ton_list.power[ton_ind] + av_tm + vf;
                }
            }
        }
    }

    for (nton_ind=0; nton_ind<25; nton_ind++)
    {
        PL = nTon_list.power[nton_ind];
        PL_i = nTon_list.index[nton_ind];
        zj = LTq_B[Map[PL_i]-1]; // krit. band bezogen auf freq. indices
        av_nm = -1.525 - 0.175 * zj - 0.5;
        for (mask_ind=0;mask_ind<102;mask_ind++)
        {
            dz = LTq_B[mask_ind] - zj;  // diff. in Bark zum maskierer
            if (dz >= -3 && dz < -1)
            {
                vf = 17*(dz+1)-(0.4*magnitude_dB[PL_i]+6);  // calc. mask function vf (corresponds to matrix B with slopes S1,S2...example U. Zoelzer)
                LTnm[nton_ind][mask_ind] = PL + av_nm + vf;
            }
            else if( dz >= -1 && dz < 0)
            {
                vf = (0.4*magnitude_dB[PL_i]+6)*dz; // calc. mask function vf (corresponds to matrix B with slopes S1,S2...example U. Zoelzer)
                LTnm[nton_ind][mask_ind] = PL + av_nm + vf;
            }
            else if( dz >= 0 && dz < 1)
            {
                vf = -17*dz;    // calc. mask function vf (corresponds to matrix B with slopes S1,S2...example U. Zoelzer)
                LTnm[nton_ind][mask_ind] = PL + av_nm + vf;
            }
            else if( dz >= 1 && dz < 8)
            {
                vf = -(dz -1)*(17-0.15*magnitude_dB[PL_i])-17;  // calc. mask function vf (corresponds to matrix B with slopes S1,S2...example U. Zoelzer)
                LTnm[nton_ind][mask_ind] = PL + av_nm + vf;
            }
        }
    }
}
