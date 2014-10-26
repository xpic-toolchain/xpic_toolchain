int lesser ( int a, int b ) {
	return a < b;
}

main ()
{
	volatile int zero = 0;
	volatile int minusten = -10;
	volatile int plusten = 10;
	volatile int thousand = 1024;

	if ( lesser(zero,zero) == 1 ) { /* 0 < 0 == false  */
		return -1;
	}

	if ( lesser(minusten,zero) == 0 ) { /* -10 < 0 == true */
		return -1;
	}

	if ( lesser(plusten,minusten) == 1 ) { /* 10 < -10 == false */
		return -1;
	}

	if ( lesser(minusten,minusten) == 1 ) { /* -10 < -10 == false */
		return -1;
	}

	if ( lesser(thousand,thousand) == 1 ) { /* 1024 < 1024 == false */
		return -1;
	}

	return 0;
}
