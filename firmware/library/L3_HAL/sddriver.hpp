#pragma once

#include <cstdint>
#include "L1_Drivers/gpio.hpp"
#include "L1_Drivers/ssp.hpp"
#include "L2_Utilities/debug_print.hpp"

// Relevant Datasheet: https://www.sdcard.org/downloads/pls/pdf/index.php?p=Part1_Physical_Layer_Simplified_Specification_Ver6.00.jpg&f=Part1_Physical_Layer_Simplified_Specification_Ver6.00.pdf&e=EN_SS1
class SdInterface
{
 public:
    // // @constructor     SdInterface()
    // constexpr SdInterface() = 0;

    // @destructor      ~SdInterface
    virtual ~SdInterface() {};

    // @struct          SdResponse_t
    // @description     (see below)
    // TODO(#151): Define a response frame struct since there are various
    // responses that will be sent, depending on the command sent previously
    struct SdResponse_t
    {
        union {
            uint64_t qWord;
            struct {
                uint32_t hi;
                uint32_t lo;
            } dWord;
            uint8_t byte[8];
        } data;
        uint8_t length;
    };

    // @enum class      SdResponseType
    // @description     Enumerator for response type codes
    enum class SdResponseType
    {
        kR1,
        kR1b,
        kR2,
        kR3,
        kR4,
        kR5,
        kR6,
        kR7
    };

    // @enum class      SdType
    // @description     Enumerator for SD Card Capacity type codes
    enum class SdType
    {
        kSDSC,   // up to 2GB
        kSDHC,   // up to 32GB
        kSDXC    // up to 256GB
    };

    // @struct          SdCard
    // @description     Structure for recording information about the SD Card
    struct SdCard_t
    {
        union {
            uint32_t dWord;
            uint16_t word[2];
            uint8_t byte[4];
        } ocr;
        SdType type;
        SdResponse_t response;
    };

    // @enum class      SdOcr
    // @description     bit shift counts provided to allow easy checking of
    //                  the OCR flag bits for the card's operating conditions
    enum class SdOcr
    {
        kIsDualVoltage  = 7,    // OCR bit 7: Supports both Low and High
                                // Voltage Ranges
        kIs27vTo28v     = 15,   // OCR bit 15: Supports 2.7V-2.8V
        kIs28vTo29v     = 16,   // OCR bit 16: Supports 2.8V-2.9V
        kIs29vTo30v     = 17,   // OCR bit 17: Supports 2.9V-3.0V
        kIs30vTo31v     = 18,   // OCR bit 18: Supports 3.0V-3.1V
        kIs31vTo32v     = 19,   // OCR bit 19: Supports 3.1V-3.2V
        kIs32vTo33v     = 20,   // OCR bit 20: Supports 3.2V-3.3V
        kIs33vTo34v     = 21,   // OCR bit 21: Supports 3.3V-3.4V
        kIs34vTo35v     = 22,   // OCR bit 22: Supports 3.4V-3.5V
        kIs35vTo36v     = 23,   // OCR bit 23: Supports 3.5V-3.6V
        kIs18vOk        = 24,   // OCR bit 24: Switching to 1.8V is
                                // accepted (S18A)
        kIsUhs2         = 29,   // OCR bit 29: Card is UHS-II Compatible
        kCcs            = 30,   // OCR bit 30: Card Capacity Status (used)
                                // to determine if a card is SDSC or not
        kPwrUpComplete  = 31    // OCR bit 31: Card Power Up Status (i.e.
                                // if card has finished the power cycle
                                // process)
    };

    // @enum class      SdCommand
    // @description     a collection of SPI-supported command codes to be used
    //                  with SendCmd()
    // @note            OR'ing of b0100_0000 is meant to conform with the SD
    //                  protocol (i.e. first bits must be b01)
    enum class SdCommand
    {
        kGarbage    = 0xFF,         // "SendGarbage" command; instructs
                                    // SendCmd() to send 0xFFFFFFFF as data
                                    // and even set the checksum as 0xFF
        kReset      = 0x40 | 0,     // CMD0: reset the sd card (force it to go
                                    // to the idle state)
        kInit       = 0x40 | 1,     // CMD1: starts an initiation of the card
        kGetOp      = 0x40 | 8,     // CMD8: request the sd card's support of
                                    // the provided host's voltage ranges
        kGetStatus  = 0x04 | 13,    // CMD13: get status register
        kChgBlkLen  = 0x40 | 16,    // CMD16: change block length (only
                                    // effective in SDSC cards; SDHC/SDXC
                                    // cards are locked to 512-byte blocks)
        kReadBlk    = 0x40 | 17,    // CMD17: read a single block of data
        kReadBlks   = 0x40 | 18,    // CMD18: read many blocks of data until
                                    // a "CMD12" frame is sent
        kWriteBlk   = 0x40 | 24,    // CMD24: write a single block of data
        kWriteBlks  = 0x40 | 25,    // CMD25: write many blocks of data until
                                    // a "CMD12" frame is sent
        kDelFrom    = 0x40 | 32,    // CMD32: set address of the start block
                                    // for deletion
        kDelTo      = 0x40 | 33,    // CMD33: set address of the end block for
                                    // deletion
        kDel        = 0x40 | 38,    // CMD38: begin deletion from the block
                                    // range specified by the vector
                                    // [DEL_FROM : DEL_TO]
        kAcBegin    = 0x40 | 55,    // CMD55: signals the start of an
                                    // application-specific command
        kGetOcr     = 0x40 | 58,    // CMD58: request data from the
                                    // operational conditions register
        kAcInit     = 0x40 | 41     // ACMD41: application-specific version of
                                    // CMD1 (must precede with CMD55)
    };

