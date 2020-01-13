
#include "OWI.h"


/*! \brief  Compute the CRC8 value of a data set.
 *
 *  This function will compute the CRC8 or DOW-CRC of inData using seed
 *  as inital value for the CRC.
 *
 *  \param  inData  One byte of data to compute CRC from.
 *
 *  \param  seed    The starting value of the CRC.
 *
 *  \return The CRC8 of inData with seed as initial value.
 *
 *  \note   Setting seed to 0 computes the crc8 of the inData.
 *
 *  \note   Constantly passing the return value of this function 
 *          As the seed argument computes the CRC8 value of a
 *          longer string of data.
 */
unsigned char OWI::ComputeCRC8(unsigned char inData, unsigned char seed)
{
    unsigned char bitsLeft;
    unsigned char temp;

    for (bitsLeft = 8; bitsLeft > 0; bitsLeft--)
    {
        temp = ((seed ^ inData) & 0x01);
        if (temp == 0)
        {
            seed >>= 1;
        }
        else
        {
            seed ^= 0x18;
            seed >>= 1;
            seed |= 0x80;
        }
        inData >>= 1;
    }
    return seed;    
}


/*! \brief  Compute the CRC16 value of a data set.
 *
 *  This function will compute the CRC16 of inData using seed
 *  as inital value for the CRC.
 *
 *  \param  inData  One byte of data to compute CRC from.
 *
 *  \param  seed    The starting value of the CRC.
 *
 *  \return The CRC16 of inData with seed as initial value.
 *
 *  \note   Setting seed to 0 computes the crc16 of the inData.
 *
 *  \note   Constantly passing the return value of this function 
 *          As the seed argument computes the CRC16 value of a
 *          longer string of data.
 */
unsigned int OWI::ComputeCRC16(unsigned char inData, unsigned int seed)
{
    unsigned char bitsLeft;
    unsigned char temp;

    for (bitsLeft = 8; bitsLeft > 0; bitsLeft--)
    {
        temp = ((seed ^ inData) & 0x01);
        if (temp == 0)
        {
            seed >>= 1;
        }
        else
        {
            seed ^= 0x4002;
            seed >>= 1;
            seed |= 0x8000;
        }
        inData >>= 1;
    }
    return seed;    
}


/*! \brief  Calculate and check the CRC of a 64 bit ROM identifier.
 *  
 *  This function computes the CRC8 value of the first 56 bits of a
 *  64 bit identifier. It then checks the calculated value against the
 *  CRC value stored in ROM.
 *
 *  \param  romvalue    A pointer to an array holding a 64 bit identifier.
 *
 *  \retval OWI_CRC_OK      The CRC's matched.
 *  \retval OWI_CRC_ERROR   There was a discrepancy between the calculated and the stored CRC.
 */
unsigned char OWI::CheckRomCRC(unsigned char * romValue)
{
    unsigned char i;
    unsigned char crc8 = 0;
    
    for (i = 0; i < 7; i++)
    {
        crc8 = ComputeCRC8(*romValue, crc8);
        romValue++;
    }
    if (crc8 == (*romValue))
    {
        return OWI_CRC_OK;
    }
    return OWI_CRC_ERROR;
}

unsigned char OWI::CheckScratchPadCRC(unsigned char * scratchpad)
{
    unsigned char i;
    unsigned char crc8 = 0;
    
    for (i = 0; i < 8; i++)
    {
        crc8 = ComputeCRC8(*scratchpad, crc8);
        scratchpad++;
    }
    if (crc8 == (*scratchpad))
    {
        return OWI_CRC_OK;
    }
    return OWI_CRC_ERROR;
}


/*! \brief  Sends one byte of data on the 1-Wire(R) bus(es).
 *  
 *  This function automates the task of sending a complete byte
 *  of data on the 1-Wire bus(es).
 *
 *  \param  data    The data to send on the bus(es).
 *  
 *  \param  pins    A bitmask of the buses to send the data to.
 */
void OWI::SendByte(unsigned char data)
{
    unsigned char temp;
    unsigned char i;
    
    // Do once for each bit
    for (i = 0; i < 8; i++)
    {
        // Determine if lsb is '0' or '1' and transmit corresponding
        // waveform on the bus.
        temp = data & 0x01;
        if (temp)
        {
            WriteBit1();
        }
        else
        {
            WriteBit0();
        }
        // Right shift the data to get next bit.
        data >>= 1;
    }
}


