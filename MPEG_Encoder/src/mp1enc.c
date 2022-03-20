//-----------------------------------------------------------------------
// Description:  MPEG-1 Layer I Encoder
//
// Author:       Kim Radmacher
//
// Date:         
//               19-Feb-15
// Date of final version: 
//               23-Jun-15
// Date of porting to x86 and refactoring:
//               06-Mae-22
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

#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "analysis_coeffs_float.h"  // filterbank coef
#include "mpeg_tables.h"            // mpeg-1 tables

#if defined(_MSC_VER)
#include <getopt.h>
#else
#include <unistd.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif
#include "wavreader.h"
#include "wavwriter.h"
#ifdef __cplusplus
}
#endif

//#define DEBUG

#define BUFLEN (12*32) //384   // bufflength for EDMA buffer
#define NFFT 512     // fft length
#define PI 3.14159265358979
#define RADIX 2      // radix 2 fft
#define BANDSIZE 32  // 32 filter subbands

const uint32_t syncWords[2] = {0xCCCCAAAA, 0xAAAAF0F0};
//unsigned int table_Xmt[BUFLEN+sizeof(syncWords)];    // buffer
short original[BUFLEN+sizeof(syncWords)] = {0};        // Original samples

// FFT variables
typedef struct Complex_tag {float re, im;} Complex;
Complex W[NFFT/RADIX];        // twiddle constants
Complex xFFT[NFFT];           // 512 FFT input
Complex *pxFFT;               // pointer reference
Complex *pxFFT_old;           // pointer reference
Complex xFFT_old[128];        // buffer for last 128 samples (overlapping)
#pragma DATA_ALIGN(W,sizeof(Complex))           // align W on boundary
#pragma DATA_ALIGN(xFFT,sizeof(Complex))        // align input x on boundary
#pragma DATA_ALIGN(xFFT_old,sizeof(Complex))    // align input x on boundary

int bitrate = 192;          // Bitrate default to 196 kbps
float work_fft[BUFLEN];     // second buffer for swapping pointer
float *pWork_fft;           // pointer reference
short iTwid[NFFT/2];        // index for twiddle constants W
short iData[NFFT];          // index for bitrev X
float hanning[NFFT];
float magnitude_dB[NFFT/2]; // fft result - dB magnitude
float magnitude[NFFT/2];    // fft result - magnitude
float abs_inphase;          // for complex abs estimation
float abs_quadrature;       // for complex abs estimation

