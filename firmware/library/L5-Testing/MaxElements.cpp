#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "MAX7456.hpp"
#include "MaxElements.hpp"

// ==============================
// Max Layout Manager
// ==============================

MaxLayoutManager::MaxLayoutManager(
    void (*nChipSelect)(bool Select),
    uint8_t (*nSPI)(uint8_t Data)
) : MAX7456(nChipSelect, nSPI)
{
    ElementsCount = 0;
    for(int i = 0; i < TOTAL_ELEMENTS; i++)
    {
        Elements[i] = NULL;
    }
}
bool MaxLayoutManager::Add(MaxElement * Elem)
{
    if(ElementsCount < TOTAL_ELEMENTS)
    {
        Elements[ElementsCount++] = Elem;
    }
    return true;
}
bool MaxLayoutManager::Remove(MaxElement * Elem)
{
    if(ElementsCount > 0)
    {
        Elements[ElementsCount--] = NULL;
    }
    return true;
}
bool MaxLayoutManager::RemoveAll()
{
    for (int i = 0; i < TOTAL_ELEMENTS; ++i)
    {
        Elements[i] = NULL;
    }
    ElementsCount = 0;
    return true;
}
bool MaxLayoutManager::Render()
{
    for (int i = 0; Elements[i] != NULL; ++i)
    {
        const char * Graphic = Elements[i]->GetGraphic();
        #if 0
            printf("%s\n", Graphic);
        #else
            WriteString(
                CoordsToPosition(
                    Elements[i]->getX(),
                    Elements[i]->getY()
                ),
                (char *)Graphic
            );
        #endif
    }
    return true;
}

// ==============================
// Max Element
// ==============================

MaxElement::MaxElement(uint8_t Text_Size)
{
    X = 0;
    Y = 0;
    Graphic = new char[Text_Size+1];
}
const char * const MaxElement::GetGraphic()
{
    return (const char * const) Graphic;
}
uint8_t MaxElement::getX()
{
    return X;
}
uint8_t MaxElement::getY()
{
    return Y;
}
void MaxElement::setX(uint8_t nX)
{
    X = nX;
}
void MaxElement::setY(uint8_t nY)
{
    Y = nY;
}
void MaxElement::setCoords(uint8_t nX, uint8_t nY)
{
    X = nX;
    Y = nY;
}
MaxElement::~MaxElement()
{
    delete [] Graphic;
}

// ==============================
// Bar
// ==============================

Bar::Bar() : MaxElement(Bar::GRAPHIC_SIZE)
{
    Label[0] = 0;
    set(100);
}
Bar::Bar(uint8_t New_Precent) : MaxElement(Bar::GRAPHIC_SIZE)
{
    Label[0] = 0;
    set(New_Precent);
}
Bar::Bar(const char * New_Label) : MaxElement(Bar::GRAPHIC_SIZE)
{
    setLabel(New_Label);
}
Bar::Bar(uint8_t New_Precent, const char * New_Label) : MaxElement(Bar::GRAPHIC_SIZE)
{
    set(New_Precent);
    setLabel(New_Label);
}

// uint8_t Bar::BarFill(
//     uint8_t Percent,
//     uint8_t Range_Low,
//     uint8_t Range_High,
//     uint8_t Divides
// )
// {
//     uint8_t Result = 0;
//     uint8_t Sections = (Range_High-Range_Low)/Divides;
//     if(Percent == Range_Low)
//     {
//         Result = 0;
//     }
//     else if(Percent)

//     return Result;
// }

void Bar::updateGraphic()
{
    char Buffer[GRAPHIC_SIZE] = { 0 };

    uint8_t low     = ( 1 <= Precent && Precent < 25)  ?  (Precent/5)       : 0;
    uint8_t midlow  = (25 <= Precent && Precent < 50)  ? ((Precent-25)/5) : 0;
    uint8_t midhigh = (50 <= Precent && Precent < 75)  ? ((Precent-50)/5) : 0;
    uint8_t high    = (75 <= Precent && Precent < 100) ? ((Precent-75)/5) : 0;

    low             = (Precent >= 25)  ? 4 : low;
    midlow          = (Precent >= 50)  ? 4 : midlow;
    midhigh         = (Precent >= 75)  ? 4 : midhigh;
    high            = (Precent == 100) ? 4 : high;

    sprintf(Buffer, "%s[%d%d%d%d]", Label, low, midlow, midhigh, high);
    decodeString(Graphic, Buffer);
}

void Bar::setLabel(const char * New_Label)
{
    strcpy(Label, New_Label);

    updateGraphic();
}

