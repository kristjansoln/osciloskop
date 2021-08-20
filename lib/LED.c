/*
 * LED.c
 *
 * Created: 24-Feb-16 11:38:24
 * Author : maticpi
 */ 

#include "LED.h"

void LED_Init()
{
	//PORT E Bits 0-3 LEDs, bits 4-7 not implemented
	PORTE=0;
	DDRE=0x0F;
}

void LED_AllOff(void)
{
	PORTE=0;
}

void LED_AllOn(void)
{
	PORTE=0xFF;
}

void LED_0On()  {PORTE |= (1<<0); }
void LED_0Off() {PORTE &= ~(1<<0);}
void LED_0Tgl() {PORTE ^= (1<<0);}
void LED_1On()  {PORTE |= (1<<1); }
void LED_1Off() {PORTE &= ~(1<<1);}
void LED_1Tgl() {PORTE ^= (1<<1);}
void LED_2On()  {PORTE |= (1<<2); }
void LED_2Off() {PORTE &= ~(1<<2);}
void LED_2Tgl() {PORTE ^= (1<<2);}
void LED_3On()  {PORTE |= (1<<3); }
void LED_3Off() {PORTE &= ~(1<<3);}
void LED_3Tgl() {PORTE ^= (1<<3);}
	
void LED_Set(uint8_t leds)
{
	PORTE |= leds;
}

void LED_Clr(uint8_t leds)
{
	PORTE &= ~leds;
}

uint8_t LED_Get()
{
//	return (PORTE & 0x0F);	//mask is't necessary because upper 4 bits are allways read as 0
	return PORTE;
}
