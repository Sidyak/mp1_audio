//---------------------------------------------------------------------
// Description:    fir filter for floating point
//---------------------------------------------------------------------

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
