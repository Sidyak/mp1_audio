//----------------------------------------------------------------------------
// Description       encoder initialization header
// Author:           Kim Radmacher
// Created on:       22.01.2015
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

#ifndef INIT_H_
#define INIT_H_

#include <stdint.h>

//#define DEBUG

#ifdef DEBUG
#include <stdio.h>
#endif

#define MAX_CHANNEL 2
#define BUFLEN (12*32) // buffer length: 12 samples and 32 subbands = 384
#define NFFT 512    // fft length
#define PI 3.14159265358979
#define RADIX 2        // radix 2 fft
#define BANDSIZE 32    // 32 filter subbands
#define constant_factor_log_to_20log10 8.68588963806504
#define constant_factor_log_to_10log10 4.34294481903252
#define log2value 0.693147180559945
#define NA 0
#define TONAL 1
#define NTONAL 2
#define MIN_POWER -100

extern unsigned int *table_Xmt;    // Xmt buffer

extern short input[BUFLEN];            // for defined input (simulation)
extern int noise;                    // for adding noise to input

/***** FFT variables *****/
typedef struct Complex_tag {float re, im;} Complex;
extern int16_t* convert_buf;
extern Complex W[NFFT/RADIX];        // twiddle constants
extern Complex xFFT[NFFT];            // 512 FFT input
extern Complex *pxFFT;                // pointer reference
extern Complex *pxFFT_old;            // pointer reference
extern Complex xFFT_old[128];        // buffer for last 128 samples (overlapping)
extern float work_fft[384];            // second buffer for swapping pointer
extern float *pWork_fft;            // pointer reference
extern short iTwid[NFFT/2];            // index for twiddle constants W
extern short iData[NFFT];            // index for bitrev X
extern float hanning[NFFT];
extern float magnitude_dB[NFFT/2];    // fft result - dB magnitude
extern float magnitude[NFFT/2];        // fft result - magnitude
extern float abs_inphase;            // for complex abs estimation
extern float abs_quadrature;        // for complex abs estimation

// lookup table for fast log calculation
extern float logLuT[1000];

/***** Psychoacoustic Model variables *****/
/* masker structure */
struct Masker_List {
    short index[25];    // masker index
    float power[25];    // masker power level
};
extern struct Masker_List Ton_list;
extern struct Masker_List nTon_list;
extern float scf[BANDSIZE];        // scale factors
extern float Ls[32];            // sound pressure
extern float LTtm[25][102];        // individual tonal mask threshold
extern float LTnm[25][102];        // individual non-tonal mask threshold
extern float LTg[102];            // gloabal mask threshold
extern float LTmin[BANDSIZE];    // min. mask threshold
extern float SMR[BANDSIZE];        // signal-mask-ratio
extern float MNR[BANDSIZE];        // mask-noise-ratio
extern short BSCF[BANDSIZE];    // bit values for scale factors
extern short BSPL[BANDSIZE];     // bit values for subbands
extern short flag[NFFT/2];        // flag if tonal or non-tonal masker
extern short ton_ind;            // index
extern short nton_ind;            // index
extern short Ton_list_leng;        // length of tonals
extern short compr_rate;        // bit-rate depending compression rate
extern short cb;                // callable number of bits at given bit-rate
extern float adb;                // currently available data bits
extern short fft_done, fb_done;    // busy flags
extern short first_FRAME;
extern short BSCF_done;
extern short band_cnt;
extern float scf_rx[BANDSIZE];
extern short tot_bits;
extern short cnt_out,out_flag;
extern uint8_t FRAME1[16*1024*1024];//2*BUFLEN*sizeof(short)];
extern uint8_t *pFRAME1;
extern short cnt_FRAME_fill;
extern short index_nTon;

/***** lookup table for mid-thread quantization *****/
extern float exp2LUT[14];

