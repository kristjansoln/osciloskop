/*
* oscilloscope.h
*
* Created: 6. 08. 2021 19:06:22
* Author: Kristjan �oln
*
* Projekt Osciloskop je prvi del projekta Chopper driver - Mi�ko gonilnik za stepper motor.
* Cilj projekta Osciloskop je dovolj hitro branje �estih AD vhodov, da bo krmilnik
* lahko krmilil tok skozi navitja preko hitrega preklapljanja krmilnih tranzistorjev.
* Dodatna funkcionalnost tega projekta je prikaz napetosti v odvisnosti od �asa na grafu,
* torej osnovna funkcionalnost osciloskopa, ki se bo lahko izklju�ila zaradi �asovne
* potratnosti pisanja na LCD.
*/

#ifndef OSCILLOSCOPE_H_
#define OSCILLOSCOPE_H_


// Hardware level
#include <avr/io.h>
#include <avr/interrupt.h>
#include "lib/buffer.h"

typedef enum osc_AD_init_type {OSC_AD_INIT_USE_POOLING, OSC_AD_INIT_USE_INTERRUPT} osc_AD_init_type_t;

void osc_IO_init();	// Initialize IO ports. Modify this function if necessary.
void osc_ADC_init(osc_AD_init_type_t init_type); // Initialize AD converter, use OSC_AD_INIT_USE_POOLING, OSC_AD_INIT_USE_INTERRUPT as argument. Dont forget sei().
void osc_ADC_select_channel(uint8_t channel);	// Select ADC channel to read from.
uint8_t osc_ADC_Read_by_pooling();		// Return the value of the currently selected ADC channel using pooling.
void osc_ADC_start_conversion();        // Write to ADCSRA to start a conversion

// System level

// Application level
#include "lib/LCD_Ili9341.h"
#define LCD_USE_PRINTF
void osc_LCD_init();    // Prepare the LCD to be used within the oscilloscope function
void osc_LCD_show_value_at_XY(int x, int y, int value);     // Print data to LCD. Consider using printf() tho.

#endif