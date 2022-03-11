//----------------------------------------------------------------------------
// Description       find tonal and non-tonal masker
// Author:           Kim Radmacher
// Created on:
//----------------------------------------------------------------------------

#include "init.h"
#include <math.h>

void find_tonals(void)
{
    float manti,logM, power;
    short expo;
    int tmp_INT;
    short nTon_list_index[25] = {1,2,3,4,5,6,7,9,10,12,14,17,19,22,26,31,37,45,52,62,74,90,108,140,184};

    // reset list of tonals
    for(ton_ind=0; ton_ind<25; ton_ind++)
    {
        Ton_list.power[ton_ind] = MIN_POWER;
        nTon_list.power[ton_ind] = MIN_POWER;
        Ton_list.index[ton_ind] = 0;
        nTon_list.index[ton_ind] = 0;
    }
    for(ki=0;ki<NFFT/2;ki++)
    {
        flag[ki] = NA;
    }

    ton_ind = 0;

    for(ki=2; ki<=250; ki++) // according to MPEG ISO
    {
        // local maximum
        if (magnitude_dB[ki]>magnitude_dB[ki-1] && magnitude_dB[ki]>=magnitude_dB[ki+1])
        {
            if (ki<63)
            {
                if (magnitude_dB[ki]-magnitude_dB[ki-2]>=7 && magnitude_dB[ki]-magnitude_dB[ki+2]>=7)
                {
                    Ton_list.index[ton_ind] = ki;
                    flag[ki] = TONAL;
                    Ton_list.power[ton_ind] = magnitude[ki - 1] + magnitude[ki] + magnitude[ki + 1];    // sum non-log power
                    ton_ind++;
                }
            } 
            else if (ki>64 && ki<=128)
            {
                if (magnitude_dB[ki]-magnitude_dB[ki-3]>=7 && magnitude_dB[ki]-magnitude_dB[ki-2]>=7 && magnitude_dB[ki]-magnitude_dB[ki+2]>=7 && magnitude_dB[ki]-magnitude_dB[ki+3]>=7)
                {
                    Ton_list.index[ton_ind] = ki;
                    flag[ki] = TONAL;
                    Ton_list.power[ton_ind] = magnitude[ki - 1] + magnitude[ki] + magnitude[ki + 1];    // sum non-log power
                    ton_ind++;
                }
            }
            else if (ki>=128 && ki<=250)
            {
                if (magnitude_dB[ki]-magnitude_dB[ki-6]>=7 && magnitude_dB[ki]-magnitude_dB[ki-5]>=7 && magnitude_dB[ki]-magnitude_dB[ki-4]>=7 && magnitude_dB[ki]-magnitude_dB[ki-3]>=7 && magnitude_dB[ki]-magnitude_dB[ki-2]>=7 && magnitude_dB[ki]-magnitude_dB[ki+2]>=7 && magnitude_dB[ki]-magnitude_dB[ki+3]>=7 && magnitude_dB[ki]-magnitude_dB[ki+4]>=7 && magnitude_dB[ki]-magnitude_dB[ki+5]>=7 && magnitude_dB[ki]-magnitude_dB[ki+6]>=7)
                {
                   Ton_list.index[ton_ind] = ki;
                   flag[ki] = TONAL;
                   Ton_list.power[ton_ind] = magnitude[ki - 1] + magnitude[ki] + magnitude[ki + 1];    // sum non-log power
                   ton_ind++;
                }
            }
        }
    }
    
    Ton_list_leng=ton_ind;    // take number of found tonal masker
    // log tonal power
    for(ton_ind=0; ton_ind<Ton_list_leng; ton_ind++)
    {
        tmp_INT = (*(int*)&Ton_list.power[ton_ind]);
        manti = (tmp_INT & 0x007fffff)/8388608.0;
        logM = logLuT[(short)(((manti*1000)))];
        expo = ((tmp_INT & 0x7f800000)>>23);
        Ton_list.power[ton_ind] = ((logM + (float)((expo-127)*log2value)) * constant_factor_log_to_10log10);
    }

    // for each bark band / CB (1...25 values for 24 bands)
    for (nton_ind=0; nton_ind < 25; nton_ind++)
    {    
        // calc power for non tonal components
        // static non tonal masker are always at same position f_bark
        power  = MIN_POWER; // init. to minimal power
        for (ki = CB_f[nton_ind]-1; ki <= (CB_f[nton_ind+1]-2); ki++)
        {
            // get k indices for bark band
            if (flag[ki] == NA)
            {    // checke for "free" masker
                power += magnitude[ki];
                flag[ki] = 13;   // k index must not come up again, so we use 13 for anything else than 0,TONAL or NTONAL
            }
        }
        
        index_nTon = nTon_list_index[nton_ind]-1; //LTq_f[index_nTon]-1;

        if (flag[index_nTon] == TONAL)
        {
            index_nTon++; // two tonals must not be side by side
        }

        nTon_list.index[nton_ind] = index_nTon;
        // log non tonal power
        tmp_INT = (*(int*)&power);
        manti = (tmp_INT & 0x007fffff)/8388608.0;
        logM = logLuT[(short)(((manti*1000)))];
        expo = ((tmp_INT & 0x7f800000)>>23);
        nTon_list.power[nton_ind] = ((logM + (float)((expo-127)*log2value)) * constant_factor_log_to_10log10);
        flag[index_nTon] = NTONAL;
    }
}
