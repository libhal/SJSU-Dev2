#pragma once

#if !defined(TARGET)
#define TARGET Application
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

constexpr const Target kTarget = Target::TARGET;

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

}  // namespace build
}  // namespace sjsu
