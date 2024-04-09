/***************************************************************/
/* This Program is to demonstrate the On Chip ADC Functionality */
/****************************************************************
Author 		: Ashish
Board 		: SBC_ARM7
Processor 	: LPC2148

ADC 0 Mapping :  AD0.1 -> P0.28
				 AD0.2 -> P0.29

Jumper Settings :  No jumper selection

*****************************************************************/
#include "lpc214x.h"
#include "stdio.h"

void ADCInit()
{
	PINSEL1 &= 0xF0FFFFFF;			//Configure pins P0.28 and P0.29 as ADC pins
	PINSEL1 |= 0x05000000;

	AD0CR = 0x00210302;				//Set Burst mode, CLK_DIV, Channels, 10bit ADC/11 cycles
	AD0CR |= 1<<24;				   	//Start ADC
}

unsigned int ADC_Read(void)
{
	static unsigned int ad1_data;
	if(AD0GDR & 0x80000000)		   		//Check DONE bit
		{
			ad1_data = (AD0GDR & 0x0000FFC0)>>6;			//Assign ADC result to ad1_data n display in mV
		}			 
	return ((ad1_data*3300)/1024);
}





