//----------------------------------------------------------------------------
// Description        bit allocation algorithm
// Author:            Kim Radmacher
// Created on:
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

#define MAX_BITS_ALLOC 1
#define MORE_BITS_POSSIBLE 0
#define NO_MIN_MNR_AVAILABLE 10000.0

void bit_alloc(int bitrate, int fs, int bps)
{
//    short compr_rate;//=(48*16)/BITRATE;    // Kompressionsrate - darauf achten, dass diese immer ganzzahlig ist -> BITRATE= {48,96,128,192,256,384,768}
//    short cb;//=(12*32*16)/compr_rate;  // Verfuegbare Anzahl an Bits fuer den Frame bei gegebener Bitrate
    float bbal = 128;                    // Anzahl an Bits, die fuer die Bitzuweisung benoetigt werden
    float banc = 32;                      // Anzahl an Bits, die fuer die Zusatzinformationen benoetigt werden (adding header size)
    float ancillary_data=bbal+banc;    // Bitanzahl fuer Zusatzinformation
    float adb;    // Anzahl an Bits, die fuer Samples und SCF verfuegbar sind
    float bitleng = 0;                // aktuelle/momentane Anzahl an verwendeten Bits
    float snr_values[BANDSIZE];
    float MNRmin = 0;
    short n_band = 0, n_snr = 0;        // Laufvar.
    short Bit_done_List_index = 0;    // Counter fuer Teilbaender fuer die keine Bits mehr zugewiesen werden muessen/duerfen
    short Bit_done_List[BANDSIZE] = {0};    // Array fuer Liste der Subbaender, die bereits maximale Anzahl an Bits bekommen haben
    short index = 0;                    // index des Subband dem gerade Bitszugewiesen wird
    short SMR_over_0dB = 32;                // Fuer Anzahl der zu Codierten Teilbaender

//    SMR_over_0dB=0;
    compr_rate = (fs*bps)/(bitrate*1e3); // TODO: e.g. 48*16/196 = 3.9 -> 3, so using round?
    cb = (12*BANDSIZE*bps)/compr_rate;
    // Berechnung des MNR fuer gegebenes (minimales) SMR (Signal- zu Mithoerschwellenabstand)
    for(n_band=0;n_band < BANDSIZE;n_band++)
    {
//        MNR[n_band]=MIN_POWER;    // init to min power
        BSCF[n_band] = 0;            // init 0 Bits
        BSPL[n_band] = 0;            // init 0 Bits
        Bit_done_List[n_band] = MORE_BITS_POSSIBLE;// init 0, da fuer alle Teilbnder noch Bits zugewiesen werden knnen/mssen
        // zunchst werden 0 Bit zugewiesen -> snr=0
        snr_values[n_band] = SNR[0];        // init to lowest value of SNR ISO Tabelle

//        SMR_over_0dB++;        // increment Anzahl der relevanten Teilbnder (ber 0 dB)
//        if (SMR[n_band] > 0){    // SMR > 0 -> Wenn berhaupt Bits im Subband n_band zugewiesen werden mssen
            // finde nchst greren SNR im Vergl. zum SMR aus ISO Tabelle
//            for(n_snr=0;n_snr < 15;n_snr++){    // for all snr values
//                if( snr_values[n_band] > SNR[n_snr]-SMR[n_band] && SMR[n_band]-SNR[n_snr] >= 0 )//SNR[n_snr]-SMR[n_band] >= 0 )//
//                    snr_values[n_band]=SNR[n_snr];    // bernimm SNR
//            }
            MNR[n_band] = snr_values[n_band] - SMR[n_band];    // Berechne MNR
//        }
    }

    // Berechne solange noch Bits verfgbar sind
    // oder maximale Bitanzahl pro Subband erreicht (also maximiere MNR)
    bitleng = ancillary_data;    // init mit Bitanzahl fr Zusatzinfos
    adb = cb - bitleng;        // init verfgbare Bits adb
    while( ((adb-30)) >= 0 && Bit_done_List_index < SMR_over_0dB && MNRmin != NO_MIN_MNR_AVAILABLE)
    {
        /* 1. Bestimmung des Teilbandes mit dem kleinsten MNR ungleich MIN_POWER */
        MNRmin = NO_MIN_MNR_AVAILABLE;    // init auf hohen Wert
        for(n_band=0;n_band < BANDSIZE;n_band++)
        {
            //if(MNR[n_band] < MNRmin && MNR[n_band] > MIN_POWER && Bit_done_List[n_band] == MORE_BITS_POSSIBLE){
            if(MNR[n_band] < MNRmin && Bit_done_List[n_band] == MORE_BITS_POSSIBLE)
            {
                    MNRmin = MNR[n_band];    // minimales MNR aller Subbnder (grer 0 dB)
                    index = n_band;        // index des Subband mit minimalen MNR
            }
        }
        /* 2. Erhhung der Bitanzahl fr dieses Teilband */
        if(BSCF[index] == 0)
        {    // wenn Subband das erste mal min MNR hat
            BSCF[index] = 6;    // einmalig Zuweisung 6 Bit fr Scalfactor (SCF)
            BSPL[index] = Bit_leng[BSPL[index]+1];    // erhhe Bitanzahl fr Teilband-Samples
        }
        else
        {                // hier nur bei mehrmaligen min MNR des Subbands
            if(BSPL[index] >= 15)    // maximale Bitanzahl bereits erreicht
                Bit_done_List[index] = MAX_BITS_ALLOC;//MNR[index];  // nehme diesen Index in verbotene Liste auf
            else if(Bit_done_List[index] == MORE_BITS_POSSIBLE) //if(length(find(Bit_done_List>0)) < length(find(MNR>0)))    // wenn noch Subbnder da, die Bits bekommen knnen
                BSPL[index] = Bit_leng[BSPL[index]];    // erhhe auf nchste Bitanzahl im MPEG Standard -> Beachte Bitabstand [0,2,3,4,5], daher kein +1 im index ntig
        }

        /* 3. Erneute Berechnung des MNR in diesem Teilband (index) */
        if(snr_values[index] >= SNR[14])
        {    // maximaler SNR aus ISO Tabelle durch Bitanzahl bereits erreicht
            Bit_done_List[index] = MAX_BITS_ALLOC;//MNR[index];
            // else wenn noch nicht alle MNR in Liste MNRmax aufgenommen
        }
        else if(Bit_done_List[index] == MORE_BITS_POSSIBLE)
        {//if(length(find(Bit_done_List>0)) < length(find(MNR>0)))    // wenn noch Subbnder da, die Bits bekommen knnen
            n_snr = 14;
            while( snr_values[index] != SNR[n_snr] && n_snr > 0 )
            {    // Suche index des aktuellen snr_values[index] in SNR ISO Tabelle um einen Wert hher zu setzen
                n_snr--;
            }
            snr_values[index] = SNR[n_snr+1];    // setzte einen SNR Wert hher
            MNR[index] = snr_values[index] - SMR[index];    // Berechne neuen MNR in diesem Teilband
        }

        /* 4. Berechnung der notwendigen Bits fr alle Teilbnder und Skalenfaktoren
           und Vergleich mit der maximalen Anzahl. Wenn die Bitanzahl kleiner als die
           Maximalanzahl ist, beginnt die nchste Iterationsschleife bei Schritt 1.   */
        bitleng = ancillary_data;    // init mit Bitanzahl fr Bitzuweisung je Teilband und fr Zusatzinfos
        Bit_done_List_index = 0;    // init
        for(n_band=0;n_band < BANDSIZE;n_band++)
        {
            bitleng = bitleng + BSCF[n_band] + 12*BSPL[n_band];    // Summiere neu zugewiesene Bits auf - 12*BSPL[, da 12 Samples je Subband
            if(Bit_done_List[n_band] == MAX_BITS_ALLOC)    // Wenn im Teilband n_band keine Bits mehr zugewiesen werden mssen/drfen...
                Bit_done_List_index++;        // ... wird Bit_done_List_index um eins erhht. Wenn alle Teilbnder -> Bit_done_List_index=32 -> Ende der while() bzw. Bitzuweisung
        }
        adb = cb - bitleng;    // aktuallisiere noch verfgbare Bits adb
    }
    //printf("adb=%d\n",adb);
    //coded_subband_leng=SMR_over_0dB;
}
