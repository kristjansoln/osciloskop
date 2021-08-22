/*
* Osciloskop
* Created: 8/1/2021 8:58:13 PM
* Author: Kristjan Soln
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

// inits
#ifndef LCD_USE_PRINTF
#define LCD_USE_PRINTF
#endif

	LED_Init();
	LED_AllOff();

	KBD_Init();

	osc_IO_init();
	osc_ADC_init(OSC_AD_INIT_USE_INTERRUPT);
	osc_LCD_init();
	osc_ADC_select_channel(2); // X kanal joysticka

	sei();

	// main program
	while (1)
	{
		// Čakaj na stisk tipke
		KBD_flush();
		char x;
		while (1)
		{
			KBD_Read();
			x = KBD_GetReleasedKey();
			if (x == BTN_OK)
				break;
			else if (x == BTN_ESC)
				osc_LCD_clear();
		}
		// Začetek branja
		LED_3On();
		b_ADC_active_flag = 1;
		osc_ADC_Read_by_pooling(); // Prebere eno vrednost, da se izogne prvi (nepravilni) meritvi. Vmes disabla interrupte.
		osc_ADC_start_conversion();

		// Čaka da je buffer poln
		while (b_ADC_active_flag == 1)
		{
		}

		// Izpis na ekran
		LED_AllOff();
		osc_LCD_display_vals(&buff, OSC_LCD_USE_DOTS);
	}
}
