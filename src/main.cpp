
#include "ch.h"
#include "hal.h"
#include "cmsis_os.h"

#include "ds18b20.h"
#include "OWI.h"

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
  
	OWI owi;
	owi.Init(WIRE1_DS1);
	OWI::OWI_device owi_devices[2];
	uint8_t num = 0;
	owi.SearchDevices(owi_devices, 2, &num);
	DS18B20_T DS1820_data;
	DS1820_data.skip_romid = 1;
	DS1820_init(&DS1820_data, &owi);
	auto yes = DS1820_is(&DS1820_data);

	
	
	auto id2 = osTimerCreate (osTimer(Timer1), osTimerPeriodic, nullptr);
	if (id2 != nullptr)  {
		// Periodic timer created
	}
	osTimerStart(id2, 1000);
	  
	while(1) {
		auto T = DS1820_exec(&DS1820_data);
		osDelay(1000);
//		sdWrite(&SD1, (uint8_t*)"Test\n\r", sizeof("Test\n\r") - 1);
	}
}