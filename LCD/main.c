#include<avr/io.h>
#include<util/delay.h>
#include "HD44780.h"
#include "twi.h"
#include "konwersja.h"
#include "DS1721.h"
#include "RTC.h"
#include "timers.h"
#define SCL_CLOCK 100000

#define stopien		0xDF

#define t0_var			(1<<PD0)
#define t1_var			(1<<PD1)
#define podsw_LCD		(1<<PB1)
#define brzeczyk		(1<<PB7)
#define led				(1<<PB2)
#define ir_in			(1<<PD3)

#define sw_up		(1<<PD5)
#define sw_left		(1<<PD6)
#define sw_right	(1<<PD7)
#define sw_down		(1<<PB0)
#define sw_przycisk	(1<<PD2)

#define sw_up_dir		PIND
#define sw_left_dir		PIND
#define sw_right_dir	PIND
#define sw_down_dir		PINB
#define przycisk_dir	PIND

#define left		0
#define right		1
#define up			2
#define down		3
#define przycisk	4

#define godzina		0
#define minuta		1
#define sekunda		2

#define rok_reszta	0
#define rok_RAM		1
#define miesiac		2
#define dzien		3
#define dzien_tyg	4



int temp_0[3];
int temp_1[3];
uint8_t czas[3];
uint8_t data[5];
uint8_t rok_temp=0;
uint8_t czas_temp=0;

int temp_0_min[]={0,99,99};
int temp_1_min[]={0,99,99};
uint8_t czas_temp_0_min[3];
uint8_t czas_temp_1_min[3];

uint8_t sw_check(uint8_t sw_nr)
{
	uint8_t temp=0;

	switch (sw_nr)
	{
		case left:
			if (!(sw_left_dir & sw_left))
				temp=1;
			break;
		case right:
			if (!(sw_right_dir & sw_right))
				temp=1;
			break;
		case up:
			if (!(sw_up_dir & sw_up))
				temp=1;
			break;
		case down:
			if (!(sw_down_dir & sw_down))
				temp=1;
			break;
		case przycisk:
			if (!(przycisk_dir & sw_przycisk))
				temp=1;
			break;
		default:
			temp=0;
	}
	if (temp == 1)
	{
		PORTB ^= brzeczyk;
		_delay_ms(50);
		PORTB ^= brzeczyk;
	}
	return temp;
}

void ustawianie_czasu(uint8_t h, uint8_t m, uint8_t s)	
{
	TWI_Init();
	TWI_Start();
	TWI_Write(0xA0);			
	TWI_Write(0x04);			// rejest godzin
	TWI_Write(IntToBcd(h));
	TWI_Stop();
	TWI_Start();
	TWI_Write(0xA0);			
	TWI_Write(0x03);			// rejestr minut
	TWI_Write(IntToBcd(m));
	TWI_Stop();
	TWI_Start();
	TWI_Write(0xA0);			
	TWI_Write(0x02);			// rejest sekund
	TWI_Write(IntToBcd(s));
	TWI_Stop();
}
// 			Opis rejestrów daty
//
// 0x05		| Y7 Y6 | Y5 Y4 Y3 Y2 Y1 Y0 |
//			|  rok  |   dzien w BCD		|
//
// 0x06		| W7 W6 W5 | M4 M3 M2 M1 M0 |
//		    | dzien tyg| miesiac w BCD  |
//
// 0x10


void ustawienie_daty(uint8_t y, uint8_t m, uint8_t d, uint8_t wd)
{
	uint8_t y_temp;
	uint8_t y_reszta;

	y_reszta	= y % 4;
	y_temp		= y / 4;
	TWI_Start();
	TWI_Write(0xA0);			
	TWI_Write(0x05);
	//TWI_Write(0x28);
	TWI_Write((y_reszta << 6) | IntToBcd(d));		
	TWI_Stop();
	TWI_Start();
	TWI_Write(0xA0);			
	TWI_Write(0x06);
	//TWI_Write(0x42);
	TWI_Write((wd << 5) | IntToBcd(m));	
	TWI_Stop();

	TWI_Start();
	TWI_Write(0xA0);			
	TWI_Write(0x10);
	TWI_Write(y_temp);
	TWI_Stop();

}

