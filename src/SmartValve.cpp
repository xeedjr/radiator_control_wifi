/*
 * SmartValve.cpp
 *
 *  Created on: 30 ñ³÷. 2020 ð.
 *      Author: Bogdan
 */
#include <new>
#include "ch.h"
#include "hal.h"
#include "cmsis_os.h"
#include "SmartValve.h"

SmartValve::SmartValve() {
	l9110s.init(L9110S_IA, L9110S_IB);

	mail = osMailCreate(osMailQ(mail), NULL);      // create mail queue
		
	thread_id = osThreadCreate (osThread (SmartValve_thread), this);
	if (thread_id == NULL)  {
		// Periodic timer created
		terminate();
	}
		
	Timer_StopDriveId = osTimerCreate (osTimer(Timer_StopDrive), osTimerOnce, this);
	if (Timer_StopDriveId == NULL)  {
		// Periodic timer created
		terminate();
	}
}

SmartValve::~SmartValve() {
	// TODO Auto-generated destructor stub
}

void SmartValve::Timer_StopDrive_Callback(void) {
	Message *mptr;
	mptr = (Message*)osMailAlloc(mail, osWaitForever);       // Allocate memory
	if (mptr != NULL) {
		mptr->msgId = Message::kTimer;
		osMailPut(mail, mptr);                         // Send Mail
	}
}

void SmartValve::open (void) {
	Message *mptr;
	mptr = (Message*)osMailAlloc(mail, osWaitForever);       // Allocate memory
	if (mptr != NULL) {
		mptr->msgId = Message::kOpenCmd;
		osMailPut(mail, mptr);                         // Send Mail
	}
}
void SmartValve::close (void) {
	Message *mptr;
	mptr = (Message*)osMailAlloc(mail, osWaitForever);       // Allocate memory
	if (mptr != NULL) {
		mptr->msgId = Message::kCloseCmd;
		osMailPut(mail, mptr);                         // Send Mail
	}
}
 
void SmartValve::thread(void) {
	while(1) {
		osEvent ev;
		Message  *rptr;
		
		ev = osMailGet(mail, osWaitForever);        // wait for mail
		if (ev.status == osEventMail) {
			rptr = (Message*)ev.value.p;
			/// Use
			switch (rptr->msgId) {
			case Message::kTimer:
				switch (state) {
				case kOpening:
				case kOpeningPause: 
					switch (state) {
					case kOpening:
					{
						currentBalansms += openDuration;
						l9110s.set_direction(L9110S::kStop);
						if (currentBalansms > kFullMove) {
							/// completed
							state = kOpen;
						} else {
							// not yet
							state = kOpeningPause;
							osTimerStart(Timer_StopDriveId, openDuration*1000);
						};
						break;
					}					
					case kOpeningPause:
					{
						state = kOpening;
						l9110s.set_direction(L9110S_OPEN);
						osTimerStart(Timer_StopDriveId, openDuration*1000);
						break;
					}}
					break;
				case kClosing:
				case kClosingPause:
					switch (state) {
					case kClosing:
					{
						if ((currentBalansms + openDuration) >= openDuration)
							currentBalansms -= openDuration;
						else 
							currentBalansms = 0;
							
						l9110s.set_direction(L9110S::kStop);
						if (currentBalansms == 0) {
							/// completed
							state = kClose;
						} else {
							// not yet
							state = kClosingPause;
							osTimerStart(Timer_StopDriveId, openDuration*1000);
						};
						break;
					};
					case kClosingPause:
					{
						state = kClosing;
						l9110s.set_direction(L9110S_CLOSE);
						osTimerStart(Timer_StopDriveId, openDuration*1000);
						break;
					}};
					break;
				}
				break;
			case Message::kCloseCmd:
				if (state == kOpen || state == kUnknown) {
					// close
					state = kClosing;
					osTimerStart(Timer_StopDriveId, openDuration*1000);
					l9110s.set_direction(L9110S_CLOSE);
				}
				break;
			case Message::kOpenCmd:
				if (state == kClose || state == kUnknown) {
					/// fully closed can be opened
					state = kOpening;
					osTimerStart(Timer_StopDriveId, openDuration*1000);
					l9110s.set_direction(L9110S_OPEN);
				}
				break;
			}
			/// End Use
			osMailFree(mail, rptr);                    // free memory allocated for mail
		}
	}
}
