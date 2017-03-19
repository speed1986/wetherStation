#include <avr/io.h>
#include "TWI.h"
#include <util/twi.h>

#define SCL_CLOCK 100000

#define ADRW_ACK 18
#define DATA_ACK 28

#define godzina		0
#define minuta		1
#define sekunda		2

#define rok_reszta	0
#define rok_RAM		1
#define miesiac		2
#define dzien		3
#define dzien_tyg	4

void odczyt_czasu(uint8_t *);
void odczyt_daty(uint8_t *);

char* wysw_dzien_tyg(uint8_t);
void wysw_czas(uint8_t *);

