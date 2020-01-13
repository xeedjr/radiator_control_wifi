
#pragma once

#ifndef WIN32
	#include <util/delay.h>
	#define delay_us(t) _delay_us(t)
	#define delay_loop2(C) _delay_loop_2(C)
#else
	/// TODO
	#define delay_us(t)
#endif

#define WIRE1_PORT_DELAY_US(US_TIME) delay_us(US_TIME)