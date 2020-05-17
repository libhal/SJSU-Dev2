# Common Guidelines across Levels

## Hierarchy of inclusion

![Inclusion Hierarchy](levels/img/inclusion-hierarchy.png)

### Explanation

Arrows indicate which levels can include from. For example, `L2_HAL` can include
`L1_Peripherals`, and `L3_Application` can include `L2_HAL`.

`L3_Applications` should refrain from including `L1_Peripherals`. If an
`L3_Application` needs to include from `L1_Peripherals`, it must only take the
peripheral interface and not a specific platforms driver. `L0_Platform` can
include everything, except testing libraries, as its startup may need code from
any of the levels. In order to hit the **light weight** goal, keep the code in
startup to an absolute minimum.

Everything can include from `utility/` and `third_party/`. Although, some third
party libraries should not be included in some library code levels. For example,
microrl, used for command line control, should not be included in anything in
`L1_Peripherals` or even `L2_HAL`. There may be consideration in the future to
separate the third party libraries into their own set of levels that match the
library levels.

This approach is meant to keep a levels of isolation between the categories of
files. Doing this can keep from circular dependencies and unreasonable coupling
between the layers.

Utilities are meant to be usable on all levels.
