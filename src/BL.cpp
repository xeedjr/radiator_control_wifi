/*
 * BL.cpp
 *
 *  Created on: 30 ñ³÷. 2020 ð.
 *      Author: Bogdan
 */
#include <new>
#include "ch.h"
#include "hal.h"
#include "cmsis_os.h"
#include "BL.h"

BL::BL() {
	l9110s.init(L9110S_IA, L9110S_IB);
	owi.Init(WIRE1_DS1);
	OWI::device owi_devices[2] = {0};
	uint8_t num = 0;
	owi.SearchDevices(owi_devices, 2, &num);
	sensor.skip_romid = 1;
	sensor.init(&owi);
	auto yes = sensor.is();
	float T = sensor.exec();

	radio_hal = new(RF24HAL_Chibios_place) RF24HAL_Chibios();
	radio = new(RF24_place) RF24(radio_hal);
	
	radio->begin();                           // Setup and configure rf radio
	radio->setChannel(2);
	radio->setPALevel(RF24_PA_MAX);
	radio->setDataRate(RF24_1MBPS);
	radio->setAutoAck(1);                     // Ensure autoACK is enabled
	radio->setRetries(2,15);                  // Optionally, increase the delay between retries & # of retries
	radio->setCRCLength(RF24_CRC_8);          // Use 8-bit CRC for performance
	radio->enableDynamicPayloads();
//	auto rate = radio->getCRCLength();
	// Write on our talking pipe
	radio->openWritingPipe(pipe);
	// Listen on our listening pipe
	radio->openReadingPipe(1,pipe);
	radio->startListening();
	
	auto id2 = osTimerCreate (osTimer(Timer1), osTimerPeriodic, this);
	if (id2 == NULL)  {
		// Periodic timer created
		return;
	}
	osTimerStart(id2, 1000);
	
	Timer_RF24RecvId = osTimerCreate (osTimer(Timer_RF24Recv), osTimerPeriodic, this);
	if (Timer_RF24RecvId == NULL)  {
		// Periodic timer created
		return;
	}
	osTimerStart(Timer_RF24RecvId, 100);
		
	Timer_StopDriveId = osTimerCreate (osTimer(Timer_StopDrive), osTimerOnce, this);
	if (Timer_StopDriveId == NULL)  {
		// Periodic timer created
		return;
	}
		
	thread_id = osThreadCreate (osThread (BL_thread), this);
	if (thread_id == NULL)  {
		// Periodic timer created
		return;
	}
	
}

BL::~BL() {
	// TODO Auto-generated destructor stub
}

void BL::Timer1_Callback(void) {
	osSignalSet(thread_id, Message::kTimer1);
}

void BL::Timer_StopDrive_Callback(void) {
	osSignalSet(thread_id, Message::kTimerStopDrive);
}

void BL::Timer_RF24Recv_Callback(void) {
	osSignalSet(thread_id, Message::kTimerRF24Recv);
}

void BL::radio_send(float T, float setT) {
	uint8_t data [25] = {0};
	
	data[0] = 1;
	data[1] = (uint8_t) T;	
	data[2] = (uint8_t) setT;	
	
	// First, stop listening so we can talk
	radio->stopListening();

	// Send the final one back.
	bool res = radio->write( data, 3 );

	// Now, resume listening so we catch the next packets.
	radio->startListening();
}

void BL::radio_check(void) {
	uint8_t data [25] = {0};
	if ( radio->available() )
	{
		// Dump the payloads until we've gotten everything
		bool done = false;
		while (!done)
		{
			// Fetch the payload, and see if this was the last one.
			uint8_t len = radio->getDynamicPayloadSize();
			done = radio->read( data, len );
							
			switch (data[0]) {
				case 1 : // set temperatue
				{
					float Temperature = data[1];
					setTemperature(Temperature);
				}
				break;
			}
		}
	}
}

void BL::thread(void) {
	while(1) {
		osEvent ev;
	
		ev = osSignalWait(0, osWaitForever);
	
		if ((ev.value.signals & Message::kSetT) > 0) {
			temperature = msg.data.setT.t;
		}
		if ((ev.value.signals & Message::kTimer1) > 0) {
			T = sensor.exec();
			if (T > 5.0) {
				if (T > (temperature+1.0)) {
					/// close
					if (is_open) {
						l9110s.set_direction(L9110S::kB);
						osTimerStart(Timer_StopDriveId, 60000);
						is_open = false;
					}
				}
				if (T < (temperature-1.0)) {
					// open
					if (!is_open) {
						l9110s.set_direction(L9110S::kA);
						osTimerStart(Timer_StopDriveId, 60000);
						is_open = true;
					}
				}
			};
		}
		if ((ev.value.signals & Message::kTimerStopDrive) > 0) {
			l9110s.set_direction(L9110S::kStop);
		}
		if ((ev.value.signals & Message::kTimerRF24Recv) > 0) {
			radio_check();
			radio_send(T, temperature);
		}
	}
}

