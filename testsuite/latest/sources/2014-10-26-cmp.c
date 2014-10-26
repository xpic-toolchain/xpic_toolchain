int equals ( int a, int b ) {
	return a == b;
}

main ()
{
	volatile int zero = 0;
	volatile int minusten = -10;
	volatile int plusten = 10;
	volatile int thousand = 1024;

	if ( equals(zero,zero) == 0 ) {
		return -1;
	}

	if ( equals(minusten,zero) == 1 ) {
		return -1;
	}

	if ( equals(plusten,minusten) == 1 ) {
		return -1;
	}

	if ( equals(minusten,minusten) == 0 ) {
		return -1;
	}

	if ( equals(thousand,thousand) == 0 ) {
		return -1;
	}

	return 0;
}
