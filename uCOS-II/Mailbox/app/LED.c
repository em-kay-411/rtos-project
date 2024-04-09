/********************************************************/
/* This Program is to demonstrate the LED Functionality */
/*******************************************************
Author 		: Ashish
Board 		: SBC_ARM7
Processor 	: LPC2148

LED Mapping 	: LED0 thru LED7 : P0.4 thru P0.11

Jumper Settings :

********************************************************/

#include<lpc214x.h>

/*----LED Mapping for the board----*/
#define LED_MASK 	0x000FF000


/*---- LED initialize function ----*/
void LEDInit(void)
{
// PINSEL0 &= ~LED_MASK;	  		//Set LED pins as GPIO
 IODIR0 |= LED_MASK;			// set the direction of the pins as  output : 1
}


void LEDset(unsigned char temp)
{
IOSET0 = (1<<(temp+12));
}

void LEDclr(unsigned char temp)
{
IOCLR0 = (1<<(temp+12));
}
