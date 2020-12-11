
/****************************************************************************

    Compile with: 
    gcc example.c -std=c99 -pedantic -W -Wall

****************************************************************************/  

  


#include "header.h"    
    
  

  
  int main()
  {

      
    printf( "Big Integer Multiplication -- Checks and Performance \n\n");
    
    
    /*******************************/
    /* Testing of functionality    */
    /*******************************/
    
    /*
    * For testing, we can use different patterns as control samples
    * Numbers of form 0xFFFFF...FF are suitable since one can immediatly check the result.
    */
    
    {
  
      printf( "Testing multiplication with fixed length numbers...\n" );
    
      const bigintexpo k = 5;
      const bigintlength l = 1 << k; /* Length */
      
      /*Iteration*/
      long int v = 0, vmax = 1000;
      
      for( v = 0; v < vmax; v++ )
      {
    
		if( vmax % 127 == 0 ) srand( clock() + rand() + v );
		  
        bigint P[l], Q[l];
        bigint R1[2*l], R2[2*l];
                
        mbiShuffle( l, P, 0 );
        mbiShuffle( l, Q, 0 );
        
        /* Naiv method */
        mbiNaivMultiplication( k, R1, P, Q );
        
        /* Karatsuba-Ofmann */
        mbiMultiply( k, R2, P, Q );
        
        /* Comparing */
        if( mbiCompare( l*2, R1, R2 ) != 0 )
        {
          printf("-- Error occurred!\n Length:%ld\n", l );
          printf("-- Naiv method:\n");
          mbiOutput( l*2, R1 );
          printf("-- Karatsuba-Ofmann:\n");
          mbiOutput( l*2, R2 );
          return 1;
        }
        
      }
      
//       printf( "...done.\n" );
      
    }
    

    /*******************************/
    /* Testing of functionality    */
    /* Numbers of arbitrary length */
    /*******************************/
    
    {
      
      printf( "Testing multiplikation of arbitrary length numbers...\n" );
    
      const bigintlength l_act = 32;
      /* Factors have size 32 in memory */
      /* Only a part of this will be used semantically */
      const bigintlength l_1 = 5;
      const bigintlength l_2 = 17;
      const bigintlength l_prod = l_1 + l_2;
      
      /* iteration*/
      long int v = 0, vmax = 10;
      
      for( v = 0; v < vmax; v++ ){
    
        bigint P[l_act], Q[l_act];
        bigint R1[l_act*2], R2[l_act*2];
        
        bigintexpo k = getFittingExponent( l_1 > l_2 ? l_1 : l_2 );
        
        assert( k == 5 );
        
        mbiSetZero( l_act, P );
        mbiSetZero( l_act, Q );
        mbiShuffle( l_1, P, 0 );
        mbiShuffle( l_2, Q, 0 );
        
        mbiSetZero( l_act*2, R1 );
        mbiSetZero( l_act*2, R2 );
        
        /* Naiv method */
        mbiNaivMultiplication( k, R1, P, Q );
        
        /* Karatsuba-Ofmann */
        mbiMultiplikation( R2, l_1, P, l_2, Q );
        
        /* Comparison */
        if( mbiCompare( l_prod, R1, R2 ) != 0 && mbiCompare( l_act*2, R1, R2 ) != 0 )
        {
          printf("-- Error occured with arbitrary length numbers\n" );
          printf("-- Naiv method:\n");
          mbiOutput( l_prod, R1 );
          printf("-- Karatsuba-Ofmann:\n");
          mbiOutput( l_prod, R2 );
          return 1;
        }
        
      }
      
//       printf( "...done.\n" );
    
    }
    

    /***************/
    /* Performance */
    /***************/
    
    {
      
      printf( "Performance Test....\n" );
    
      long int t1, t2;
    
      t1 = clock();
    
      const bigintexpo k = 14;
      const bigintlength l = 1 << k; /* Number of digits */
      
      long int v = 0, vmax = 100;
      
      /* printf( "Multiply %ld-times two numbers of length %ld\n", vmax, l ); */
      printf( "-- %ld multiplication of two numbers of %ld digits each in base 2^32\n", vmax, l );
      printf( "-- approx. %lld decimal digits\n", (long long) 9 * l );

      /*
      * We can use several samples:
      * Iterate one specific multiplication
      * A specific subset of mulitplications
      * random numbers
      * ...
      */
      
      for( v = 0; v < vmax; v++ ){
    
        bigint P[l], Q[l];
        bigint R[2*l];
        
        srand( time(NULL) * rand() + v*v*v*v );
        
        mbiShuffle( l, P, 0 );
        mbiShuffle( l, Q, 0 );
        
        mbiMultiply( k, R, P, Q );
        
      }
      
      t2 = clock();
      
      printf( "-- time: %ld0 ms \n", (t2-t1)/10000 );
    
//       printf( "...done.\n" );
    
    }
    
    return 0;
    

    
  }
  
  
  
  
  
  
  
  
 
