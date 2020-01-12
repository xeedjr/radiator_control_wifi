
#include "ch.h"
#include "hal.h"
#include "cmsis_os.h"

extern const SerialConfig sd1_config;

void Timer1_Callback  (void const *arg) {
	palTogglePad(IOPORT2, PORTB_LED1);
};
osTimerDef (Timer1, Timer1_Callback);

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
  
	auto id2 = osTimerCreate (osTimer(Timer1), osTimerPeriodic, nullptr);
	if (id2 != nullptr)  {
		// Periodic timer created
	}
	osTimerStart(id2, 1000);
	  
	while(1) {
		osDelay(1000);
		sdWrite(&SD1, (uint8_t*)"Test\n\r", sizeof("Test\n\r") - 1);
	}
}