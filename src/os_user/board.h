/*
    ChibiOS - Copyright (C) 2006..2018 Giovanni Di Sirio

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#ifndef _BOARD_H_
#define _BOARD_H_

#include "hal.h"

/*
 * Setup for the Arduino Mega board.
 */

/*
 * Board identifier.
 */
#define BOARD_ARDUINO_MEGA
#define BOARD_NAME "Arduino Mega"

/*
 * IO pins assignments.
 */
#define PORTB_LED1  5


/*
 * IO lines assignments.
 */
#define LINE_LED1   PAL_LINE(IOPORT2, PORTB_LED1)
#define WIRE1_DS1	PAL_LINE(IOPORT4, 2U)
#define L9110S_IA	PAL_LINE(IOPORT3, 0U)
#define L9110S_IB	PAL_LINE(IOPORT3, 1U)

/*
 * Port A setup.
 * All inputs with pull-ups.
 */
#define VAL_DDRA    0x00
#define VAL_PORTA   0xFF

/*
 * Port B setup.
 * All inputs except PB5 which has a LED connected.
 */
#define VAL_DDRB    0b00101110
#define VAL_PORTB   0b11010001

/*
 * Port C setup.
 * C0 C1 - output
 */
#define VAL_DDRC    0b00000011
#define VAL_PORTC   0b11111100

/*
 * Port D setup.
 * All inputs with pull-ups.
 */
#define VAL_DDRD    0x00
#define VAL_PORTD   0xFF

/*
 * Port E setup.
 * All inputs except PE1 (Serial TX0).
 */
#define VAL_DDRE    0x02
#define VAL_PORTE   0xFF

/*
 * Port F setup.
 * All inputs with pull-ups.
 */
#define VAL_DDRF    0x00
#define VAL_PORTF   0xFF

/*
 * Port G setup.
 * All inputs with pull-ups.
 */
#define VAL_DDRG    0x00
#define VAL_PORTG   0xFF

/*
 * Port H setup.
 * All inputs with pull-ups.
 */
#define VAL_DDRH    0x00
#define VAL_PORTH   0xFF

/*
 * Port J setup.
 * All inputs with pull-ups.
 */
#define VAL_DDRJ    0x00
#define VAL_PORTJ   0xFF

/*
 * Port K setup.
 * All inputs with pull-ups.
 */
#define VAL_DDRK    0x00
#define VAL_PORTK   0xFF

/*
 * Port L setup.
 * All inputs with pull-ups.
 */
#define VAL_DDRL    0x00
#define VAL_PORTL   0xFF


#if !defined(_FROM_ASM_)
#ifdef __cplusplus
extern "C" {
#endif
  void boardInit(void);
#ifdef __cplusplus
}
#endif
#endif /* _FROM_ASM_ */

#endif /* _BOARD_H_ */
