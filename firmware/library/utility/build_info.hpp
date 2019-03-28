#pragma once

#if !defined(TARGET)
#define TARGET Application
#endif

namespace build
{

enum class Target
{
  Bootloader = 0,  // NOLINT
  Application,  // NOLINT
  HostTest,  // NOLINT
};

constexpr const Target kTarget = Target::TARGET;

constexpr const char * Stringify(Target target)
{
  const char * result = "";
  switch (target)
  {
    case Target::Bootloader:
      result = "bootloader";
      break;
    case Target::Application:
      result = "application";
      break;
    case Target::HostTest:
      result = "host test";
      break;
    default:
      break;
  }
  return result;
}

}  // namespace build

