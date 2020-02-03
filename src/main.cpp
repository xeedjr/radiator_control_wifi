#include <new>
#include "ch.h"
#include "hal.h"
#include "cmsis_os.h"

#include "RF24.h"
#include "RF24HALChibios.h"
#include "BL.h"

extern const SerialConfig sd1_config;

uint8_t BL_placed[sizeof(BL)];
BL* bl = nullptr;

int main () {
	/*
   * System initializations.
   * - HAL initialization, this also initializes the configured device drivers
   *   and performs the board-specific initializations.
   * - Kernel initialization, the main() function becomes a thread and the
   *   RTOS is active.
   */
	halInit();
	osKernelInitialize();
	
	palClearPad(IOPORT2, PORTB_LED1);
	/*
	* Activates the serial driver 1 using the driver default configuration.
	*/
	sdStart(&SD1, &sd1_config);
  
	bl = new(BL_placed) BL;
  
	while(1) {
		osDelay(10000);
	}
}