// lookup table for fast log calculation
float logLuT[1000] = 
{
    0.0, 0.0009995003, 0.0019980027, 0.0029955090, 0.0039920213, 0.0049875415, 0.0059820717, 0.0069756137, 0.0079681696, 
    0.0089597414, 0.0099503309, 0.0109399400, 0.0119285709, 0.0129162253, 0.0139029052, 0.0148886125, 0.0158733492, 
    0.0168571171, 0.0178399181, 0.0188217542, 0.0198026273, 0.0207825392, 0.0217614918, 0.0227394870, 0.0237165266, 
    0.0246926126, 0.0256677467, 0.0266419309, 0.0276151670, 0.0285874569, 0.0295588022, 0.0305292050, 0.0314986671, 
    0.0324671901, 0.0334347761, 0.0344014267, 0.0353671438, 0.0363319292, 0.0372957847, 0.0382587121, 0.0392207132, 
    0.0401817896, 0.0411419433, 0.0421011760, 0.0430594895, 0.0440168854, 0.0449733656, 0.0459289319, 0.0468835859, 
    0.0478373294, 0.0487901642, 0.0497420919, 0.0506931143, 0.0516432332, 0.0525924501, 0.0535407669, 0.0544881853, 
    0.0554347069, 0.0563803334, 0.0573250666, 0.0582689081, 0.0592118596, 0.0601539228, 0.0610950994, 0.0620353909, 
    0.0629747992, 0.0639133257, 0.0648509723, 0.0657877405, 0.0667236320, 0.0676586485, 0.0685927915, 0.0695260626, 
    0.0704584636, 0.0713899961, 0.0723206616, 0.0732504617, 0.0741793982, 0.0751074725, 0.0760346863, 0.0769610411, 
    0.0778865387, 0.0788111804, 0.0797349680, 0.0806579030, 0.0815799870, 0.0825012215, 0.0834216081, 0.0843411484, 
    0.0852598440, 0.0861776962, 0.0870947069, 0.0880108773, 0.0889262092, 0.0898407040, 0.0907543633, 0.0916671885, 
    0.0925791813, 0.0934903431, 0.0944006754, 0.0953101798, 0.0962188577, 0.0971267107, 0.0980337403, 0.0989399479, 
    0.0998453350, 0.1007499031, 0.1016536537, 0.1025565883, 0.1034587084, 0.1043600153, 0.1052605107, 0.1061601958, 
    0.1070590723, 0.1079571415, 0.1088544049, 0.1097508640, 0.1106465201, 0.1115413747, 0.1124354293, 0.1133286853, 
    0.1142211441, 0.1151128071, 0.1160036758, 0.1168937515, 0.1177830357, 0.1186715297, 0.1195592351, 0.1204461531, 
    0.1213322852, 0.1222176327, 0.1231021971, 0.1239859798, 0.1248689820, 0.1257512053, 0.1266326509, 0.1275133203, 
    0.1283932148, 0.1292723357, 0.1301506845, 0.1310282624, 0.1319050709, 0.1327811112, 0.1336563848, 0.1345308930, 
    0.1354046370, 0.1362776183, 0.1371498381, 0.1380212979, 0.1388919989, 0.1397619424, 0.1406311297, 0.1414995623, 
    0.1423672413, 0.1432341681, 0.1441003440, 0.1449657703, 0.1458304482, 0.1466943792, 0.1475575644, 0.1484200051, 
    0.1492817027, 0.1501426584, 0.1510028735, 0.1518623493, 0.1527210870, 0.1535790879, 0.1544363533, 0.1552928844, 
    0.1561486825, 0.1570037488, 0.1578580846, 0.1587116912, 0.1595645697, 0.1604167214, 0.1612681476, 0.1621188495, 
    0.1629688283, 0.1638180852, 0.1646666216, 0.1655144385, 0.1663615372, 0.1672079190, 0.1680535850, 0.1688985365, 
    0.1697427746, 0.1705863006, 0.1714291156, 0.1722712209, 0.1731126177, 0.1739533071, 0.1747932904, 0.1756325686, 
    0.1764711431, 0.1773090150, 0.1781461854, 0.1789826555, 0.1798184266, 0.1806534997, 0.1814878760, 0.1823215568, 
    0.1831545431, 0.1839868361, 0.1848184370, 0.1856493469, 0.1864795669, 0.1873090983, 0.1881379421, 0.1889660995, 
    0.1897935716, 0.1906203596, 0.1914464646, 0.1922718876, 0.1930966300, 0.1939206926, 0.1947440768, 0.1955667835, 
    0.1963888140, 0.1972101693, 0.1980308505, 0.1988508587, 0.1996701951, 0.2004888607, 0.2013068567, 0.2021241841, 
    0.2029408440, 0.2037568375, 0.2045721657, 0.2053868297, 0.2062008306, 0.2070141694, 0.2078268472, 0.2086388651, 
    0.2094502242, 0.2102609255, 0.2110709701, 0.2118803590, 0.2126890934, 0.2134971743, 0.2143046026, 0.2151113796, 
    0.2159175062, 0.2167229835, 0.2175278125, 0.2183319943, 0.2191355299, 0.2199384204, 0.2207406667, 0.2215422699, 
    0.2223432311, 0.2231435513, 0.2239432315, 0.2247422727, 0.2255406759, 0.2263384422, 0.2271355726, 0.2279320680, 
    0.2287279296, 0.2295231583, 0.2303177551, 0.2311117210, 0.2319050570, 0.2326977641, 0.2334898434, 0.2342812957, 
    0.2350721222, 0.2358623237, 0.2366519013, 0.2374408560, 0.2382291887, 0.2390169005, 0.2398039922, 0.2405904649, 
    0.2413763196, 0.2421615571, 0.2429461786, 0.2437301849, 0.2445135771, 0.2452963560, 0.2460785226, 0.2468600779, 
    0.2476410229, 0.2484213585, 0.2492010856, 0.2499802053, 0.2507587183, 0.2515366258, 0.2523139286, 0.2530906277, 
    0.2538667240, 0.2546422184, 0.2554171119, 0.2561914054, 0.2569650998, 0.2577381961, 0.2585106952, 0.2592825979, 
    0.2600539053, 0.2608246183, 0.2615947377, 0.2623642645, 0.2631331995, 0.2639015438, 0.2646692981, 0.2654364635, 
    0.2662030408, 0.2669690309, 0.2677344346, 0.2684992530, 0.2692634869, 0.2700271372, 0.2707902048, 0.2715526905, 
    0.2723145953, 0.2730759201, 0.2738366656, 0.2745968329, 0.2753564228, 0.2761154361, 0.2768738737, 0.2776317366, 
    0.2783890255, 0.2791457414, 0.2799018851, 0.2806574575, 0.2814124594, 0.2821668918, 0.2829207554, 0.2836740511, 
    0.2844267797, 0.2851789422, 0.2859305394, 0.2866815721, 0.2874320412, 0.2881819475, 0.2889312919, 0.2896800751, 
    0.2904282981, 0.2911759617, 0.2919230667, 0.2926696140, 0.2934156043, 0.2941610385, 0.2949059175, 0.2956502421, 
    0.2963940131, 0.2971372312, 0.2978798974, 0.2986220125, 0.2993635772, 0.3001045925, 0.3008450590, 0.3015849776, 
    0.3023243492, 0.3030631745, 0.3038014543, 0.3045391895, 0.3052763809, 0.3060130291, 0.3067491352, 0.3074846997, 
    0.3082197237, 0.3089542077, 0.3096881527, 0.3104215594, 0.3111544286, 0.3118867611, 0.3126185577, 0.3133498192, 
    0.3140805463, 0.3148107398, 0.3155404006, 0.3162695293, 0.3169981268, 0.3177261938, 0.3184537311, 0.3191807395, 
    0.3199072197, 0.3206331726, 0.3213585988, 0.3220834992, 0.3228078744, 0.3235317253, 0.3242550527, 0.3249778572, 
    0.3257001396, 0.3264219008, 0.3271431413, 0.3278638621, 0.3285840638, 0.3293037471, 0.3300229129, 0.3307415619, 
    0.3314596948, 0.3321773123, 0.3328944153, 0.3336110043, 0.3343270803, 0.3350426438, 0.3357576957, 0.3364722366, 
    0.3371862674, 0.3378997886, 0.3386128011, 0.3393253056, 0.3400373028, 0.3407487934, 0.3414597781, 0.3421702577, 
    0.3428802329, 0.3435897044, 0.3442986729, 0.3450071391, 0.3457151037, 0.3464225675, 0.3471295311, 0.3478359953, 
    0.3485419607, 0.3492474281, 0.3499523982, 0.3506568716, 0.3513608491, 0.3520643314, 0.3527673191, 0.3534698130, 
    0.3541718137, 0.3548733220, 0.3555743385, 0.3562748639, 0.3569748989, 0.3576744443, 0.3583735006, 0.3590720685, 
    0.3597701488, 0.3604677422, 0.3611648492, 0.3618614706, 0.3625576071, 0.3632532593, 0.3639484279, 0.3646431136, 
    0.3653373170, 0.3660310389, 0.3667242798, 0.3674170405, 0.3681093216, 0.3688011237, 0.3694924476, 0.3701832940, 
    0.3708736633, 0.3715635564, 0.3722529739, 0.3729419164, 0.3736303846, 0.3743183791, 0.3750059006, 0.3756929498, 
    0.3763795272, 0.3770656336, 0.3777512695, 0.3784364357, 0.3791211328, 0.3798053613, 0.3804891220, 0.3811724155, 
    0.3818552425, 0.3825376035, 0.3832194992, 0.3839009302, 0.3845818972, 0.3852624008, 0.3859424416, 0.3866220203, 
    0.3873011375, 0.3879797938, 0.3886579898, 0.3893357262, 0.3900130035, 0.3906898225, 0.3913661837, 0.3920420878, 
    0.3927175353, 0.3933925269, 0.3940670632, 0.3947411447, 0.3954147723, 0.3960879463, 0.3967606675, 0.3974329364, 
    0.3981047537, 0.3987761200, 0.3994470358, 0.4001175018, 0.4007875186, 0.4014570867, 0.4021262068, 0.4027948796, 
    0.4034631054, 0.4041308851, 0.4047982191, 0.4054651081, 0.4061315527, 0.4067975533, 0.4074631108, 0.4081282255, 
    0.4087928982, 0.4094571294, 0.4101209196, 0.4107842696, 0.4114471798, 0.4121096508, 0.4127716833, 0.4134332778, 
    0.4140944348, 0.4147551550, 0.4154154390, 0.4160752872, 0.4167347004, 0.4173936790, 0.4180522236, 0.4187103349, 
    0.4193680133, 0.4200252594, 0.4206820739, 0.4213384573, 0.4219944101, 0.4226499329, 0.4233050262, 0.4239596907, 
    0.4246139269, 0.4252677354, 0.4259211167, 0.4265740713, 0.4272265999, 0.4278787029, 0.4285303810, 0.4291816347, 
    0.4298324646, 0.4304828711, 0.4311328549, 0.4317824164, 0.4324315563, 0.4330802751, 0.4337285734, 0.4343764516, 
    0.4350239103, 0.4356709502, 0.4363175716, 0.4369637752, 0.4376095614, 0.4382549309, 0.4388998842, 0.4395444218, 
    0.4401885442, 0.4408322519, 0.4414755456, 0.4421184258, 0.4427608929, 0.4434029474, 0.4440445901, 0.4446858213, 
    0.4453266415, 0.4459670514, 0.4466070514, 0.4472466421, 0.4478858240, 0.4485245976, 0.4491629634, 0.4498009219, 
    0.4504384738, 0.4510756194, 0.4517123593, 0.4523486940, 0.4529846241, 0.4536201500, 0.4542552723, 0.4548899914,
    0.4555243080, 0.4561582224, 0.4567917353, 0.4574248470, 0.4580575582, 0.4586898693, 0.4593217809, 0.4599532934, 
    0.4605844073, 0.4612151232, 0.4618454415, 0.4624753628, 0.4631048876, 0.4637340162, 0.4643627494, 0.4649910874, 
    0.4656190309, 0.4662465804, 0.4668737362, 0.4675004990, 0.4681268692, 0.4687528473, 0.4693784339, 0.4700036292, 
    0.4706284340, 0.4712528486, 0.4718768736, 0.4725005094, 0.4731237566, 0.4737466155, 0.4743690868, 0.4749911708, 
    0.4756128680, 0.4762341790, 0.4768551042, 0.4774756441, 0.4780957991, 0.4787155698, 0.4793349567, 0.4799539601, 
    0.4805725806, 0.4811908186, 0.4818086747, 0.4824261492, 0.4830432428, 0.4836599557, 0.4842762885, 0.4848922417, 
    0.4855078158, 0.4861230111, 0.4867378282, 0.4873522676, 0.4879663296, 0.4885800148, 0.4891933236, 0.4898062565, 
    0.4904188140, 0.4910309964, 0.4916428043, 0.4922542382, 0.4928652984, 0.4934759854, 0.4940862998, 0.4946962418, 
    0.4953058121, 0.4959150110, 0.4965238391, 0.4971322966, 0.4977403842, 0.4983481023, 0.4989554512, 0.4995624315, 
    0.5001690436, 0.5007752879, 0.5013811649, 0.5019866751, 0.5025918188, 0.5031965966, 0.5038010088, 0.5044050560, 
    0.5050087384, 0.5056120567, 0.5062150112, 0.5068176024, 0.5074198306, 0.5080216964, 0.5086232002, 0.5092243424, 
    0.5098251234, 0.5104255437, 0.5110256038, 0.5116253039, 0.5122246447, 0.5128236264, 0.5134222496, 0.5140205147, 
    0.5146184220, 0.5152159721, 0.5158131653, 0.5164100021, 0.5170064828, 0.5176026080, 0.5181983781, 0.5187937934, 
    0.5193888544, 0.5199835616, 0.5205779152, 0.5211719158, 0.5217655638, 0.5223588596, 0.5229518036, 0.5235443962, 
    0.5241366378, 0.5247285289, 0.5253200699, 0.5259112612, 0.5265021032, 0.5270925962, 0.5276827408, 0.5282725374, 
    0.5288619863, 0.5294510879, 0.5300398427, 0.5306282511, 0.5312163134, 0.5318040302, 0.5323914017, 0.5329784284, 
    0.5335651107, 0.5341514491, 0.5347374438, 0.5353230954, 0.5359084041, 0.5364933705, 0.5370779949, 0.5376622777, 
    0.5382462193, 0.5388298202, 0.5394130806, 0.5399960011, 0.5405785819, 0.5411608236, 0.5417427264, 0.5423242908, 
    0.5429055172, 0.5434864060, 0.5440669575, 0.5446471722, 0.5452270505, 0.5458065927, 0.5463857992, 0.5469646704, 
    0.5475432067, 0.5481214085, 0.5486992762, 0.5492768101, 0.5498540107, 0.5504308784, 0.5510074134, 0.5515836162, 
    0.5521594873, 0.5527350268, 0.5533102354, 0.5538851132, 0.5544596608, 0.5550338784, 0.5556077665, 0.5561813255, 
    0.5567545557, 0.5573274574, 0.5579000312, 0.5584722772, 0.5590441960, 0.5596157879, 0.5601870533, 0.5607579925, 
    0.5613286059, 0.5618988939, 0.5624688569, 0.5630384952, 0.5636078092, 0.5641767993, 0.5647454658, 0.5653138091, 
    0.5658818295, 0.5664495275, 0.5670169034, 0.5675839576, 0.5681506904, 0.5687171022, 0.5692831933, 0.5698489642, 
    0.5704144152, 0.5709795466, 0.5715443588, 0.5721088522, 0.5726730271, 0.5732368839, 0.5738004229, 0.5743636446, 
    0.5749265492, 0.5754891371, 0.5760514087, 0.5766133643, 0.5771750043, 0.5777363290, 0.5782973389, 0.5788580342, 
    0.5794184152, 0.5799784825, 0.5805382362, 0.5810976768, 0.5816568045, 0.5822156199, 0.5827741231, 0.5833323146, 
    0.5838901946, 0.5844477636, 0.5850050219, 0.5855619699, 0.5861186078, 0.5866749360, 0.5872309550, 0.5877866649, 
    0.5883420662, 0.5888971592, 0.5894519442, 0.5900064216, 0.5905605918, 0.5911144550, 0.5916680116, 0.5922212620, 
    0.5927742064, 0.5933268453, 0.5938791789, 0.5944312076, 0.5949829318, 0.5955343517, 0.5960854677, 0.5966362802, 
    0.5971867894, 0.5977369958, 0.5982868995, 0.5988365011, 0.5993858007, 0.5999347988, 0.6004834957, 0.6010318917, 
    0.6015799870, 0.6021277822, 0.6026752774, 0.6032224730, 0.6037693694, 0.6043159669, 0.6048622657, 0.6054082663, 
    0.6059539689, 0.6064993738, 0.6070444815, 0.6075892922, 0.6081338062, 0.6086780239, 0.6092219456, 0.6097655716, 
    0.6103089023, 0.6108519378, 0.6113946787, 0.6119371251, 0.6124792775, 0.6130211361, 0.6135627012, 0.6141039732, 
    0.6146449524, 0.6151856391, 0.6157260336, 0.6162661362, 0.6168059473, 0.6173454671, 0.6178846961, 0.6184236344, 
    0.6189622824, 0.6195006404, 0.6200387087, 0.6205764877, 0.6211139777, 0.6216511789, 0.6221880916, 0.6227247163, 
    0.6232610531, 0.6237971024, 0.6243328646, 0.6248683398, 0.6254035285, 0.6259384309, 0.6264730473, 0.6270073781, 
    0.6275414235, 0.6280751838, 0.6286086594, 0.6291418506, 0.6296747576, 0.6302073808, 0.6307397204, 0.6312717768, 
    0.6318035503, 0.6323350412, 0.6328662497, 0.6333971762, 0.6339278209, 0.6344581842, 0.6349882664, 0.6355180677, 
    0.6360475885, 0.6365768291, 0.6371057897, 0.6376344706, 0.6381628722, 0.6386909948, 0.6392188385, 0.6397464038, 
    0.6402736910, 0.6408007002, 0.6413274318, 0.6418538862, 0.6423800635, 0.6429059641, 0.6434315883, 0.6439569364, 
    0.6444820086, 0.6450068052, 0.6455313266, 0.6460555730, 0.6465795447, 0.6471032421, 0.6476266653, 0.6481498146, 
    0.6486726905, 0.6491952930, 0.6497176226, 0.6502396795, 0.6507614641, 0.6512829765, 0.6518042170, 0.6523251860, 
    0.6528458838, 0.6533663105, 0.6538864666, 0.6544063522, 0.6549259677, 0.6554453134, 0.6559643894, 0.6564831962, 
    0.6570017339, 0.6575200029, 0.6580380034, 0.6585557358, 0.6590732002, 0.6595903970, 0.6601073265, 0.6606239889, 
    0.6611403844, 0.6616565135, 0.6621723763, 0.6626879731, 0.6632033042, 0.6637183699, 0.6642331704, 0.6647477060, 
    0.6652619771, 0.6657759838, 0.6662897264, 0.6668032052, 0.6673164205, 0.6678293726, 0.6683420616, 0.6688544880, 
    0.6693666519, 0.6698785536, 0.6703901934, 0.6709015716, 0.6714126884, 0.6719235441, 0.6724341390, 0.6729444732, 
    0.6734545472, 0.6739643611, 0.6744739153, 0.6749832099, 0.6754922453, 0.6760010217, 0.6765095394, 0.6770177986, 
    0.6775257997, 0.6780335427, 0.6785410282, 0.6790482562, 0.6795552270, 0.6800619410, 0.6805683984, 0.6810745993, 
    0.6815805442, 0.6820862332, 0.6825916666, 0.6830968447, 0.6836017677, 0.6841064359, 0.6846108495, 0.6851150089, 
    0.6856189141, 0.6861225656, 0.6866259636, 0.6871291082, 0.6876319999, 0.6881346387, 0.6886370251, 0.6891391592, 
    0.6896410412, 0.6901426715, 0.6906440503, 0.6911451779, 0.6916460544, 0.6921466802, 0.6926470555
};

