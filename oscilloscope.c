/*
* oscilloscope_lib.c
*
* Created: 6. 08. 2021 19:05:54
* Author: Kristjan Šoln
* main branch
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
	DDRC &= ~((1 << DDRC2) | (1 << DDRC3));	   // joystick pins as inputs
	PORTC &= ~((1 << PORTC2) & (1 << PORTC3)); // disable pullups on joystick pins
	DIDR0 |= (1 << PINC2) | (1 << PINC3);	   // Disable digital input buffer on ADC channel 2 and 3

	return;
}


void osc_ADC_init(osc_AD_init_type_t init_type)
{
	// ADMUX register
	ADMUX &= ~((3 << REFS0) | (1 << ADLAR) | (0xF << MUX0)); // clear previous bits
	ADMUX |= (01 << REFS0);									 // select AVCC as voltage reference
	ADMUX |= (1 << ADLAR);									 // left adjust the result (8 bit precision allows this)
	// Channel selection => druga funkcija (write to MUX bits)

	// ADCSRA register
	ADCSRA = 0x00;			// Clear register
	ADCSRA |= (6 << ADPS0); // ADC clock prescaler - 110 - 64 - 250 kHz - TODO: TO PROBAJ POJA�AT NA 500 kHz
	/* NOTE:
	 * Resolucija se (o�itno) nastavi zgolj s prescalerjem, torej �e bo ADC clock
	 * vi�ji od 200 kHz, bo resolucija manj�a. Ne vem, �e se za to ADC conv. end flag
	 * setta prej, ampak je vsaj frekvenca vi�ja.
	 */

// ADCSRB register
	ADCSRB = 0x00; // Clear register
	
	// Power reduction register
	PRR0 &= ~(1<<PRADC);	// Disable ADC power reduction
	
	// Triggering and interrupts setup. 	NOTE: dont forget sei();
	switch(init_type)
	{
		case OSC_AD_INIT_USE_POOLING:	// Triggera se ročno, na koncu moraš sam poolat da prebereš rezultat.
		break;
		case OSC_AD_INIT_USE_INTERRUPT: // Triggera se ro�no, na koncu konverzije pro�i interrupt.
		ADCSRA |= (1<<ADIE);	// AD interrupt enable
		//sei(); To raje naredi v mainu (?)
		break;
	}
	
	ADCSRA |= (1<<ADEN); // ADC ENABLE - povsem na koncu
	return;
}

void osc_ADC_select_channel(uint8_t channel)
{
	ADMUX = (ADMUX & 0xF0) | (channel & 0x0F); // This is safer
	//ADMUX = (ADMUX & 0xF0) | channel;		// This is faster
	return;
}

uint8_t osc_ADC_Read_by_pooling()
{
	ADCSRA |= (1 << ADSC);
	while (ADCSRA & (1 << ADSC)) {}
	return ADCH; 	// Read the 8 bytes of the result => the upper byte of the 16b reg.
}

////////////////////////////
// system level functions
////////////////////////////

///////////////////////////////
// application level functions
///////////////////////////////

void osc_LCD_init()
{
	LCD_Init();		// Initialize the LCD
	// TODO: Prepare to draw
	return;
}

void osc_LCD_show_value_at_XY(int x, int y, int value)
{
	char txt[50];
	sprintf(txt, "%4d", value);
	UG_PutString(x, y, txt);
	return;
}
