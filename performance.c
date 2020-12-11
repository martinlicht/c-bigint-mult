
/****************************************************************************

    Compile with: 
    gcc example.c -std=c99 -pedantic -W -Wall

****************************************************************************/  

  


#include "header.h"    
    
  

  
  int main()
  {

      
    printf( "Big Integer Multiplication -- Checks and Performance \n\n");
    
    /***************/
    /* Performance */
    /***************/
    
    {
      
      printf( "Performance Test....\n" );
    
      long int t1, t2;
    
      t1 = clock();
    
      const bigintexpo k = 16;
      const bigintlength l = 1 << k; /* Number of digits */
      
      long int v = 0, vmax = 10;
      
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
  
  
  
  
  
  
  
  
 
