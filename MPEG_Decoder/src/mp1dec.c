//-----------------------------------------------------------------------
// Description:  MPEG-1 Layer I Decoder
//
// Author:       Kim Radmacher
// Date:         
//               19-Feb-15
// Date of final version:
//               23-Jun-15
// Date of porting to x86 and refactoring:
//               06-Mae-22
//-----------------------------------------------------------------------

/* some includes*/
#include "analysis_coeffs_float.h"    // filterbank coef
#include "mpeg_tables.h"            // mpeg-1 tables
#include "wavwriter.h"
#if 0
#include "dsk6713.h"                // dsk support file
#include "c6713dskinit.h"            // codec-DSK support file
#endif
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#if defined(_MSC_VER)
#include <getopt.h>
#else
#include <unistd.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif
#include "wavwriter.h"
#ifdef __cplusplus
}
#endif

//#define DEBUG

#define BUFLEN          384    // buffer length
#define PI              3.14159265358979
#define BANDSIZE        32    // 32 filter subbands
#define COMPRESSED      1
#define ORIGINAL        2

#if 0
/* c6713 timer, interrupt, gpio headder, edma, mcbsp */
#include <csl.h>
#include <csl_timer.h>
#include <csl_irq.h>
#include <csl_gpio.h>
#include <csl_gpiohal.h>
#include <csl_edma.h>
#include <csl_mcbsp.h>
#endif

unsigned int table_Xmt[BUFLEN];    // EDMA Xmt buffer
unsigned int table_Rcv[BUFLEN];    // EDMA Rcv buffer
short direction=-1;                // EDMA Interrupt Rcv=0, Xmt=1, default=-1
short cnt_samp=0;
short count_fb=0, count=0, count_12=0, cnt_out=0, count_12_synthese=0;    // counter for filterbank and polayphase rotating switch

/***** some control variables *****/
int i_m = 0,k_m = 0;

/***** Filterbank variables *****/
float M[32][64],T[32][64], S[32][12]={0};
float teta=0;
float INT_y,INT_y1,INT_y2;    // current polyphase outputs
float out_delay[64];          // polyphase component outputs
float y_rx[12][BANDSIZE];     // demultiplexed subbands
float Out1[768];              // 64 polyphases * 12 samples=768
float *pOut1;                 // pointer reference

/***** Psychoacoustic Model variables *****/
short FRAME1[448]={0};        // Rcv Frame
short *pFRAME1;               // pointer reference
short BSPL_rx[BANDSIZE];      // received bit values for subbands
float scf_rx[BANDSIZE];       // received bit values for scalefactors
short tot_bits_rx;            // number of received bits
short cnt_FRAME_read=0;       // array index for received data
short start_decoding=0;       // start decoding flag
short buffer[BUFLEN]={0};     // McBSP buffer
short start_frame_offset=0;   // start sequence to data offset
short start_found=0;          // flag for correct star sequence found

#if 0
/* McBSP to AIC23 channel left+right union */
union {
    Uint32 both;
    short channel[2];
} McBSP_data;

/* gpio pin toggle for timing measurment */
GPIO_Handle gpio_handle;

short toggle=0;
/* EDMA channel handles */
EDMA_Handle hEdmaXmt;
EDMA_Handle hEdmaRcv;
EDMA_Handle hEdmaReloadXmt;
EDMA_Handle hEdmaReloadRcv;
/* EDMA configuration */
EDMA_Config gEdmaConfigXmt;
EDMA_Config gEdmaConfigRcv;
/* McBSP handles */
MCBSP_Handle hMcbsp0;
MCBSP_Handle hMcbsp1;
/* McBSP configuration */
MCBSP_Config Mymcbsp0;
MCBSP_Config Mymcbsp1;

/*  Function Prototypes */
void initEdma0(void);
#endif

void init_table(void);

#if 0
void initMcbsp0();
void initMcbsp1();
#endif

void swapPointer_fb(float** inp, float** wrk);
float fir_filter(float delays[], float coe[], short N_delays, float x_n);
void calc_cos_mod_synthese(void);
void calc_polyphase_synthese_fb(void);
void rx_frame(void);

