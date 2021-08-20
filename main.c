/*
* Osciloskop
* Created: 8/1/2021 8:58:13 PM
* Author: Kristjan �oln
*
* Projekt Osciloskop je prvi del projekta Chopper driver - Misko gonilnik za stepper motor.
* Cilj projekta Osciloskop je dovolj hitro branje sestih AD vhodov, da bo krmilnik
* lahko krmilil tok skozi navitja preko hitrega preklapljanja krmilnih tranzistorjev.
* Dodatna funkcionalnost tega projekta je prikaz napetosti v odvisnosti od casa na grafu,
* torej osnovna funkcionalnost osciloskopa, ki se bo lahko izkljucila zaradi casovne
* potratnosti pisanja na LCD.
*/

#include <avr/io.h>
#include "lib/LED.h"
#include "lib/kbd.h"
#include "oscilloscope.h"

int main(void)
{
// var declarations
char b_ADC_active_flag = 0;

// inits
#ifndef LCD_USE_PRINTF
#define LCD_USE_PRINTF
#endif
	LCD_Init();

	LED_Init();
	LED_AllOff();

	KBD_Init();

	osc_IO_init();
	osc_ADC_init(OSC_AD_INIT_USE_INTERRUPT);
	osc_ADC_select_channel(2);	// X kanal joysticka
	sei();

	printf("Init done \n");

	// main program
	while (1)
	{
		// Čakaj na stisk tipke
		KBD_flush();
		while (KBD_GetReleasedKey() != BTN_OK)
			KBD_Read();

		LED_3Tgl();
		b_ADC_active_flag = 1;
		osc_ADC_start_conversion();
	}
}
