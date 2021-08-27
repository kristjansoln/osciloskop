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

// HARDWARE LEVEL
#include <avr/io.h>
#include <avr/interrupt.h>
#include "lib/buffer.h"

// Define the range of ADC channels -> remember to edit buffer declarations
// IMPORTANT: These define the range of channels (Bottom = 1 and Upper = 4 means usage of channel 1, 2, 3 and 4).
#define OSC_ADC_BOTTOM_ADC_CHANNEL_NUM 0
#define OSC_ADC_UPPER_ADC_CHANNEL_NUM 3

typedef enum osc_AD_init_type
{
    OSC_AD_INIT_USE_POOLING,
    OSC_AD_INIT_USE_INTERRUPT
} osc_AD_init_type_t;

extern char b_ADC_active_flag; // This flag is set while ADC is filling up the buffer

char c_current_ADC_channel; // Current ADC channel and buffer

// Global buffer declarations
extern struct buffer_t buff_0;
extern struct buffer_t buff_1;
extern struct buffer_t buff_2;
extern struct buffer_t buff_3;
//extern struct buffer_t buff_4;
//extern struct buffer_t buff_5;
//extern struct buffer_t buff_6;
//extern struct buffer_t buff_7;
extern struct buffer_t *buffer_pointers[8]; // Sem shrani pointerje na bufferje

void osc_IO_init();                              // Initialize IO ports. Modify this function if necessary.
void osc_ADC_init(osc_AD_init_type_t init_type); // Initialize AD converter, use OSC_AD_INIT_USE_POOLING, OSC_AD_INIT_USE_INTERRUPT as argument. Dont forget sei().
void osc_ADC_output_high();                      // Set output pin high.
void osc_ADC_output_low();                       // Set output pin low.
void osc_ADC_select_channel(uint8_t channel);    // Select ADC channel to read from.
uint8_t osc_ADC_Read_by_pooling();               // Return the value of the currently selected ADC channel using pooling. Momentarily disables ADC interrupts.
void osc_ADC_start_conversion();                 // Write to ADCSRA to start a conversion
void osc_ADC_increment_channel();                // Sets the next channel according to the buffer array and macro definitions. 


// SYSTEM LEVEL
#define OSC_CTRL_UPPER_TRESH_VOLTAGE 110         // Upper treshold for voltage control
#define OSC_CTRL_BOTTOM_TRESH_VOLTAGE 100        // Bottom treshold for voltage control

char prev_chan;                                 // Previous channel, for use in voltage controller function

void osc_voltage_controller(char val, char prev_chan_v);               // Compare the parameter to tresholds and set the output pin high/low accordingly.


// APPLICATION LEVEL
#include "lib/LCD_Ili9341.h"
#ifndef LCD_USE_PRINTF
#define LCD_USE_PRINTF
#endif

#define OSC_LCD_Y_OFFSET 20
#define OSC_LCD_X_OFFSET 40
#define OSC_LCD_Y_SCALE_FACTOR 0.75f

typedef enum osc_LCD_display_type
{
    OSC_LCD_USE_DOTS,
    OSC_LCD_USE_LINES
} osc_LCD_display_t;
const int osc_LCD_colors_array[8]; // Definicija arraya za hitro izbiranje barv pri printu krivulj na graf

void osc_LCD_init();                                                                         // Prepare the LCD to be used within the oscilloscope function
void osc_LCD_show_value_at_XY(int x, int y, int value);                                      // Print data to LCD by coordinates
void osc_LCD_clear();                                                                        // Clear all
void osc_LCD_draw_bg();                                                                      // Draw background screen
void osc_LCD_draw_legend();                                                                  // Draw legend screen
void osc_LCD_draw_dot_by_xy(int x, int y, int color);                                        // Draw dot using coordinates
void osc_LCD_draw_dot_by_val(char val, int x_offset, int color);                             // Draw dot using value and offset
void osc_LCD_draw_line_by_val(char val, int x_offset, int color);                            // Draw line using value and offset
void osc_LCD_display_vals(struct buffer_t *buff, osc_LCD_display_t display_type, int color_index); // Display buffer data on LCD. Use OSC_LCD_USE_DOTS or OSC_LCD_USE_LINES. Use 0 to 7 for color_index, preferably the index of the current channel.

#endif