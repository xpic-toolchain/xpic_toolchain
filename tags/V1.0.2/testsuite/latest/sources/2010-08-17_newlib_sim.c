//####y* NEWLIB_MAIN/history *
// MODIFICATION HISTORY
// DATE      WHO   DESCRIPTION
// -------- ----- -------------------------------------------------------------
// 12.08.10 JK     simple test for newlib
// ----------------------------------------------------------------------------
// SYNOPSIS
// newlib test
//
// - to generate a reference linux program use:
//   gcc -O2 -D X86_EXE -o newlib_exe tb/CFLOW/PRN2SIM/NEWLIB/soft/xpic/src/main.c
//
// SOURCE
// ----------------------------------------------------------------------------
// global defines & macro definitions
// ----------------------------------------------------------------------------
#include <stdlib.h>

// #define NULL 0
// gcc

void libc_mem_init(void);

double Eval( int N )
{
	int i, j;
	double Sum, Answer;
	double *C;

	C = malloc( sizeof( double ) * ( N + 1 ) );
	if( C == NULL )
	  abort();
		//printf( "Out of space!!! \n" );

	C[ 0 ] = 1.0;

	for( i = 1; i <= N; i++ )
	{
		Sum = 0.0;
		for( j = 0; j < i; j++ )
			Sum += C[ j ];

		C[ i ] = 2.0 * Sum / i + i;
	}

	Answer = C[ N ];
	free( C );

	return Answer;
}

main()
{
	double val;

	//sim_message("TEST stdlib for XPIC with INTRAM",disp_only,0);
	//sim_message("- call libc_mem_init",disp_only,0);
	libc_mem_init();
	//sim_message("- call Eval",disp_only,0);
	val = Eval( 10 );
	//printf( "%f\n", val );
	//sim_message("STOP CLOCK : finished successfully",stop_clock,0);
	return 0;
}

//####
