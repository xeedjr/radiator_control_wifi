#include "ds18b20.h"

void DS1820_init(DS18B20_T *DS1820_data, OWI* bus)
{
    DS1820_data->bus = bus;
}

#define CRC8INIT	0x00
#define CRC8POLY	0x18              //0X18 = X^8+X^5+X^4+X^0

unsigned char crc8 ( unsigned char *data_in, unsigned int number_of_bytes_to_read )
{
	unsigned char	crc;
	unsigned int    loop_count;
	unsigned char   bit_counter;
	unsigned char   data;
	unsigned char   feedback_bit;
	
	crc = CRC8INIT;

	for (loop_count = 0; loop_count != number_of_bytes_to_read; loop_count++)
	{
		data = data_in[loop_count];
		
		bit_counter = 8;
		do {
			feedback_bit = (crc ^ data) & 0x01;
	
			if ( feedback_bit == 0x01 ) {
				crc = crc ^ CRC8POLY;
			}
			crc = (crc >> 1) & 0x7F;
			if ( feedback_bit == 0x01 ) {
				crc = crc | 0x80;
			}
		
			data = data >> 1;
			bit_counter--;
		
		} while (bit_counter > 0);
	}
	
	return crc;
}

unsigned char ds1820_read_scratchpad(DS18B20_T *DS1820_data, unsigned char *data )  /// returns config bitfield
{
    if (!DS1820_data->bus->DetectPresence()){
      return 0;
    };

    if (DS1820_data->skip_romid == 1)
    {
    	// skip rom
    	DS1820_data->bus->SendByte(DS1820_CMD_SKIPROM);
    }
    else
    {
    	//rom
		DS1820_data->bus->SendByte(DS1820_CMD_MATCHROM);
		DS1820_data->bus->SendByte(DS1820_data->id[0]);
		DS1820_data->bus->SendByte(DS1820_data->id[1]);
		DS1820_data->bus->SendByte(DS1820_data->id[2]);
		DS1820_data->bus->SendByte(DS1820_data->id[3]);
		DS1820_data->bus->SendByte(DS1820_data->id[4]);
		DS1820_data->bus->SendByte(DS1820_data->id[5]);
		DS1820_data->bus->SendByte(DS1820_data->id[6]);
		DS1820_data->bus->SendByte(DS1820_data->id[7]);
    };

	DS1820_data->bus->SendByte(DS1820_CMD_READSCRATCHPAD);

    data[0] = DS1820_data->bus->ReceiveByte();    /// 0 Tlsb
    data[1] = DS1820_data->bus->ReceiveByte();    /// 1 Tmsb
    data[2] = DS1820_data->bus->ReceiveByte();    /// 2 Thlim
    data[3] = DS1820_data->bus->ReceiveByte();    /// 3 Tllim
    data[4] = DS1820_data->bus->ReceiveByte();    /// 4 Config
    data[5] = DS1820_data->bus->ReceiveByte();    /// 5 RES0
    data[6] = DS1820_data->bus->ReceiveByte();    /// 6 RES1
    data[7] = DS1820_data->bus->ReceiveByte();    /// 7 RES2
    data[8] = DS1820_data->bus->ReceiveByte();    /// 8 CRC

    if (data[8] != crc8(data, 8)) {
        return 2;	/// if CRC is bad
    };

    return 1;
}

unsigned char ds1820_start_conv(DS18B20_T *DS1820_data)
{
    if (!DS1820_data->bus->DetectPresence()){
      return 0;
    };

    if (DS1820_data->skip_romid == 1)
    {
    	// skip rom
    	DS1820_data->bus->SendByte(DS1820_CMD_SKIPROM);
    }
    else
    {
    	//rom
		DS1820_data->bus->SendByte(DS1820_CMD_MATCHROM);
		DS1820_data->bus->SendByte(DS1820_data->id[0]);
		DS1820_data->bus->SendByte(DS1820_data->id[1] );
		DS1820_data->bus->SendByte(DS1820_data->id[2]);
		DS1820_data->bus->SendByte(DS1820_data->id[3]);
		DS1820_data->bus->SendByte(DS1820_data->id[4]);
		DS1820_data->bus->SendByte(DS1820_data->id[5]);
		DS1820_data->bus->SendByte(DS1820_data->id[6]);
		DS1820_data->bus->SendByte(DS1820_data->id[7]);
    };

    DS1820_data->bus->SendByte(DS1820_CMD_CONVERTTEMP);

	return 1;
}

//ïåðåâ³ðÿº íàÿâí³ñòü äàò÷èêà íà ë³í³¿ ³ ïåðåâ³ðÿº ÷è äàò÷èê ÄÑ18S20 ³íàêøå ÅÐÐÎÎÎÎÎÎÐ
/***********************************************************************************
    Return :
        0 - if termal sensor is off
        1 - if termal sensor is on
        2 - if termal sensor not on not off
************************************************************************************/
unsigned char DS1820_is(DS18B20_T *DS1820_data)
{
    if (!DS1820_data->bus->DetectPresence()){
      return 0;
    };

    return 1;
}

#include "cmsis_os.h"
/// read temperature wit delay
float DS1820_exec(DS18B20_T *DS1820_data)
{
    unsigned char scratchpad[9];
    signed int T = 0;
    float Tem = 1.0;
    unsigned char high = 0;
    unsigned char low = 0;

    if (ds1820_start_conv( DS1820_data ) == 0)
    {
        // error cant start conversion
		return -999;
    };
	
	osDelay(1000);
	
	///read temperature register
	if (ds1820_read_scratchpad (DS1820_data, scratchpad) == 1)	{
		// read good
		T = scratchpad[1];
		T <<= 8;
		T |= scratchpad[0];

		Tem = 1.0;

		if (T < 0)
		{
			Tem = -1.0;
			T = T * -1;
		};

		high = T >> 4;
		low = T & 0x0F;

		Tem = Tem * high;
		Tem = Tem + (low * 0.0625);
	} else {
		return -999;
	}
	
	return Tem;
}

