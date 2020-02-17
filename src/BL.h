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
#include "SmartValve.h"
#include <functional>

class BL {
	void thread(void);
	static void BL_thread(const void* thisp) {
		BL* p = (BL*)thisp;
		p->thread();
	};
	osThreadDef (BL_thread, osPriorityNormal, 256, 0);              // define Thread and specify to allow three instances

	void Timer1_Callback(void);
	void static BL_Timer1_Callback  (void const *arg) {
		BL* p = (BL*)arg;
		p->Timer1_Callback();
	};
	osTimerDef (Timer1, BL_Timer1_Callback);

	void Timer_RF24Send_Callback(void);
	void static BL_Timer_RF24Send_Callback  (void const *arg) {
		BL* p = (BL*)arg;
		p->Timer_RF24Send_Callback();
	};
	osTimerDef (Timer_RF24Send, BL_Timer_RF24Send_Callback);
	osTimerId Timer_RF24SendId = {0};

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
	typedef union {
		struct {
			uint8_t cmdId;
			float setT;
		} cmd;
		struct {
			uint8_t cmdId;
			float setedT;
			float currentT;
		} ans;
	} RadiatorMsg;
	
	typedef struct {
		enum {
			kSetT = 1,
			kTimer1,
			kTimerDrive,
			kTimerRF24Recv,
			kTimerRF24Send,
		} msgid;
		union {
			struct {
				float t;
			} setT;
			struct {
			} timer1;
		} data;
	} Message;
	
	osThreadId thread_id;

	float SetedT = 18.0;
	OWI owi;
	DS18B20 sensor;
	float T;

	SmartValve smart_valve;

	void radio_send(RadiatorMsg msg);
	void radio_check(void);
	
	osMailQDef(mail, 2, Message);                    // Define mail queue
	osMailQId  mail;
	
public:
	BL();
	virtual ~BL();

	void setTemperature(float t);

	struct PersistantStor {
		float setT;
		float setT_inv;
	} ;
};

