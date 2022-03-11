//----------------------------------------------------------------------------
// Description        polyphase synthesis filterbank
// Author:            Kim Radmacher
// Created on:        18.12.2014
//----------------------------------------------------------------------------

#include "init.h"

void calc_polyphase_synthese_fb(void)
{
    short i_12_32 = 0;
    cnt_out = 0;
    count_12_synthese = 0;
    
    for(i_12_32=0; i_12_32<BUFLEN; i_12_32++)
    {
    //----------------------------------  sample 0 ---------------------------------
        if (cnt_out == 0) {
            cnt_out++;
            //Interpolation
            INT_y1 = fir_filter( H_filt_320_delays, // compute polyphase FIR out and save
                    E0, N_delays_H_filt_320_delays, pOut1[count_12_synthese*64+0]);
            INT_y2 = fir_filter( H_filt_3232_delays, // compute polyphase FIR out and save
                    E32, N_delays_H_filt_3232_delays, out_delay[32]);
            out_delay[32]=pOut1[count_12_synthese*64+32];
            INT_y=INT_y1+INT_y2;
            //sum=sum+y1_part_fb[32]+y1_part_fb[0];
        }
    //----------------------------------  sample 1 ---------------------------------
        else if (cnt_out == 1) {
            cnt_out++;
            //Interpolation
            INT_y1 = fir_filter( H_filt_321_delays, // compute polyphase FIR out and save
                    E1, N_delays_H_filt_321_delays, pOut1[count_12_synthese*64+1]);
            INT_y2 = fir_filter( H_filt_3233_delays, // compute polyphase FIR out and save
                    E33, N_delays_H_filt_3233_delays, out_delay[33]);
            out_delay[33]=pOut1[count_12_synthese*64+33];
            INT_y=INT_y1+INT_y2;
            //sum=sum+y1_part_fb[31]+y1_part_fb[63];
        }
    //----------------------------------  sample 2 ---------------------------------
        else if (cnt_out == 2) {
            cnt_out++;
            //Interpolation
            INT_y1 = fir_filter( H_filt_322_delays, // compute polyphase FIR out and save
                    E2, N_delays_H_filt_322_delays, pOut1[count_12_synthese*64+2]);
            INT_y2 = fir_filter( H_filt_3234_delays, // compute polyphase FIR out and save
                    E34, N_delays_H_filt_3234_delays, out_delay[34]);
            out_delay[34]=pOut1[count_12_synthese*64+34];
            INT_y=INT_y1+INT_y2;
            //sum=sum+y1_part_fb[30]+y1_part_fb[62];
        }
    //----------------------------------  sample 3 ---------------------------------
        else if (cnt_out == 3) {
            cnt_out++;
            //Interpolation
            INT_y1 = fir_filter( H_filt_323_delays, // compute polyphase FIR out and save
                    E3, N_delays_H_filt_323_delays, pOut1[count_12_synthese*64+3]);
            INT_y2 = fir_filter( H_filt_3235_delays, // compute polyphase FIR out and save
                    E35, N_delays_H_filt_3235_delays, out_delay[35]);
            out_delay[35]=pOut1[count_12_synthese*64+35];
            INT_y=INT_y1+INT_y2;
            //sum=sum+y1_part_fb[29]+y1_part_fb[61];
        }
    //----------------------------------  sample 4 ---------------------------------
        else if (cnt_out == 4) {
            cnt_out++;
            //Interpolation
            INT_y1 = fir_filter( H_filt_324_delays, // compute polyphase FIR out and save
                    E4, N_delays_H_filt_324_delays, pOut1[count_12_synthese*64+4]);
            INT_y2 = fir_filter( H_filt_3236_delays, // compute polyphase FIR out and save
                    E36, N_delays_H_filt_3236_delays, out_delay[36]);
            out_delay[36]=pOut1[count_12_synthese*64+36];
            INT_y=INT_y1+INT_y2;
            //sum=sum+y1_part_fb[28]+y1_part_fb[60];
        }
    //----------------------------------  sample 5 ---------------------------------
        else if (cnt_out == 5) {
            cnt_out++;
            //Interpolation
            INT_y1 = fir_filter( H_filt_325_delays, // compute polyphase FIR out and save
                    E5, N_delays_H_filt_325_delays, pOut1[count_12_synthese*64+5]);
            INT_y2 = fir_filter( H_filt_3237_delays, // compute polyphase FIR out and save
                    E37, N_delays_H_filt_3237_delays, out_delay[37]);
            out_delay[37]=pOut1[count_12_synthese*64+37];
            INT_y=INT_y1+INT_y2;
            //sum=sum+y1_part_fb[27]+y1_part_fb[59];
        }
    //----------------------------------  sample 6 ---------------------------------
        else if (cnt_out == 6) {
            cnt_out++;
            //Interpolation
            INT_y1 = fir_filter( H_filt_326_delays, // compute polyphase FIR out and save
                    E6, N_delays_H_filt_326_delays, pOut1[count_12_synthese*64+6]);
            INT_y2 = fir_filter( H_filt_3238_delays, // compute polyphase FIR out and save
                    E38, N_delays_H_filt_3238_delays, out_delay[38]);
            out_delay[38]=pOut1[count_12_synthese*64+38];
            INT_y=INT_y1+INT_y2;
            //sum=sum+y1_part_fb[26]+y1_part_fb[58];
        }
    //----------------------------------  sample 7 ---------------------------------
        else if (cnt_out == 7) {
            cnt_out++;
            //Interpolation
            INT_y1 = fir_filter( H_filt_327_delays, // compute polyphase FIR out and save
                    E7, N_delays_H_filt_327_delays, pOut1[count_12_synthese*64+7]);
            INT_y2 = fir_filter( H_filt_3239_delays, // compute polyphase FIR out and save
                    E39, N_delays_H_filt_3239_delays, out_delay[39]);
            out_delay[39]=pOut1[count_12_synthese*64+39];
            INT_y=INT_y1+INT_y2;
            //sum=sum+y1_part_fb[25]+y1_part_fb[57];
        }
    //----------------------------------  sample 8 ---------------------------------
        else if (cnt_out == 8) {
            cnt_out++;
            //Interpolation
            INT_y1 = fir_filter( H_filt_328_delays, // compute polyphase FIR out and save
                    E8, N_delays_H_filt_328_delays, pOut1[count_12_synthese*64+8]);
            INT_y2 = fir_filter( H_filt_3240_delays, // compute polyphase FIR out and save
                    E40, N_delays_H_filt_3240_delays, out_delay[40]);
            out_delay[40]=pOut1[count_12_synthese*64+40];
            INT_y=INT_y1+INT_y2;
            //sum=sum+y1_part_fb[24]+y1_part_fb[56];
        }
    //----------------------------------  sample 9 ---------------------------------
        else if (cnt_out == 9) {
            cnt_out++;
            //Interpolation
            INT_y1 = fir_filter( H_filt_329_delays, // compute polyphase FIR out and save
                    E9, N_delays_H_filt_329_delays, pOut1[count_12_synthese*64+9]);
            INT_y2 = fir_filter( H_filt_3241_delays, // compute polyphase FIR out and save
                    E41, N_delays_H_filt_3241_delays, out_delay[41]);
            out_delay[41]=pOut1[count_12_synthese*64+41];
            INT_y=INT_y1+INT_y2;
            //sum=sum+y1_part_fb[23]+y1_part_fb[55];
        }
    //----------------------------------  sample 10 ---------------------------------
        else if (cnt_out == 10) {
            cnt_out++;
            //Interpolation
            INT_y1 = fir_filter( H_filt_3210_delays, // compute polyphase FIR out and save
                    E10, N_delays_H_filt_3210_delays, pOut1[count_12_synthese*64+10]);
            INT_y2 = fir_filter( H_filt_3242_delays, // compute polyphase FIR out and save
                    E42, N_delays_H_filt_3242_delays, out_delay[42]);
            out_delay[42]=pOut1[count_12_synthese*64+42];
            INT_y=INT_y1+INT_y2;
            //sum=sum+y1_part_fb[22]+y1_part_fb[54];
        }
    //----------------------------------  sample 11 ---------------------------------
        else if (cnt_out == 11) {
            cnt_out++;
            //Interpolation
            INT_y1 = fir_filter( H_filt_3211_delays, // compute polyphase FIR out and save
                    E11, N_delays_H_filt_3211_delays, pOut1[count_12_synthese*64+11]);
            INT_y2 = fir_filter( H_filt_3243_delays, // compute polyphase FIR out and save
                    E43, N_delays_H_filt_3243_delays, out_delay[43]);
            out_delay[43]=pOut1[count_12_synthese*64+43];
            INT_y=INT_y1+INT_y2;
            //sum=sum+y1_part_fb[21]+y1_part_fb[53];
        }
    //----------------------------------  sample 12 ---------------------------------
        else if (cnt_out == 12) {
            cnt_out++;
            //Interpolation
            INT_y1 = fir_filter( H_filt_3212_delays, // compute polyphase FIR out and save
                    E12, N_delays_H_filt_3212_delays, pOut1[count_12_synthese*64+12]);
            INT_y2 = fir_filter( H_filt_3244_delays, // compute polyphase FIR out and save
                    E44, N_delays_H_filt_3244_delays, out_delay[44]);
            out_delay[44]=pOut1[count_12_synthese*64+44];
            INT_y=INT_y1+INT_y2;
            //sum=sum+y1_part_fb[20]+y1_part_fb[52];
        }
    //----------------------------------  sample 13 ---------------------------------
        else if (cnt_out == 13) {
            cnt_out++;
            //Interpolation
            INT_y1 = fir_filter( H_filt_3213_delays, // compute polyphase FIR out and save
                    E13, N_delays_H_filt_3213_delays, pOut1[count_12_synthese*64+13]);
            INT_y2 = fir_filter( H_filt_3245_delays, // compute polyphase FIR out and save
                    E45, N_delays_H_filt_3245_delays, out_delay[45]);
            out_delay[45]=pOut1[count_12_synthese*64+45];
            INT_y=INT_y1+INT_y2;
            //sum=sum+y1_part_fb[19]+y1_part_fb[51];
        }
    //----------------------------------  sample 14 ---------------------------------
        else if (cnt_out == 14) {
            cnt_out++;
            //Interpolation
            INT_y1 = fir_filter( H_filt_3214_delays, // compute polyphase FIR out and save
                    E14, N_delays_H_filt_3214_delays, pOut1[count_12_synthese*64+14]);
            INT_y2 = fir_filter( H_filt_3246_delays, // compute polyphase FIR out and save
                    E46, N_delays_H_filt_3246_delays, out_delay[46]);
            out_delay[46]=pOut1[count_12_synthese*64+46];
            INT_y=INT_y1+INT_y2;
            //sum=sum+y1_part_fb[18]+y1_part_fb[50];
        }
    //----------------------------------  sample 15 ---------------------------------
        else if (cnt_out == 15) {
            cnt_out++;
            //Interpolation
            INT_y1 = fir_filter( H_filt_3215_delays, // compute polyphase FIR out and save
                    E15, N_delays_H_filt_3215_delays, pOut1[count_12_synthese*64+15]);
            INT_y2 = fir_filter( H_filt_3247_delays, // compute polyphase FIR out and save
                    E47, N_delays_H_filt_3247_delays, out_delay[47]);
            out_delay[47]=pOut1[count_12_synthese*64+47];
            INT_y=INT_y1+INT_y2;
            //sum=sum+y1_part_fb[17]+y1_part_fb[49];
        }
    //----------------------------------  sample 16 ---------------------------------
        else if (cnt_out == 16) {
            cnt_out++;
            //Interpolation
            INT_y1 = fir_filter( H_filt_3216_delays, // compute polyphase FIR out and save
                    E16, N_delays_H_filt_3216_delays, pOut1[count_12_synthese*64+16]);
            INT_y2 = fir_filter( H_filt_3248_delays, // compute polyphase FIR out and save
                    E48, N_delays_H_filt_3248_delays, out_delay[48]);
            out_delay[48]=pOut1[count_12_synthese*64+48];
            INT_y=INT_y1+INT_y2;
            //sum=sum+y1_part_fb[16]+y1_part_fb[48];
        }
    //----------------------------------  sample 17 ---------------------------------
        else if (cnt_out == 17) {
            cnt_out++;
            //Interpolation
            INT_y1 = fir_filter( H_filt_3217_delays, // compute polyphase FIR out and save
                    E17, N_delays_H_filt_3217_delays, pOut1[count_12_synthese*64+17]);
            INT_y2 = fir_filter( H_filt_3249_delays, // compute polyphase FIR out and save
                    E49, N_delays_H_filt_3249_delays, out_delay[49]);
            out_delay[49]=pOut1[count_12_synthese*64+49];
            INT_y=INT_y1+INT_y2;
            //sum=sum+y1_part_fb[15]+y1_part_fb[47];
        }
    //----------------------------------  sample 18 ---------------------------------
        else if (cnt_out == 18) {
            cnt_out++;
            //Interpolation
            INT_y1 = fir_filter( H_filt_3218_delays, // compute polyphase FIR out and save
                    E18, N_delays_H_filt_3218_delays, pOut1[count_12_synthese*64+18]);
            INT_y2 = fir_filter( H_filt_3250_delays, // compute polyphase FIR out and save
                    E50, N_delays_H_filt_3250_delays, out_delay[50]);
            out_delay[50]=pOut1[count_12_synthese*64+50];
            INT_y=INT_y1+INT_y2;
            //sum=sum+y1_part_fb[14]+y1_part_fb[46];
        }
    //----------------------------------  sample 19 ---------------------------------
        else if (cnt_out == 19) {
            cnt_out++;
            //Interpolation
            INT_y1 = fir_filter( H_filt_3219_delays, // compute polyphase FIR out and save
                    E19, N_delays_H_filt_3219_delays, pOut1[count_12_synthese*64+19]);
            INT_y2 = fir_filter( H_filt_3251_delays, // compute polyphase FIR out and save
                    E51, N_delays_H_filt_3251_delays, out_delay[51]);
            out_delay[51]=pOut1[count_12_synthese*64+51];
            INT_y=INT_y1+INT_y2;
            //sum=sum+y1_part_fb[13]+y1_part_fb[45];
        }
    //----------------------------------  sample 20 ---------------------------------
        else if (cnt_out == 20) {
            cnt_out++;
            //Interpolation
            INT_y1 = fir_filter( H_filt_3220_delays, // compute polyphase FIR out and save
                    E20, N_delays_H_filt_3220_delays, pOut1[count_12_synthese*64+20]);
            INT_y2 = fir_filter( H_filt_3252_delays, // compute polyphase FIR out and save
                    E52, N_delays_H_filt_3252_delays, out_delay[52]);
            out_delay[52]=pOut1[count_12_synthese*64+52];
            INT_y=INT_y1+INT_y2;
            //sum=sum+y1_part_fb[12]+y1_part_fb[44];
        }
    //----------------------------------  sample 21 ---------------------------------
        else if (cnt_out == 21) {
            cnt_out++;
            //Interpolation
            INT_y1 = fir_filter( H_filt_3221_delays, // compute polyphase FIR out and save
                    E21, N_delays_H_filt_3221_delays, pOut1[count_12_synthese*64+21]);
            INT_y2 = fir_filter( H_filt_3253_delays, // compute polyphase FIR out and save
                    E53, N_delays_H_filt_3253_delays, out_delay[53]);
            out_delay[53]=pOut1[count_12_synthese*64+53];
            INT_y=INT_y1+INT_y2;
            //sum=sum+y1_part_fb[11]+y1_part_fb[43];
        }
    //----------------------------------  sample 22 ---------------------------------
        else if (cnt_out == 22) {
            cnt_out++;
            //Interpolation
            INT_y1 = fir_filter( H_filt_3222_delays, // compute polyphase FIR out and save
                    E22, N_delays_H_filt_3222_delays, pOut1[count_12_synthese*64+22]);
            INT_y2 = fir_filter( H_filt_3254_delays, // compute polyphase FIR out and save
                    E54, N_delays_H_filt_3254_delays, out_delay[54]);
            out_delay[54]=pOut1[count_12_synthese*64+54];
            INT_y=INT_y1+INT_y2;
            //sum=sum+y1_part_fb[10]+y1_part_fb[42];
        }
    //----------------------------------  sample 23 ---------------------------------
        else if (cnt_out == 23) {
            cnt_out++;
            //Interpolation
            INT_y1 = fir_filter( H_filt_3223_delays, // compute polyphase FIR out and save
                    E23, N_delays_H_filt_3223_delays, pOut1[count_12_synthese*64+23]);
            INT_y2 = fir_filter( H_filt_3255_delays, // compute polyphase FIR out and save
                    E55, N_delays_H_filt_3255_delays, out_delay[55]);
            out_delay[55]=pOut1[count_12_synthese*64+55];
            INT_y=INT_y1+INT_y2;
            //sum=sum+y1_part_fb[9]+y1_part_fb[41];
        }
    //----------------------------------  sample 24 ---------------------------------
        else if (cnt_out == 24) {
            cnt_out++;
            //Interpolation
            INT_y1 = fir_filter( H_filt_3224_delays, // compute polyphase FIR out and save
                    E24, N_delays_H_filt_3224_delays, pOut1[count_12_synthese*64+24]);
            INT_y2 = fir_filter( H_filt_3256_delays, // compute polyphase FIR out and save
                    E56, N_delays_H_filt_3256_delays, out_delay[56]);
            out_delay[56]=pOut1[count_12_synthese*64+56];
            INT_y=INT_y1+INT_y2;
            //sum=sum+y1_part_fb[8]+y1_part_fb[40];
        }
    //----------------------------------  sample 25 ---------------------------------
        else if (cnt_out == 25) {
            cnt_out++;
            //Interpolation
            INT_y1 = fir_filter( H_filt_3225_delays, // compute polyphase FIR out and save
                    E25, N_delays_H_filt_3225_delays, pOut1[count_12_synthese*64+25]);
            INT_y2 = fir_filter( H_filt_3257_delays, // compute polyphase FIR out and save
                    E57, N_delays_H_filt_3257_delays, out_delay[57]);
            out_delay[57]=pOut1[count_12_synthese*64+57];
            INT_y=INT_y1+INT_y2;
            //sum=sum+y1_part_fb[7]+y1_part_fb[39];
        }
    //----------------------------------  sample 26 ---------------------------------
        else if (cnt_out == 26) {
            cnt_out++;
            //Interpolation
            INT_y1 = fir_filter( H_filt_3226_delays, // compute polyphase FIR out and save
                    E26, N_delays_H_filt_3226_delays, pOut1[count_12_synthese*64+26]);
            INT_y2 = fir_filter( H_filt_3258_delays, // compute polyphase FIR out and save
                    E58, N_delays_H_filt_3258_delays, out_delay[58]);
            out_delay[58]=pOut1[count_12_synthese*64+58];
            INT_y=INT_y1+INT_y2;
            //sum=sum+y1_part_fb[6]+y1_part_fb[38];
        }
    //----------------------------------  sample 27 ---------------------------------
        else if (cnt_out == 27) {
            cnt_out++;
            //Interpolation
            INT_y1 = fir_filter( H_filt_3227_delays, // compute polyphase FIR out and save
                    E27, N_delays_H_filt_3227_delays, pOut1[count_12_synthese*64+27]);
            INT_y2 = fir_filter( H_filt_3259_delays, // compute polyphase FIR out and save
                    E59, N_delays_H_filt_3259_delays, out_delay[59]);
            out_delay[59]=pOut1[count_12_synthese*64+59];
            INT_y=INT_y1+INT_y2;
            //sum=sum+y1_part_fb[5]+y1_part_fb[37];
        }
    //----------------------------------  sample 28 ---------------------------------
        else if (cnt_out == 28) {
            cnt_out++;
            //Interpolation
            INT_y1 = fir_filter( H_filt_3228_delays, // compute polyphase FIR out and save
                    E28, N_delays_H_filt_3228_delays, pOut1[count_12_synthese*64+28]);
            INT_y2 = fir_filter( H_filt_3260_delays, // compute polyphase FIR out and save
                    E60, N_delays_H_filt_3260_delays, out_delay[60]);
            out_delay[60]=pOut1[count_12_synthese*64+60];
            INT_y=INT_y1+INT_y2;
            //sum=sum+y1_part_fb[4]+y1_part_fb[36];
        }
    //----------------------------------  sample 29 ---------------------------------
        else if (cnt_out == 29) {
            cnt_out++;
            //Interpolation
            INT_y1 = fir_filter( H_filt_3229_delays, // compute polyphase FIR out and save
                    E29, N_delays_H_filt_3229_delays, pOut1[count_12_synthese*64+29]);
            INT_y2 = fir_filter( H_filt_3261_delays, // compute polyphase FIR out and save
                    E61, N_delays_H_filt_3261_delays, out_delay[61]);
            out_delay[61]=pOut1[count_12_synthese*64+61];
            INT_y=INT_y1+INT_y2;
            //sum=sum+y1_part_fb[3]+y1_part_fb[35];
        }
    //----------------------------------  sample 30 ---------------------------------
        else if (cnt_out == 30) {
            cnt_out++;
            //Interpolation
            INT_y1 = fir_filter( H_filt_3230_delays, // compute polyphase FIR out and save
                    E30, N_delays_H_filt_3230_delays, pOut1[count_12_synthese*64+30]);
            INT_y2 = fir_filter( H_filt_3262_delays, // compute polyphase FIR out and save
                    E62, N_delays_H_filt_3262_delays, out_delay[62]);
            out_delay[62]=pOut1[count_12_synthese*64+62];
            INT_y=INT_y1+INT_y2;
            //sum=sum+y1_part_fb[2]+y1_part_fb[34];
        }
    //----------------------------------  sample 31 ---------------------------------
        else if (cnt_out == 31) {
            //Interpolation
            INT_y1 = fir_filter( H_filt_3231_delays, // compute polyphase FIR out and save
                    E31, N_delays_H_filt_3231_delays, pOut1[count_12_synthese*64+31]);
            INT_y2 = fir_filter( H_filt_3263_delays, // compute polyphase FIR out and save
                    E63, N_delays_H_filt_3263_delays, out_delay[63]);
            out_delay[63]=pOut1[count_12_synthese*64+63];
            INT_y=INT_y1+INT_y2;

            cnt_out=0;
            count_12_synthese++;
        }
#if 0
        AIC23_data.channel[LEFT]  = (short)(INT_y*32767*32);
        AIC23_data.channel[RIGHT] = (short)(INT_y*32767*32);
        table_Xmt[i_12_32]=AIC23_data.both;
#endif
        buffer[i_12_32] = (short)(INT_y*32/**32767*32*/);
    }
}
