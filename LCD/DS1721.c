#include "DS1721.h"
#include "HD44780.h"

void odczyt_temp(int * bufor, uint8_t adr)
{
	TWI_Init();
	do{
		TWI_Start();
		TWI_Write(0x90 | adr<<1);							//wyslanie adresu do czujnika i WRITE
		TWI_Write(0x51);
		TWI_Stop();
		TWI_Start();
		TWI_Write(0x90 | adr<<1);
		TWI_Write(0xAA);
		TWI_Start();
		TWI_Write(0x91 | adr<<1);
		bufor[czesc_calk]	= TWI_Read(ACK) << 8;
		bufor[czesc_calk]	= bufor[czesc_calk] >> 8;
		bufor[czesc_ulamk] = TWI_Read(NACK) * 10/256;
		TWI_Stop();
	}while(bufor[czesc_calk]==-60);

//__________ interpretacja temperatury ujemnej _______________
	if (bufor[czesc_calk]<0)
	{
		bufor[znak_temp] = 1;
		if(bufor[czesc_ulamk]!=0){	
			bufor[czesc_calk] += 1;
			bufor[czesc_ulamk] = 10 - bufor[czesc_ulamk];}
	}
	else
		bufor[znak_temp] = 0;
}
	
void wysw_temp(int * bufor, uint8_t jednostka)
{
	if (bufor[czesc_calk]>=0){	
		if (bufor[znak_temp] == 1)						// -1 < temp < 0
			LCD_WriteText("-");	
		else									//      temp >= 0
			LCD_WriteText(" ");	
	}

	LCD_WriteInt(bufor[czesc_calk],0);
	LCD_WriteText(",");
	LCD_WriteInt(bufor[czesc_ulamk],0);

	if (jednostka==1){
		LCD_WriteData(stopien);
		LCD_WriteText("C");}
	else
		LCD_WriteText("  ");
}
void sprawdz_temp_min(int * bufor_t, int * bufor_t_min, uint8_t * bufor_cz, uint8_t * bufor_cz_min)
{
	uint8_t zapisz_dane = 0;

	if (bufor_t[czesc_calk] < bufor_t_min[czesc_calk])
		zapisz_dane = 1;
	else if (bufor_t[czesc_calk] == bufor_t_min[czesc_calk])
	{
		if ((bufor_t[znak_temp]==1) && (bufor_t[czesc_ulamk] > bufor_t_min[czesc_ulamk]))
			zapisz_dane = 1;
		else if ((bufor_t[znak_temp]==0) && (bufor_t[czesc_ulamk] < bufor_t_min[czesc_ulamk]))
			zapisz_dane = 1;
	}

	if (zapisz_dane == 1)
	{
		for (uint8_t i=0;i<3;i++)
		{
			bufor_t_min[i]=bufor_t[i];
			bufor_cz_min[i]=bufor_cz[i];
		}
		zapisz_dane = 0;
	}
}
