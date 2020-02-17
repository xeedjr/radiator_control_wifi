/*
 * RFSmartValve.h
 *
 *  Created on: 30 ñ³÷. 2020 ð.
 *      Author: Bogdan
 */
#pragma once

#include <functional>
#include "cmsis_os.h"
#include "RF24.h"
#include "RF24HALChibios.h"
#include "MailBox.h"
#include "ds18b20.h"
#include "OWI.h"
#include "L9110S.h"

class SmartValve {
	void thread(void);
	static void SmartValve_thread(const void* thisp) {
		SmartValve* p = (SmartValve*)thisp;
		p->thread();
	};
	osThreadDef (SmartValve_thread, osPriorityNormal, 416, 0);              // define Thread and specify to allow three instances
	osThreadId thread_id;
		
	void Timer_StopDrive_Callback(void);
	void static SmartValve_Timer_StopDrive_Callback  (void const *arg) {
		SmartValve* p = (SmartValve*)arg;
		p->Timer_StopDrive_Callback();
	};
	osTimerDef (Timer_StopDrive, SmartValve_Timer_StopDrive_Callback);
	osTimerId Timer_StopDriveId = {0};
	
	enum {
		kUnknown,
		kClosing,
		kClosingPause,
		kClose,
		kOpening,
		kOpeningPause,
		kOpen,
	} state = kUnknown;

	L9110S l9110s;
	#define L9110S_CLOSE L9110S::kB
	#define L9110S_OPEN L9110S::kA
	
	const uint32_t kFullMove = 70; // 70 sec for full move
	uint32_t currentBalansms = 0;
	uint16_t openDuration = 5;

	typedef struct {
		enum {
			kTimer,
			kCloseCmd,
			kOpenCmd,
		} msgId;
		union {
		} data;
	} Message;
	osMailQDef(mail, 2, Message);                    // Define mail queue
	osMailQId  mail = NULL;
	

public:
	SmartValve();
	virtual ~SmartValve();
	
	void open (void);
	void close (void);
};