    // @function        SendCmd()
    // @description     sends a command frame to the SD Card and sets the
    //                  response
    // @parameter       (SdCommand) sdc                 the command code to
    //                                                  send as the first byte
    //                  (uint32_t) arg                  the 4-byte argument
    //                                                  for the command
    //                  ~(uint8_t*) response_buffer     pointer to a buffer to
    //                                                  house the response
    //                                                  frame
    //                  ~(uint32_t) delay               delay (ms) between
    //                                                  asserting CS and
    //                                                  sending a cmd frame
    //                  ~(bool) keep_alive              determines whether to
    //                                                  de-assert CS after
    //                                                  receiving response
    // @returns         on success: the number of bytes given in the response
    //                  on failure: -1
    virtual uint32_t SendCmd(
        SdCommand sdc,
        uint32_t arg,
        uint8_t response_buffer[] = nullptr,
        uint32_t delay = 0,
        bool keep_alive = false) = 0;

    // @function        InitializeHardware()
    // @description     initializes all required hardware constructs to talk
    //                  to the SD Card via SPI
    // @parameter       (?)
    // @returns         (?)
    virtual void InitializeHardware() = 0;

    // @function        InitializeSdCard()
    // @description     runs the SD Card initialization sequence, effectively
    //                  resetting communication with the card
    // @parameter       (SdCard*) sd                    a pointer to the SD
    //                                                  Card data structure
    // @returns         on init success: true
    //                  on init failure: false
    virtual bool InitializeSdCard(SdCard_t* sd) = 0;

    // @function        ReadBlock()
    // @description     reads any number of 512-byte blocks from the SD Card
    //                  and returns the received block
    //                  This function assumes the block length is locked to
    //                  512-bytes (as is the case for SDHC/SDXC cards).
    // @parameter       (?)
    // @returns         (?)
    // TODO(#160):  Write a function that enables the reading of 1 or more
    //              blocks of data from an SD Card
    virtual void ReadBlock() = 0;

    // @function        WriteBlock()
    // @description     writes any number of 512-byte blocks from the SD Card
    //                  and returns the received block
    //                  This function assumes the block length is locked to
    //                  512-bytes (as is the case for SDHC/SDXC cards).
    // @parameter       (?)
    // @returns         (?)
    // TODO(#161):  Write a function that enables the writing of 1 or more
    //              blocks of data to an SD Card
    virtual void WriteBlock() = 0;

    // @function        DeleteBlock()
    // @description     reads any number of 512-byte block from the SD Card
    //                  and returns the received block
    //                  This function assumes the block length is locked to
    //                  512-bytes (as is the case for SDHC/SDXC cards).
    // @parameter       (?)
    // @returns         (?)
    // TODO(#162):  Write a function that enables the deletion of 1 or more
    //              blocks of data from an SD Card
    virtual void DeleteBlock() = 0;

    // virtual
    virtual void GenerateCRCTable() = 0;

    // virtual
    virtual unsigned char CRCAdd(
        unsigned char crc,
        unsigned char message_byte) = 0;

    // virtual
    virtual unsigned char GetCrc(unsigned char message[], uint8_t length) = 0;

    // virtual
    virtual void PrintResponse(
        uint8_t* buf,
        uint8_t len,
        uint8_t type = 0) = 0;

    // @function        GetCrc()
    // @description     returns the CRC7 code for a given message (includes
    //                  command byte and up to a 32-bit argument)
    // @parameter       (unsigned char*) message        array of the command
    //                                                  byte and relevant
    //                                                  argument bytes
    // @parameter       (uint8_t) length                length (in bytes) of
    //                                                  the given message
    // virtual unsigned char GetCrc(
    //     unsigned char* message,
    //     uint8_t length) = 0;
};