/***** Filterbank variables *****/
extern short tw_sample;        // counter for 12 subband samples
extern float delay[64];
extern float filt_outA[768], filt_outB[768];    // 64 polyphases * 12 samples
extern float M[32][64],T[32][64], S[32][12];
extern float teta, maxValue;
extern float *y1_part_fb, *pWork_fb;
extern float INT_y,INT_y1,INT_y2;
extern float out_delay[64];
extern float y_rx[12][BANDSIZE];
extern short count_poly,count_12_synthese, ind_calc_overlap, ORIGINAL;
extern float Out1[768],Out2[768];    // 64 Polyphasen 12 Sample Output buffer
extern float *pOut1;
extern float *pOut2;
extern short count_fb,count_fft,count_12;

/* toggle gpio pin for timing measurement */
extern short toggle;

/***** some control variables *****/
extern short ki;
extern short cnt_;
extern short cnt_samp;        // counter for 384 samples
extern short count_INT, i_m,k_m;
extern float inL,inR;        // current left and right sample

/* from mpeg_tables.h */
extern float table_scf[63];
extern float LTq_dB[102];
extern float LTq[102];
extern short LTq_f[102];
extern float LTq_B[102];
extern short Map[256];
extern short CB_i[26];
extern short CB_f[26];
extern short Bit_leng[15];
extern short No_steps[15];
extern float SNR[15];
extern float quantization_table[14][3];

/*  Function Prototypes */
extern float fir_filter(float delays[], float coe[], short N_delays, float x_n);

/* filterbank */
#define N_delays_H_filt_320_delays 16
#define N_delays_H_filt_321_delays 16
#define N_delays_H_filt_322_delays 16
#define N_delays_H_filt_323_delays 16
#define N_delays_H_filt_324_delays 16
#define N_delays_H_filt_325_delays 16
#define N_delays_H_filt_326_delays 16
#define N_delays_H_filt_327_delays 16
#define N_delays_H_filt_328_delays 16
#define N_delays_H_filt_329_delays 16
#define N_delays_H_filt_3210_delays 16
#define N_delays_H_filt_3211_delays 16
#define N_delays_H_filt_3212_delays 16
#define N_delays_H_filt_3213_delays 16
#define N_delays_H_filt_3214_delays 16
#define N_delays_H_filt_3215_delays 16
#define N_delays_H_filt_3216_delays 16
#define N_delays_H_filt_3217_delays 16
#define N_delays_H_filt_3218_delays 16
#define N_delays_H_filt_3219_delays 16
#define N_delays_H_filt_3220_delays 16
#define N_delays_H_filt_3221_delays 16
#define N_delays_H_filt_3222_delays 16
#define N_delays_H_filt_3223_delays 16
#define N_delays_H_filt_3224_delays 16
#define N_delays_H_filt_3225_delays 16
#define N_delays_H_filt_3226_delays 16
#define N_delays_H_filt_3227_delays 16
#define N_delays_H_filt_3228_delays 16
#define N_delays_H_filt_3229_delays 16
#define N_delays_H_filt_3230_delays 16
#define N_delays_H_filt_3231_delays 16
#define N_delays_H_filt_3232_delays 16
#define N_delays_H_filt_3233_delays 16
#define N_delays_H_filt_3234_delays 16
#define N_delays_H_filt_3235_delays 16
#define N_delays_H_filt_3236_delays 16
#define N_delays_H_filt_3237_delays 16
#define N_delays_H_filt_3238_delays 16
#define N_delays_H_filt_3239_delays 16
#define N_delays_H_filt_3240_delays 16
#define N_delays_H_filt_3241_delays 16
#define N_delays_H_filt_3242_delays 16
#define N_delays_H_filt_3243_delays 16
#define N_delays_H_filt_3244_delays 16
#define N_delays_H_filt_3245_delays 16
#define N_delays_H_filt_3246_delays 16
#define N_delays_H_filt_3247_delays 16
#define N_delays_H_filt_3248_delays 16
#define N_delays_H_filt_3249_delays 16
#define N_delays_H_filt_3250_delays 16
#define N_delays_H_filt_3251_delays 16
#define N_delays_H_filt_3252_delays 16
#define N_delays_H_filt_3253_delays 16
#define N_delays_H_filt_3254_delays 16
#define N_delays_H_filt_3255_delays 16
#define N_delays_H_filt_3256_delays 16
#define N_delays_H_filt_3257_delays 16
#define N_delays_H_filt_3258_delays 16
#define N_delays_H_filt_3259_delays 16
#define N_delays_H_filt_3260_delays 16
#define N_delays_H_filt_3261_delays 16
#define N_delays_H_filt_3262_delays 16
#define N_delays_H_filt_3263_delays 16

