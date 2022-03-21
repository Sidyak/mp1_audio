//----------------------------------------------------------------------------
// Description       polyphase analysis filterbank
// Author:           Kim Radmacher
// Created on:       18.12.2014
//
// Copyright (C) 2022 Kim Radmacher
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
// express or implied.
// See the License for the specific language governing permissions
// and limitations under the License.
//----------------------------------------------------------------------------

#include "init.h"

int calc_polyphase_fb(int16_t *input, int channels)
{
    short i_12_32 = 0;

    for(i_12_32=0; i_12_32<BUFLEN; i_12_32++)
    {
        // Read audio inputs
        if(channels == 1)
        {
            inL = (float)input[i_12_32]/32767.0;
            inR = inL;
        }
        else if(channels == 2)
        {
            // interleaved left right channel
            inL = (float)input[i_12_32*2]/32767.0;
            inR = (float)input[i_12_32*2+1]/32767.0;
        }
        else
        {
            return -1;
        }

        inL = (inL+inR)/2;            // MS-Signal: M=(L+R)/2 S=(L-R)/2

        pWork_fft[BUFLEN-1-i_12_32] = inL;    // FFT input buffer
        //----------------------------------  sample 0 ---------------------------------
            if (count_poly == 0) {
                count_poly++;
                //Decimation
                pWork_fb[0+count_12*64] = fir_filter( H_filt_320_delays_dec,   // compute polyphase FIR out and save
                        E0, N_delays_H_filt_320_delays, inL);
                pWork_fb[32+count_12*64] = fir_filter( H_filt_3232_delays_dec, // compute polyphase FIR out and save
                        E32, N_delays_H_filt_3232_delays, delay[32]);
                delay[32] = inL;                         // now update delay
            }
        //----------------------------------  sample 1 ---------------------------------
            else if (count_poly == 1) {
                count_poly++;
                //Decimation
                pWork_fb[63+count_12*64] = fir_filter( H_filt_3263_delays_dec, // compute polyphase FIR out and save
                        E63, N_delays_H_filt_3263_delays, delay[63]);
                pWork_fb[31+count_12*64] = fir_filter( H_filt_3231_delays_dec, // compute polyphase FIR out and save
                        E31, N_delays_H_filt_3231_delays, delay[31]);
                delay[63] = delay[31];                     // now update delay
                delay[31] = inL;                         // now update delay
            }
        //----------------------------------  sample 2 ---------------------------------
            else if (count_poly == 2) {
                count_poly++;
                //Decimation
                pWork_fb[62+count_12*64] = fir_filter( H_filt_3262_delays_dec, // compute polyphase FIR out and save
                        E62, N_delays_H_filt_3262_delays, delay[62]);
                pWork_fb[30+count_12*64] = fir_filter( H_filt_3230_delays_dec, // compute polyphase FIR out and save
                        E30, N_delays_H_filt_3230_delays, delay[30]);
                delay[62] = delay[30];                     // now update delay
                delay[30] = inL;                         // now update delay
            }
        //----------------------------------  sample 3 ---------------------------------
            else if (count_poly == 3) {
                count_poly++;
                //Decimation
                pWork_fb[61+count_12*64] = fir_filter( H_filt_3261_delays_dec, // compute polyphase FIR out and save
                        E61, N_delays_H_filt_3261_delays, delay[61]);
                pWork_fb[29+count_12*64] = fir_filter( H_filt_3229_delays_dec, // compute polyphase FIR out and save
                        E29, N_delays_H_filt_3229_delays, delay[29]);
                delay[61] = delay[29];                     // now update delay
                delay[29] = inL;                         // now update delay
            }
        //----------------------------------  sample 4 ---------------------------------
            else if (count_poly == 4) {
                count_poly++;
                //Decimation
                pWork_fb[60+count_12*64] = fir_filter( H_filt_3260_delays_dec, // compute polyphase FIR out and save
                        E60, N_delays_H_filt_3260_delays, delay[60]);
                pWork_fb[28+count_12*64] = fir_filter( H_filt_3228_delays_dec, // compute polyphase FIR out and save
                        E28, N_delays_H_filt_3228_delays, delay[28]);
                delay[60] = delay[28];                         // now update delay
                delay[28] = inL;                         // now update delay
            }
        //----------------------------------  sample 5 ---------------------------------
            else if (count_poly == 5) {
                count_poly++;
                //Decimation
                pWork_fb[59+count_12*64] = fir_filter( H_filt_3259_delays_dec, // compute polyphase FIR out and save
                        E59, N_delays_H_filt_3259_delays, delay[59]);
                pWork_fb[27+count_12*64] = fir_filter( H_filt_3227_delays_dec, // compute polyphase FIR out and save
                        E27, N_delays_H_filt_3227_delays, delay[27]);
                delay[59] = delay[27];                     // now update delay
                delay[27] = inL;                         // now update delay
                //sum=sum+pWork_fb[27]+pWork_fb[59];
            }
        //----------------------------------  sample 6 ---------------------------------
            else if (count_poly == 6) {
                count_poly++;
                //Decimation
                pWork_fb[58+count_12*64] = fir_filter( H_filt_3258_delays_dec, // compute polyphase FIR out and save
                        E58, N_delays_H_filt_3258_delays, delay[58]);
                pWork_fb[26+count_12*64] = fir_filter( H_filt_3226_delays_dec, // compute polyphase FIR out and save
                        E26, N_delays_H_filt_3226_delays, delay[26]);
                delay[58] = delay[26];                     // now update delay
                delay[26] = inL;                         // now update delay
            }
        //----------------------------------  sample 7 ---------------------------------
            else if (count_poly == 7) {
                count_poly++;
                //Decimation
                pWork_fb[57+count_12*64] = fir_filter( H_filt_3257_delays_dec, // compute polyphase FIR out and save
                        E57, N_delays_H_filt_3257_delays, delay[57]);
                pWork_fb[25+count_12*64] = fir_filter( H_filt_3225_delays_dec, // compute polyphase FIR out and save
                        E25, N_delays_H_filt_3225_delays, delay[25]);
                delay[57] = delay[25];                         // now update delay
                delay[25] = inL;                         // now update delay
            }
        //----------------------------------  sample 8 ---------------------------------
            else if (count_poly == 8) {
                count_poly++;
                //Decimation
                pWork_fb[56+count_12*64] = fir_filter( H_filt_3256_delays_dec, // compute polyphase FIR out and save
                        E56, N_delays_H_filt_3256_delays, delay[56]);
                pWork_fb[24+count_12*64] = fir_filter( H_filt_3224_delays_dec, // compute polyphase FIR out and save
                        E24, N_delays_H_filt_3224_delays, delay[24]);
                delay[56] = delay[24];                         // now update delay
                delay[24] = inL;                         // now update delay
            }
        //----------------------------------  sample 9 ---------------------------------
            else if (count_poly == 9) {
                count_poly++;
                //Decimation
                pWork_fb[55+count_12*64] = fir_filter( H_filt_3255_delays_dec, // compute polyphase FIR out and save
                        E55, N_delays_H_filt_3255_delays, delay[55]);
                pWork_fb[23+count_12*64] = fir_filter( H_filt_3223_delays_dec, // compute polyphase FIR out and save
                        E23, N_delays_H_filt_3223_delays, delay[23]);
                delay[55] = delay[23];                         // now update delay
                delay[23] = inL;                         // now update delay
            }
        //----------------------------------  sample 10 ---------------------------------
            else if (count_poly == 10) {
                count_poly++;
                //Decimation
                pWork_fb[54+count_12*64] = fir_filter( H_filt_3254_delays_dec, // compute polyphase FIR out and save
                        E54, N_delays_H_filt_3254_delays, delay[54]);
                pWork_fb[22+count_12*64] = fir_filter( H_filt_3222_delays_dec, // compute polyphase FIR out and save
                        E22, N_delays_H_filt_3222_delays, delay[22]);
                delay[54] = delay[22];                         // now update delay
                delay[22] = inL;                         // now update delay
            }
        //----------------------------------  sample 11 ---------------------------------
            else if (count_poly == 11) {
                count_poly++;
                //Decimation
                pWork_fb[53+count_12*64] = fir_filter( H_filt_3253_delays_dec, // compute polyphase FIR out and save
                        E53, N_delays_H_filt_3253_delays, delay[53]);
                pWork_fb[21+count_12*64] = fir_filter( H_filt_3221_delays_dec, // compute polyphase FIR out and save
                        E21, N_delays_H_filt_3221_delays, delay[21]);
                delay[53] = delay[21];                         // now update delay
                delay[21] = inL;                         // now update delay
            }
        //----------------------------------  sample 12 ---------------------------------
            else if (count_poly == 12) {
                count_poly++;
                //Decimation
                pWork_fb[52+count_12*64] = fir_filter( H_filt_3252_delays_dec, // compute polyphase FIR out and save
                        E52, N_delays_H_filt_3252_delays, delay[52]);
                pWork_fb[20+count_12*64] = fir_filter( H_filt_3220_delays_dec, // compute polyphase FIR out and save
                        E20, N_delays_H_filt_3220_delays, delay[20]);
                delay[52] = delay[20];                         // now update delay
                delay[20] = inL;                         // now update delay
            }
        //----------------------------------  sample 13 ---------------------------------
            else if (count_poly == 13) {
                count_poly++;
                //Decimation
                pWork_fb[51+count_12*64] = fir_filter( H_filt_3251_delays_dec, // compute polyphase FIR out and save
                        E51, N_delays_H_filt_3251_delays, delay[51]);
                pWork_fb[19+count_12*64] = fir_filter( H_filt_3219_delays_dec, // compute polyphase FIR out and save
                        E19, N_delays_H_filt_3219_delays, delay[19]);
                delay[51] = delay[19];                         // now update delay
                delay[19] = inL;                         // now update delay
            }
        //----------------------------------  sample 14 ---------------------------------
            else if (count_poly == 14) {
                count_poly++;
                //Decimation
                pWork_fb[50+count_12*64] = fir_filter( H_filt_3250_delays_dec, // compute polyphase FIR out and save
                        E50, N_delays_H_filt_3250_delays, delay[50]);
                pWork_fb[18+count_12*64] = fir_filter( H_filt_3218_delays_dec, // compute polyphase FIR out and save
                        E18, N_delays_H_filt_3218_delays, delay[18]);
                delay[50] = delay[18];                         // now update delay
                delay[18] = inL;                         // now update delay
            }
        //----------------------------------  sample 15 ---------------------------------
            else if (count_poly == 15) {
                count_poly++;
                //Decimation
                pWork_fb[49+count_12*64] = fir_filter( H_filt_3249_delays_dec, // compute polyphase FIR out and save
                        E49, N_delays_H_filt_3249_delays, delay[49]);
                pWork_fb[17+count_12*64] = fir_filter( H_filt_3217_delays_dec, // compute polyphase FIR out and save
                        E17, N_delays_H_filt_3217_delays, delay[17]);
                delay[49] = delay[17];                         // now update delay
                delay[17] = inL;                         // now update delay
            }
        //----------------------------------  sample 16 ---------------------------------
            else if (count_poly == 16) {
                count_poly++;
                //Decimation
                pWork_fb[48+count_12*64] = fir_filter( H_filt_3248_delays_dec, // compute polyphase FIR out and save
                        E48, N_delays_H_filt_3248_delays, delay[48]);
                pWork_fb[16+count_12*64] = fir_filter( H_filt_3216_delays_dec, // compute polyphase FIR out and save
                        E16, N_delays_H_filt_3216_delays, delay[16]);
                delay[48] = delay[16];                         // now update delay
                delay[16] = inL;                         // now update delay
            }
        //----------------------------------  sample 17 ---------------------------------
            else if (count_poly == 17) {
                count_poly++;
                //Decimation
                pWork_fb[47+count_12*64] = fir_filter( H_filt_3247_delays_dec, // compute polyphase FIR out and save
                        E47, N_delays_H_filt_3247_delays, delay[47]);
                pWork_fb[15+count_12*64] = fir_filter( H_filt_3215_delays_dec, // compute polyphase FIR out and save
                        E15, N_delays_H_filt_3215_delays, delay[15]);
                delay[47] = delay[15];                         // now update delay
                delay[15] = inL;                         // now update delay
            }
        //----------------------------------  sample 18 ---------------------------------
            else if (count_poly == 18) {
                count_poly++;
                //Decimation
                pWork_fb[46+count_12*64] = fir_filter( H_filt_3246_delays_dec, // compute polyphase FIR out and save
                        E46, N_delays_H_filt_3246_delays, delay[46]);
                pWork_fb[14+count_12*64] = fir_filter( H_filt_3214_delays_dec, // compute polyphase FIR out and save
                        E14, N_delays_H_filt_3214_delays, delay[14]);
                delay[46] = delay[14];                         // now update delay
                delay[14] = inL;                         // now update delay
            }
        //----------------------------------  sample 19 ---------------------------------
            else if (count_poly == 19) {
                count_poly++;
                //Decimation
                pWork_fb[45+count_12*64] = fir_filter( H_filt_3245_delays_dec, // compute polyphase FIR out and save
                        E45, N_delays_H_filt_3245_delays, delay[45]);
                pWork_fb[13+count_12*64] = fir_filter( H_filt_3213_delays_dec, // compute polyphase FIR out and save
                        E13, N_delays_H_filt_3213_delays, delay[13]);

                delay[45] = delay[13];                         // now update delay
                delay[13] = inL;                         // now update delay
            }
        //----------------------------------  sample 20 ---------------------------------
            else if (count_poly == 20) {
                count_poly++;
                //Decimation
                pWork_fb[44+count_12*64] = fir_filter( H_filt_3244_delays_dec, // compute polyphase FIR out and save
                        E44, N_delays_H_filt_3244_delays, delay[44]);
                pWork_fb[12+count_12*64] = fir_filter( H_filt_3212_delays_dec, // compute polyphase FIR out and save
                        E12, N_delays_H_filt_3212_delays, delay[12]);
                delay[44] = delay[12];                         // now update delay
                delay[12] = inL;                         // now update delay
            }
        //----------------------------------  sample 21 ---------------------------------
            else if (count_poly == 21) {
                count_poly++;
                //Decimation
                pWork_fb[43+count_12*64] = fir_filter( H_filt_3243_delays_dec, // compute polyphase FIR out and save
                        E43, N_delays_H_filt_3243_delays, delay[43]);
                pWork_fb[11+count_12*64] = fir_filter( H_filt_3211_delays_dec, // compute polyphase FIR out and save
                        E11, N_delays_H_filt_3211_delays, delay[11]);
                delay[43] = delay[11];                         // now update delay
                delay[11] = inL;                         // now update delay
            }
        //----------------------------------  sample 22 ---------------------------------
            else if (count_poly == 22) {
                count_poly++;
                //Decimation
                pWork_fb[42+count_12*64] = fir_filter( H_filt_3242_delays_dec, // compute polyphase FIR out and save
                        E42, N_delays_H_filt_3242_delays, delay[42]);
                pWork_fb[10+count_12*64] = fir_filter( H_filt_3210_delays_dec, // compute polyphase FIR out and save
                        E10, N_delays_H_filt_3210_delays, delay[10]);
                delay[42] = delay[10];                         // now update delay
                delay[10] = inL;                         // now update delay
            }
        //----------------------------------  sample 23 ---------------------------------
            else if (count_poly == 23) {
                count_poly++;
                //Decimation
                pWork_fb[41+count_12*64] = fir_filter( H_filt_3241_delays_dec, // compute polyphase FIR out and save
                        E41, N_delays_H_filt_3241_delays, delay[41]);
                pWork_fb[9+count_12*64] = fir_filter( H_filt_329_delays_dec, // compute polyphase FIR out and save
                        E9, N_delays_H_filt_329_delays, delay[9]);
                delay[41] = delay[9];                         // now update delay
                delay[9] = inL;                         // now update delay
            }
        //----------------------------------  sample 24 ---------------------------------
            else if (count_poly == 24) {
                count_poly++;
                //Decimation
                pWork_fb[40+count_12*64] = fir_filter( H_filt_3240_delays_dec, // compute polyphase FIR out and save
                        E40, N_delays_H_filt_3240_delays, delay[40]);
                pWork_fb[8+count_12*64] = fir_filter( H_filt_328_delays_dec, // compute polyphase FIR out and save
                        E8, N_delays_H_filt_328_delays, delay[8]);
                delay[40] = delay[8];                         // now update delay
                delay[8] = inL;                         // now update delay
            }
        //----------------------------------  sample 25 ---------------------------------
            else if (count_poly == 25) {
                count_poly++;
                //Decimation
                pWork_fb[39+count_12*64] = fir_filter( H_filt_3239_delays_dec, // compute polyphase FIR out and save
                        E39, N_delays_H_filt_3239_delays, delay[39]);
                pWork_fb[7+count_12*64] = fir_filter( H_filt_327_delays_dec, // compute polyphase FIR out and save
                        E7, N_delays_H_filt_327_delays, delay[7]);
                delay[39] = delay[7];                         // now update delay
                delay[7] = inL;                             // now update delay
            }
        //----------------------------------  sample 26 ---------------------------------
            else if (count_poly == 26) {
                count_poly++;
                //Decimation
                pWork_fb[38+count_12*64] = fir_filter( H_filt_3238_delays_dec, // compute polyphase FIR out and save
                        E38, N_delays_H_filt_3238_delays, delay[38]);
                pWork_fb[6+count_12*64] = fir_filter( H_filt_326_delays_dec, // compute polyphase FIR out and save
                        E6, N_delays_H_filt_326_delays, delay[6]);
                delay[38] = delay[6];                         // now update delay
                delay[6] = inL;                             // now update delay
            }
        //----------------------------------  sample 27 ---------------------------------
            else if (count_poly == 27) {
                count_poly++;
                //Decimation
                pWork_fb[37+count_12*64] = fir_filter( H_filt_3237_delays_dec, // compute polyphase FIR out and save
                        E37, N_delays_H_filt_3237_delays, delay[37]);
                pWork_fb[5+count_12*64] = fir_filter( H_filt_325_delays_dec, // compute polyphase FIR out and save
                        E5, N_delays_H_filt_325_delays, delay[5]);
                delay[37] = delay[5];                         // now update delay
                delay[5] = inL;                             // now update delay
            }
        //----------------------------------  sample 28 ---------------------------------
            else if (count_poly == 28) {
                count_poly++;
                //Decimation
                pWork_fb[36+count_12*64] = fir_filter( H_filt_3236_delays_dec, // compute polyphase FIR out and save
                        E36, N_delays_H_filt_3236_delays, delay[36]);
                pWork_fb[4+count_12*64] = fir_filter( H_filt_324_delays_dec, // compute polyphase FIR out and save
                        E4, N_delays_H_filt_324_delays, delay[4]);
                delay[36] = delay[4];                         // now update delay
                delay[4] = inL;                             // now update delay
            }
        //----------------------------------  sample 29 ---------------------------------
            else if (count_poly == 29) {
                count_poly++;
                //Decimation
                pWork_fb[35+count_12*64] = fir_filter( H_filt_3235_delays_dec, // compute polyphase FIR out and save
                        E35, N_delays_H_filt_3235_delays, delay[35]);
                pWork_fb[3+count_12*64] = fir_filter( H_filt_323_delays_dec, // compute polyphase FIR out and save
                        E3, N_delays_H_filt_323_delays, delay[3]);
                delay[35] = delay[3];                         // now update delay
                delay[3] = inL;                             // now update delay
            }
        //----------------------------------  sample 30 ---------------------------------
            else if (count_poly == 30) {
                count_poly++;
                //Decimation
                pWork_fb[34+count_12*64] = fir_filter( H_filt_3234_delays_dec, // compute polyphase FIR out and save
                        E34, N_delays_H_filt_3234_delays, delay[34]);
                pWork_fb[2+count_12*64] = fir_filter( H_filt_322_delays_dec, // compute polyphase FIR out and save
                        E2, N_delays_H_filt_322_delays, delay[2]);
                delay[34] = delay[2];                         // now update delay
                delay[2] = inL;                             // now update delay
            }
        //----------------------------------  sample 31 ---------------------------------
            else if (count_poly == 31) {
                //Decimation
                pWork_fb[33+count_12*64] = fir_filter( H_filt_3233_delays_dec, // compute polyphase FIR out and save
                        E33, N_delays_H_filt_3233_delays, delay[33]);
                pWork_fb[1+count_12*64] = fir_filter( H_filt_321_delays_dec, // compute polyphase FIR out and save
                        E1, N_delays_H_filt_321_delays, delay[1]);
                delay[33] = delay[1];                         // now update delay
                delay[1] = inL;                             // now update delay
                count_poly=0;
                count_12++;
            }
    }

    return 0;
}
