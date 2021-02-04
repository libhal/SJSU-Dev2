// DO NOT MODIFY THIS PROJECT OR FILE!
// Copy this folder and modify that copied project folder.

#include "platforms/utility/startup.hpp"

namespace sjsu
{
/// This instance of `InitializePlatform()` created in `startup.cpp` is a "weak"
/// instance, meaning it can be completely swapped out with another "strong"
/// declaration of the function. The purpose of defining it here is to define a
/// version that isn't coupled to anything. Normally `startup.cpp` defines
/// global objects that are used in `InitializePlatform()`, but with this
/// definition, those objects are not referenced anymore by any part of the
/// code, and are thus garbage collected at compile time by the compiler.
void InitializePlatform() {}
}  // namespace sjsu

int main()
{
  // NOTE: This project is meant for advanced users and NOT for typical
  //       development! If you simply want to start a project, simply copy
  //       `starter` or `hello_world`.
  //
  //       Do not copy this project for usage for prototyping. This project is
  //       meant to demonstrate how to reduce the binary size of an image and
  //       decouple a project from the main libraries by using the
  //       configuration flags in `project_config.hpp` and by declaring
  //       `InitializePlatform()` as empty.
  //
  //       This project is for you if you want to develop with no coupling with
  //       the SJSU-Dev2 libraries.
  return 0;
}
