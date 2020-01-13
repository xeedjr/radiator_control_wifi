#include "ds18b20.h"

void DS18B20::init(OWI* bus)
{
    this->bus = bus;
}

#define CRC8INIT	0x00
#define CRC8POLY	0x18              //0X18 = X^8+X^5+X^4+X^0

unsigned char DS18B20::crc8 (unsigned char *data_in, unsigned int number_of_bytes_to_read )
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

unsigned char DS18B20::read_scratchpad(unsigned char *data )  /// returns config bitfield
{
    if (!bus->DetectPresence()){
      return 0;
    };

    if (skip_romid == 1)
    {
    	// skip rom
    	bus->SendByte(CMD_SKIPROM);
    }
    else
    {
    	//rom
		bus->SendByte(CMD_MATCHROM);
		bus->SendByte(id[0]);
		bus->SendByte(id[1]);
		bus->SendByte(id[2]);
		bus->SendByte(id[3]);
		bus->SendByte(id[4]);
		bus->SendByte(id[5]);
		bus->SendByte(id[6]);
		bus->SendByte(id[7]);
    };

	bus->SendByte(CMD_READSCRATCHPAD);

    data[0] = bus->ReceiveByte();    /// 0 Tlsb
    data[1] = bus->ReceiveByte();    /// 1 Tmsb
    data[2] = bus->ReceiveByte();    /// 2 Thlim
    data[3] = bus->ReceiveByte();    /// 3 Tllim
    data[4] = bus->ReceiveByte();    /// 4 Config
    data[5] = bus->ReceiveByte();    /// 5 RES0
    data[6] = bus->ReceiveByte();    /// 6 RES1
    data[7] = bus->ReceiveByte();    /// 7 RES2
    data[8] = bus->ReceiveByte();    /// 8 CRC

    if (data[8] != crc8(data, 8)) {
        return 2;	/// if CRC is bad
    };

    return 1;
}

unsigned char DS18B20::start_conv()
{
    if (!bus->DetectPresence()){
      return 0;
    };

    if (skip_romid == 1)
    {
    	// skip rom
    	bus->SendByte(CMD_SKIPROM);
    }
    else
    {
    	//rom
		bus->SendByte(CMD_MATCHROM);
		bus->SendByte(id[0]);
		bus->SendByte(id[1] );
		bus->SendByte(id[2]);
		bus->SendByte(id[3]);
		bus->SendByte(id[4]);
		bus->SendByte(id[5]);
		bus->SendByte(id[6]);
		bus->SendByte(id[7]);
    };

    bus->SendByte(CMD_CONVERTTEMP);

	return 1;
}

//ïåðåâ³ðÿº íàÿâí³ñòü äàò÷èêà íà ë³í³¿ ³ ïåðåâ³ðÿº ÷è äàò÷èê ÄÑ18S20 ³íàêøå ÅÐÐÎÎÎÎÎÎÐ
/***********************************************************************************
    Return :
        0 - if termal sensor is off
        1 - if termal sensor is on
        2 - if termal sensor not on not off
************************************************************************************/
unsigned char DS18B20::is()
{
    if (!bus->DetectPresence()){
      return 0;
    };

    return 1;
}

#include "cmsis_os.h"
/// read temperature wit delay
float DS18B20::exec()
{
    unsigned char scratchpad[9];
    signed int T = 0;
    float Tem = 1.0;
    unsigned char high = 0;
    unsigned char low = 0;

    if (start_conv() == 0)
    {
        // error cant start conversion
		return -999;
    };
	
	osDelay(1000);
	
	///read temperature register
	if (read_scratchpad (scratchpad) == 1)	{
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