extern float E0[16], E1[16], E2[16], E3[16], E4[16], E5[16], E6[16], E7[16], E8[16], E9[16], E10[16],E11[16],E12[16],E13[16],E14[16],E15[16];
extern float E16[16],E17[16],E18[16],E19[16],E20[16],E21[16],E22[16],E23[16],E24[16],E25[16],E26[16],E27[16],E28[16],E29[16],E30[16],E31[16];
extern float E32[16],E33[16],E34[16],E35[16],E36[16],E37[16],E38[16],E39[16],E40[16],E41[16],E42[16],E43[16],E44[16],E45[16],E46[16],E47[16];
extern float E48[16],E49[16],E50[16],E51[16],E52[16],E53[16],E54[16],E55[16],E56[16],E57[16],E58[16],E59[16],E60[16],E61[16],E62[16],E63[16];

extern float H_filt_320_delays_dec[N_delays_H_filt_320_delays];
extern float H_filt_321_delays_dec[N_delays_H_filt_321_delays];
extern float H_filt_322_delays_dec[N_delays_H_filt_322_delays];
extern float H_filt_323_delays_dec[N_delays_H_filt_323_delays];
extern float H_filt_324_delays_dec[N_delays_H_filt_324_delays];
extern float H_filt_325_delays_dec[N_delays_H_filt_325_delays];
extern float H_filt_326_delays_dec[N_delays_H_filt_326_delays];
extern float H_filt_327_delays_dec[N_delays_H_filt_327_delays];
extern float H_filt_328_delays_dec[N_delays_H_filt_328_delays];
extern float H_filt_329_delays_dec[N_delays_H_filt_329_delays];
extern float H_filt_3210_delays_dec[N_delays_H_filt_3210_delays];
extern float H_filt_3211_delays_dec[N_delays_H_filt_3211_delays];
extern float H_filt_3212_delays_dec[N_delays_H_filt_3212_delays];
extern float H_filt_3213_delays_dec[N_delays_H_filt_3213_delays];
extern float H_filt_3214_delays_dec[N_delays_H_filt_3214_delays];
extern float H_filt_3215_delays_dec[N_delays_H_filt_3215_delays];
extern float H_filt_3216_delays_dec[N_delays_H_filt_3216_delays];
extern float H_filt_3217_delays_dec[N_delays_H_filt_3217_delays];
extern float H_filt_3218_delays_dec[N_delays_H_filt_3218_delays];
extern float H_filt_3219_delays_dec[N_delays_H_filt_3219_delays];
extern float H_filt_3220_delays_dec[N_delays_H_filt_3220_delays];
extern float H_filt_3221_delays_dec[N_delays_H_filt_3221_delays];
extern float H_filt_3222_delays_dec[N_delays_H_filt_3222_delays];
extern float H_filt_3223_delays_dec[N_delays_H_filt_3223_delays];
extern float H_filt_3224_delays_dec[N_delays_H_filt_3224_delays];
extern float H_filt_3225_delays_dec[N_delays_H_filt_3225_delays];
extern float H_filt_3226_delays_dec[N_delays_H_filt_3226_delays];
extern float H_filt_3227_delays_dec[N_delays_H_filt_3227_delays];
extern float H_filt_3228_delays_dec[N_delays_H_filt_3228_delays];
extern float H_filt_3229_delays_dec[N_delays_H_filt_3229_delays];
extern float H_filt_3230_delays_dec[N_delays_H_filt_3230_delays];
extern float H_filt_3231_delays_dec[N_delays_H_filt_3231_delays];
extern float H_filt_3232_delays_dec[N_delays_H_filt_3232_delays];
extern float H_filt_3233_delays_dec[N_delays_H_filt_3233_delays];
extern float H_filt_3234_delays_dec[N_delays_H_filt_3234_delays];
extern float H_filt_3235_delays_dec[N_delays_H_filt_3235_delays];
extern float H_filt_3236_delays_dec[N_delays_H_filt_3236_delays];
extern float H_filt_3237_delays_dec[N_delays_H_filt_3237_delays];
extern float H_filt_3238_delays_dec[N_delays_H_filt_3238_delays];
extern float H_filt_3239_delays_dec[N_delays_H_filt_3239_delays];
extern float H_filt_3240_delays_dec[N_delays_H_filt_3240_delays];
extern float H_filt_3241_delays_dec[N_delays_H_filt_3241_delays];
extern float H_filt_3242_delays_dec[N_delays_H_filt_3242_delays];
extern float H_filt_3243_delays_dec[N_delays_H_filt_3243_delays];
extern float H_filt_3244_delays_dec[N_delays_H_filt_3244_delays];
extern float H_filt_3245_delays_dec[N_delays_H_filt_3245_delays];
extern float H_filt_3246_delays_dec[N_delays_H_filt_3246_delays];
extern float H_filt_3247_delays_dec[N_delays_H_filt_3247_delays];
extern float H_filt_3248_delays_dec[N_delays_H_filt_3248_delays];
extern float H_filt_3249_delays_dec[N_delays_H_filt_3249_delays];
extern float H_filt_3250_delays_dec[N_delays_H_filt_3250_delays];
extern float H_filt_3251_delays_dec[N_delays_H_filt_3251_delays];
extern float H_filt_3252_delays_dec[N_delays_H_filt_3252_delays];
extern float H_filt_3253_delays_dec[N_delays_H_filt_3253_delays];
extern float H_filt_3254_delays_dec[N_delays_H_filt_3254_delays];
extern float H_filt_3255_delays_dec[N_delays_H_filt_3255_delays];
extern float H_filt_3256_delays_dec[N_delays_H_filt_3256_delays];
extern float H_filt_3257_delays_dec[N_delays_H_filt_3257_delays];
extern float H_filt_3258_delays_dec[N_delays_H_filt_3258_delays];
extern float H_filt_3259_delays_dec[N_delays_H_filt_3259_delays];
extern float H_filt_3260_delays_dec[N_delays_H_filt_3260_delays];
extern float H_filt_3261_delays_dec[N_delays_H_filt_3261_delays];
extern float H_filt_3262_delays_dec[N_delays_H_filt_3262_delays];
extern float H_filt_3263_delays_dec[N_delays_H_filt_3263_delays];

