

#include "LPC17xx.h"

class SSP
{
private: 
	/*SSP register lookup table*/
	LPC_SSP_TypeDef* SSP[3] = {LPC_SSP0, LPC_SSP1, LPC_SSP2};

	/*SSP pins*/


	/*SSP initialization struct*/
	struct parameters
	{
		Peripheral PSSP; //one of the 3 SSP interfaces
		uint16_t SSEL; //SSP chip select port-pin
		uint16_t SCK; //SSP clock port-pin
		uint16_t MISO; //master in, slave out port-pin
		uint16_t MOSI; //master out, slave in port-pin
		uint8_t MODE; //master = 1, slave = 0
		uint8_t DSZE; //data size, number of bits
		uint8_t FRFT; //frame format
		uint8_t CPOL; //clock polarity
		uint8_t CPHA; //clock phase
		uint8_t SCR; //serial clock rate
		uint8_t CPSR; //clock prescaler
	}init_struct;

public:
	/*SSP peripherals*/
	enum Peripheral
	{
		SSP0 = 0,
		SSP1 = 1,
		SSP2 = 2
	};

	/*SSP frame formats*/
	enum FrameModes
	{
		SPI = 0, 
        TI = 1, 
        MICRO = 2
	};

	/*SSP data size for frame packets*/
	enum DataSize
	{
		FOUR 	= 0011,	// 4-bit  transfer
		FIVE 	= 0100,	// 5-bit  transfer
		SIX 	= 0101,	// 6-bit  transfer
		SEVEN 	= 0110,	// 7-bit  transfer
		EIGHT 	= 0111,	// 8-bit  transfer
		NINE 	= 1000,	// 9-bit  transfer
		TEN 	= 1001,	// 10-bit transfer
		ELEVEN 	= 1010,	// 11-bit transfer
		TWELVE 	= 1011,	// 12-bit transfer
		THIRTEEN 	= 1100,	// 13-bit transfer
		FOURTEEN 	= 1101,	// 14-bit transfer
		FIFTEEN 	= 1110,	// 15-bit transfer
		SIXTEEN 	= 1111,	// 16-bit transfer
	};

	/**
	 * 
	 */
	bool init();

	/**
	 * 
	 */
	bool enable_master();

	/**
	 * 
	 */
	bool get_status();

	/**
	 * 
	 */
	uint8_t transfer(Peripheral ssp, uint8_t data);

	SSP();
	~SSP();
	
};