void set_time(uint8_t * bufor)
{
	uint8_t pozycja = 3;		//informuje o aktualnej pozycji kursora wskazuj¹cego zmienn¹ do ustawiania
	uint8_t set_poz = 0;		//informuje o zmiennej aktualnie ustawianej
	uint8_t set_on 	= 0;		//informuje czy byl ustawianiy czas
	uint8_t time_s[3];			//tablica lokalna do ustawiania czasu
	
	for (uint8_t i=0;i<3;i++)	//przepisanie aktualnego czasu do tablicy lokalnej
		time_s[i] = bufor[i];

	LCD_Clear();
	LCD_GoTo4(3,0);
	LCD_WriteText("Set Time");
	LCD_GoTo4(pozycja,2);
	LCD_WriteText("^^");
	do
	{
		LCD_GoTo4(3,1);
		wysw_czas(time_s);
		if(sw_check(right))
		{
			LCD_GoTo4(pozycja,2);
			LCD_WriteText("  ");
			if (pozycja==3 || pozycja==6)
				pozycja += 3;
			else if (pozycja==9)
				pozycja = 3;
			LCD_GoTo4(pozycja,2);
			LCD_WriteText("^^");
			_delay_ms(300);
		}
		set_poz = (pozycja / 3) - 1;
		if(sw_check(up))
		{
			set_on = 1;
			if (set_poz == godzina)							
			{
				if(time_s[godzina]<23)
					time_s[godzina]++;
				else
					time_s[godzina] = 0;
			}
			else if ((set_poz == minuta) || (set_poz == sekunda))
			{
				if((time_s[set_poz])<59)
					time_s[set_poz]++;
				else
					time_s[set_poz] = 0;
			}
			_delay_ms(300);
		}
		else if(sw_check(down))
		{
			set_on = 1;
			if (set_poz == godzina)
			{
				if(time_s[godzina]>0)
					time_s[godzina]--;
				else
					time_s[godzina] = 23;
			}
			if ((set_poz == minuta) || (set_poz == sekunda))
			{
				if((time_s[set_poz])>0)
					time_s[set_poz]--;
				else
					time_s[set_poz] = 59;
			}
			_delay_ms(300);
		}
	}while(!sw_check(left));
	if (set_on==1)
		ustawianie_czasu(time_s[godzina], time_s[minuta], time_s[sekunda]);
}

void set_date()
{
	LCD_Clear();
	LCD_GoTo4(4,0);
	LCD_WriteText("Set Date");
	LCD_GoTo4(3,1);
	LCD_WriteText("03/12/2014");
	do{}while(!sw_check(left));
}

void menu()
{
	uint8_t pozycja	= 0;
	LCD_Clear();
	LCD_GoTo4(1,0);
	LCD_WriteText("Set Time");
	LCD_GoTo4(1,1);
	LCD_WriteText("Set Date");
	LCD_GoTo4(0,pozycja);
	LCD_WriteText(">");
	do
	{
		if(sw_check(up) || sw_check(down))
		{
			LCD_GoTo4(0,pozycja);
			LCD_WriteText(" ");
			if (pozycja==1)
				pozycja = 0;
			else
				pozycja = 1;
			LCD_GoTo4(0,pozycja);
			LCD_WriteText(">");
		}
		_delay_ms(300);
		if(sw_check(right) && pozycja ==0)
			set_time(czas);
		if(sw_check(right) && pozycja ==1)
			set_date();
	}while(!sw_check(left));
	LCD_Clear();
}

void temp_min()
{
	LCD_Clear();
	LCD_GoTo4(0,0); LCD_WriteText("Temp. min.:");
	LCD_GoTo4(0,3); LCD_WriteText("Esc");
	LCD_GoTo4(11,3); LCD_WriteText("Clear");
	do
	{
		LCD_GoTo4(0,1);	LCD_WriteData(0x7F);	wysw_temp(temp_0_min,0);
		LCD_GoTo4(8,1);	wysw_czas(czas_temp_0_min);
		LCD_GoTo4(0,2); LCD_WriteData(0x7E);	wysw_temp(temp_1_min,0);
		LCD_GoTo4(8,2); wysw_czas(czas_temp_1_min);

		if(sw_check(right))
		{
			for (int i=0;i<3;i++)
			{
				temp_0_min[i]		= 0;
				temp_1_min[i]		= 0;
				czas_temp_0_min[i]	= 0;
				czas_temp_1_min[i]	= 0;
			}

		}
	}while(!sw_check(left));
	LCD_Clear();
}
void temp_max()
{
	LCD_Clear();
	LCD_GoTo4(0,0); LCD_WriteText("Temp. max.:");
	LCD_GoTo4(0,3); LCD_WriteText("Esc");
	LCD_GoTo4(11,3); LCD_WriteText("Clear");
	do
	{

	}while(!sw_check(left));
	LCD_Clear();

}

