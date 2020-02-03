/*
 * RFBL.h
 *
 *  Created on: 30 ñ³÷. 2020 ð.
 *      Author: Bogdan
 */
#pragma once

#include "cmsis_os.h"
#include "RF24.h"
#include "RF24HALChibios.h"
#include "MailBox.h"
#include "ds18b20.h"
#include "OWI.h"
#include "L9110S.h"

class BL {
	void thread(void);
	static void BL_thread(const void* thisp) {
		BL* p = (BL*)thisp;
		p->thread();
	};
	osThreadDef (BL_thread, osPriorityNormal, 512, 0);              // define Thread and specify to allow three instances

	void Timer1_Callback(void);
	void static BL_Timer1_Callback  (void const *arg) {
		BL* p = (BL*)arg;
		p->Timer1_Callback();
	};
	osTimerDef (Timer1, BL_Timer1_Callback);
	
	void Timer_StopDrive_Callback(void);
	void static BL_Timer_StopDrive_Callback  (void const *arg) {
		BL* p = (BL*)arg;
		p->Timer_StopDrive_Callback();
	};
	osTimerDef (Timer_StopDrive, BL_Timer_StopDrive_Callback);
	osTimerId Timer_StopDriveId = {0};
	bool is_open = false;

	void Timer_RF24Recv_Callback(void);
	void static BL_Timer_RF24Recv_Callback  (void const *arg) {
		BL* p = (BL*)arg;
		p->Timer_RF24Recv_Callback();
	};
	osTimerDef (Timer_RF24Recv, BL_Timer_RF24Recv_Callback);
	osTimerId Timer_RF24RecvId = {0};
	const uint64_t pipe = { 0xF0F0F0F0E1LL };
	uint8_t RF24HAL_Chibios_place[sizeof(RF24HAL_Chibios)];
	uint8_t RF24_place[sizeof(RF24)];
	RF24HAL_Chibios* radio_hal;
	RF24* radio;

	typedef struct {
		enum {
			kSetT = 1 << 0,
			kTimer1 = 1 << 1,
			kTimerStopDrive = 1 << 2,
			kTimerRF24Recv = 1 << 3,
		} msgid;
		union {
			struct {
				float t;
			} setT;
			struct {
			} timer1;
		} data;
	} Message;
	Message msg;
	//MailBox<Message, 10> mb;
	osThreadId thread_id;

	float temperature = 18.0;
	L9110S l9110s;
	OWI owi;
	DS18B20 sensor;
	float T;

	void radio_send(float T, float setT);
	void radio_check(void);

public:
	BL();
	virtual ~BL();

	void setTemperature(float t) {
		msg.data.setT.t = t;
		osSignalSet(thread_id, Message::kSetT);
	}
};

