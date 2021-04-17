#include <avr/io.h>
#include "TWI.h"
#include <util/twi.h>

#define SCL_CLOCK 100000

#define ADRW_ACK 18
#define DATA_ACK 28

#define stopien		0xDF

#define znak_temp	0
#define czesc_calk	1
#define czesc_ulamk	2

#define godzina		0
#define minuta		1
#define sekunda		2

#define rok_reszta	0
#define rok_RAM		1
#define miesiac		2
#define dzien		3
#define dzien_tyg	4




void odczyt_temp(int *,uint8_t);
void wysw_temp(int *,uint8_t);
void sprawdz_temp_min(int *,int *,uint8_t *,uint8_t *);
