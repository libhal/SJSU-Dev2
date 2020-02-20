#pragma once

namespace sjsu
{
namespace robotics
{
// represents type of things sent from mission control. will build this out.
struct ParamsStruct
{
  // drive mode e.g. crab, debug, spin, drive
  int drive_mode;
  // angle of a given wheel
  double wheel_angle;
  // angle of a given arm component
  double arm_angle;
  // speed of hub motor
  double wheel_speed;
  // true if wheel should run in reverse or not
  bool drive_reverse;
  // true/false depending if we are controlling wheel a
  bool wheel_a;
  // true/false depending if we are controlling wheel b
  bool wheel_b;
  // true/false depending if we are controlling wheel c
  bool wheel_c;
  // true/false depending if we are controlling wrist
  bool wrist;
  // true/false depending if we are controlling shoulder
  bool shoulder;
  // true/false depending if we are controlling rotunda
  bool rotunda;
};
}  // namespace robotics
}  // namespace sjsu