#if 0
/************************************************** EDMA ISR ************************************************************************/
interrupt void edma_complete_int (void) {
    if (EDMA_CIPR & 1<<EDMA_CHA_REVT0){
        EDMA_CIPR = (1<<EDMA_CHA_REVT0);    // clear channel interrupt pending bit for Rcv
        direction = 0;                        // Rcv flag
    }
    if (EDMA_CIPR & 1<<EDMA_CHA_XEVT1) {
        EDMA_CIPR = (1<<EDMA_CHA_XEVT1);    // clear channel interrupt pending bit Xmt
        direction = 1;                        // Xmt flag
    }
    return;
}
#endif

void usage(const char* name)
{
    fprintf(stderr, "%s in.mp1 out.wav\n", name);
}

int main(int argc, char *argv[])
{
    const char *infile, *outfile;
    FILE *in;
    void *wavOut;
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

    in = fopen(infile, "rb");
    if (!in) {
        perror(infile);
        return 1;
    }

    sample_rate = 44100;
    bits_per_sample = 16*2;
    channels = 1;//2;
    wavOut = wav_write_open(outfile, sample_rate, bits_per_sample, channels);

    if (!wavOut)
    {
        fprintf(stderr, "Unable to open wav file for writing %s\n", infile);
        return 1;
    }

#if 0
    IRQ_globalDisable();                   //disable interrupts
    /* Select McBsp 0 connection to daughtercard on J3 */
    DSK6713_rset(DSK6713_MISC , MCBSP1SEL);    // McBSP0 routes to Daughter Card
    IRQ_globalDisable();                       //disable interrupts
    CSL_init();                                // init chip support library
    /* Initialize the LED and DIP switch modules of the BSL */
    DSK6713_DIP_init();    // init DIP Switches
    /* gpio config for timing measurment */
    gpio_handle = GPIO_open(GPIO_DEV0,GPIO_OPEN_RESET);
    GPIO_Config gpio_config = {
              0x00000000, /* gpgc - GPIO Global Control register */
              0xFFFFFFFF, /* gpen - GPIO Enable register */
              0xFFFFFFFF, /* gdir - GPIO Direction register -> 0 : inp, 1 : outp */
              0xFFFFFFFF, /* gpval - GPIO Value register */
              0x00000000, /* gphm - GPIO0 High Mask register -> all interrupts disabled for io pins */
              0x00000000, /* gplm - GPIO1 High Mask register -> all interrupts to cpu or edma disabled  */
              0x00000000  /* gppol - GPIO0 Interrupt Polarity register -> default state */
      };
      GPIO_config(gpio_handle,&gpio_config);
#endif    
    /* create cos-mod-matrix */
    for(i_m=0; i_m<32; i_m++)
    {
        for(k_m=0; k_m<64; k_m++)
        {
            teta = (pow(-1,i_m))*PI/4;
            M[i_m][k_m] = 2*(cos((i_m+0.5)*(k_m-511/2)*PI/32+teta));
            T[i_m][k_m] = 2*(cos((i_m+0.5)*(k_m-511/2)*PI/32-teta));
        }
    }
   
    /* pointer references */
    pOut1        = Out1;
    pFRAME1        = FRAME1;

    /* clear Frame data */
    for(i_m=0; i_m<(448); i_m++)
    {
        pFRAME1[i_m]=0x0000;    // init value - Transmission takes 8 ms at 192 kbps
    }

#if 0
    /* clear and disable EDMA channel 12 and 15 events */
    EDMA_EER &= ~((1<<EDMA_CHA_REVT0) | (1<<EDMA_CHA_XEVT1));
    EDMA_ECR = ((1<<EDMA_CHA_REVT0) | (1<<EDMA_CHA_XEVT1));
#endif

    init_table();     /* init Tables */

#if 0
    initMcbsp1();     /* Xtm data through DAC */
    initMcbsp0();    /* Rcv data from Daughter Card Connector */
    /* set EDMA channel Reg.-Bits */
    EDMA_EER |= ((1<<EDMA_CHA_REVT0) | (1<<EDMA_CHA_XEVT1));
    EDMA_ESR = ((1<<EDMA_CHA_REVT0) | (1<<EDMA_CHA_XEVT1));
    /* enable Channel interrupt No. 13 and 14 */
    EDMA_CIER |= ((1 << EDMA_CHA_REVT0) | (1 << EDMA_CHA_XEVT1));
    /* create EDMA Interrupt */
    IRQ_map(EDMA_CHA_REVT0, 8);            // map McBSP1 Rcv to INT8 - EDMA
    IRQ_reset(EDMA_CHA_REVT0);            // reset codec INT8
    IRQ_enable(EDMA_CHA_REVT0);            // enable CODEC event Rcv INT8
    IRQ_set(EDMA_CHA_REVT0);            // manually start the first interrupt
    IRQ_map(EDMA_CHA_XEVT1, 8);            // map McBSP1 Xmt to INT8 - EDMA
    IRQ_reset(EDMA_CHA_XEVT1);            // reset codec INT8
    IRQ_enable(EDMA_CHA_XEVT1);            // enable CODEC event Xmt INT8
    IRQ_set(EDMA_CHA_XEVT1);            // manually start the first interrupt
    IRQ_globalEnable();                   // globally enable interrupts
    IRQ_nmiEnable();                      // enable NMI interrupt
#endif

    start_frame_offset = 0;

    while(1)
    {

        int readBytes = fread(table_Rcv, 1, BUFLEN/**channels*/*sizeof(int32_t), in);
        if(readBytes == 0)
        {
            printf("File seems to end\n");
            break;    
        }
#if 0        
        printf("Read %d Bytes from file\n", readBytes);
#endif

//        if(direction != -1)
        {        // Rcv or Xmt Interrupt pending
//            if(direction == 0)
            {        // Rcv through SPI
#if 0
                /* check for changed start sequence */
                if( (table_Rcv[start_frame_offset] != 0xCCCCAAAA) && (table_Rcv[start_frame_offset+1] != 0xAAAAF0F0) && start_found != ORIGINAL )
                    start_found=0;
                if( (table_Rcv[start_frame_offset] != 0xAAAAC0C0) && (table_Rcv[start_frame_offset+1] != 0xF0F0AAAA) && start_found != COMPRESSED )
                    start_found=0;
#endif
#if 1
                //start_frame_offset = 2;
                //start_found=COMPRESSED;
#endif

                /* find start sequence and the offset to data */
                if( start_found==0 )
                {
                    start_frame_offset=0;
                    start_decoding=0;
                    while( start_found==0 )
                    {
                        if( (table_Rcv[start_frame_offset]==0xCCCCAAAA) && (table_Rcv[start_frame_offset+1]==0xAAAAF0F0) )
                        {
                            printf("Preamble for COMPRESSED found\n");
                            start_found=COMPRESSED;    // start sequence for compressed data
                        }
                        else if( (table_Rcv[start_frame_offset]==0xAAAAC0C0) && (table_Rcv[start_frame_offset+1]==0xF0F0AAAA) )
                        {
                            printf("Preamble for ORIGINAL found\n");
                            start_found=ORIGINAL;    // start sequence for original data samples
                        }
                        else
                        {
                            printf("start_frame_offset = %d\n", start_frame_offset);
                            start_frame_offset++;    // increment offset
                        }
                    }

                    if( (start_frame_offset > 157) )
                    {    //    if offset > 157, data is too long to fit into table_Rcv array buffer
                        start_frame_offset=0;            // reset start offset
                        start_found=0;                    // reset start found flag
                        fprintf(stderr, "start_frame_offset = %d\n", start_frame_offset);
                        return -1;
#if 0
                        Mymcbsp0.spcr = 0x00401800;        // reset McBSP0
                        MCBSP_config(hMcbsp0, &Mymcbsp0);// update McBSP0 configs
                        for(i_m=0;i_m < 35000;i_m++);     // just wait about 3.5 ms for another Rcv data timing
                        Mymcbsp0.spcr = 0x00411801;        // take McBSP0 out of reset
                        MCBSP_config(hMcbsp0, &Mymcbsp0);// update McBSP0 configs
#endif
                    }
                    else
                    { // copy received data into Frame
                        for(i_m=0; i_m < (224); i_m++)
                        {
#if DEBUG
                            printf("%d\n", table_Rcv[start_frame_offset+2+i_m]);
#endif
                            pFRAME1[i_m*2] = (short)(0x0000FFFF & table_Rcv[start_frame_offset+2+i_m]);            // lower 16 Bit from rcv 32 Bit
                            pFRAME1[i_m*2+1] = (short)((0xFFFF0000 & table_Rcv[start_frame_offset+2+i_m])>>16);    // upper 16 Bit from rcv 32 Bit
                        }
                        if(start_found==COMPRESSED)
                        {    // compressed start sequence
#if 0
                            /* toggle gpio pin for timing measurment */
                            toggle=1;
                            GPIO_pinWrite(gpio_handle,(Uint32)4, (Uint32) toggle);
#endif
                            /* Receive data and demultiplex 384 subband samples */
                            rx_frame();
                            /* Synthesis FILTERBANK */
                            calc_cos_mod_synthese();        // cosinus modulation
                            count_12_synthese=0;
                            calc_polyphase_synthese_fb();    // polyphase filterbank

#if 0
                            /* toggle gpio pin for timing measurment */
                            toggle=0;
                            GPIO_pinWrite(gpio_handle,(Uint32)4, (Uint32) toggle);
#endif
                        }
                        start_decoding=1;                // begin decoding
                    }
                }
                else
                { // copy received data into Frame
                    for(i_m=0; i_m < (224); i_m++)
                    {
                        pFRAME1[i_m*2] = (short)(0x0000FFFF & table_Rcv[start_frame_offset+2+i_m]);            // lower 16 Bit from rcv 32 Bit
                        pFRAME1[i_m*2+1] = (short)((0xFFFF0000 & table_Rcv[start_frame_offset+2+i_m])>>16);    // upper 16 Bit from rcv 32 Bit
                    }
                    if(start_found==COMPRESSED)
                    {    // compressed start sequence
#if 0
                        /* toggle gpio pin for timing measurment */
                        toggle=1;
                        GPIO_pinWrite(gpio_handle,(Uint32)4, (Uint32) toggle);
#endif
                        /* Receive data and demultiplex 384 subband samples */
                        rx_frame();
                        /* Synthesis FILTERBANK */
                        calc_cos_mod_synthese();        // cosinus modulation
                        count_12_synthese=0;
                        calc_polyphase_synthese_fb();    // polyphase filterbank

#if 0  
                        /* toggle gpio pin for timing measurment */
                        toggle=0;                      
                        GPIO_pinWrite(gpio_handle,(Uint32)4, (Uint32) toggle);
#endif
                    }
                    start_decoding=1;                // begin decoding
                }
#if 0
                if(direction==1)    // if Xmt EDMA Interrupt is pending
                    direction=1;    // hold this direction
                else                // otherwise
                    direction=-1;    // reset direction
#endif
            }
#if 0
            /* Xmt data do DAC - Output decoded audio samples */
            if(direction == 1 && start_decoding == 1)
#else
            if(start_decoding == 1)
#endif
            {
                start_decoding=0;
                /* copy data to Xmt_EDMA_Buffer */
                if(start_found == COMPRESSED)
                {    /* Compressed */
                    for(i_m=0; i_m<BUFLEN; i_m++)
                    {
#if 0
                        McBSP_data.channel[LEFT]  = buffer[i_m];
                        McBSP_data.channel[RIGHT] = buffer[i_m];
                        table_Xmt[i_m]=McBSP_data.both;
#else
                        table_Xmt[i_m] = ((int32_t)buffer[i_m]<<16) & (int32_t)buffer[i_m];

                        int16_t oL = (int16_t)buffer[i_m];
                        int16_t oR = (int16_t)buffer[i_m];
                        wav_write_data(wavOut, (unsigned char*)&oL, 2);
                        wav_write_data(wavOut, (unsigned char*)&oR, 2);
#endif
                    }
                }
                else if(start_found == ORIGINAL)
                {/* ORIGINAL */
                    for(i_m=0; i_m<BUFLEN; i_m++)
                    {
#if 0                    
                        McBSP_data.channel[LEFT]  = pFRAME1[i_m];//(short)(0x0000FFFF & table_Rcv[start_frame_offset+2+i_m]);
                        McBSP_data.channel[RIGHT] = McBSP_data.channel[LEFT];
                        table_Xmt[i_m]=McBSP_data.both;
#else
                        table_Xmt[i_m] = ((int32_t)buffer[i_m]<<16) & (int32_t)buffer[i_m];

                        int16_t oL = (int16_t)buffer[i_m];
                        int16_t oR = (int16_t)buffer[i_m];
                        wav_write_data(wavOut, (unsigned char*)&oL, 2);
                        wav_write_data(wavOut, (unsigned char*)&oR, 2);
#endif
                    }
                }
#if 0
                if(direction==0)    // if Rcv EDMA Interrupt is pending
                    direction=0;    // hold this direction
                else                // otherwise
                    direction=-1;    // reset direction
#endif
            }
#if 0
            /* reset direction if start decoding reseted */
            if(direction == 1 && start_decoding == 0)
                direction=-1;

            // clear any pending EDMA complete interrupt events
            ICR = (1 << 8);
            EDMA_CIPR = ((1<<EDMA_CHA_REVT0) | (1 << EDMA_CHA_XEVT1));
#endif
        }
    } // end while(1)

    wav_write_close(wavOut);
    fclose(in);

    return 0;
}

