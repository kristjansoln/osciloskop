/*
* oscilloscope.c
*
* Created: 6. 08. 2021 19:05:54
* Author: Kristjan Šoln
*/

#include "oscilloscope.h"

////////////////////////////
// hardware level functions
////////////////////////////

char b_ADC_active_flag = 0;

// Inicializacija globalnega bufferja
struct buffer_t buff_0 = {0};
struct buffer_t buff_1 = {0};
struct buffer_t buff_2 = {0};
struct buffer_t buff_3 = {0};
//struct buffer_t buff_4 = {0};
//struct buffer_t buff_5 = {0};
//struct buffer_t buff_6 = {0};
//struct buffer_t buff_7 = {0};
struct buffer_t *buffer_pointers[] = {&buff_0, &buff_1, &buff_2, &buff_3, 0, 0, 0, 0};
//struct buffer_t *buffer_pointers[] = {0, 0, &buff_2, &buff_3, 0, 0, 0, 0};

void osc_IO_init()
{
	// Inputs configuration
	DDRC &= ~((1 << DDRC0) | (1 << DDRC1) | (1 << DDRC2) | (1 << DDRC3) | (1 << DDRC4) | (1 << DDRC5)); // pins as inputs
	DDRE &= ~((1 << DDRE2) | (1 << DDRE3));
	PORTC &= ~((1 << PORTC0) | (1 << PORTC1) | (1 << PORTC2) | (1 << PORTC3) | (1 << PORTC4) | (1 << PORTC5)); // disable pullups on pins
	PORTE &= ~((1 << PORTE2) | (1 << PORTE3));
	DIDR0 = 0xFF; // Disable digital input buffer on all ADC channels.

	// Outputs configuration
	// In case this config is modified, remember to modify the  voltage_controller function.
	// Output index = channel index = bit offset
	DDRE |= ((1 << DDRE0) | (1 << DDRE1) | (1 << DDRE2) | (1 << DDRE3));
	PORTE |= ((1 << PORTE0) | (1 << PORTE1) | (1 << PORTE2) | (1 << PORTE3));
	return;
}

void osc_ADC_init(osc_AD_init_type_t init_type)
{
	// ADMUX register
	ADMUX &= ~((3 << REFS0) | (1 << ADLAR) | (0xF << MUX0)); // clear previous bits
	ADMUX |= (01 << REFS0);									 // select AVCC as voltage reference
	ADMUX |= (1 << ADLAR);									 // left adjust the result (8 bit precision allows this)

	// ADCSRA register
	ADCSRA = 0x00; // Clear register
	//ADCSRA |= (6 << ADPS0);				// ADC clock prescaler - 0b110 - 64 - 250 kHz - TODO: TO PROBAJ POJACAT NA 500 kHz
	//ADCSRA |= (1 << ADPS0) | (1 << ADPS2);	// ADC clock prescaler - 0b101 - 32 - 500 kHz
	//ADCSRA |= (1 << ADPS2); // ADC clock prescaler - 0b100 - 16 - 1 MHz
	ADCSRA |= (1 << ADPS1) | (1 << ADPS0);	// ADC clock prescaler - 0b101 - 8 - 2 MHz
	//ADCSRA |= (1 << ADPS1);				// ADC clock prescaler - 0b100 - 4 - 4 MHz

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

void osc_ADC_output_high()
{
	PORTB |= (1 << PORTB1);
	return;
}

void osc_ADC_output_low()
{
	PORTB &= ~(1 << PORTB1);
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
		prev_chan = c_current_ADC_channel;
		osc_ADC_increment_channel();
		ADCSRA |= (1 << ADSC);
		// Run controller function
		osc_voltage_controller(c_value, prev_chan);
	}

	return;
}

////////////////////////////
// system level functions
////////////////////////////

void osc_voltage_controller(char val, char prev_chan_v)
{
	// This function is dependent on the output configuration
	// Output index = channel index = bit offset
	if (val > OSC_CTRL_UPPER_TRESH_VOLTAGE)
	{
		PORTE |= (1 << prev_chan_v); // Stop charging - pin to high
	}
	else if (val < OSC_CTRL_BOTTOM_TRESH_VOLTAGE)
	{
		PORTE &= ~(1 << prev_chan_v); // Start charging - pin to low
	}
	// else do nothing - value within treshold
	return;
}

