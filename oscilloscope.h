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

#include <avr/io.h>

// Hardware level
typedef enum osc_AD_init_type {OSC_AD_INIT_USE_POOLING, OSC_AD_INIT_USE_INTERRUPT} osc_AD_init_type_t;

void osc_IO_init();	// Initialize IO ports - can modify
void osc_AD_init(osc_AD_init_type_t init_type); // Initialize AD converter, use OSC_AD_INIT_USE_POOLING, OSC_AD_INIT_USE_INTERRUPT as argument
void osc_AD_select_channel(uint8_t channel);	// Select ADC channel to read from
uint16_t osc_ADC_Read_by_pooling();		// Return the value of the currently selected ADC channel using pooling

// System level

// Application level
#include "lib/LCD_Ili9341.h"
#define LCD_USE_PRINTF
void osc_LCD_init();
void osc_LCD_show_value_at_XY(int x, int y, int value);

#endif