// Psychoacoustic Model variables
// masker structure
struct Masker_List {
    short index[25];    // masker index
    float power[25];    // masker power level
};
struct Masker_List Ton_list;
struct Masker_List nTon_list;
float scf[BANDSIZE];        // scale factors
float Ls[32] = {0};         // sound pressure
float LTtm[25][102] = {0};  // individual tonal mask threshold
float LTnm[25][102] = {0};  // individual non-tonal mask threshold
float LTg[102] = {0};       // gloabal mask threshold
float LTmin[BANDSIZE] = {0};// min. mask threshold
float SMR[BANDSIZE] = {0};  // signal-mask-ratio
float MNR[BANDSIZE] = {0};  // mask-noise-ratio
short BSCF[BANDSIZE] = {0}; // bit values for scale factors
short BSPL[BANDSIZE] = {0}; // bit values for subbands
short flag[NFFT/2] = {0};   // flag if tonal or non-tonal masker
short ton_ind = 0;          // index
short nton_ind = 0;         // index
short Ton_list_leng = 0;    // length of tonals
short compr_rate;           // bit-rate depending compression rate
short cb;                   // callable number of bits at given bit-rate
float adb;                  // currently available data bits
short fft_done = 0, fb_done = 0; // busy flags
short first_FRAME = 1;
short BSCF_done = 0;
short band_cnt = 0;
float scf_rx[BANDSIZE];
short tot_bits = 0;
short cnt_out=0,out_flag = 0;
#ifdef FIX_FOR_REAL_BITRATE_REDUCTION
uint8_t FRAME1[16*1024*1024];//sizeof(short)*2*BUFLEN] = {0};
uint8_t *pFRAME1;
uint8_t *pFRAME1_write;
#else
short FRAME1[2*BUFLEN] = {0};
short *pFRAME1;
#endif
uint32_t cnt_FRAME_fill = 0;
short index_nTon = 0;

