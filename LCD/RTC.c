#include "RTC.h"
#include "HD44780.h"
#include "konwersja.h"

void odczyt_czasu(uint8_t * bufor)
{
	TWI_Init();
	TWI_Start();
	TWI_Write(0xA0);
	TWI_Write(0x02);
	TWI_Stop();
	TWI_Start();
	TWI_Write(0xA1);
	bufor[sekunda]	= BcdToInt(TWI_Read(ACK));
	bufor[minuta]	= BcdToInt(TWI_Read(ACK));
	bufor[godzina]	= BcdToInt(TWI_Read(NACK));
	TWI_Stop();
}
void odczyt_daty(uint8_t * bufor)
{
	uint8_t temp = 0;
	TWI_Init();
	TWI_Start();
	TWI_Write(0xA0);
	TWI_Write(0x05);
	TWI_Stop();
	TWI_Start();
	TWI_Write(0xA1);
	temp = TWI_Read(ACK);
	bufor[rok_reszta]	= temp >> 6;
	bufor[dzien]		= temp & 0x3F;
	temp = TWI_Read(NACK);
	bufor[miesiac]		= temp & 0x1F;
	bufor[dzien_tyg]	= temp >> 5;
	TWI_Stop();
	TWI_Start();
	TWI_Write(0xA0);
	TWI_Write(0x10);
	TWI_Stop();
	TWI_Start();
	TWI_Write(0xA1);
	bufor[rok_RAM]	= TWI_Read(NACK);
	TWI_Stop();
}

char* wysw_dzien_tyg(uint8_t dzien_t)
{
	switch(dzien_t)
	{
		case 0: return " niedziela  ";	break;
		case 1: return "poniedzialek";	break;
		case 2: return "   wtorek   ";	break;
		case 3: return "   sroda    ";	break;
		case 4: return "  czwartek  ";	break;
		case 5: return "   piatek   ";	break;
		case 6: return "   sobota   ";	break;
		default: return "            ";
	}
}
void wysw_czas(uint8_t * bufor)
{
	LCD_WriteInt(bufor[godzina],1);
	LCD_WriteText(":");
	LCD_WriteInt(bufor[minuta],1);
	LCD_WriteText(":");
	LCD_WriteInt(bufor[sekunda],1);
}