int main(void)
{
	PORTC = 0x30;		// pull-up na liniach SDA, SCL (interfejs I2C)
	DDRD = 0x00;		// port D jako wejœcia
	PORTD = 0xFF;		// pull-up na wejœciach portu D
	DDRB = 0xFE;		// PB0 - wejœcie, PB1..PB7 - wyjœcia
	PORTB |= (sw_down) | (brzeczyk);
	LCD_Initalize();
	Timer1_Init();
	
	//PORTB |= podsw_LCD; // w³¹czenie podœwietlania LCD przy starcie 
	OCR1A = 0xFF;
	
	LCD_GoTo4(0,0);
	LCD_WriteText("..."); 


	while(1)
	{
		


//////////////////// KONTRAST ////////////////////		
		while(sw_check(przycisk) && sw_check(up))
			OCR1A+=15;
		while(sw_check(przycisk) && sw_check(down))
			OCR1A-=15;

////////////////////// MENU //////////////////////
		if (sw_check(right))
			menu();
		if (sw_check(down))
			temp_min();
		if (sw_check(up))
			temp_max();

//////////////////////////////////////////////////

		LCD_GoTo4(0,0);
		LCD_WriteData(0x7F);
		if(!(PIND & t0_var)){
			odczyt_temp(temp_0,0);		
			wysw_temp(temp_0,0);}
		else
			LCD_WriteText(" OFF   ");

		LCD_GoTo4(8,0);
		LCD_WriteData(0x7E);
		if(!(PIND & t1_var)){
			odczyt_temp(temp_1,1);						
			wysw_temp(temp_1,1);}
		else
			LCD_WriteText(" OFF   ");
		
		LCD_GoTo4(4,1);
		odczyt_czasu(czas);	
		wysw_czas(czas);

		if (czas[minuta]==0 && czas_temp==59)
		{	
			PORTB ^= brzeczyk;
			_delay_ms(80);
			PORTB ^= brzeczyk;
			_delay_ms(40);
			PORTB ^= brzeczyk;
			_delay_ms(80);
			PORTB ^= brzeczyk;
		}
		else if (czas[minuta]==30 && czas_temp==29)
		{
			PORTB ^= brzeczyk;
			_delay_ms(80);
			PORTB ^= brzeczyk;
		}
		czas_temp = czas[minuta];

		odczyt_daty(data);
		if (data[rok_reszta]==0 && rok_temp==3)
		{
			rok_temp=data[rok_reszta];
			data[rok_RAM]++;
			TWI_Start();
			TWI_Write(0xA0);			
			TWI_Write(0x10);
			TWI_Write(data[rok_RAM]);
			TWI_Stop();
		}
		else
			rok_temp=data[rok_reszta];
		
		LCD_GoTo4(2,2);
		LCD_WriteText(wysw_dzien_tyg(data[dzien_tyg]));
	
		LCD_GoTo4(3,3);
		LCD_WriteInt(BcdToInt(data[dzien]),1);					LCD_WriteText("/");
		LCD_WriteInt(BcdToInt(data[miesiac]),1);				LCD_WriteText("/");
		LCD_WriteText("20");
		if ((data[rok_RAM] * 4 + data[rok_reszta])<10)
			LCD_WriteText("0");
		LCD_WriteInt(data[rok_RAM] * 4 + data[rok_reszta],0);

		sprawdz_temp_min(temp_0, temp_0_min, czas, czas_temp_0_min);
		sprawdz_temp_min(temp_1, temp_1_min, czas, czas_temp_1_min);

		//PORTB ^= led;
		//_delay_ms(200);
		
	}

	return 0;
}




