// this is the ssp test file

#include <assert.h>
#include <cstdio>
#include "L0_LowLevel/LPC40xx.h"
#include "L1_Drivers/pin_configure.hpp"
#include "L1_Drivers/ssp.hpp"
#include "L5_Testing/testing_frameworks.hpp"

TEST_CASE("Testing SSP dependency injection", "[ssp]")
{
    // Simulate LPC TypeDefs
    LPC_SSP_TypeDef local_ssp[2] =
    {
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
    };
    LPC_IOCON_TypeDef local_iocon;
    LPC_SC_TypeDef local_sysclock;

    Ssp::ssp_registers[0] = &local_ssp[0];
    PinConfigure::pin_map =
        reinterpret_cast<PinConfigure::PinMap_t *>(&local_iocon);
    Ssp::sysclock_register = &local_sysclock;

    // Set up Mock for PinCongiure
    fakeit::Mock<PinConfigureInterface> mock_mosi;
    fakeit::Mock<PinConfigureInterface> mock_miso;
    fakeit::Mock<PinConfigureInterface> mock_sck;

    fakeit::Fake(Method(mock_mosi, SetPinFunction));
    fakeit::Fake(Method(mock_miso, SetPinFunction));
    fakeit::Fake(Method(mock_sck, SetPinFunction));

    PinConfigureInterface &mosi = mock_mosi.get();
    PinConfigureInterface &miso = mock_miso.get();
    PinConfigureInterface &sck = mock_sck.get();

    Ssp test_spi(SspInterface::Peripheral::kSsp0, mosi, miso, sck);
    test_spi.Initialize();

    SECTION("Test SSP Power")
    {
        constexpr uint8_t kSsp0PowerPosition = 21;
        constexpr uint32_t kMask = 0b1 << kSsp0PowerPosition;
        CHECK((local_sysclock.PCONP & kMask) == kMask);
    }

    SECTION("Test SSP Parameters")
    {
        constexpr uint8_t kMasterPosition = 2;
        constexpr uint32_t kMasterMask = 0b1 << kMasterPosition;
        constexpr uint8_t kDataSize = 0b0111;
        constexpr uint8_t kFramePosition = 4;
        constexpr uint32_t kFrameMask = 0b11 << kFramePosition;
        constexpr uint8_t kPolarityPosition = 6;
        constexpr uint32_t kPolarityMask = 0b1 << kPolarityPosition;
        constexpr uint8_t kPhasePosition = 7;
        constexpr uint32_t kPhaseMask = 0b1 << kPhasePosition;
        constexpr uint8_t kDivider = 0;
        constexpr uint8_t kClockPrescaler = 0b10;

        CHECK((local_ssp[0].CR1 & kMasterMask) == 0);
        CHECK((local_ssp[0].CR0 & 0b1111) == kDataSize);
        CHECK((local_ssp[0].CR0 & kFrameMask) == 0);
        CHECK((local_ssp[0].CR0 & kPolarityMask) == kPolarityMask);
        CHECK((local_ssp[0].CR0 & kPhaseMask) == 0);
        CHECK((local_ssp[0].CR0 & (0b1111'1111 << 8)) == kDivider);
        CHECK((local_ssp[0].CPSR & 0b11) == kClockPrescaler);
    }

    SECTION("Test Pin Numbers")
    {
        fakeit::Verify(Method(
            mock_mosi, SetPinFunction).Using(Ssp::kEnableSspPins));
        fakeit::Verify(Method(
            mock_miso, SetPinFunction).Using(Ssp::kEnableSspPins));
        fakeit::Verify(Method(
            mock_sck, SetPinFunction).Using(Ssp::kEnableSspPins));
    }
}
