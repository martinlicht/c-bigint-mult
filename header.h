#ifndef C_BIGINT_MULT
#define C_BIGINT_MULT

/*****************************************************************************

  This program is an implementation of the Karatsuba-Ofmann-Algorithm
  a.k.a. Karatsuba-Algorithm, for an asymptotically faster multiplication
  of big integer numbers than via school method.

  This program has been written as participation at a programming contest
  by the some peers of the students council of the Mathematicians at the
  University of Bonn, Germany (Rheinische Friedrich-Wilhelms-Universität Bonn)
  in winter of 2008

  It includes some basic algorithms like addition, subtraction and
  - of course - multiplication. My intention was to optimize it for speed,
  but nevertheless, I think the code is still easily to understand,
  well-documentend and features a very efficient usage of memory. And, well,
  I think this piece is pretty fazt ;-)

  It's interresting to notice that the total amount of memory could be even
  reduced, in a tradeoff for speed, due to a additional copy-statement one
  would require to use. Yet, you cannot implement this algorithm in-place.

  This program is only capable of Big Integers whose number of digits is a
  power of 2 - whereas a digit is a 'unsigned long int'.

  The Karatsuba-multiplication method switches to naiv multiplication according
  to school method, once recursion tree has reached a depth where the number of
  digits of the factors falls bellow a certain length, thus making later method
  the more efficient algorithm. The threshold has been chosen empirically on my
  system, therefore you may want to modify it to improve your speed.

  Martin W. Licht

****************************************************************************/









  /*********************************************/
  /* Includes                                  */
  /*********************************************/

  #include <assert.h>
  #include <limits.h>
  #include <math.h>
  #include <stdio.h>
  #include <stdlib.h>
  #include <string.h>
  #include <time.h> 
  
  
  
  
  /*********************************************/
  /* Datatypes and constants                   */
  /*********************************************/
  

  typedef unsigned long int bigint;
  typedef unsigned long int bigintlength; 
  typedef unsigned char     bigintexpo;
  
  typedef unsigned char bool;
  
  #define false ((bool)0)
  #define true  ((bool)1)
  
  #define DIGIT_MAX  ((bigint)ULONG_MAX)
  #define DIGTI_MIN  ((bigint)0)
  #define DIGIT_ZERO ((bigint)0)

  
  
  
  /*********************************************/
  /* Elementary operations                     */
  /*********************************************/
  
  
  
  /*
  * 
  * Outputs a Big Int.
  * 
  * Remark:
  * z points to an array of digits of length n. The output is in
  * hexadecimal, 8-Hexdigits per Bigint-digit. After the output there is a
  * linebreak.
  */
  void mbiOutput( bigintlength n, const bigint* z )
  {
    assert( z != NULL );
    while( n >= 1 )
    {
      printf( "%lx ", *(z+(n-1)) );
      n--;
    }
    printf( "\n" );
  }
  
  

  /*
  *
  * Copies a Big Int
  *
  * Remark:
  * Copies the array of n digits src points to into the n digits dest
  * points to. 
  */
  void mbiCopy( bigintlength n, bigint* dest, const bigint* src )
  {
    bigintlength i; 
    assert( dest != NULL );
    assert( src != NULL );  
    for( i = 0; i < n; i++ ) dest[i] = src[i];
  }
  
  /*
  * 
  * Sets a Big Number to zero
  * 
  * Remark:
  * dest points to n digits, which will be set to zero, i.e. 0x00000000.
  */
  void mbiSetZero( bigintlength n, bigint* dest )
  {
    bigintlength i;
    assert( dest != NULL );
    for( i = 0; i < n; i++ ) dest[i] = (bigint)0;
  }
  
  
  /*
  * 
  * Compares two Big Ints
  * 
  * Remark:
  * z1 and z2 point to Big Integers with n digits each. In case of
  * either z1 > z2, z1 = z2 or z1 < z2, the return is 1, 0 or -1, respectively.
  */
  int mbiCompare( bigintlength n, const bigint* z1, const bigint* z2 )
  {
    for( ; n > 0; n-- )
      if( z1[n-1] > z2[n-1] )
        return 1;
      else if( z2[n-1] > z1[n-1] )
        return -1;
    
    return 0;
  }
  
  
  /*
  * 
  * Compares two Big Ints whose sizes may differ
  * 
  * Remark:
  * None
  */
  int mbiCompare2( bigintlength n1, const bigint* z1, bigintlength n2, const bigint* z2 )
  {
    if( n1 > n2 )
      while( z1[n1-1] == 0 && n1 > n2 ) n1--;
    else if( n2 > n1 )
      while( z2[n2-1] == 0 && n2 > n1 ) n2--;
      
    if( n1 > n2 ) return 1;
    if( n2 > n1 ) return -1;
      
    for( ; n1 > 0; n1-- )
      if( z1[n1-1] > z2[n1-1] )
        return 1;
      else if( z2[n1-1] > z1[n1-1] )
        return -1;
    
    return 0;
  }
  
  /*
  * 
  * Tells whether an Int is equal to zero
  * 
  * Remark: 
  * None
  */
  bool mbiIsZero( bigintlength n, const bigint* z )
  {
    for( ; n > 0; n-- ) 
        if( z[n-1] != 0x0 ) 
            return false;
    return true;
  }
  
  
  /*
  *
  * Tell how much space is needed to represent that number
  * 
  * Remark:
  * After call, 'bytes' and 'bits' will tell how many digits and how many bits are neccessary to
  * represent that number in memory
  */
  void mbiGetNumericalLength( bigintlength n, const bigint* z, bigintlength* bytes, unsigned long* bits )
  {
    
    for( *bytes = n; *bytes > 0; (*bytes)-- )
      if( z[*bytes-1] == 0 ) 
        continue; 
      else 
        break;
    
    if( *bytes == 0 ){
        *bits = 0;
        return;
    } //No non-empty byte 
    
    *bits = sizeof( bigint ) * 8;
    bigint t = ((bigint)1) << (sizeof(bigint)*8-1);
    
    for( ; t != 0; t /= 2, (*bits)-- )
    {
      if( ( z[*bytes-1] & t ) != 0 ) break;
    }
    
    assert( *bits <= 8*sizeof(bigint) );
    
  }
  
  
  
  /*
  * Performs right shift
  * Remarks: NONE
  */
  void mbiRightShift( bigintlength n, bigint* z, bigintlength r, bool setzero )
  {
    assert( r <= n );
    for( bigintlength i = r; i < n; i++ )
      z[i-r] = z[i];
    if( setzero ) mbiSetZero( r, z + n - r );
  }
  
  /*
  * Performs right shift, bit-wise precision
  * Remarks: NONE
  */
  void mbiBitRightShift( bigintlength n, bigint* z, unsigned long r, bool setzero )
  {
    if( r > 8*sizeof(bigint) ){
      mbiRightShift( n, z, r / (8*sizeof(bigint)), setzero );
      r %= 8*sizeof(bigint);
    }
    assert( r <= 8*sizeof( bigint ) );
    bigint maske = 0;
    maske = ~maske;
    maske = maske << r;
    maske = ~maske;
    for( bigintlength i = 0; i < n ; i++ )
    {
      z[i] = z[i] >> r;
      if( i+1 < n ){
        bigint mem = maske & z[i+1];
        mem = mem << (sizeof(bigint)*8-r);
        z[i] |= mem;
      }
    }
    setzero = !setzero;
  }
  
  /*
  * Performs left shift
  * Remarks: NONE
  */
  void mbiLeftShift( bigintlength n, bigint* z, bigintlength r )
  {
    for( bigintlength i = n-r; i > 0; i-- )
    {
      z[r+i-1] = z[i-1];
    }
    mbiSetZero( r, z );
  }
  
  /*
  * Performs left shift, bitwise-precision
  * Remarks: NONE
  */
  void mbiBitLeftShift( bigintlength n, bigint* z, unsigned long r )
  {
    
    if( r > 8*sizeof(bigint) ){
      mbiLeftShift( n, z, r / (8*sizeof(bigint)) );
      r %= 8*sizeof(bigint);
    }
    
    assert( r <= 8 * sizeof( bigint ) );
    bigint maske = 0;
    maske = ~maske;
    maske >>= r;
    maske = ~maske;
    for( bigintlength i = n; i > 0; i-- )
    {
      z[i-1] <<= r;
      if( i-1 > 0 ){
        bigint mem = maske & z[i-2];
        mem >>= (sizeof(bigint)*8-r);
        z[i-1] |= mem;
      }
    }
  }
  
  
  
  

  /*
  * Fills a Big Int with random values
  * Remarks: Uses standard 'rand' function
  */
  void mbiShuffle( bigintlength n, bigint* z, bigint modulo )
  {
    if( modulo != 0 )
        for( bigintlength i = 0; i < n; i++ )
            z[i] = (bigint) rand() % modulo;  
    else
        for( bigintlength i = 0; i < n; i++ )
            z[i] = (bigint) rand();
    
  }
  
  /*
  * Set all the digits to a certain value
  * Remarks: None
  */
  void mbiSetDigits( bigintlength n, bigint* z, bigint digit )
  {
    for( bigintlength i = 0; i < n; i++ )
      z[i] = digit;
  }

  
  

  /**************************/
  /* Elementary Arithmetics */
  /**************************/

  /*
  * Adds a big int to another big int, taking into account the carry.
  * Remark: dest and add point to Big Int of length n. add is added onto dest.
    carry points to a boolean value which (i) gives input, whether 1 shall
    be added to the sum or not (ii) whether there has been an overflow has
    taken place during execution (1) or not (0). It's a simulation of a
    carry-save-adder.
  */
  bool mbiAdd( bigintlength n, bigint* dest, const bigint* add, bool* carry )
  {
    
    bigintlength i;
    for( i = 0; i < n; i++ )
    {
      dest[i] += add[i];
      if( *carry ) dest[i]++;
      if( ( !(*carry) && add[i] > dest[i] ) || ( *carry && add[i] >= dest[i] ) )
        *carry = true;
      else
        *carry = false;
    }
    
    return *carry;
  }
  
  
  
  /*
  * Adds to big ints, saves the result and takes into account the carry.
  * Remark: Works like Add, but with two summands and an argument for a
    pointer the sum is written to. dest and add1 are allowed to point to
    the same block of memory
  */
  bool mbiCopyAdd( bigintlength n, bigint* dest, const bigint* add1, const bigint* add2, bool* carry )
  {
    assert( add2 != dest );
    bigintlength i;
    for( i = 0; i < n; i++ )
    {
      dest[i] = add1[i] + add2[i];
      if( *carry ) dest[i]++;
      
      if( ( !(*carry) && add2[i] > dest[i] ) || ( *carry && add2[i] >= dest[i] ) )
        *carry = true;
      else
        *carry = false;
    }
    
    return *carry;
  }
  
  
  /*
  * Increments a big int, taking into account the carry.
  * Remark: dest points to a Big Int of length n. dest is incremented by 1.
    carry points to a boolean value which (i) gives input, whether 1 shall be
    added to the sum or not (ii) whether there has been an overflow has taken
    place during execution (1) or not (0). It's a simulation of a
    carry-save-adder.
  */
  bool mbiInc( bigintlength n, bigint* dest, bool* carry )
  {
    
    bigintlength i;
    
    dest[0]++;
    if( dest[0] == (bigint)0 ) *carry = true;
    
    for( i = 1; *carry == true && i < n; i++ )
    {
      dest[i]++;
      if( dest[i] == (bigint)0 ) *carry = true;
      else *carry = false;
    }
    
    return *carry;
    
  }
  
  
  
  /*
  * Subtracts a big int from another big int, taking into account the carry.
  * Remark: dest and sub point to Big Int of length n. sub is subtracted
    from dest. carry points to a boolean value which (i) gives input,
    whether 1 shall be drawn from the difference or not (ii) whether there
    has been an overflow has taken place during execution (1) or not (0).
    It's a simulation of a carry-save-adder.
  */
  bool mbiSub( bigintlength n, bigint* dest, const bigint* sub, bool* carry )
  {
    
    bigint z;
    bigintlength i;
    for( i = 0; i < n; i++ )
    {
    
      z = dest[i];
      z -= sub[i];
      if( *carry ) z--;
      
      if( ( !*carry && dest[i] < z ) || ( *carry && dest[i] <= z ) )
        *carry = true;
      else
        *carry = false;
      
      dest[i] = z;
      
    }
    
    return *carry;
    
  }
  
  
  /*
  * Adds to big ints, saves the result and takes into account the carry.
  * Remark: Works like Add, but with two summands and an argument for a
    pointer the sum is written to. dest and add1 are allowed to point to
    the same block of memory
  */
  /* EXTRA */
  /* NOT TESTED */
  bool mbiCopySub( bigintlength n, bigint* dest, const bigint* add1, const bigint* add2, bool* carry )
  {
    assert( add2 != dest );
    bigint z;
    bigintlength i;
    for( i = 0; i < n; i++ )
    {
    
      z = add1[i];
      z -= add2[i];
      if( *carry ) z--;
      
      if( ( !*carry && add1[i] < z ) || ( *carry && add1[i] <= z ) )
        *carry = true;
      else
        *carry = false;
      
      dest[i] = z;
      
    }
    
    return *carry;
  }
  
  
  /*
  * Increments a big int, taking into account the carry.
  * Remark: dest points to a Big Int of length n. dest is incremented by 1.
    carry points to a boolean value which (i) gives input, whether 1 shall be
    added to the sum or not (ii) whether there has been an overflow has taken
    place during execution (1) or not (0). It's a simulation of a
    carry-save-adder.
  */
  /* EXTRA */
  bool mbiDec( bigintlength n, bigint* dest, bool* carry )
  {
    
    bigintlength i;
    
    dest[0]--;
    if( dest[0] == (bigint)DIGIT_MAX ) *carry = true;
    
    for( i = 1; *carry == true && i < n; i++ )
    {
      dest[i]--;
      if( dest[i] == (bigint)DIGIT_MAX ) *carry = true;
      else *carry = false;
    }
    
    return *carry;
    
  }
  


  
  

  
  
  /*********************************************/
  /* Other multiplication operations           */
  /*********************************************/



  /*
  * Multiplies to Big ints according to basic school method
  * Remark: Bigints a and b must have the same length, which must be of
    form 2^{k}. p points to a Bigint of double size - 2^{k+1} - the
    result is saved in.
  */
  void mbiNaivMultiplication( bigintexpo k, bigint* p, const bigint* a, const bigint* b )
  {
    
    bigintlength i, j;
    bigintlength length = 1 << k;
    
    mbiSetZero( length*2, p );
    
    /* Collect the carries and add them later */
    bigint* overflows = malloc( sizeof(bigint)*length*2 );
    assert( overflows != NULL );
    mbiSetZero( length*2, overflows );
    
    /* main part */
    for( i = 0; i < length; i++ )
    {
      for( j = 0; j < length; j++ ){
        
        long long w1, w2, w3;
        bool carry  = false;
        
        w1 = a[i];
        w2 = b[j];
        w3 = w1*w2;
        
        /*
        * If the rest of the code works properly
        * we can use the following line
        * without fear of memory corruption
        */
        mbiAdd( 2, p+(i+j), (bigint*)&w3, &carry );
        if(carry) overflows[i+j+2]++;
        
      }
    }
    
    /* evaluate the carries */
    bool carry = false;
    mbiAdd( 2*length, p, overflows, &carry );
    assert(!carry);
    
    free( overflows );
    
  }
 
 
  /*
  * Multiplies to Big ints of size <= 128 according to basic school method
  * Remarks: Works like a NaivMultiplication, but it has been optimized
    for and is restricted to the case of k <= 7. Could be even more sped-up
    by using uchars for the overflows
  */
  void mbiNaivMultiplicationRestricted( bigintexpo k, bigint* p, const bigint* a, const bigint* b )
  {
    
    assert( k <= 7 );
    bigintlength i, j;
    bigintlength length = 1 << k;
    assert( length <= 128 );
    
    mbiSetZero( length*2, p );
    
    /* Collect the carries and add them later */
    bigint overflows[256];
    mbiSetZero( length*2, overflows );
    
    /* main part */
    for( i = 0; i < length; i++ )
    {
      for( j = 0; j < length; j++ ){
        
        long long w1, w2, w3;
        bool carry  = false;
        
        w1 = a[i];
        w2 = b[j];
        w3 = w1*w2;
        
        /*
        * If the rest of the code works properly
        * we can use the following line
        * without fear of memory corruption
        */
        mbiAdd( 2, p+(i+j), (bigint*)&w3, &carry );
        if(carry) overflows[i+j+2]++;
        
      }
    }
    
    /* evaluate the carries */
    bool carry = false;
    mbiAdd( 2*length, p, overflows, &carry );
    assert(!carry);
    
  }
  
  
  
 
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  /*******************************************************/
  /* Karatsuba-Ofmann-Algorithm                          */
  /*******************************************************/
  
  
  /*
  * Multiplies to Big ints according to Karatsuba-Ofmann
  * Bemerkung: Bigints a and b must have the same length, which must be of
    form 2^{k}. p points to a Bigint of double size - 2^{k+1} - the result
    is saved in.
  */
  void mbiMultiply( bigintexpo k, bigint* p, const bigint* a, const bigint* b )
  {
    
    /* calculate length of a and b */
    bigintlength length = 1 << k;
    
    /*
    * if we are below the threshold, switch to school method
    * Die Wechselgrenze wurde empirisch ueber den Daumen geschaetzt
    */  
    if( k <= 5 ){
      mbiNaivMultiplication( k, p, a, b );
      return;
    }
    
    /***************************************/
    /* Declaration and initialization      */
    /***************************************/
    
    /* pointers to the low- and high-parts of the factors */
    const bigint *al,*ah,*bl,*bh;
    al = a;
    bl = b;
    ah = a + length/2;
    bh = b + length/2;
    
    /* Pointers to target memory */
    bigint *u1, *u2, *u3, *u4;
    u1 = p;
    u2 = p + length/2;
    u3 = p + length;
    u4 = p + length + length/2;
    
    /* Pointer to heap */
    //bigint *heap = malloc( length * sizeof(bigint) );
    bigint heap[length];
	
	assert( heap != NULL );
    bigint *aux1, *aux2;
    aux1 = heap;
    aux2 = heap + length/2;
    
    /* Pointer to products ahbh and albl */
    bigint *ahbh = u3;
    bigint *albl = u1;
    
    /* Memory for some of the carries
    due to performance issues
    */
    bigint overflow = (bigint)0;
    
    
    /******************************/
    /* Calculations               */
    /******************************/
    
    /* Calculate middle product */
    
    /* Calculate (al + ah)(bl + bh) */
    
    /* Calculate the two sums */  
    bool carrya = false;
    bool carryb = false;
    mbiCopyAdd( length/2, u4, al, ah, &carrya );
    mbiCopyAdd( length/2, u1, bl, bh, &carryb );
    
    /* Multiply both */
      
    /* First recursion */  
    mbiMultiply( k-1, aux1, u1, u4 );
    
    /* Do some additions, to calculate the high-part of the product */ 
    
    bool carryt = false;
      
    carryt = false;
    if( carryb ) mbiAdd( length/2, aux2, u4, &carryt );
    if( carryt ) overflow++;
  
    carryt = false;
    if( carrya ) mbiAdd( length/2, aux2, u1, &carryt );
    if( carryt ) overflow++;
    
    if( carrya && carryb ) overflow++;
    
    /*
    * Now the heap is (al+ah)(bl+bh)
    * We write al*bl and ah*bh directly into the target memory
    */  
    
    /* Calculate albl */
    mbiMultiply( k-1, albl, al, bl );
    
    /* Calculate ahbh */
    mbiMultiply( k-1, ahbh, ah, bh );
    
    /* Sub the two products from the heap */
    
    bool carrys = false;
     
    carrys = false;
    mbiSub( length, heap, albl, &carrys );
    if( carrys ) overflow--;
    
    carrys = false;
    mbiSub( length, heap, ahbh, &carrys );
    if( carrys ) overflow--;
    
    /*
    * Add the term at the heap to right place in the target
    * Evaluate the overflows
    */
    
    bool carryq = false;
    
    mbiAdd( length, u2, heap, &carryq );
    if( carryq ) overflow++;
    
    if( overflow == 1 ){
      carryq = false;
      mbiInc( length/2, u4, &carryq );
    }else if( overflow == 2 ){
      carryq = false;
      mbiInc( length/2, u4, &carryq );
      carryq = false;
      mbiInc( length/2, u4, &carryq );
    }
        
    /**********************************/
    /* Result is in the target memory */
    /**********************************/
    
    /* free all memory */
    //free( heap );   
    
    
    /*{
      bigint* test = malloc( length*2 * sizeof(bigint) );
      NaivMultiplication( k, test, a, b );
      if( Compare( length*2, test, p ) ){
        printf( "Hauptrechnung: %d\n", length );
        OutputBigint( length, a );
        OutputBigint( length, b );
        OutputBigint( length*2, p );
        OutputBigint( length*2, test );
        assert(false);
      }
      free(test);
    }*/

  }
    
    
    
  
  
  
      
      
  
  
  
  
  
  
  /*********************************************/
  /* Non-essential operations                  */
  /* for an easier handling                    */
  /*********************************************/
    
  /*
  * Returns an exponent k, s.t. n <= 2^{k}
  * Bemerkung: Absolutely no optimization
  */
  bigintexpo getFittingExponent( bigintlength n )
  {
    bigintexpo k = 0;
    while ( (bigintlength)(1L<<k) < n && k <= (bigintexpo)254 ) k++;
    return k;
  }
  
  
    
  /*
  * Multiplies numbers of arbitrary length
  * Remark: It wraps the karatsuba-multiplication, s.t. we can multiply numbers
  * whose length is not a power of 2. Absolutely no optimization for this
  */
  void mbiMultiplikation( bigint* dest, bigintlength n1, const bigint* fak1, bigintlength n2, const bigint* fak2 )
  {
    
    assert( dest != NULL );
    assert( fak1 != NULL );
    assert( fak2 != NULL );
    
    bigintlength nmax = n1 > n2 ? n1 : n2;
    bigintexpo k = getFittingExponent( nmax );
    
    if( (1L<<k) == 0 ){
      printf( "Error: Summands are to long\n" );
      return;
    }
    
    bigint *tempfak1, *tempfak2;
    bigint *tempdest;
    
    tempdest = calloc( sizeof(bigint) , (1<<k) * 4 );
    tempfak1 = tempdest + (1<<k)*2;
    tempfak2 = tempdest + (1<<k)*3;
//     tempfak1 = calloc( sizeof(bigint) , (1<<k) ); 
//     tempfak2 = calloc( sizeof(bigint) , (1<<k) );
//     tempdest = calloc( sizeof(bigint) , (1<<k)*2 );
    
    assert( tempdest != NULL );
    assert( tempfak1 != NULL );
    assert( tempfak2 != NULL );
    
    mbiCopy( n1, tempfak1, fak1 );
    mbiCopy( n2, tempfak2, fak2 );
    
    mbiMultiply( k, tempdest, tempfak1, tempfak2 );
    
    mbiCopy( n1+n2, dest, tempdest );
    
    //free( tempfak1 );
    //free( tempfak2 );
    free( tempdest );
  } 
  
  
  
  
  
  
  
  
#endif
