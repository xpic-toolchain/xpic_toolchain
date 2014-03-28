#define FALSE   0
#define TRUE    !FALSE

typedef unsigned short  crc;

#define CRC_NAME            "CRC-CCITT"
#define POLYNOMIAL          0x1021
#define INITIAL_REMAINDER   0xFFFF
#define FINAL_XOR_VALUE     0x0000
#define REFLECT_DATA        FALSE
#define REFLECT_REMAINDER   FALSE
#define CHECK_VALUE         0x29B1


// Derive parameters from the standard-specific parameters in crc.h.
#define WIDTH    (8 * sizeof(crc))
#define TOPBIT   (1 << (WIDTH - 1))

#if (REFLECT_DATA == TRUE)
#undef  REFLECT_DATA
#define REFLECT_DATA(X)         ((unsigned char) reflect((X), 8))
#else
#undef  REFLECT_DATA
#define REFLECT_DATA(X)         (X)
#endif

#if (REFLECT_REMAINDER == TRUE)
#undef  REFLECT_REMAINDER
#define REFLECT_REMAINDER(X)    ((crc) reflect((X), WIDTH))
#else
#undef  REFLECT_REMAINDER
#define REFLECT_REMAINDER(X)    (X)
#endif


// Function:    reflect()
// Description: Reorder the bits of a binary sequence, by reflecting
//              them about the middle position.
// Notes:       No checking is done that nBits <= 32.
// Returns:     The reflection of the original data.

static unsigned long
reflect(unsigned long data, unsigned char nBits)
{
	unsigned long  reflection = 0x00000000;
	unsigned char  bit;

	// Reflect the data about the center bit.
	for (bit = 0; bit < nBits; ++bit)
	{

		// If the LSB bit is set, set the reflection of it.
		if (data & 0x01)
		{
			reflection |= (1 << ((nBits - 1) - bit));
		}

		data = (data >> 1);
	}

	return (reflection);

}


// Function:    crcSlow()
// Description: Compute the CRC of a given message.
// Notes
// Returns:     The CRC of the message.
crc crcSlow(unsigned char const message[], int nBytes)
{
	crc remainder = INITIAL_REMAINDER;
	int byte;
	unsigned char bit;

	// Perform modulo-2 division, a byte at a time.
	for (byte = 0; byte < nBytes; ++byte)
	{
		// Bring the next byte into the remainder.
		remainder ^= (REFLECT_DATA(message[byte]) << (WIDTH - 8));

		// Perform modulo-2 division, a bit at a time.
		for (bit = 8; bit > 0; --bit)
		{

			// Try to divide the current data bit.
			if (remainder & TOPBIT)
			{
				remainder = (remainder << 1) ^ POLYNOMIAL;
			}
			else
			{
				remainder = (remainder << 1);
			}
		}
	}

	// The final remainder is the CRC result.
	return (REFLECT_REMAINDER(remainder) ^ FINAL_XOR_VALUE);
}

// ----------------------------------------------------------------------

int main(void)
{
	unsigned char  test[] = "123456789";
	crc result;

	result = crcSlow(test, 9);

	if(result != CHECK_VALUE)
	{
	  return 1;
	}
	else
	{
	  return 0;
	}
}
