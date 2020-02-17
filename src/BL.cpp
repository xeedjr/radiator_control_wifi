/*
 * BL.cpp
 *
 *  Created on: 30 ñ³÷. 2020 ð.
 *      Author: Bogdan
 */
#include <new>
#include <avr/eeprom.h>

#include "ch.h"
#include "hal.h"
#include "cmsis_os.h"
#include "BL.h"

EEMEM BL::PersistantStor eem;


BL::BL() {
	owi.Init(WIRE1_DS1);
//	OWI::device owi_devices[2] = {0};
//	uint8_t num = 0;
//	owi.SearchDevices(owi_devices, 2, &num);
	sensor.skip_romid = 1;
	sensor.init(&owi);
	sensor.setResolution(DS18B20::k9bit);
//	auto yes = sensor.is();
//	float T = sensor.exec();

	radio_hal = new(RF24HAL_Chibios_place) RF24HAL_Chibios();
	radio = new(RF24_place) RF24(radio_hal);

	eeprom_busy_wait();
	uint32_t ST = eeprom_read_dword((uint32_t*)&eem.setT);
	eeprom_busy_wait();
	uint32_t ST_inv = eeprom_read_dword((uint32_t*)&eem.setT_inv);	
	if (ST == ~(ST_inv)) {
		/// good
		SetedT = eeprom_read_float(&eem.setT);
	}

	radio->begin();                           // Setup and configure rf radio
	radio->setChannel(2);
	radio->setPALevel(RF24_PA_MAX);
	radio->setDataRate(RF24_1MBPS);
	radio->setAutoAck(1);                     // Ensure autoACK is enabled
	radio->setRetries(2,15);                  // Optionally, increase the delay between retries & # of retries
	radio->setCRCLength(RF24_CRC_8);          // Use 8-bit CRC for performance
	radio->enableDynamicPayloads();
	auto rate = radio->getCRCLength();
	if (rate != RF24_CRC_8) {
		terminate();
	}
	// Write on our talking pipe
	radio->openWritingPipe(pipe);
	// Listen on our listening pipe
	radio->openReadingPipe(1,pipe);
	radio->startListening();
	 
	mail = osMailCreate(osMailQ(mail), NULL);      // create mail queue
	
	auto id2 = osTimerCreate (osTimer(Timer1), osTimerPeriodic, this);
	if (id2 == NULL)  {
		// Periodic timer created
		terminate();
	}
	osTimerStart(id2, 1000);
	
	Timer_RF24RecvId = osTimerCreate (osTimer(Timer_RF24Recv), osTimerPeriodic, this);
	if (Timer_RF24RecvId == NULL)  {
		// Periodic timer created
		terminate();
	}
	osTimerStart(Timer_RF24RecvId, 10);
		
	Timer_RF24SendId = osTimerCreate (osTimer(Timer_RF24Send), osTimerPeriodic, this);
	if (Timer_RF24SendId == NULL)  {
		// Periodic timer created
		terminate();
	}
	osTimerStart(Timer_RF24SendId, 500);

	thread_id = osThreadCreate (osThread (BL_thread), this);
	if (thread_id == NULL)  {
		// Periodic timer created
		terminate();
	}
}

BL::~BL() {
	// TODO Auto-generated destructor stub
}

void BL::Timer1_Callback(void) {
	Message *mptr;
	mptr = (Message*)osMailAlloc(mail, osWaitForever);       // Allocate memory
	if (mptr != NULL) {
		mptr->msgid = Message::kTimer1;
		osMailPut(mail, mptr);                         // Send Mail
	}
}

void BL::Timer_RF24Recv_Callback(void) {
	Message *mptr;
	mptr = (Message*)osMailAlloc(mail, osWaitForever);       // Allocate memory
	if (mptr != NULL) {
		mptr->msgid = Message::kTimerRF24Recv;
		osMailPut(mail, mptr);                         // Send Mail
	}
}

void BL::Timer_RF24Send_Callback(void) {
	Message *mptr;
	mptr = (Message*)osMailAlloc(mail, osWaitForever);       // Allocate memory
	if (mptr != NULL) {
		mptr->msgid = Message::kTimerRF24Send;
		osMailPut(mail, mptr);                         // Send Mail
	}
}

void BL::setTemperature(float t) {
	Message *mptr;
	mptr = (Message*)osMailAlloc(mail, osWaitForever);       // Allocate memory
	if (mptr != NULL) {
		mptr->msgid = Message::kSetT;
		mptr->data.setT.t = t;
		osMailPut(mail, mptr);                         // Send Mail
	}
}
	
void BL::radio_send(RadiatorMsg msg) {
	// First, stop listening so we can talk
	radio->stopListening();

	// Send the final one back.
	bool res = radio->write( (void*)&msg.ans, sizeof(msg.ans) );

	// Now, resume listening so we catch the next packets.
	radio->startListening();
}

void BL::radio_check(void) {
	RadiatorMsg msg;
	
	if ( radio->available() )
	{
		// Dump the payloads until we've gotten everything
		bool done = false;
		while (!done)
		{
			// Fetch the payload, and see if this was the last one.
			uint8_t len = radio->getDynamicPayloadSize();
			if (len == sizeof(msg.cmd))
			{
				done = radio->read( (void*)&msg.cmd, len );
							
				switch (msg.cmd.cmdId) {
					case 1 : // set temperatue
					{
						float Temperature = msg.cmd.setT;
						setTemperature(Temperature);
					}
					break;
				}
			} else {
				done = true;
			}
		}
	}
}

void BL::thread(void) {
	while(1) {
		osEvent ev;
		Message  *rptr;
	
		ev = osMailGet(mail, osWaitForever);        // wait for mail
		//ev = osSignalWait(0, osWaitForever);
	    if (ev.status == osEventMail) {
		    rptr = (Message*)ev.value.p;
			/// Use
			switch (rptr->msgid) {
				case Message::kSetT:
				{
					SetedT = rptr->data.setT.t;
					uint32_t ST_inv = ~(*(uint32_t*)((void*)&SetedT));
					eeprom_write_float(&eem.setT, SetedT);
					eeprom_write_dword((uint32_t*)&eem.setT_inv, ST_inv);
				}
				break;
				case Message::kTimer1:
				{
					T = sensor.exec();
					if (T > 5.0) {
						if (T > (SetedT+1.0)) {
							/// close
							smart_valve.close();
						}
						if (T < (SetedT-1.0)) {
							// open
							smart_valve.open();
						}
					};
				}
				break;
				case Message::kTimerRF24Recv:
				{
					radio_check();
				}
				break;
				case Message::kTimerRF24Send:
					RadiatorMsg msg;
					msg.ans.currentT = T;
					msg.ans.setedT = SetedT;
					radio_send(msg);
				break;
			}
			/// End Use
		    osMailFree(mail, rptr);                    // free memory allocated for mail
	    }
	}
}

