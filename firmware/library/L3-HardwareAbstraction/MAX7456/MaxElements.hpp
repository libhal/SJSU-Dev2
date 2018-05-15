#include <stdint.h>

#include "MAX7456.hpp"

#ifndef MAXELEMENTS_H
#define MAXELEMENTS_H

// ==============================
// Max Element
// ==============================

class MaxElement
{
public:
    MaxElement(uint8_t Text_Size);
    const char * const GetGraphic();
    uint8_t getX();
    uint8_t getY();
    void setX(uint8_t nX);
    void setY(uint8_t nY);
    void setCoords(uint8_t nX, uint8_t nY);
    ~MaxElement();
protected:
    char * Graphic;
    uint8_t X;
    uint8_t Y;
};

// ==============================
// Max Layout Manager
// ==============================

class MaxLayoutManager: public MAX7456
{
public:
    MaxLayoutManager(
        void (*nChipSelect)(bool Select),
        uint8_t (*nSPI)(uint8_t Data)
    );
    bool Add(MaxElement *);
    bool Remove(MaxElement *);
    bool RemoveAll();
    bool Render();
    static const uint8_t TOTAL_ELEMENTS     = 16;

    MaxElement * Elements[TOTAL_ELEMENTS];
    uint32_t ElementsCount;
};

// ==============================
// Bar
// ==============================

#define LINUX_TESTING true

class Bar: public MaxElement
{
private:
    static const uint8_t LABEL_MAX_CHARACTERS = 9;
    static const uint8_t GRAPHIC_SIZE = 15;
    char Label[LABEL_MAX_CHARACTERS+1];
    uint8_t Precent;
public:
    Bar();
    Bar(uint8_t HP);
    Bar(const char * New_Label);
    Bar(uint8_t HP, const char * New_Label);
    void set(uint8_t HP);
    void setLabel(const char * New_Label);
    void updateGraphic();
    void decodeString(char * Buffer, char * HPString);

    #if 1 // if on production mode

    static const uint8_t LEFT_BAR_BRACKET       = 0xED;
    static const uint8_t RIGHT_BAR_BRACKET      = 0xF3;
    static const uint8_t EMPTY_BAR              = 0xF2;
    static const uint8_t FULL_BAR               = 0xEE;

    #else

    static const uint8_t LEFT_BAR_BRACKET       = '[';
    static const uint8_t RIGHT_BAR_BRACKET      = ']';
    static const uint8_t EMPTY_BAR              = '4';
    static const uint8_t FULL_BAR               = '0';

    #endif
};

// ==============================
// Label
// ==============================

class Label: public MaxElement
{
private:
    uint32_t length = 0;
public:
    Label();
    Label(const char *);
    void setText(const char * str);
    void center();
    void middle();
    void origin();
};

// ==============================
// Cursor
// ==============================

class Cursor: public MaxElement
{
public:
    static const uint8_t TANK_TARGET        = 0xCC;
    Cursor();
};

// ==============================
// Score
// ==============================

class Score: public MaxElement
{
private:
    uint16_t Value;
public:
    Score();
    Score(uint16_t score);
    void updateGraphic();
    uint16_t get();
    void set(uint16_t score);
    uint16_t add(uint16_t Added_Value);
    uint16_t sub(uint16_t Subbed_Value);
    Score & operator= (uint16_t value);
    Score & operator+=(uint16_t value);
    Score & operator-=(uint16_t value);
};

// ==============================
// Power Ups
// ==============================

class PowerUps: public MaxElement
{
public:
    static const uint8_t GRAPHIC_SIZE       = 3;
    static const uint8_t SHINE_LEFT         = 0xF5;
    static const uint8_t SHINE_RIGHT        = 0xF6;
    static const uint8_t OPEN_CIRCLE        = 0xF7;
    static const uint8_t CLOSED_CIRCLE      = 0xF8;
    static const uint8_t WAVE               = 0xFA;
    static const uint8_t HOUR_GLASS         = 0xFB;
    static const uint8_t PLAY_PAUSE         = 0xF4;
    static const uint8_t EMPTY              = 0xEB;

    enum POWER_UPS_CODE_E
    {
        NONE = 0,
        SHEILD,
        SPEEDBOOST,
        WIDESHOT,
        FREEZE,
        BLIND
    };

    PowerUps();
    uint8_t get();
    void set(POWER_UPS_CODE_E Code);
    void updateGraphic();

private:
    POWER_UPS_CODE_E Power_Up;
};

#endif