#pragma once

#if !defined(TARGET)
#define TARGET Application
#endif

#if !defined(PLATFORM)
#define PLATFORM lpc40xx
#endif

namespace sjsu
{
namespace build
{
/// Defines the number of build targets available for SJSU-Dev2.
enum class Target
{
  Application = 0,  // NOLINT
  HostTest,         // NOLINT
};
/// Defines the list of build platforms that SJSU-Dev2 can build applications
/// for.
enum class Platform
{
  lpc17xx,  // NOLINT
  lpc40xx,  // NOLINT
  linux,    // NOLINT
};

constexpr const Target kTarget     = Target::TARGET;
constexpr const Platform kPlatform = Platform::PLATFORM;

/// @param target - the target to convert to a string
/// @return a string representation of the target.
constexpr const char * Stringify(Target target)
{
  const char * result = "invalid";
  switch (target)
  {
    case Target::Application: result = "application"; break;
    case Target::HostTest: result = "host test"; break;
    default: break;
  }
  return result;
}
/// @param platform - the platform to convert to a string
/// @return a string representation of the platform.
constexpr const char * Stringify(Platform platform)
{
  const char * result = "invalid";
  switch (platform)
  {
    case Platform::lpc17xx: result = "lpc17xx"; break;
    case Platform::lpc40xx: result = "lpc40xx"; break;
    case Platform::linux: result = "linux"; break;
    default: break;
  }
  return result;
}
}  // namespace build
}  // namespace sjsu
