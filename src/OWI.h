#pragma once

#include <string.h> // Used for memcpy.
#include "hal.h"
#include "port.h"

class OWI {
	
	ioline_t pin;
	
public:
	/****************************************************************************
	 Macros
	****************************************************************************/
	/*! \brief Pull 1-Wire bus low.
	 *
	 *  This macro sets the direction of the 1-Wire pin(s) to output and 
	 *  pull the line(s) low.
	 *  
	 *  \param bitMask  A bitmask of the buses to pull low.
	 */
	#define OWI_PULL_BUS_LOW(pin)        \
		palSetLineMode(pin, PAL_MODE_OUTPUT_PUSHPULL); \
		palClearLine(pin);

	/*! \def    OWI_RELEASE_BUS(bitMask)
	 *
	 *  \brief  Release the bus. 
	 *
	 *  This macro releases the bus and enables the internal pull-up if
	 *  it is used.
	 *
	 *  \param  bitMask A bitmask of the buses to release.
	 */
	#ifdef OWI_USE_INTERNAL_PULLUP            
	#else
		// Set 1-Wire pin(s) to input mode. No internal pull-up enabled.
		#define OWI_RELEASE_BUS(pin) \
			palSetLineMode(pin, PAL_MODE_INPUT);
	#endif

	// Uncomment one of the two following lines to choose between
	// software only and UART driver.
	#define     OWI_SOFTWARE_DRIVER
	//#define     OWI_UART_DRIVER

	/*! Use U(S)ART double speed
	 *
	 *  Set this define to '1' to enable U(S)ART double speed. More information
	 *  can be found in the data sheet of the AVR.
	 *
	 *  \note   The UART Baud Rate Register settings are also affected by this
	 *          setting.
	 */
	#define     OWI_UART_2X         1


	/*! UART Baud Rate register setting that results in 115200 Baud
	 *
	 *  This define should be set to the UBRR value that will generate
	 *  a Baud rate of 115200. See data sheet for more information and 
	 *  examples of Baud rate settings.
	 */
	#define     OWI_UBRR_115200     8


	/*! UART Baud Rate register setting that results in 9600 Baud
	 *
	 *  This define should be set to the UBRR value that will generate
	 *  a Baud rate of 9600. See data sheet for more information and 
	 *  examples of Baud rate settings.
	 */
	#define     OWI_UBRR_9600       103



	/*****************************************************************************
	 Other defines
	*****************************************************************************/
	// Pin bitmasks.



	/*****************************************************************************
	 Timing parameters
	*****************************************************************************/

	//#define     OWI_DELAY_OFFSET_CYCLES    13   //!< Timing delay when pulling bus low and releasing bus.

	// Bit timing delays in clock cycles (= us*clock freq in MHz).
	#define     OWI_DELAY_A_STD_MODE    (6/4)
	#define     OWI_DELAY_B_STD_MODE    (64)
	#define     OWI_DELAY_C_STD_MODE    (60)
	#define     OWI_DELAY_D_STD_MODE    (10)
	#define     OWI_DELAY_E_STD_MODE    (9/4)
	#define     OWI_DELAY_F_STD_MODE    (55)
	//#define     OWI_DELAY_G_STD_MODE  ((0   * CPU_FREQUENCY) - OWI_DELAY_OFFSET_CYCLES) / 4
	#define     OWI_DELAY_H_STD_MODE    (480)
	#define     OWI_DELAY_I_STD_MODE    (70)
	#define     OWI_DELAY_J_STD_MODE    (410)



	typedef struct
	{
		unsigned char id[8];    //!< The 64 bit identifier.
	} OWI_device;


	#define SEARCH_SUCCESSFUL     0x00
	#define SEARCH_CRC_ERROR      0x01
	#define SEARCH_ERROR          0xff
	#define AT_FIRST              0xff
	
	void SendByte(unsigned char data);
	unsigned char ReceiveByte();
	void SkipRom();
	void ReadRom(unsigned char * romValue);
	void MatchRom(unsigned char * romValue);
	unsigned char SearchRom(unsigned char * bitPattern, unsigned char lastDeviation);
	unsigned char SearchDevices(OWI_device * devices, unsigned char numDevices, unsigned char *num);
	unsigned char FindFamily(unsigned char familyID, OWI_device * devices, unsigned char numDevices, unsigned char lastNum);
	
	/****************************************************************************
	 ROM commands
	****************************************************************************/
	#define     OWI_ROM_READ    0x33    //!< READ ROM command code.
	#define     OWI_ROM_SKIP    0xcc    //!< SKIP ROM command code.
	#define     OWI_ROM_MATCH   0x55    //!< MATCH ROM command code.
	#define     OWI_ROM_SEARCH  0xf0    //!< SEARCH ROM command code.


	/****************************************************************************
	 Return codes
	****************************************************************************/
	#define     OWI_ROM_SEARCH_FINISHED     0x00    //!< Search finished return code.
	#define     OWI_ROM_SEARCH_FAILED       0xff    //!< Search failed return code.


	/****************************************************************************
	 UART patterns
	****************************************************************************/
	#define     OWI_UART_WRITE1     0xff    //!< UART Write 1 bit pattern.
	#define     OWI_UART_WRITE0     0x00    //!< UART Write 0 bit pattern.
	#define     OWI_UART_READ_BIT   0xff    //!< UART Read bit pattern.
	#define     OWI_UART_RESET      0xf0    //!< UART Reset bit pattern.


	#define     OWI_CRC_OK      0x00    //!< CRC check succeded
	#define     OWI_CRC_ERROR   0x01    //!< CRC check failed

	unsigned char ComputeCRC8(unsigned char inData, unsigned char seed);
	unsigned int ComputeCRC16(unsigned char inData, unsigned int seed);
	unsigned char CheckRomCRC(unsigned char * romValue);
	unsigned char CheckScratchPadCRC(unsigned char * scratchpad);

	void Init(ioline_t pin);
	void WriteBit1();
	void WriteBit0();
	unsigned char ReadBit();
	unsigned char DetectPresence();	
};