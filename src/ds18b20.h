#pragma once

#include "OWI.h"

class DS18B20 {
public:
	// ds1820 scratchpad registers
	#define SP_TLSB  0
	#define SP_TMSB  1
	#define SP_HLIM  2
	#define SP_LLIM  3
	#define SP_CFG   4
	#define SP_RES0  5
	#define SP_RES1  6
	#define SP_RES2  7
	#define SP_CRC   8

	// ds1820 rom registers
	#define ROM_DEVTYPE  0
	#define ROM_SERIAL1  1
	#define ROM_SERIAL2  2
	#define ROM_SERIAL3  3
	#define ROM_SERIAL4  4
	#define ROM_SERIAL5  5
	#define ROM_SERIAL6  6
	#define ROM_CRC      7

	// ds1820 command set
	#define CMD_READROM           0x33
	#define CMD_MATCHROM          0x55
	#define CMD_SKIPROM           0xCC
	#define CMD_CONVERTTEMP       0x44
	#define CMD_WRITESCRATCHPAD   0x4E
	#define CMD_READSCRATCHPAD    0xBE
	#define CMD_COPYSCRATCHPAD    0x48

	unsigned char id[8];    //!< The 64 bit identifier.
	unsigned char skip_romid;
	OWI* bus;

	void init(OWI* bus);
	unsigned char crc8 (unsigned char *data_in, unsigned int number_of_bytes_to_read );
	unsigned char read_scratchpad (unsigned char *data );  /// returns config bitfield;  // returns config bitfield
	unsigned char read_rom (unsigned char pin, unsigned char *data );  // returns rom
	unsigned char is(); //визначає чи підключений датчик
	unsigned char start_conv();
	float exec();
};
