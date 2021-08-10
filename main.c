/*
* Osciloskop
* Created: 8/1/2021 8:58:13 PM
* Author: Kristjan �oln
*
* Projekt Osciloskop je prvi del projekta Chopper driver - Mi�ko gonilnik za stepper motor.
* Cilj projekta Osciloskop je dovolj hitro branje �estih AD vhodov, da bo krmilnik
* lahko krmilil tok skozi navitja preko hitrega preklapljanja krmilnih tranzistorjev.
* Dodatna funkcionalnost tega projekta je prikaz napetosti v odvisnosti od �asa na grafu,
* torej osnovna funkcionalnost osciloskopa, ki se bo lahko izklju�ila zaradi �asovne 
* potratnosti pisanja na LCD.
*/

#include <avr/io.h>
#define OSC_DISPLAY_TO_LCD	// Define this to enable writing to LCD
#include "oscilloscope.h"


int main(void)
{
	// ---------------------------- Hardware level ---------------------------
	#ifndef LCD_USE_PRINTF
	#define LCD_USE_PRINTF
	#endif
	LCD_Init();
	osc_IO_init();
	osc_ADC_init(OSC_AD_INIT_USE_INTERRUPT);
	
	printf("Init done \n");
	sei();	
	
	// ---------------------------- System level ----------------------------

	// ---------------------------- Application level ----------------------------

	// main program
	while (1)
	{
		// ---------------------------- Hardware level ----------------------------
		
		// ---------------------------- System level ----------------------------

		// ---------------------------- Application level ----------------------------

	}
}