void swapPointer_fb(float** inp, float** wrk)
{
    float *tmp=*inp;
    *inp=*wrk;
    *wrk=tmp;
}

/* EDMA variables initialization for Xmt and Rcv */
void init_table(void){
    short ind=0;
    for(ind=0;ind<BUFLEN;ind++)
    {
        table_Xmt[ind] = 0;
        table_Rcv[ind] = 0;
        buffer[ind] = 0;
    }
}

#if 0
/* Configure EDMA SPI receive channel */
void initEdma0(void){
    // get hEdmaXmt handle and reset channel for McBSP1 writes
    hEdmaRcv = EDMA_open(EDMA_CHA_REVT0, EDMA_OPEN_RESET);
    // Get the address of DR for McBSP0
    gEdmaConfigRcv.src = MCBSP_getRcvAddr(hMcbsp0);
    gEdmaConfigRcv.dst = (Uint32)&table_Rcv;
    gEdmaConfigRcv.opt = 0x203D0002;    // 001=HIGH PRIO | 00=32BIT | 0=1DIM | 00=SUM_NONE | 0=1DIM | 01=DUM_INC | 1=TCINT | 1101=INTERRUPT->EDMA_CHA_REVT0 | 00000000000000 | 1=LINK | 0=ELE_SYNC;
    gEdmaConfigRcv.cnt = (Uint32)BUFLEN;
    gEdmaConfigRcv.rld = (Uint32)BUFLEN;
    gEdmaConfigRcv.idx = 0x00000000;
    EDMA_config(hEdmaRcv, &gEdmaConfigRcv);
    // get handle for reload table
    hEdmaReloadRcv = EDMA_allocTable(-1);
    // Configure the Rcv reload table
    EDMA_config(hEdmaReloadRcv, &gEdmaConfigRcv);
    // link back to table start
    EDMA_link(hEdmaRcv,hEdmaReloadRcv);
    EDMA_link(hEdmaReloadRcv, hEdmaReloadRcv);
    // enable EDMA channel
    EDMA_enableChannel(hEdmaRcv);
}
/* McBSP0 configurations to Rcv data through Daughter Card Connector J3 */
void initMcbsp0(void){
    /* Open the codec data McBSP */
    hMcbsp0 = MCBSP_open(MCBSP_DEV0, MCBSP_OPEN_RESET);
    Mymcbsp0.spcr = 0x00000000;    // RESET STATE: FRST=GRST=XRST=RRST=0
    MCBSP_config(hMcbsp0, &Mymcbsp0);
    // Receive Control Register (RCR):
    Mymcbsp0.rcr  = 0x000000A0;    // 0 Databit Delay and 32 Bit Data receive (16 Bit change A to 4)
    // Transmit Control Register (XCR):
    Mymcbsp0.xcr  = 0x000000A0;    // 0 Databit Delay and 32 Bit Data receive (16 Bit change A to 4)
    // Sample Rate Generator Register (SRGR):
    Mymcbsp0.srgr = 0x20000000;    // internal CLK from CPU without CLK-Divider
    // Multichannel Control Register (MCR):
    Mymcbsp0.mcr  = 0x0;    // DEFAULT
    // Receive Channel Enable Register (RCER):
    Mymcbsp0.rcer = 0x0;    // DEFAULT
    // Transmit Channel Enable Registers (XCER):
    Mymcbsp0.xcer = 0x0;    // DEFAULT
    // Pin Control Register (PCR):
    Mymcbsp0.pcr  = 0x0F;    // FS and CLK as inputs
    MCBSP_config(hMcbsp0, &Mymcbsp0);
    Mymcbsp0.spcr = 0x00001800;    // SPI Mode CLKSTP=11b
    MCBSP_config(hMcbsp0, &Mymcbsp0);
    Mymcbsp0.spcr = 0x00401800;    // GRST=1 | SPI Mode CLKSTP=11b
    MCBSP_config(hMcbsp0, &Mymcbsp0);
    initEdma0();  // Initialize the EDMA controller (See below)
    // again Serial Port Control Register (SPCR):
    Mymcbsp0.spcr = 0x00411801;    // xrst=rrst=1 OUT OF RESET
    MCBSP_config(hMcbsp0, &Mymcbsp0);
}
/* Configure EDMA for McBSP1 Xmt Channel*/
void initEdma1(void){
 /* Configure transmit channel */
    // get hEdmaRcv handle and reset channel for McBSP1 writes
    hEdmaXmt = EDMA_open(EDMA_CHA_XEVT1, EDMA_OPEN_RESET);
    // Get the scr address of DR for McBSP1
    gEdmaConfigXmt.dst = MCBSP_getXmtAddr(hMcbsp1);
    gEdmaConfigXmt.src = (Uint32)&table_Xmt;
    gEdmaConfigXmt.opt = 0x211E0002;    // 001=HIGH PRIO | 00=32BIT | 0=1DIM | 01=SUM_NONE | 0=1DIM | 00=DUM_INC | 1=TCINT | 1110=INTERRUPT->EDMA_CHA_XEVT1 | 00000000000000 | 1=LINK | 0=ELE_SYNC;
    gEdmaConfigXmt.cnt = (Uint32)BUFLEN;
    gEdmaConfigXmt.rld = (Uint32)BUFLEN;
    gEdmaConfigXmt.idx = 0x00000000;    // DEFAULT
    // then configure the Xmt table
    EDMA_config(hEdmaXmt, &gEdmaConfigXmt);
    // get handle for reload table
      hEdmaReloadXmt = EDMA_allocTable(-1);
    // Configure the Xmt reload table
    EDMA_config(hEdmaReloadXmt, &gEdmaConfigXmt);
    // link back to table start
    EDMA_link(hEdmaXmt,hEdmaReloadXmt);
    EDMA_link(hEdmaReloadXmt, hEdmaReloadXmt);
    // enable EDMA channel
    EDMA_enableChannel(hEdmaXmt);
}
/* McBSP1 configurations to Xmt data from DAC */
void initMcbsp1(void){
    /* Open the codec data McBSP */
    hMcbsp1 = MCBSP_open(MCBSP_DEV1, MCBSP_OPEN_RESET);
    Mymcbsp1.spcr = 0x00000000;    // RESET STATE: FRST=GRST=XRST=RRST=0
    MCBSP_config(hMcbsp1, &Mymcbsp1);
    // Receive Control Register (RCR):
    Mymcbsp1.rcr  = 0x000000A0;    // 0 Databit Delay and 32 Bit Data receive (16 Bit change A to 4)
    // Transmit Control Register (XCR):
    Mymcbsp1.xcr  = 0x000000A0;    // 0 Databit Delay and 32 Bit Data receive (16 Bit change A to 4)
    // Sample Rate Generator Register (SRGR):
    Mymcbsp1.srgr = 0x00000000;    // CLK from AIC23 Codec
    // Multichannel Control Register (MCR):
    Mymcbsp1.mcr  = 0x0;    // DEFAULT
    // Receive Channel Enable Register (RCER):
    Mymcbsp1.rcer = 0x0;    // DEFAULT
    // Transmit Channel Enable Registers (XCER):
    Mymcbsp1.xcer = 0x0;    // DEFAULT
    // Pin Control Register (PCR):
    Mymcbsp1.pcr  = 0x03;        // FS, CLK as inputs from AIC23 Codec
    MCBSP_config(hMcbsp1, &Mymcbsp1);
    initEdma1();  // Initialize the EDMA controller (See below)
    // again Serial Port Control Register (SPCR):
    Mymcbsp1.spcr = 0x00010001;    // take McBSP1 out of reset
    MCBSP_config(hMcbsp1, &Mymcbsp1);
}
#endif
