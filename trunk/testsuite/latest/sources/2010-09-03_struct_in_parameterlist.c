/* BI: The arguments for eatStruct are not located correctly
 * The stack parameters are stored at [r7 + #4], [r7 + #8] ...
 * The first Stack parameter should be passed in [r7 + #0]!
 */

typedef struct
{
  unsigned char* a;
  int b;
} PROTOCOL_DATA_UNIT;

int eatStruct(int x, int y, char z, PROTOCOL_DATA_UNIT pduData, int trailer)
{
  return pduData.b;
}

int main(void)
{
  unsigned char buffer[1];
  PROTOCOL_DATA_UNIT pduData;

  pduData.a = buffer;
  pduData.b = 0;

  return eatStruct(1, 1, 1, pduData, 1);
}
