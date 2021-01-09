
/**
 * This is the header file for the CAN driver.
 * 
 * The CAN controller configured will have no ID filters, and the
 * bit rate is set to 400KBS. 
 *
 * Polling will be required by the user, since the functions executed when CAN 
 * messages are received are too complex and long, and will not be suitable to
 * be placed in a interrupt handler (the alternative is to have the interrupt 
 * set a valid bit and poll that bit in the main loop. Unfortunately, clearing
 * the interrupt mask means setting the pending number of CAN messages to 0, 
 * which means, depending on the rate of messages being received, some messages
 * will be dropped.)
 *
 * Note: Please define the following fields in the main file 
 * (or any file that will include this header file):
 * - CAN_msg_t CAN_rx_msg
 * - CAN_msg_t CAN_tx_msg
 */

#include "stm32f10x.h"

#ifndef CAN_H
#define CAN_H

enum BITRATE{CAN_50KBPS, CAN_100KBPS, CAN_125KBPS, CAN_250KBPS, CAN_500KBPS, CAN_1000KBPS};

typedef struct
{
	uint16_t id;
	uint8_t  data[8];
	uint8_t  len;
} CAN_msg_t;

typedef const struct
{
	uint8_t TS2;
	uint8_t TS1;
	uint8_t BRP;
} CAN_bit_timing_config_t;

extern CAN_bit_timing_config_t can_configs[6];

/**
 * Initializes the CAN controller with specified bit rate.
 *
 * @params: bitrate - Specified bitrate. If this value is not one of the defined constants, bit rate will be defaulted to 125KBS
 *
 */
 void CANInit(enum BITRATE bitrate);
 
/**
 * Decodes CAN messages from the data registers and populates a 
 * CAN message struct with the data fields.
 * 
 * @preconditions A valid CAN message is received
 * @params CAN_rx_msg - CAN message struct that will be populated
 * 
 */
 void CANReceive(CAN_msg_t* CAN_rx_msg);
 
/**
 * Encodes CAN messages using the CAN message struct and populates the 
 * data registers with the sent.
 * 
 * @params CAN_rx_msg - CAN message struct that will be populated
 * 
 */
 void CANSend(CAN_msg_t* CAN_tx_msg);
 
 void CANSetFilter(uint16_t id);
 
 void CANSetFilters(uint16_t* ids, uint8_t num);
 
/**
 * Returns whether there are CAN messages available.
 *
 * @returns If pending CAN messages are in the CAN controller
 *
 */
 uint8_t CANMsgAvail(void);
 
 extern CAN_msg_t CAN_rx_msg;  // Holds receiving CAN messages
 extern CAN_msg_t CAN_tx_msg;  // Holds transmitted CAN messagess

#endif