/*! \brief  Receives one byte of data from the 1-Wire(R) bus.
 *
 *  This function automates the task of receiving a complete byte 
 *  of data from the 1-Wire bus.
 *
 *  \param  pin     A bitmask of the bus to read from.
 *  
 *  \return     The byte read from the bus.
 */
unsigned char OWI::ReceiveByte()
{
    unsigned char data;
    unsigned char i;

    // Clear the temporary input variable.
    data = 0x00;
    
    // Do once for each bit
    for (i = 0; i < 8; i++)
    {
        // Shift temporary input variable right.
        data >>= 1;
        // Set the msb if a '1' value is read from the bus.
        // Leave as it is ('0') else.
        if (ReadBit())
        {
            // Set msb
            data |= 0x80;
        }
    }
    return data;
}


/*! \brief  Sends the SKIP ROM command to the 1-Wire bus(es).
 *
 *  \param  pins    A bitmask of the buses to send the SKIP ROM command to.
 */
void OWI::SkipRom()
{
    // Send the SKIP ROM command on the bus.
    SendByte(OWI_ROM_SKIP);
}


/*! \brief  Sends the READ ROM command and reads back the ROM id.
 *
 *  \param  romValue    A pointer where the id will be placed.
 *
 *  \param  pin     A bitmask of the bus to read from.
 */
void OWI::ReadRom(unsigned char * romValue)
{
    unsigned char bytesLeft = 8;

    // Send the READ ROM command on the bus.
    SendByte(OWI_ROM_READ);
    
    // Do 8 times.
    while (bytesLeft > 0)
    {
        // Place the received data in memory.
        *romValue++ = ReceiveByte();
        bytesLeft--;
    }
}


/*! \brief  Sends the MATCH ROM command and the ROM id to match against.
 *
 *  \param  romValue    A pointer to the ID to match against.
 *
 *  \param  pins    A bitmask of the buses to perform the MATCH ROM command on.
 */
void OWI::MatchRom(unsigned char * romValue)
{
    unsigned char bytesLeft = 8;   
    
    // Send the MATCH ROM command.
    SendByte(OWI_ROM_MATCH);

    // Do once for each byte.
    while (bytesLeft > 0)
    {
        // Transmit 1 byte of the ID to match.
        SendByte(*romValue++);
        bytesLeft--;
    }
}


/*! \brief  Sends the SEARCH ROM command and returns 1 id found on the 
 *          1-Wire(R) bus.
 *
 *  \param  bitPattern      A pointer to an 8 byte char array where the 
 *                          discovered identifier will be placed. When 
 *                          searching for several slaves, a copy of the 
 *                          last found identifier should be supplied in 
 *                          the array, or the search will fail.
 *
 *  \param  lastDeviation   The bit position where the algorithm made a 
 *                          choice the last time it was run. This argument 
 *                          should be 0 when a search is initiated. Supplying 
 *                          the return argument of this function when calling 
 *                          repeatedly will go through the complete slave 
 *                          search.
 *
 *  \param  pin             A bit-mask of the bus to perform a ROM search on.
 *
 *  \return The last bit position where there was a discrepancy between slave addresses the last time this function was run. Returns OWI_ROM_SEARCH_FAILED if an error was detected (e.g. a device was connected to the bus during the search), or OWI_ROM_SEARCH_FINISHED when there are no more devices to be discovered.
 *
 *  \note   See main.c for an example of how to utilize this function.
 */
unsigned char OWI::SearchRom(unsigned char * bitPattern, unsigned char lastDeviation)
{
    unsigned char currentBit = 1;
    unsigned char newDeviation = 0;
    unsigned char bitMask = 0x01;
    unsigned char bitA;
    unsigned char bitB;

    // Send SEARCH ROM command on the bus.
    SendByte(OWI_ROM_SEARCH);
    
    // Walk through all 64 bits.
    while (currentBit <= 64)
    {
        // Read bit from bus twice.
        bitA = ReadBit();
        bitB = ReadBit();

        if (bitA && bitB)
        {
            // Both bits 1 (Error).
            newDeviation = OWI_ROM_SEARCH_FAILED;
            return SEARCH_ERROR;
        }
        else if (bitA ^ bitB)
        {
            // Bits A and B are different. All devices have the same bit here.
            // Set the bit in bitPattern to this value.
            if (bitA)
            {
                (*bitPattern) |= bitMask;
            }
            else
            {
                (*bitPattern) &= ~bitMask;
            }
        }
        else // Both bits 0
        {
            // If this is where a choice was made the last time,
            // a '1' bit is selected this time.
            if (currentBit == lastDeviation)
            {
                (*bitPattern) |= bitMask;
            }
            // For the rest of the id, '0' bits are selected when
            // discrepancies occur.
            else if (currentBit > lastDeviation)
            {
                (*bitPattern) &= ~bitMask;
                newDeviation = currentBit;
            }
            // If current bit in bit pattern = 0, then this is
            // out new deviation.
            else if ( !(*bitPattern & bitMask)) 
            {
                newDeviation = currentBit;
            }
            // IF the bit is already 1, do nothing.
            else
            {
            
            }
        }
                
        
        // Send the selected bit to the bus.
        if ((*bitPattern) & bitMask)
        {
            WriteBit1();
        }
        else
        {
            WriteBit0();
        }

        // Increment current bit.    
        currentBit++;

        // Adjust bitMask and bitPattern pointer.    
        bitMask <<= 1;
        if (!bitMask)
        {
            bitMask = 0x01;
            bitPattern++;
        }
    }
    return newDeviation;
}