// lookup table for mid-thread quantization 
float exp2LUT[14]=
{ 
    0.500000000000000, 0.250000000000000, 0.125000000000000, 0.062500000000000, 0.031250000000000, 0.015625000000000,  0.007812500000000,
    0.003906250000000, 0.001953125000000,  0.000976562500000, 0.000488281250000, 0.000244140625000, 0.000122070312500, 0.000061035156250
};

// Filterbank variables
short tw_sample;        // counter for 12 subband samples
float delay[64]={0};
float filt_outA[768]={0}, filt_outB[768]={0};    // 64 polyphases * 12 samples
float M[32][64],T[32][64], S[32][12]={0};
float teta=0, maxValue=0;
float *y1_part_fb, *pWork_fb;
float INT_y,INT_y1,INT_y2;
float out_delay[64];
float y_rx[12][BANDSIZE];
short count_poly = 0,count_12_synthese = 0, ind_calc_overlap, ORIGINAL = 0;
float Out1[768],Out2[768];    // 64 Polyphasen 12 Sample Output buffer
float *pOut1;
float *pOut2;
short count_fb = 0, count_fft = 63, count_12 = 0;

// some control variables
short ki=0;
short cnt_=0;
short cnt_samp=0;        // counter for 384 samples
short count_INT=0, i_m=0, k_m=0;
float inL=0,inR=0;       // current left and right sample
#if 0
void init_table(void);
#endif
float fir_filter(float delays[], float coe[], short N_delays, float x_n);
int calc_polyphase_fb(int16_t *input, int channels);
void calc_cos_mod(void);
void cfftr2_dit( float *x, float *w, short N);
void bitrev(float *xs, short *index, int n);
void digitrev_index(short *index, int n, int radix);
void calc_mag_log(void);
void alpha_beta_mag(void);
void calc_overlap(void);
void scalefactor(void);
void sound_pressure(void);
void find_tonals(void);
void dec_tonals(int bitrate);
void indiv_mask_th(void);
void global_mask_th(void);
void min_mask_th(void);
void calc_SMR(void);
void bit_alloc(int bitrate, int fs, int bps);
int quantization_and_tx_frame(uint32_t byteOffset);