///////////////////////////////
// application level functions
///////////////////////////////
const int osc_LCD_colors_array[8] = {ILI9341_YELLOW, ILI9341_RED, ILI9341_BLUE, ILI9341_PURPLE,
									 ILI9341_CYAN, ILI9341_PINK, ILI9341_OLIVE, ILI9341_ORANGE};

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
	ILI9341_fillScreen(ILI9341_BLACK);
	osc_LCD_draw_bg();
	return;
}

void osc_LCD_draw_bg()
{
	// Draw axis
	ILI9341_drawFastVLine(OSC_LCD_X_OFFSET, OSC_LCD_Y_OFFSET, (int)(255.f * OSC_LCD_Y_SCALE_FACTOR), ILI9341_WHITE);
	ILI9341_drawFastHLine(OSC_LCD_X_OFFSET, OSC_LCD_Y_OFFSET + (int)(255.f * OSC_LCD_Y_SCALE_FACTOR), BUFFER_LENGTH, ILI9341_WHITE);
	// Draw treshold lines
	ILI9341_drawFastHLine(OSC_LCD_X_OFFSET, OSC_LCD_Y_OFFSET + (int)((float)(255 - OSC_CTRL_UPPER_TRESH_VOLTAGE) * OSC_LCD_Y_SCALE_FACTOR), BUFFER_LENGTH, ILI9341_DARKGREY);
	ILI9341_drawFastHLine(OSC_LCD_X_OFFSET, OSC_LCD_Y_OFFSET + (int)((float)(255 - OSC_CTRL_BOTTOM_TRESH_VOLTAGE) * OSC_LCD_Y_SCALE_FACTOR), BUFFER_LENGTH, ILI9341_DARKGREY);
	// Oznake
	UG_PutString(5, OSC_LCD_Y_OFFSET, "255");
	UG_PutString(5, OSC_LCD_Y_OFFSET + (int)(255.f * OSC_LCD_Y_SCALE_FACTOR) - 10, "0");
	char temp[5];
	sprintf(temp, "%d", OSC_CTRL_UPPER_TRESH_VOLTAGE);
	UG_PutString(5, OSC_LCD_Y_OFFSET + (int)((float)(255 - OSC_CTRL_UPPER_TRESH_VOLTAGE) * OSC_LCD_Y_SCALE_FACTOR) - 5, temp);
	sprintf(temp, "%d", OSC_CTRL_BOTTOM_TRESH_VOLTAGE);
	UG_PutString(5, OSC_LCD_Y_OFFSET + (int)((float)(255 - OSC_CTRL_BOTTOM_TRESH_VOLTAGE) * OSC_LCD_Y_SCALE_FACTOR) - 5, temp);
	return;
}

void osc_LCD_draw_legend()
{
	char string_to_display[20];
	//{"a", "b", "c", "d", "e", "f", "g", "h"}[i];
	const char *pin_tags[8] = {"PC0", "PC1", "PC2", "PC3", "PC4", "PC5", "PE2", "PE3"};

	ILI9341_fillScreen(ILI9341_BLACK);

	for (int i = 0; i < 8; i++)
	{
		// Draw dot (channel-in-use indicator)
		if ((i >= OSC_ADC_BOTTOM_ADC_CHANNEL_NUM) && (i <= OSC_ADC_UPPER_ADC_CHANNEL_NUM))
			UG_PutString(10, (40 + i * 20), "*");
		// Print channel name
		sprintf(string_to_display, "AD channel %d", i);
		UG_PutString(20, (40 + i * 20), string_to_display);
		// Print pin number
		UG_PutString(170, (40 + i * 20), pin_tags[i]);
		// Draw line
		ILI9341_drawFastHLine(210, (40 + i * 20 + 5), 70, osc_LCD_colors_array[i]);
	}
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

void osc_LCD_display_vals(struct buffer_t *buff, osc_LCD_display_t display_type, int color_index)
{
	char data = 0;

	for (int i = 0; i < BUFFER_LENGTH; i++)
	{
		if (BUFF_get_data(buff, &data) == BUFFER_OK) // Print to LCD
		{
			switch (display_type) // Izberi način prikaza
			{
			case OSC_LCD_USE_DOTS:
				osc_LCD_draw_dot_by_val(data, (i + 1), osc_LCD_colors_array[color_index]);
				break;
			case OSC_LCD_USE_LINES:
				osc_LCD_draw_line_by_val(data, i + 1, osc_LCD_colors_array[color_index]);
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