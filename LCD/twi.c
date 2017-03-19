#include "TWI.h"
#include <util/twi.h>

#define F_CPU 8000000
#define SCL_CLOCK 100000

#define ADRW_ACK 18
#define DATA_ACK 28
//-------------------------------------
// Generate START signal
//-------------------------------------
void TWI_Start(void)
{
TWCR=(1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
while (!(TWCR & (1<<TWINT)));
}
//-------------------------------------
// Generate STOP signal
//-------------------------------------
void TWI_Stop(void)
{
TWCR=(1<<TWINT)|(1<<TWEN)|(1<<TWSTO);
while ((TWCR & (1<<TWSTO)));
}
//-------------------------------------
// Write byte to slave
//-------------------------------------
char TWI_Write(char data)
{
TWDR=data;
TWCR=(1<<TWINT) | (1<<TWEN);
while (!(TWCR & (1<<TWINT)));
if((TWSR==ADRW_ACK) | (TWSR==DATA_ACK))
  return 0;
else
  return 1;
}
//-------------------------------------
// Read byte from slave
//-------------------------------------
char TWI_Read(char ack)
{
TWCR=ack 
	   ? ((1 << TWINT) | (1 << TWEN) | (1 << TWEA)) 
	   : ((1 << TWINT) | (1 << TWEN)) ;
while (!(TWCR & (1<<TWINT)));
return TWDR;
}
//-------------------------------------
// Initialize TWI
//-------------------------------------
void TWI_Init(void)
{
TWBR=((F_CPU/SCL_CLOCK)-16)/2;
}


