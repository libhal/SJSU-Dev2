#pragma once

#if !defined(PLATFORM)
#define PLATFORM lpc40xx
#endif

namespace sjsu
{
namespace build
{
/// Defines the list of build platforms that SJSU-Dev2 can build applications
/// for.
enum class Platform
{
  host,         // NOLINT
  lpc17xx,      // NOLINT
  lpc40xx,      // NOLINT
  linux,        // NOLINT
  msp432p401r,  // NOLINT
  stm32f10x,    // NOLINT
  stm32f4xx,    // NOLINT
};

constexpr const Platform kPlatform = Platform::PLATFORM;

constexpr bool IsPlatform(Platform platform)
{
  return kPlatform == platform;
}

/// @param platform - the platform to convert to a string
/// @return a string representation of the platform.
constexpr const char * Stringify(Platform platform)
{
  const char * result = "invalid";
  switch (platform)
  {
    case Platform::host: result = "host"; break;
    case Platform::lpc17xx: result = "lpc17xx"; break;
    case Platform::lpc40xx: result = "lpc40xx"; break;
    case Platform::linux: result = "linux"; break;
    case Platform::msp432p401r: result = "msp432p401r"; break;
    case Platform::stm32f10x: result = "stm32f10x"; break;
    case Platform::stm32f4xx: result = "stm32f4xx"; break;
    default: break;
  }
  return result;
}
}  // namespace build
}  // namespace sjsu
