Common Guidelines across Levels
================================

.. toctree::
  :maxdepth: 5

  levels/L0-SystemControllers-porting-guide.rst
  levels/L1-Driver-guide.rst
  levels/L2-Hal-guide.rst
  levels/L3-Application-guide.rst

Sphere of inclusion
--------------------
All source files, regardless of level, can include files from other files on
their same level, sources within utility, and 1 level below themselves.

Explanation
++++++++++++
This approach is meant to keep a levels of isolation between the categories of files. For example, lets take L1_Drivers

L1_Drivers contains platform specific peripheral drivers. Typically these
drivers need to understand the memory map of the platform they are on, thus
they must include files from L0_LowLevel. A driver may need to utilize other
drivers within the L1_Drivers directory, thus inclusion across files within L1_Drivers is permitted.

L1_Drivers, under any normal circumstances, should NOT include files above
Level 1, as this could result in circular dependencies and/or coupling between
L1 and higher level files.

For L2_HAL, for which H.A.L. stands for H.ardware A.bstraction L.ayer, these modules should only be including across the L2 directory or at the L1 level. The HAL layer for SJSU-Dev2 should be agnostic to how peripherals in the L1 directory are implemented and should not need to include the L0 files.

This continues for all levels.