/*! \brief  Perform a 1-Wire search
 *
 *  This function shows how the OWI_SearchRom function can be used to 
 *  discover all slaves on the bus. It will also CRC check the 64 bit
 *  identifiers.
 *
 *  \param  devices Pointer to an array of type OWI_device. The discovered 
 *                  devices will be placed from the beginning of this array.
 *
 *  \param  numDevices   The number of the device array.
 *
 *  \param  pin  
 *
 *  \retval SEARCH_SUCCESSFUL   Search completed successfully.
 *  \retval SEARCH_CRC_ERROR    A CRC error occured. Probably because of noise
 *                              during transmission.
 */
unsigned char OWI::SearchDevices(OWI_device * devices, unsigned char numDevices, unsigned char *num)
{
    unsigned char i, j;
    unsigned char * newID;
    unsigned char * currentID;
    unsigned char lastDeviation;
    unsigned char numFoundDevices;
    
    //сбрасываем адреса 1Wire устройств    
    for (i = 0; i < numDevices; i++)
    {
        for (j = 0; j < 8; j++)
        {
            devices[i].id[j] = 0x00;
        }
    }
    
    numFoundDevices = 0;
    newID = devices[0].id;
    lastDeviation = 0;
    currentID = newID;

    do  
    {
      memcpy(newID, currentID, 8);
      if (!DetectPresence()){
        return SEARCH_ERROR;        
      };
      lastDeviation = SearchRom(newID, lastDeviation);
      currentID = newID;
      numFoundDevices++;
      newID=devices[numFoundDevices].id;                
    } while(lastDeviation != OWI_ROM_SEARCH_FINISHED);            

    
    // Go through all the devices and do CRC check.
    for (i = 0; i < numFoundDevices; i++)
    {
        // If any id has a crc error, return error.
        if(CheckRomCRC(devices[i].id) != OWI_CRC_OK)
        {
            return SEARCH_CRC_ERROR;
        }
        (*num)++;
    }
    // Else, return Successful.
    return SEARCH_SUCCESSFUL;
}

/*! \brief  Find the first device of a family based on the family id
 *
 *  This function returns a pointer to a device in the device array
 *  that matches the specified family.
 *
 *  \param  familyID    The 8 bit family ID to search for.
 *
 *  \param  devices     An array of devices to search through.
 *
 *  \param  numDevices        The size of the array 'devices'
 *
 *  \return A pointer to a device of the family.
 *  \retval NULL    if no device of the family was found.
 */
unsigned char OWI::FindFamily(unsigned char familyID, OWI_device * devices, unsigned char numDevices, unsigned char lastNum)
{
    unsigned char i;
    
    if (lastNum == AT_FIRST){
      i = 0;
    }
    else{
      i = lastNum + 1;      
    }
      
    // Search through the array.
    while (i < numDevices)
    {
        // Return the pointer if there is a family id match.
        if ((*devices).id[0] == familyID)
        {
            return i;
        }
        devices++;
        i++;
    }
    return SEARCH_ERROR;
}

#ifdef OWI_SOFTWARE_DRIVER

#pragma GCC optimize 2
/*! \brief Initialization of the one wire bus(es). (Software only driver)
 *  
 *  This function initializes the 1-Wire bus(es) by releasing it and
 *  waiting until any presence sinals are finished.
 *
 *  \param  pins    A bitmask of the buses to initialize.
 */
void OWI::Init(ioline_t pin)
{
	this->pin = pin;
    OWI_RELEASE_BUS(pin);
    // The first rising edge can be interpreted by a slave as the end of a
    // Reset pulse. Delay for the required reset recovery time (H) to be 
    // sure that the real reset is interpreted correctly.
    WIRE1_PORT_DELAY_US(OWI_DELAY_H_STD_MODE);
}


