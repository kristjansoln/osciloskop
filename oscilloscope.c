/*
* oscilloscope_lib.c
*
* Created: 6. 08. 2021 19:05:54
*  Author: Kristjan Šoln
*/


#include "oscilloscope.h"

////////////////////////////
// hardware level functions
////////////////////////////

void osc_IO_init()
{
	// Initialize the IO pins
	// X - PC2 - ADC2
	// Y - PC3 - ADC3
	DDRC &= ~((1<<DDRC2) | (1<<DDRC3));		// joystick pins as inputs
	PORTC &= ~((1<<PORTC2) & (1<<PORTC3));	// disable pullups on joystick pins
	DIDR0 |= (1<<PINC2) | (1<<PINC3);	// Disable digital input buffer on ADC channel 2 and 3
	
	return;
}


void osc_AD_init(osc_AD_init_type_t init_type)
{
	// ADMUX register
	ADMUX &= ~((3<<REFS0) | (1<<ADLAR) | (0xF<<MUX0));	// clear previous bits
	ADMUX |= (01<<REFS0);	// select AVCC as voltage reference
	ADMUX |= (1<<ADLAR);	// left adjust the result (8 bit precision allows this=
	// Channel selection => write to MUX bits.
	
	// ADCSRA register
	ADCSRA = 0x00;	// Clear register
	ADCSRA |= (6<<ADPS0);	// ADC clock prescaler - 110 - 64 - 250 kHz - TODO: TO PROBAJ POJAÈAT NA 500 kHz
	/* NOTE:
	 * Resolucija se (oèitno) nastavi zgolj s prescalerjem, torej èe bo ADC clock
	 * višji od 200 kHz, bo resolucija manjša. Ne vem, èe se za to ADC conv. end flag
	 * setta prej, ampak je vsaj frekvenca višja.
	 */
	
	// Triggering and interrupts
	// TODO: dont forget sei();
	// TODO: finish this
	switch(init_type)
	{
		case OSC_AD_INIT_USE_POOLING:
		break;
		case OSC_AD_INIT_USE_INTERRUPT:
		break;
	}
	return;
}

void osc_AD_select_channel(uint8_t channel)
{
	ADMUX = (ADMUX & 0xF0) | (channel & 0x0F );	// This is safer
	//ADMUX = (ADMUX & 0xF0) | channel;		// This is faster
	return;
}

uint8_t osc_ADC_Read_by_pooling()
{
	ADCSRA |= (1<<ADSC);
	while (ADCSRA & (1<<ADSC)) {}
	// Read the 8 bytes of the result => the upper byte of the 16b reg.
	return ADCH;
}


////////////////////////////
// system level functions
////////////////////////////


///////////////////////////////
// application level functions
///////////////////////////////


void osc_LCD_init()
{
	// Init the LCD
	LCD_Init();
	
	// TODO: Prepare to draw
	return;
}

void osc_LCD_show_value_at_XY(int x, int y, int value)
{
	char txt[50];
	sprintf(txt,"%4d", value);
	UG_PutString(x,y,txt);
	return;
}


