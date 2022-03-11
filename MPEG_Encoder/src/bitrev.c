//    DESCRIPTION
//        This routine performs the bit-reversal of the input array x[].
//        where x[] is an array of length n 32-bit complex pairs of data.
//        This requires the index array provided by the program below.
//        This index should be generated at compile time not by the DSP.
//
//    ASSUMPTIONS
//        n is a power of 2
//
//    NOTE: If n <= 4K one can use the char (8-bit) data type for
//        the "index" variable. This would require changing the LDH when
//        loading index values in the assembly routine to LDB. This would
//        further reduce the size of the Index Table by half its size.
//
//    CYCLES    (n/4)*11 + 9
//
//    MEMORY NOTE
//        There are NO memory bank hits regarless of array alignment
//

void bitrev(float *xs, short *index, int n)
{
    int    i;
    short  i0, i1, i2, i3;
    short  j0, j1, j2, j3;
    double xi0, xi1, xi2, xi3;
    double xj0, xj1, xj2, xj3;
    short  t;
    int    a, b, ia, ib, ibs;
    int    mask;
    int    nbits, nbot, ntop, ndiff, n2, halfn;
    double *x ;
    x = (double *)xs ;
    
    nbits = 0;
    i = n;
    while (i > 1)
    {
        i = i >> 1;
        nbits++;
    }
    
    nbot    = nbits >> 1;
    ndiff   = nbits & 1;
    ntop    = nbot + ndiff;
    n2      = 1 << ntop;
    mask    = n2 - 1;
    halfn   = n >> 1;
    
    for (i0 = 0; i0 < halfn; i0 += 2)
    {
        b       = i0 & mask;
        a       = i0 >> nbot;
        if (!b) ia = index[a];
        ib      = index[b];
        ibs     = ib << nbot;
    
        j0      = ibs + ia;
        t       = i0 < j0;
        xi0     = x[i0];
        xj0     = x[j0];
    
        if (t)
        {
            x[i0] = xj0;
            x[j0] = xi0;
        }
    
        i1      = i0 + 1;
        j1      = j0 + halfn;
        xi1     = x[i1];
        xj1     = x[j1];
        x[i1] = xj1;
        x[j1] = xi1;
    
        i3      = i1 + halfn;
        j3      = j1 + 1;
        xi3     = x[i3];
        xj3     = x[j3];
        if (t)
        {
            x[i3] = xj3;
            x[j3] = xi3;
        }
    }
}
