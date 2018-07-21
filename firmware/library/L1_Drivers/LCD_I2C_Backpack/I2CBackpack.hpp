#ifndef I2CBackpack_H
#define I2CBackpack_H
#include<stdint.h>

class I2CBackpack
{

    public:
        I2CBackpack();
        bool init();
        void set4BitMode();
        void clearScreen();
        void setPosition(uint8_t row, uint8_t  col);
        void returnHome();
        void printChar();
        void displayCursor();
        void noCursor(); //No cursor will appear
        void blinkChar(); //The current character will blink
        void solidChar(); //The current character will not blink
        void setLineDisplay(uint8_t lines); //Select 1, 2, or 4 line display
        bool checkBusyFlag();//Check busy flag
        void displayOn();
        void displayOff();
        void shiftCursorLeft();
        void shiftCursorRight();
        void setFont();
        ~I2CBackpack();

    private:
        uint8_t deviceAddress;
        uint8_t col;
        uint8_t row;
        void write(uint8_t address, uint8_t data);
        void read(uint8_t address)const;

};

#endif
