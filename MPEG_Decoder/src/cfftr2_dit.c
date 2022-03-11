/*    DESCRIPTION
*
*        This routine performs the Decimation-in-Time (DIT) Radix-2 FFT 
*        of the input array x.
*        x has N complex floating point numbers arranged as successive
*        real and imaginary number pairs. Input array x contains N
*        complex points (N*2 elements). The coefficients for the
*        FFT are passed to the function in array w which contains
*        N/2 complex numbers (N elements) as successive real and
*        imaginary number pairs.
*        The FFT Coefficients w are in N/2 bit-reversed order
*        The elements of input array x are in normal order
*        The assembly routine performs 4 output samples (2 real and 2
*        imaginary) for a pass through inner loop.
*
*        Note that (bit-reversed) coefficients for higher order FFT (1024
*        point) can be used unchanged as coefficients for a lower order 
*        FFT (512, 256, 128 ... ,2)
*
*        The routine can be used to implement Inverse-FFT by any ONE of 
*        the following methods:
*
*        1.Inputs (x) are replaced by their Complex-conjugate values 
*          Output values are divided by N
*        2.FFT Coefficients (w) are replaced by their Complex-conjugates
*          Output values are divided by N
*        3.Swap Real and Imaginary values of input
*        4.Swap Real and Imaginary values of output
*        
*    TECHNIQUES
*
*        1. The inner two loops are combined into one inner loop whose 
*           loop count is N/2. 
*        2. The first 4 cycles of inner loop prolog are scheduled in 
*           parallel with the outer loop.
*        3. Load counter is not used, so extreneous loads are performed
*        4. Variables n and c share the register A6 and variables w and 
*           nsave share the register B4.
*    
*    ASSUMPTIONS
*
*        N is a integral power of 2 (4, 8,16,32 ...) and the FFT 
*        dimension (N) must atleast be 4.
*        The FFT Coefficients w are in bit-reversed order
*        The elements of input array x are in normal order
*        The imaginary coefficients of w are negated as {cos(d*0), 
*        sin(d*0), cos(d*1), sin(d*1) ...} as opposed to the normal 
*        sequence of {cos(d*0), -sin(d*0), cos(d*1), -sin(d*1) ...} 
*        where d = 2*PI/N.
*        
*    MEMORY NOTE
*
*        Arrays x (data) and w (coefficients) must reside in 
*        different memory banks to avoid memory conflicts.  If Data and 
*        Coefficents do reside in the same memory bank, add (N/2) + log2(N) + 1
*        cycles to the cycles equation below. The memory bank hits are due to 
*        scheduling of assembly code and also due to extreneous loads 
*        causing bank hits.
*
*        Data and Coefficents must be aligned on an 8 byte boundary.
*
*    CYCLES
*
*        ((2*N) + 23)*log2(N) + 6
*
*        For N=1024, Cycles = 20716
*
*    NOTATIONS
*
*        f = Function Prolog or Epilog
*        o = Outer Loop
*        p = Inner Loop Prolog
*
*    RADIX-2 FFT (DIT)
*
*    Revision Date: 5/21/98
*    
*    USAGE    
*
*        This routine is C Callable and can be called as:
*        
*        void cfftr2_dit( float *x, const float *w, short N)
*
*        x    Pointer to Array of Dimension 2*N elements holding 
*            Input to and Outputs from function cfftr2_dit()
*        w    Pointer to an array holding the coefficient (Dimension
*            n/2 complex numbers)
*        N    Number of complex points in x
*
*        If routine is not to be used as a C callable function then
*        you need to initialize values for all of the values passed
*        as these are assumed to be in registers as defined by the 
*        calling convention of the compiler, (refer to the C compiler
*        reference guide).
*
*        ARGUMENTS PASSED       ->   REGISTER
*        ---------------------------------
*        x                    ->   A4
*        w                    ->   B4
*        N                    ->   A6
*
*/

void cfftr2_dit(float* x, float* w, short n)
{
   short n2, ie, ia, i, j, k, m;
   float rtemp, itemp, c, s;

   n2 = n;
   ie = 1;

   for(k=n; k > 1; k >>= 1)
   {
      n2 >>= 1;
      ia = 0;
      for(j=0; j < ie; j++)
      {
         c = w[2*j];
         s = w[2*j+1];
         for(i=0; i < n2; i++)
         {
            m = ia + n2;
            rtemp     = c * x[2*m]   + s * x[2*m+1];
            itemp     = c * x[2*m+1] - s * x[2*m];
            x[2*m]    = x[2*ia]   - rtemp;
            x[2*m+1]  = x[2*ia+1] - itemp;
            x[2*ia]   = x[2*ia]   + rtemp;
            x[2*ia+1] = x[2*ia+1] + itemp;
            ia++;
         }
         ia += n2;
      }
      ie <<= 1;
   }
}
