#include "timers.h"



void Timer1_Init()
{
	TCCR1A=(1<<WGM10) | (1<<COM1A1) | (1<<COM1B1); //8bit PWM, phase correct
	TCCR1B=(1<<CS12) | (1<<WGM12);            
	OCR1A=0x00;
	OCR1B=0x00;
}
