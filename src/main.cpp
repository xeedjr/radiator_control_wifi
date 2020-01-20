
#include "ch.h"
#include "hal.h"
#include "cmsis_os.h"

#include "ds18b20.h"
#include "OWI.h"
#include "L9110S.h"
#include "RF24.h"
#include "RF24HALChibios.h"

extern const SerialConfig sd1_config;

L9110S l9110s;

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
  
	l9110s.init(L9110S_IA, L9110S_IB);
	//l9110s.set_direction(L9110S::kB);
  
	OWI owi;
	owi.Init(WIRE1_DS1);
	OWI::device owi_devices[2];
	uint8_t num = 0;
	owi.SearchDevices(owi_devices, 2, &num);
	DS18B20 sensor;
	sensor.skip_romid = 1;
	sensor.init(&owi);
	auto yes = sensor.is();

	
	
	auto id2 = osTimerCreate (osTimer(Timer1), osTimerPeriodic, nullptr);
	if (id2 != nullptr)  {
		// Periodic timer created
	}
	osTimerStart(id2, 1000);

	auto radio_hal = new RF24HAL_Chibios();
	auto radio = new RF24(radio_hal);
	
	radio->begin();                           // Setup and configure rf radio
	radio->setChannel(2);
	radio->setPALevel(RF24_PA_MAX);
	radio->setDataRate(RF24_1MBPS);
	radio->setAutoAck(1);                     // Ensure autoACK is enabled
	radio->setRetries(2,15);                  // Optionally, increase the delay between retries & # of retries
	radio->setCRCLength(RF24_CRC_8);          // Use 8-bit CRC for performance
	radio->setPayloadSize(1);
	auto rate = radio->getCRCLength();
	  
	while(1) {
		auto T = sensor.exec();
		osDelay(1000);
//		sdWrite(&SD1, (uint8_t*)"Test\n\r", sizeof("Test\n\r") - 1);
	}
}