#pragma once

#include "L1_Peripheral/i2c.hpp"
#include "L2_HAL/displays/lcd/lcd.hpp"

namespace sjsu
{
class LCD_I2C : public lcd
{
	public:
		LCD_I2C(uint8_t address)
		{
			_i2c.Initialize();
			_addr = address;
		}
		
		void Initialize() override
		{
			//_i2c.Initialize();
			
			Delay(100);
			WriteByte(WriteOperation::kData, 0x03);
			Delay(5);
			WriteByte(WriteOperation::kData, 0x03);
			Delay(5);
			WriteByte(WriteOperation::kData, 0x03);
			Delay(5);
			WriteByte(WriteOperation::kData, 0x02);
			Delay(5);
			WriteByte(WriteOperation::kData, 0x02);
			Delay(5);
			WriteByte(WriteOperation::kData, 0x01);
			Delay(5);
			WriteByte(WriteOperation::kData, 0x0C);
			Delay(5);
			WriteByte(WriteOperation::kData, 0x01);
			Delay(5);
			WriteByte(WriteOperation::kData, 0x06);
			Delay(5);
					
		}
	
		void WriteByte(WriteOperation operation, uint8_t byte) override
		{
			uint8_t * buffer = &byte;
			_i2c.Write(_addr, buffer, 1, I2c::kI2cTimeout);
		}
	
	private:
		I2c & _i2c;	
		uint8_t _addr;
};	
}  // namespace sjsu