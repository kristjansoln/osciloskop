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

// Inicializacija globalnega bufferja
struct buffer_t buff_2 = {0};
struct buffer_t buff_3 = {0};
struct buffer_t *buffer_pointers[] = {0, 0, &buff_2, &buff_3, 0, 0, 0, 0};

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
	ADCSRA |= (6 << ADPS0); // ADC clock prescaler - 110 - 64 - 250 kHz - TODO: TO PROBAJ POJACAT NA 500 kHz
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

	c_current_ADC_channel = OSC_ADC_BOTTOM_ADC_CHANNEL_NUM; // Nastavi kanal, na katerem bo potekala prva meritev

	ADCSRA |= (1 << ADEN); // ADC ENABLE - povsem na koncu
	return;
}

void osc_ADC_select_channel(uint8_t channel)
{
	ADMUX = (ADMUX & 0xF0) | (channel & 0x0F); // This is safer
	//ADMUX = (ADMUX & 0xF0) | channel;		// This is faster
	return;
}

void osc_ADC_increment_channel()
{
	// Increment channel + fast modulo
	if ((++c_current_ADC_channel) > OSC_ADC_UPPER_ADC_CHANNEL_NUM)
		c_current_ADC_channel = OSC_ADC_BOTTOM_ADC_CHANNEL_NUM;

	ADMUX = (ADMUX & 0xF0) | (c_current_ADC_channel & 0x0F); // This is safer
	//ADMUX = (ADMUX & 0xF0) | c_current_ADC_channel;		// This is faster
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
	// Če je buffer poln, ne proži naslednje konverzije in cleara flag, ki je indikator za konec merilnega cikla
	if (BUFF_store_data(c_value, buffer_pointers[c_current_ADC_channel]) == BUFFER_ERROR)
		b_ADC_active_flag = 0; // Zamenjaj s clearADCflag()?
	else
	{
		// Set next channel & start conversion
		osc_ADC_increment_channel();
		ADCSRA |= (1 << ADSC); 
	}
	return;
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
	osc_LCD_draw_bg();

	return;
}

void osc_LCD_show_value_at_XY(int x, int y, int value)
{
	char txt[50];
	sprintf(txt, "%4d", value);
	UG_PutString(x, y, txt);
	return;
}

void osc_LCD_clear()
{
	// Črn kvadrat čez ekran
	ILI9341_fillScreen(ILI9341_BLACK);
	osc_LCD_draw_bg();
	return;
}

void osc_LCD_draw_bg()
{
	ILI9341_drawFastVLine(OSC_LCD_X_OFFSET, OSC_LCD_Y_OFFSET, (int)(255.f * OSC_LCD_Y_SCALE_FACTOR), ILI9341_WHITE);
	ILI9341_drawFastHLine(OSC_LCD_X_OFFSET, OSC_LCD_Y_OFFSET + (int)(255.f * OSC_LCD_Y_SCALE_FACTOR), BUFFER_LENGTH, ILI9341_WHITE);

	UG_PutString(5, OSC_LCD_Y_OFFSET, "255");
	UG_PutString(5, OSC_LCD_Y_OFFSET + (int)(255.f * OSC_LCD_Y_SCALE_FACTOR) - 10, "0");
	return;
}

void osc_LCD_draw_dot_by_xy(int x, int y, int color)
{
	ILI9341_drawPixel(x, y, color);
	return;
}

void osc_LCD_draw_dot_by_val(char val, int x_offset, int color)
{
	int y = OSC_LCD_Y_OFFSET + (float)(255 - val) * OSC_LCD_Y_SCALE_FACTOR;
	int x = OSC_LCD_X_OFFSET + x_offset;
	osc_LCD_draw_dot_by_xy(x, y, color);
	return;
}

void osc_LCD_draw_line_by_val(char val, int x_offset, int color)
{
	int y = OSC_LCD_Y_OFFSET + (float)(255 - val) * OSC_LCD_Y_SCALE_FACTOR;
	int x = OSC_LCD_X_OFFSET + x_offset;
	int height = (int)((float)val * OSC_LCD_Y_SCALE_FACTOR);
	ILI9341_drawFastVLine(x, y, height, color);
	return;
}

void osc_LCD_display_vals(struct buffer_t *buff, osc_LCD_display_t display_type, int color)
{
	char data = 0;

	for (int i = 0; i < BUFFER_LENGTH; i++)
	{
		if (BUFF_get_data(buff, &data) == BUFFER_OK) // Print to LCD
		{
			switch (display_type) // Izberi način prikaza
			{
			case OSC_LCD_USE_DOTS:
				osc_LCD_draw_dot_by_val(data, (i + 1), color);
				break;
			case OSC_LCD_USE_LINES:
				osc_LCD_draw_line_by_val(data, i + 1, color);
				break;
			default:
				printf("Error displaying vals, check passed arguments");
				while (1)
				{
				}
				break;
			}
		}
		else
			break; // Buffer je prazen, koncaj pisanje
	}
	return;
}