extern float H_filt_320_delays[N_delays_H_filt_320_delays];
extern float H_filt_321_delays[N_delays_H_filt_321_delays];
extern float H_filt_322_delays[N_delays_H_filt_322_delays];
extern float H_filt_323_delays[N_delays_H_filt_323_delays];
extern float H_filt_324_delays[N_delays_H_filt_324_delays];
extern float H_filt_325_delays[N_delays_H_filt_325_delays];
extern float H_filt_326_delays[N_delays_H_filt_326_delays];
extern float H_filt_327_delays[N_delays_H_filt_327_delays];
extern float H_filt_328_delays[N_delays_H_filt_328_delays];
extern float H_filt_329_delays[N_delays_H_filt_329_delays];
extern float H_filt_3210_delays[N_delays_H_filt_3210_delays];
extern float H_filt_3211_delays[N_delays_H_filt_3211_delays];
extern float H_filt_3212_delays[N_delays_H_filt_3212_delays];
extern float H_filt_3213_delays[N_delays_H_filt_3213_delays];
extern float H_filt_3214_delays[N_delays_H_filt_3214_delays];
extern float H_filt_3215_delays[N_delays_H_filt_3215_delays];
extern float H_filt_3216_delays[N_delays_H_filt_3216_delays];
extern float H_filt_3217_delays[N_delays_H_filt_3217_delays];
extern float H_filt_3218_delays[N_delays_H_filt_3218_delays];
extern float H_filt_3219_delays[N_delays_H_filt_3219_delays];
extern float H_filt_3220_delays[N_delays_H_filt_3220_delays];
extern float H_filt_3221_delays[N_delays_H_filt_3221_delays];
extern float H_filt_3222_delays[N_delays_H_filt_3222_delays];
extern float H_filt_3223_delays[N_delays_H_filt_3223_delays];
extern float H_filt_3224_delays[N_delays_H_filt_3224_delays];
extern float H_filt_3225_delays[N_delays_H_filt_3225_delays];
extern float H_filt_3226_delays[N_delays_H_filt_3226_delays];
extern float H_filt_3227_delays[N_delays_H_filt_3227_delays];
extern float H_filt_3228_delays[N_delays_H_filt_3228_delays];
extern float H_filt_3229_delays[N_delays_H_filt_3229_delays];
extern float H_filt_3230_delays[N_delays_H_filt_3230_delays];
extern float H_filt_3231_delays[N_delays_H_filt_3231_delays];
extern float H_filt_3232_delays[N_delays_H_filt_3232_delays];
extern float H_filt_3233_delays[N_delays_H_filt_3233_delays];
extern float H_filt_3234_delays[N_delays_H_filt_3234_delays];
extern float H_filt_3235_delays[N_delays_H_filt_3235_delays];
extern float H_filt_3236_delays[N_delays_H_filt_3236_delays];
extern float H_filt_3237_delays[N_delays_H_filt_3237_delays];
extern float H_filt_3238_delays[N_delays_H_filt_3238_delays];
extern float H_filt_3239_delays[N_delays_H_filt_3239_delays];
extern float H_filt_3240_delays[N_delays_H_filt_3240_delays];
extern float H_filt_3241_delays[N_delays_H_filt_3241_delays];
extern float H_filt_3242_delays[N_delays_H_filt_3242_delays];
extern float H_filt_3243_delays[N_delays_H_filt_3243_delays];
extern float H_filt_3244_delays[N_delays_H_filt_3244_delays];
extern float H_filt_3245_delays[N_delays_H_filt_3245_delays];
extern float H_filt_3246_delays[N_delays_H_filt_3246_delays];
extern float H_filt_3247_delays[N_delays_H_filt_3247_delays];
extern float H_filt_3248_delays[N_delays_H_filt_3248_delays];
extern float H_filt_3249_delays[N_delays_H_filt_3249_delays];
extern float H_filt_3250_delays[N_delays_H_filt_3250_delays];
extern float H_filt_3251_delays[N_delays_H_filt_3251_delays];
extern float H_filt_3252_delays[N_delays_H_filt_3252_delays];
extern float H_filt_3253_delays[N_delays_H_filt_3253_delays];
extern float H_filt_3254_delays[N_delays_H_filt_3254_delays];
extern float H_filt_3255_delays[N_delays_H_filt_3255_delays];
extern float H_filt_3256_delays[N_delays_H_filt_3256_delays];
extern float H_filt_3257_delays[N_delays_H_filt_3257_delays];
extern float H_filt_3258_delays[N_delays_H_filt_3258_delays];
extern float H_filt_3259_delays[N_delays_H_filt_3259_delays];
extern float H_filt_3260_delays[N_delays_H_filt_3260_delays];
extern float H_filt_3261_delays[N_delays_H_filt_3261_delays];
extern float H_filt_3262_delays[N_delays_H_filt_3262_delays];
extern float H_filt_3263_delays[N_delays_H_filt_3263_delays];


#endif /* INIT_H_ */
