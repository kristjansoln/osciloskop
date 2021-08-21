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

char b_ADC_active_flag = 0;

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

	// ADCSRA register
	ADCSRA = 0x00;			// Clear register
	ADCSRA |= (6 << ADPS0); // ADC clock prescaler - 110 - 64 - 250 kHz - TODO: TO PROBAJ POJA�AT NA 500 kHz
	/* NOTE:
	 * Resolucija se (ocitno) nastavi zgolj s prescalerjem, torej ce bo ADC clock
	 * višji od 200 kHz, bo resolucija manjsa. Ne vem, ce se za to ADC conv. end flag
	 * setta prej, ampak je vsaj frekvenca visja.
	 */

	// ADCSRB register
	ADCSRB = 0x00; // Clear register

	// Power reduction register
	PRR0 &= ~(1 << PRADC); // Disable ADC power reduction

	// Triggering and interrupts setup. 	NOTE: dont forget sei();
	switch (init_type)
	{
	case OSC_AD_INIT_USE_POOLING: // Triggera se ročno, na koncu moraš sam poolat da prebereš rezultat.
		break;
	case OSC_AD_INIT_USE_INTERRUPT: // Triggera se ro�no, na koncu konverzije pro�i interrupt.
		ADCSRA |= (1 << ADIE);		// AD interrupt enable
		break;
	}

	ADCSRA |= (1 << ADEN); // ADC ENABLE - povsem na koncu
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
	// Momentarily disable interrupts if enabled
	char ADCSRA_old = ADCSRA;
	ADCSRA = (ADCSRA_old & ~(1 << ADIE));
	// Execute a conversion
	ADCSRA |= (1 << ADSC);
	while (ADCSRA & (1 << ADSC))
	{
	}
	// Revert ADCSRA to previous state
	ADCSRA = ADCSRA_old;
	// Read the 8 bytes of the result => the upper byte of the 16b reg.
	return ADCH;
}

void osc_ADC_start_conversion()
{
	ADCSRA |= (1 << ADSC);
	return;
}

ISR(ADC_vect) // ADC conv. complete interrupt
{
	// Preberi vrednost
	char c_value = ADCH;
	if (BUFF_store_data(c_value, &buff) == BUFFER_ERROR)
	{
		b_ADC_active_flag = 0; // Zamenjaj s clearADCflag();
		LED_0On();
	}
	else
		ADCSRA |= (1 << ADSC); // ADC start conversion
}

////////////////////////////
// system level functions
////////////////////////////

///////////////////////////////
// application level functions
///////////////////////////////

void osc_LCD_init()
{
	LCD_Init(); // Initialize the LCD
	return;
}

void osc_LCD_show_value_at_XY(int x, int y, int value)
{
	char txt[50];
	sprintf(txt, "%4d", value);
	UG_PutString(x, y, txt);
	return;
}

void osc_LCD_display_vals()
{
	char val = 0;

	while (BUFF_get_data(&buff, &val) == BUFFER_OK)
		printf("%d \n", val);
	return;
}

void osc_LCD_draw_bg()
{
	return;
}

void osc_LCD_draw_dot_by_xy(int x, int y)
{
	return;
}

void osc_LCD_draw_dot_by_val(char val, char x_offset)
{
	return;
}