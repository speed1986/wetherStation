#include "konwersja.h"

uint8_t IntToBcd(uint8_t in)
{
	uint8_t temp=0;
	temp =  in/10 << 4;
	temp |= in % 10;
	return temp;
}

uint8_t BcdToInt(uint8_t bcd)
{
	uint8_t temp=0;
	temp = 10*(bcd>>4);
	temp += bcd & 0x0F;
	return temp;
}
