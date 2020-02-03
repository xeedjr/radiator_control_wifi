/*
 * power_manegement.c
 *
 * Created: 02.01.2018 21:07:55
 *  Author: Bogdan
 */ 

#include <avr/io.h>
#include <ch.h>

static bool en_deep = false;

void enter_sleep() {
#ifdef PORT_AVR_WFI_SLEEP_IDLE
	
	if (en_deep) {
		SMCR = SLEEP_MODE_PWR_DOWN | (1 << SE);
		MCUCR |= (1 << BODS) | (1 << BODSE);
		MCUCR |= (1 << BODS);
		MCUCR &= ~(1 << BODSE);
	} else {
		SMCR = SLEEP_MODE_IDLE | (1 << SE);
	};

#endif
}

void enter_sleep_set(bool en_deep_) {
	en_deep = en_deep_;
}