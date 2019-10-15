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
enum class Target
{
  Application = 0,  // NOLINT
  HostTest,         // NOLINT
};

enum class Platform
{
  lpc17xx,  // NOLINT
  lpc40xx,  // NOLINT
  linux,    // NOLINT
};

constexpr const Target kTarget     = Target::TARGET;
constexpr const Platform kPlatform = Platform::PLATFORM;

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
