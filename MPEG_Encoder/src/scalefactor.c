//----------------------------------------------------------------------------
// Description         Scalefactor calculation
// Author:             Kim Radmacher
// Created on:
//----------------------------------------------------------------------------

#include "init.h"
#include <math.h>

void scalefactor(void)
{
float s_band_max,Sabs;
short table_len=63, ji, ind, sample;

    // take maximum of 12 samples in band 'ind' and set as minimum
    for (ind=0; ind<BANDSIZE; ind++)
    {
        s_band_max=0.0;
        for(sample=0; sample<12; sample++)
        {
            Sabs = fabs(S[ind][sample]);
            if(Sabs > s_band_max)
                s_band_max = Sabs;
        }
        if(s_band_max > table_scf[0])
        {
          scf[ind] = table_scf[0];
        }
        else 
        {
            ji = 0;
            // find next matching scalefactor to max of band i from table
            while(ji < table_len && s_band_max > table_scf[table_len - ji - 1])
            {
                ji++;
            }
            scf[ind] = table_scf[table_len - ji - 1];
       }
    }
}
