/*
* oscilloscope.h
*
* Created: 6. 08. 2021 19:06:22
* Author: Kristjan Soln
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
#include "lib/LED.h"

typedef enum osc_AD_init_type {OSC_AD_INIT_USE_POOLING, OSC_AD_INIT_USE_INTERRUPT} osc_AD_init_type_t;

extern char b_ADC_active_flag; // This flag is set while ADC is filling up the buffer

void osc_IO_init();	// Initialize IO ports. Modify this function if necessary.
void osc_ADC_init(osc_AD_init_type_t init_type); // Initialize AD converter, use OSC_AD_INIT_USE_POOLING, OSC_AD_INIT_USE_INTERRUPT as argument. Dont forget sei().
void osc_ADC_select_channel(uint8_t channel);	// Select ADC channel to read from.
uint8_t osc_ADC_Read_by_pooling();		// Return the value of the currently selected ADC channel using pooling. Momentarily disables ADC interrupts.
void osc_ADC_start_conversion();        // Write to ADCSRA to start a conversion

// System level

// Application level
#define LCD_USE_PRINTF
#include "lib/LCD_Ili9341.h"

#define OSC_LCD_Y_OFFSET 20
#define OSC_LCD_X_OFFSET 40
#define OSC_LCD_Y_SCALE_FACTOR 0.75f

typedef enum osc_LCD_display_type {OSC_LCD_USE_DOTS, OSC_LCD_USE_LINES} osc_LCD_display_t;

void osc_LCD_init();    // Prepare the LCD to be used within the oscilloscope function
void osc_LCD_show_value_at_XY(int x, int y, int value);     // Print data to LCD by coordinates
void osc_LCD_clear();		// Clear all
void osc_LCD_draw_bg();		// Draw background 
void osc_LCD_draw_dot_by_xy(int x, int y);	// Draw dot using coordinates
void osc_LCD_draw_line_by_xy(int x, int y); // Draw line using coordinates and value
void osc_LCD_draw_dot_by_val(char val, int x_offset);	// Draw dot using value and offset
void osc_LCD_draw_line_by_val(char val, int x_offset);	// Draw line using value and offset
void osc_LCD_display_vals(struct buffer_t *buff, osc_LCD_display_t display_type);    // Display buffer data on LCD. Use OSC_LCD_USE_DOTS or OSC_LCD_USE_LINES.

#endif