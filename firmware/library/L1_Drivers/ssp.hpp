/**
 ** Provide How-to for driver

 **
 **
 */

#ifndef SSP_HPP
#define SSP_HPP

#include "LPC17xx.h"

class SSP
{
private: 
	//SSP register lookup table
	LPC_SSP_TypeDef* SSP[3] = {LPC_SSP0, LPC_SSP1, LPC_SSP2};

	//SSP pins: 0 = SSEL; 1 = SCK; 2 = MOSI; 3 = MISO;
	LPC_IOCON_TypeDef* SSP0_PINS[4] = {LPC_IOCON->P0_16, LPC_IOCON->P0_15, LPC_IOCON->P0_18, LPC_IOCON->P0_17};
	LPC_IOCON_TypeDef* SSP1_PINS[4] = {LPC_IOCON->P0_6, LPC_IOCON->P0_7, LPC_IOCON->P0_9, LPC_IOCON->P0_8};
	
	//SSP initialization struct
	struct parameters
	{
		Peripheral PSSP;	//one of the 3 SSP interfaces
		LPC_IOCON_TypeDef* SSEL; 		//SSP chip select port-pin
		LPC_IOCON_TypeDef* SCK; 		//SSP clock port-pin
		LPC_IOCON_TypeDef* MISO; 		//master in, slave out port-pin
		LPC_IOCON_TypeDef* MOSI; 		//master out, slave in port-pin
		MSModes MODE; 		//master = 0, slave = 1
		DataSize DSZE; 		//data size, number of bits
		FrameModes FRFT;	//frame format
		uint8_t CPOL; 		//clock polarity
		uint8_t CPHA; 		//clock phase
		uint8_t SCR; 		//serial clock rate
		uint8_t PRSCALE; 		//clock prescaler
		uint8_t SSEL_EN;
		uint8_t SCK_EN;
		uint8_t MOSI_EN;
		uint8_t MISO_EN;

		constexpr parameters()
		{
			PSSP = SSP0;
			SSEL = SSP0_PINS[0]; 	
			SCK  = SSP0_PINS[1]; 	
			MISO = SSP0_PINS[3];
			MOSI = SSP0_PINS[2];
			MODE = MASTER; 	
			DSZE = EIGHT; 	
			FRFT = SPI;
			CPOL = 1; 	
			CPHA = 0; 	
			SCR  = 0; 	
			PRSCALE = 2;
			SSEL_EN = 2;
			SCK_EN = 2;
			MOSI_EN = 2;
			MISO_EN = 2;
		}
	}init_struct;

public:
	//SSP peripherals
	enum Peripheral
	{
		SSP0 = 0,
		SSP1 = 1,
		SSP2 = 2
	};

	//SSP frame formats
	enum FrameModes
	{
		SPI = 0, 
        TI = 1, 
        MICRO = 2
	};

	//SSP Master/slave modes
	enum MSMode
	{
		MASTER = 0;
		SLAVE = 1
	};


	//SSP data size for frame packets
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
	bool init()
	{
		uint8_t pconp_bit;

		if (init_struct.PSSP == SSP1)
		{
			pconp_bit = 10;
		}
		else if (init_struct.PSSP == SSP2)
		{
			pconp_bit = 20;
		}
		else
		{
			pconp_bit = 21;
		}

		//Power up peripheral and set Pclk
		LPC_SC->PCONP &= ~(1 << pconp_bit);
		LPC_SC->PCONP |= (1 << pconp_bit);
		LPC_SC->PCLKSEL &= ~(0x1F << 0);
		LPC_SC->PCLKSEL = 1;
		SSP[init_struct.PSSP]->CPSR &= ~(0xFF);
		SSP[init_struct.PSSP]->CPSR |= (init.struct.PRSCALE);

		//Enable SSP pins
		init_struct.SSEL = SSEL_EN;
		init_struct.SCK = SCK_EN;
		init_struct.MOSI = MOSI_EN;
		init_struct.MISO = MISO_EN;

		//Set Control Register values
		SSP[init_struct.PSSP]->CR0 &= ~((0xF << 0) | (0x3 << 4) | (0x3 << 6) | (0xFF << 8));
		SSP[init_struct.PSSP]->CR0 |= (init_struct.DSZE << 0) | (init_struct.FRFT << 4);
		SSP[init_struct.PSSP]->CR0 |= (init_struct.CPOL << 6) | (init_struct.CPHA << 7);
		SSP[init_struct.PSSP]->CR0 |= (init_struct.SCR << 8);
		SSP[init_struct.PSSP]->CR1 &= ~(1 << 2);
		SSP[init_struct.PSSP]->CR1 |= (init_struct.MODE << 2);
	}

	/**
	 * Removed, struct constructor defaults to Master SPI mode and 
	 * initialized when init() is called
	 */
	//bool enable_master()
	//{
	//}

	/**
	 * Renamed from get_status()
	 * Checks if the SSP controller is idle.
	 * 
	 * @param sspx - the perpheral SSP0-2 to check
	 * @return 1 - the controller is sending or receiving a data frame. 
	 * @return 0 - the controller is idle. 
	 */
	bool get_transfer_status(Peripheral sspx)
	{
		return (SSP[sspx]->SR & (1<<4));
	}

	/**
	 * Transfers a data frame to an external device using the SSP 
	 * data register. This functions for both transmitting and 
	 * receiving data. It is recommended this region be protected
	 * by a mutex.
	 *
	 * @param sspx - the perpheral SSP0-2 to use
	 * @param data - information to be placed in data register
	 * @return - received data from external device
	 */
	uint8_t transfer(Peripheral sspx, uint8_t data)
	{
		SSP[sspx]->DR = send;
		while (get_transfer_status(sspx));
		return SSP[sspx]->DR;
	}

	SSP()
	{
		//do nothing
	}
	~SSP()
	{
		//do nothing
	}
	
};

#endif