/*! \brief  Write a '1' bit to the bus(es). (Software only driver)
 *
 *  Generates the waveform for transmission of a '1' bit on the 1-Wire
 *  bus.
 *
 *  \param  pins    A bitmask of the buses to write to.
 */
void OWI::WriteBit1()
{
    unsigned char intState;
    
    // Disable interrupts.
    intState = osalSysGetStatusAndLockX();
    
    // Drive bus low and delay.
    OWI_PULL_BUS_LOW(pin);
    WIRE1_PORT_DELAY_US(OWI_DELAY_A_STD_MODE);
    
    // Release bus and delay.
    OWI_RELEASE_BUS(pin);
    WIRE1_PORT_DELAY_US(OWI_DELAY_B_STD_MODE);
    
    // Restore interrupts.
    osalSysRestoreStatusX(intState);
}


/*! \brief  Write a '0' to the bus(es). (Software only driver)
 *
 *  Generates the waveform for transmission of a '0' bit on the 1-Wire(R)
 *  bus.
 *
 *  \param  pins    A bitmask of the buses to write to.
 */
void OWI::WriteBit0()
{
    unsigned char intState;
    
    // Disable interrupts.
    intState = osalSysGetStatusAndLockX();
    
    // Drive bus low and delay.
    OWI_PULL_BUS_LOW(pin);
    WIRE1_PORT_DELAY_US(OWI_DELAY_C_STD_MODE);
    
    // Release bus and delay.
    OWI_RELEASE_BUS(pin);
    WIRE1_PORT_DELAY_US(OWI_DELAY_D_STD_MODE);
    
    // Restore interrupts.
    osalSysRestoreStatusX(intState);
}


/*! \brief  Read a bit from the bus(es). (Software only driver)
 *
 *  Generates the waveform for reception of a bit on the 1-Wire(R) bus(es).
 *
 *  \param  pins    A bitmask of the bus(es) to read from.
 *
 *  \return A bitmask of the buses where a '1' was read.
 */
unsigned char OWI::ReadBit()
{
    unsigned char intState;
    unsigned char bitsRead;
    
    // Disable interrupts.
    intState = osalSysGetStatusAndLockX();
    
    // Drive bus low and delay.
//	palSetLine(TP1);
    OWI_PULL_BUS_LOW(pin);
    WIRE1_PORT_DELAY_US(OWI_DELAY_A_STD_MODE);
    
    // Release bus and delay.
//	palClearLine(TP1);
    OWI_RELEASE_BUS(pin);
    WIRE1_PORT_DELAY_US(OWI_DELAY_E_STD_MODE);
    
    // Sample bus and delay.
    bitsRead = 	palReadLine(pin);
    WIRE1_PORT_DELAY_US(OWI_DELAY_F_STD_MODE);
    
    // Restore interrupts.
    osalSysRestoreStatusX(intState);
    
    return bitsRead;
}


/*! \brief  Send a Reset signal and listen for Presence signal. (software
 *  only driver)
 *
 *  Generates the waveform for transmission of a Reset pulse on the 
 *  1-Wire(R) bus and listens for presence signals.
 *
 *  \param  pins    A bitmask of the buses to send the Reset signal on.
 *
 *  \return A bitmask of the buses where a presence signal was detected.
 */
unsigned char OWI::DetectPresence()
{
    unsigned char intState;
    unsigned char presenceDetected;
    
    // Disable interrupts.
    intState = osalSysGetStatusAndLockX();
    
    // Drive bus low and delay.
    OWI_PULL_BUS_LOW(pin);
    //_delay_us(480);
    //_delay_loop_2(OWI_DELAY_H_STD_MODE);
    WIRE1_PORT_DELAY_US(OWI_DELAY_H_STD_MODE);
    
    // Release bus and delay.
    OWI_RELEASE_BUS(pin);
   // _delay_loop_2(7.3728 * 70 / 8);
    WIRE1_PORT_DELAY_US(OWI_DELAY_I_STD_MODE);
    
    // Sample bus to detect presence signal and delay.
    presenceDetected = !palReadLine(pin);
    //_delay_loop_2(7.3728 * 410 / 8);
    WIRE1_PORT_DELAY_US(OWI_DELAY_J_STD_MODE);
    
    // Restore interrupts.
   osalSysRestoreStatusX(intState);
    
    return presenceDetected;
}


#endif
