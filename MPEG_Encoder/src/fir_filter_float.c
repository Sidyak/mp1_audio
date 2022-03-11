//---------------------------------------------------------------------
// Description:    fir filter for floating point
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

float fir_filter(float FIR_delays[], float FIR_coe[], short N_delays, float x_n)
{
    short i;
    float FIR_accu = 0;

    // delays backwards, coefficients in forward direction
    FIR_delays[N_delays-1] = x_n; 
    
    FIR_accu = 0;
    for(i=0; i < N_delays; i++)
        FIR_accu += FIR_delays[N_delays-1-i] * FIR_coe[i];

    // loop to shift the delays
    for(i=1; i < N_delays; i++)                
        FIR_delays[i-1] = FIR_delays[i];

    return (FIR_accu);
}
