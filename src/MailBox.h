/*
 * MailBox.h
 *
 *  Created on: 20 марта 2016 г.
 *      Author: Bogdan
 */

#ifndef MAILBOX_H_
#define MAILBOX_H_

#include <array>

#include "cmsis_os.h"

template <typename T, int N>
class MailBox
{
	#define osMessageQDef_(name, queue_sz, type)                                 \
	const msg_t os_messageQ_buf_##name[queue_sz] = {0};                        \
	mailbox_t os_messageQ_obj_##name = {0};                                    \
	const osMessageQDef_t os_messageQ_def_##name = {                            \
		(queue_sz),                                                               \
		sizeof (type),                                                            \
		(mailbox_t*)&os_messageQ_obj_##name,                                          \
		(void *)&os_messageQ_buf_##name[0]                                        \
	}
private:
	osMessageQDef_(mail_box_ID, N, uint16_t);              // Define message queue
	osMessageQId  mail_box_ID;

	osMessageQDef_(free_mail_box_ID, N, uint16_t);              // Define message queue
	osMessageQId  free_mail_box_ID;

	std::array<T, N> buffers;

 public:
	MailBox(void) {
		mail_box_ID = osMessageCreate(osMessageQ(mail_box_ID), NULL);
	    if ( NULL == mail_box_ID )
	    {
	    	/// error
			while(1){};
	    }
	    free_mail_box_ID = osMessageCreate(osMessageQ(free_mail_box_ID), NULL);
	    if ( NULL == free_mail_box_ID )
	    {
	    	/// error
			while(1){};
	    }
		for (int i = 0; i < N; i++) {
			if (osMessagePut(free_mail_box_ID, i, osWaitForever) != osOK )
			{
				while(1){};	
			};
		}
	}

	void pop(T& item)
	{
		osEvent  peEvent_;
		peEvent_ = osMessageGet(mail_box_ID, osWaitForever);
//	    if( osEventMessage != peEvent_.status)
//	    {
	    	/// error
//			while(1){};
//	    }
		item = buffers.at(peEvent_.value.v);

		if (osMessagePut(free_mail_box_ID, peEvent_.value.v, osWaitForever) != osOK )
		{
			while(1){};	
		};
	}

	void push(T& item)
	{
		osEvent  peEvent_;
		peEvent_ = osMessageGet(free_mail_box_ID, osWaitForever);
	    if( osEventMessage != peEvent_.status)
	    {
	    	/// error
	    	while(1){};
	    }
		buffers.at(peEvent_.value.v) = item;
		if (osMessagePut(mail_box_ID, peEvent_.value.v, osWaitForever) != osOK )
		{
			while(1){};	
		};
	}

	void pushI(T& item)
	{
		osEvent  peEvent_;
		peEvent_ = osMessageGet(free_mail_box_ID, 0);
	    if( osEventMessage != peEvent_.status)
	    {
	    	/// error
	    	while(1){};
	    }
		buffers.at(peEvent_.value.v) = item;
		if (osMessagePut(mail_box_ID, peEvent_.value.v, 0) != osOK )
		{
			while(1){};	
		};
	}
};

#endif /* MAILBOX_H_ */
