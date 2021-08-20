/*
 * MLLED.h
 *
 * Created: 24-Feb-16 12:29:51
 *  Author: maticpi
 */ 

#ifndef LED_H_
#define LED_H_

#include <avr/io.h>

void LED_Init();
void LED_AllOff();
void LED_AllOn();
void LED_0On();
void LED_0Off();
void LED_0Tgl();
void LED_1On();
void LED_1Off();
void LED_1Tgl();
void LED_2On();
void LED_2Off();
void LED_2Tgl();
void LED_3On();
void LED_3Off();
void LED_3Tgl();

void LED_Set(uint8_t leds);
void LED_Clr(uint8_t leds);
uint8_t LED_Get();




#endif /* LED_H_ */