void usage(const char* name)
{
    fprintf(stderr, "%s in.wav out.mp1 bitrate\n", name);
}

int main(int argc, char *argv[])
{
    const char *infile, *outfile;
    FILE *out;
    void *wavIn;
    int format, sample_rate, channels, bits_per_sample;
    uint32_t data_length;
    int input_size;
    uint8_t* input_buf;
    int16_t* convert_buf;
    
    if (argc - optind < 2)
    {
        fprintf(stderr, "Error: not enough parameter provided\n");
        usage(argv[0]);
        return 1;
    }
    
    infile = argv[optind];
    outfile = argv[optind + 1];

    if (argc - optind > 2)
    {
        // TODO: try float for bitrate instead of int
        bitrate = atoi(argv[optind + 2]);
    }

    wavIn = wav_read_open(infile);
    if (!wavIn)
    {
        fprintf(stderr, "Unable to open wav file %s\n", infile);
        return 1;
    }
    if (!wav_get_header(wavIn, &format, &channels, &sample_rate, &bits_per_sample, &data_length))
    {
        fprintf(stderr, "Bad wav file %s\n", infile);
        return 1;
    }
    if (format != 1)
    {
        fprintf(stderr, "Unsupported WAV format %d\n", format);
        return 1;
    }

//#define PLOT_DATA
#ifndef PLOT_DATA
    input_size = data_length;
    input_buf = (uint8_t*) malloc(input_size);
    convert_buf = (int16_t*) malloc(input_size);

    if (input_buf == NULL || convert_buf == NULL)
    {
        fprintf(stderr, "Unable to allocate memory for buffer\n");
        return 1;
    }

    int read = wav_read_data(wavIn, input_buf, input_size);
#else // for plot
    data_length = BUFLEN * sizeof(short);
    input_size = data_length;
    input_buf = (uint8_t*) malloc(input_size);
    convert_buf = (int16_t*) malloc(input_size);

    int read = data_length;
    /* define input signal for simulation    */
    for(int ii = 0; ii < BUFLEN; ii++)
    {
        //convert_buf[ii]=(short)((0.24*cos(2*PI*ii/BUFLEN*65)+0.4*cos(2*PI*ii/BUFLEN*620)+0.24*cos(2*PI*ii/BUFLEN*320))*32700);
        convert_buf[ii]=(short)((0.5*cos(2*PI*ii/BUFLEN*4)+0.125*cos(2*PI*ii/BUFLEN*12)+0.25*cos(2*PI*ii/BUFLEN*60))*32767);
    }
#endif

    out = fopen(outfile, "wb");
    if (!out) {
        perror(outfile);
        return 1;
    }
    
    printf("using bitrate = %d\n", bitrate);
    
    printf("data_length = %d\tread = %d\tinput_size = %d \n", data_length, read, input_size);
    printf("sample_rate = %d\tbits_per_sample = %d\tchannels = %d \n", sample_rate, bits_per_sample, channels);

    int numSamples = read/2;

    if(numSamples < BUFLEN)
    {
        fprintf(stderr, "Error: not enough samples provided by file. Must be at least %d\n", BUFLEN);
        return -1;
    }
#ifndef PLOT_DATA // undef for plot
    for(unsigned int n = 0; n < numSamples; n++)
    {
        const uint8_t* in = &input_buf[2*n];
        convert_buf[n] = in[0] | (in[1] << 8);
    }
#endif
    // create cos-mod-matrix 
    for(i_m=0;i_m<32;i_m++)
    {
        for(k_m=0;k_m<64;k_m++)
        {
            teta=(pow(-1,i_m))*PI/4;
            M[i_m][k_m]=2*(cos((i_m+0.5)*(k_m-511/2)*PI/32+teta));
            T[i_m][k_m]=2*(cos((i_m+0.5)*(k_m-511/2)*PI/32-teta));
        }
    }
    // some pointer references
    y1_part_fb = filt_outA;
    pWork_fb = filt_outB;
    pWork_fft = work_fft;
 
    pxFFT = xFFT;
    pxFFT_old = xFFT_old;
    pOut1 = Out1;
    pOut2 = Out2;
    pFRAME1 = FRAME1;
#ifdef FIX_FOR_REAL_BITRATE_REDUCTION
    pFRAME1_write = FRAME1;
#endif

    // create twiddle factors for radix2 fft
    for( i_m = 0 ; i_m < NFFT/RADIX ; i_m++ ) 
    {
        W[i_m].re = cos(2*PI*i_m/NFFT);          // real component of W
        W[i_m].im = sin(2*PI*i_m/NFFT);          // neg imag component
    }

    digitrev_index(iTwid, NFFT/RADIX, RADIX);    // produces index for bitrev() W
    bitrev((float*)W, iTwid, NFFT/RADIX);        // bit reverse W
    digitrev_index(iData, NFFT, RADIX);          // produces index for bitrev() Data

    // create hanning window for fft
    for(i_m=0; i_m<NFFT; i_m++)
    {
        hanning[i_m]=(1/0.54)*(0.5+0.5*cos(2*PI*(i_m-255)/(NFFT-1)));    // Hann Fenster mi_mt kompensieren der Grunddämpfung (sqrt(8/2)) ooder (sqrt(8/3)) ??? check it!!!
    }

#ifdef FIX_FOR_REAL_BITRATE_REDUCTION
    memset(pFRAME1, 0xFFFF, 2*BUFLEN);
#else
    for(i_m=0; i_m < (2*BUFLEN); i_m++)
    {
        pFRAME1[i_m] = 0xFFFF;    // init value Xmt value
    }
#endif

    /* Frame synchronisation sequence (can be used for one time header in future) */
#ifdef FIX_FOR_REAL_BITRATE_REDUCTION
    //memcpy(pFRAME1, syncWords, 2*sizeof(uint32_t));

#else
    pFRAME1[0]=0xAAAA;
    pFRAME1[1]=0xCCCC;
    pFRAME1[2]=0xF0F0;
    pFRAME1[3]=0xAAAA;
#endif

    uint32_t *table_Xmt;
    int32_t samples_offset = 0;
    int32_t nFrame = 0; // frame counter
    uint32_t valid_bits;
    
    while(1)
    {        
        nFrame++;
#ifndef FIX_FOR_REAL_BITRATE_REDUCTION
        uint32_t outbuf[BUFLEN+sizeof(syncWords)];

        table_Xmt = outbuf;
#endif
        count_fb = 0;        // reset FFT counter
        count_12 = 0;        // reset FB Counter
        count_poly = 0;      // reset polyphase analysis filterbank counter

        /* ANALYSIS FILTERBANK */
        if(calc_polyphase_fb(&convert_buf[samples_offset], channels))   /* calc polyphase components */
        {
            fprintf(stderr, "ERROR: calc_polyphase_fb failed\n");
            return -1;
        }

        calc_cos_mod();        /* calc cosinus modulation */

        /* FFT */
        calc_overlap();                /* create fft input vector with 128 overlapping samples */
        cfftr2_dit((float*)pxFFT, (float*)W, NFFT);    /* TI floating-point complex radix2 fft */
        bitrev((float*)pxFFT, iData, NFFT);    /* bit reverse W */

        alpha_beta_mag();            /* fast magnitude estimation */
        calc_mag_log();            /* fast log calculation for sound pressure */

        /* PSYCHOACOUSTIC MODEL */
        scalefactor();        /* determine scalefactor     */
        sound_pressure();    /* determine sound pressure    */
        find_tonals();        /* find masker */
        dec_tonals(bitrate);        /* decimate masker */
        indiv_mask_th();    /* determine individual masking threshold */
        global_mask_th();    /* determine glabal masking threshold */
        min_mask_th();        /* determine minimum masking threshold for each subband */
        calc_SMR();            /* calc signal- to mask-ratio */
        bit_alloc(bitrate, sample_rate, bits_per_sample);        /* dynamic bit allocation */

#ifdef PLOT_DATA// for plot
    printf("\n\ncompr_rate = %d, sample_rate = %d bits_per_sample = %d \n", compr_rate, sample_rate, bits_per_sample);    
    printf("\n\nfft \n");
    for(int ii=0; ii<256; ii++)
    {
        printf("%f " , magnitude_dB[ii] );
    }

    printf("\n\nLs \n");
    for(int ii=0; ii<32; ii++)
    {
        printf("%f " , Ls[ii] );
    }

    printf("\n\nSMR \n");
    for(int ii=0; ii<32; ii++)
    {
        printf("%f " , SMR[ii] );
    }

    printf("\n\nBSPL \n");
    for(int ii=0; ii<32; ii++)
    {
        printf("%d " , BSPL[ii] );
    }
#endif

        // offset for syncWords is neccesary for the first frame
        uint32_t writeOffset = 0;
        if(nFrame == 1)
        {
            writeOffset = sizeof(syncWords);
        }

        /* QUANTIZE SUBBAND SAMPLES*/
        valid_bits = quantization_and_tx_frame(writeOffset);    /* quantize 32*12 subband samples */
#ifdef FIX_FOR_REAL_BITRATE_REDUCTION

        uint32_t residual_bits = valid_bits % 8; 

        if(residual_bits)
        {
            // TODO: is zero padding okay?
            printf("WARNING: number of bits to write (%d) not integer of a byte\n", valid_bits);
            valid_bits += (8-residual_bits); // zero padding
        }

        // write data
#ifdef DEBUG
        printf("write (%d) bits to output\n", valid_bits);
#endif

#if 0
        printf("write (0x%x)\n", pFRAME1_write[0]);
        printf("write (0x%x)\n", pFRAME1_write[1]);
        printf("write (0x%x)\n", pFRAME1_write[2]);
        printf("write (0x%x)\n", pFRAME1_write[3]);
        printf("write (0x%x)\n", pFRAME1_write[4]);
        printf("write (0x%x)\n", pFRAME1_write[5]);
        printf("write (0x%x)\n", pFRAME1_write[6]);
        printf("write (0x%x)\n", pFRAME1_write[7]);
        return -1;
#endif
        if(valid_bits/8 > (16*1024*1024))
        {
            printf("WARNING: you might want to check for buffer overflow\n");
        }
#if 0
        memcpy((void*)table_Xmt, (void*)pFRAME1_write, /*writeOffset+*/valid_bits/8);
        fwrite(outbuf, 1, /*writeOffset+*/valid_bits/8, out);
#endif
        //pFRAME1_write += valid_bits/8;
        //table_Xmt += valid_bits/8; // table_Xmt points to outbuf[384]
#else
        // write data 
        for(i_m=0; i_m < BUFLEN; i_m++)
        {
            table_Xmt[i_m] = (unsigned int)( (((unsigned int)pFRAME1[(i_m*2+1)]&0x0000FFFF)<<16) | (unsigned int)pFRAME1[(i_m*2)]&0x0000FFFF );
        }

        fwrite(outbuf, 1, BUFLEN*sizeof(uint32_t), out);
#endif
        samples_offset += BUFLEN*channels;

        //printf("\r[%d|%d]", nFrame, samples_offset);

        if((numSamples-samples_offset) < BUFLEN*channels)
        {
            break;
        }
    }    // end while(1)

#ifdef FIX_FOR_REAL_BITRATE_REDUCTION
    if(valid_bits/8 > (16*1024*1024))
    {
        printf("WARNING: Buffer too small for required bitstream\n");
    }

    fwrite(pFRAME1_write, 1, valid_bits/8, out);
#endif
    printf("\n");

    free(input_buf);
    free(convert_buf);
    fclose(out);
    wav_read_close(wavIn);

    return 0;

}

#if 0
/* buffer initialization for Xmt and Rcv */
void init_table(void)
{
    short ind=0;
    // TODO: Use memset
    for(ind=0;ind<BUFLEN;ind++)
    {
        table_Xmt[ind] = 0;
    }
}
#endif