void Bar::set(uint8_t New_Precent)
{
    Precent = (New_Precent > 100) ? 100 : New_Precent;
    updateGraphic();
}

void Bar::decodeString(char * Buffer, char * PrecentString)
{
    int i = 0;
    bool Break_Flag = false;
    for(; PrecentString[i] != 0 && !Break_Flag; i++)
    {
        switch(PrecentString[i])
        {
            case '[':
                Buffer[i] = LEFT_BAR_BRACKET;
                break;
            case ']':
                Buffer[i] = RIGHT_BAR_BRACKET;
                Buffer[++i] = 0;
                Break_Flag = true;
                break;
            case '0' ... '4':
                Buffer[i] = EMPTY_BAR-(PrecentString[i]-'0');
                break;
            default:
                Buffer[i] = PrecentString[i];
                break;
        }
    }
}


// ==============================
// Label
// ==============================

Label::Label() : MaxElement(MAX7456::DISPLAY_WIDTH)
{
    memset(Graphic, 0, MAX7456::DISPLAY_WIDTH);
    length = 0;
}

Label::Label(const char * str) : MaxElement(MAX7456::DISPLAY_WIDTH)
{
    setText(str);
}

void Label::setText(const char * str)
{
    int i;
    for (i = 0; i < MAX7456::DISPLAY_WIDTH && str[i] != '\0'; ++i)
    {
        Graphic[i] = str[i];
    }
    Graphic[i] = 0;
    length = i;
}

void Label::center()
{

    setX((MAX7456::DISPLAY_WIDTH-length)/2);
}

void Label::middle()
{
    setY(MAX7456::DISPLAY_MIDDLE);
}

void Label::origin()
{
    center();
    middle();
}

// ==============================
// Cursor
// ==============================

Cursor::Cursor() : MaxElement(2)
{
    Graphic[0] = TANK_TARGET;
    Graphic[1] = 0;
    setCoords(14, 5);
}

// ==============================
// Score
// ==============================

Score::Score() : MaxElement(11)
{
    Value = 0;
    updateGraphic();
}
Score::Score(uint16_t Initial_Value) : MaxElement(11)
{
    Value = Initial_Value;
    updateGraphic();
}
uint16_t Score::get()
{
    return Value;
}
void Score::updateGraphic()
{
    sprintf(Graphic, "SCORE:%d", Value);
}
void Score::set(uint16_t New_Value)
{
    //// If the value is above 9999
    Value = (New_Value > 9999) ? 9999 : New_Value;
    updateGraphic();
}
uint16_t Score::add(uint16_t Added_Value)
{
    uint16_t New_Value = Value+Added_Value;
    //// Protects from overflow
    Value = (New_Value < Value) ? 9999 : New_Value;
    updateGraphic();
    return Value;
}
uint16_t Score::sub(uint16_t Subbed_Value)
{
    uint16_t New_Value = Value - Subbed_Value;
    //// Protects from underflow
    Value = (New_Value > 9999) ? 0 : New_Value;
    updateGraphic();
    return Value;
}

Score & Score::operator=(uint16_t New_Value)
{
    set(New_Value);
    return *this;
}
Score & Score::operator+=(uint16_t New_Value)
{
    add(New_Value);
    return *this;
}
Score & Score::operator-=(uint16_t New_Value)
{
    sub(New_Value);
    return *this;
}

// ==============================
// Power Ups
// ==============================

PowerUps::PowerUps() : MaxElement(PowerUps::GRAPHIC_SIZE)
{
    Power_Up = NONE;
    Graphic[2] = 0;
}
uint8_t PowerUps::get()
{
    return Power_Up;
}
void PowerUps::set(POWER_UPS_CODE_E Code)
{
    Power_Up = (NONE <= Code && Code <= BLIND) ? Code : NONE;
    updateGraphic();
}
void PowerUps::updateGraphic()
{
    switch(Power_Up)
    {
        case NONE:
            Graphic[0] = EMPTY;
            Graphic[1] = EMPTY;
            break;
        case SHEILD:
            Graphic[0] = OPEN_CIRCLE;
            Graphic[1] = CLOSED_CIRCLE;
            break;
        case SPEEDBOOST:
            Graphic[0] = PLAY_PAUSE;
            Graphic[1] = PLAY_PAUSE;
            break;
        case WIDESHOT:
            Graphic[0] = EMPTY;
            Graphic[1] = WAVE;
            break;
        case FREEZE:
            Graphic[0] = EMPTY;
            Graphic[1] = HOUR_GLASS;
            break;
        case BLIND:
            Graphic[0] = SHINE_LEFT;
            Graphic[1] = SHINE_RIGHT;
            break;
        default:
            break;
    }
}