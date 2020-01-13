#pragma once

#include "OWI.h"

// ds1820 scratchpad registers
#define DS1820_SP_TLSB  0
#define DS1820_SP_TMSB  1
#define DS1820_SP_HLIM  2
#define DS1820_SP_LLIM  3
#define DS1820_SP_CFG   4
#define DS1820_SP_RES0  5
#define DS1820_SP_RES1  6
#define DS1820_SP_RES2  7
#define DS1820_SP_CRC   8

// ds1820 rom registers
#define DS1820_ROM_DEVTYPE  0
#define DS1820_ROM_SERIAL1  1
#define DS1820_ROM_SERIAL2  2
#define DS1820_ROM_SERIAL3  3
#define DS1820_ROM_SERIAL4  4
#define DS1820_ROM_SERIAL5  5
#define DS1820_ROM_SERIAL6  6
#define DS1820_ROM_CRC      7

// ds1820 command set
#define DS1820_CMD_READROM           0x33
#define DS1820_CMD_MATCHROM          0x55
#define DS1820_CMD_SKIPROM           0xCC
#define DS1820_CMD_CONVERTTEMP       0x44
#define DS1820_CMD_WRITESCRATCHPAD   0x4E
#define DS1820_CMD_READSCRATCHPAD    0xBE
#define DS1820_CMD_COPYSCRATCHPAD    0x48

#define DS1820_1_PIN  3
#define DS1820_2_PIN  2
#define DS1820_3_PIN  1
#define DS1820_4_PIN  0

#define DS1820_1_NUM  0
#define DS1820_2_NUM  1
#define DS1820_3_NUM  2
#define DS1820_4_NUM  3

#define DS1820_MAX_NUM  4


#define DS1820_MAX_TRY_READ 10

typedef struct DS18B20_T {
      unsigned char id[8];    //!< The 64 bit identifier.
      unsigned char skip_romid;
      OWI* bus;
} DS18B20_T;

#ifdef __cplusplus
extern "C" {
#endif
	extern void DS1820_init(DS18B20_T *DS1820_data, OWI* bus);
	extern unsigned char ds1820_read_scratchpad (DS18B20_T *DS1820_data, unsigned char *data );  /// returns config bitfield;  // returns config bitfield
	unsigned char ds1820_read_rom (unsigned char pin, unsigned char *data );  // returns rom
	extern unsigned char DS1820_is(DS18B20_T *DS1820_data); //визначає чи підключений датчик
	extern unsigned char ds1820_start_conv(DS18B20_T *DS1820_data);
	extern float DS1820_exec(DS18B20_T *DS1820_data);
#ifdef __cplusplus
}
#endif

