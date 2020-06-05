# FAT File System

The File Allocation Table file system, (also known as
[FAT](https://en.wikipedia.org/wiki/File_Allocation_Table)), is a system for
storing and retrieving files and their contents on a storage device. FAT is a
old, light weight, simple, and very well supported file system across almost all
operating systems and platforms. Because of these factors this file system is a
good choice for embedded systems.

In SJSU-Dev2, a 3rd party library developed by ChaN called
[FatFS](http://elm-chan.org/fsw/ff/00index_e.html) is used to implement the FAT
filesystem on the platform. Before you can actually start using FATFS on your
SD card, SPI Flash memory, RAM disk or other storage media, will need to be
registered. The guide below goes into details on how to do this.

!!! Warning
    The storage media must already have a FAT file system on it in order to use
    any of the FatFS APIs (with the exception of `f_mkfs()` and `f_mount()`).

!!! Warning
    This API does NOT support exFAT (a more modern and improved version of FAT),
    thus, if a storage media already has the exFAT file system on it, it will
    need to be reformatted as FAT32.

!!! Note
    SD cards are one of the easiest media to get started with as they only need
    SPI and GPIO to communicate and they can be plugged into computers to get
    formatted with the FAT file system.

## Registering a Storage Device
The following code demonstrates constructing an SD card object and registering
it as a FatFS drive. Failure to do this before using the FatFS APIs will result
in the APIs returning error, with no action performed. Program should not crash.

```C++
// Constructing the SD card object
sjsu::lpc40xx::Spi spi2(sjsu::lpc40xx::Spi::Bus::kSpi2);
sjsu::lpc40xx::Gpio sd_chip_select(1, 8);
sjsu::lpc40xx::Gpio sd_card_detect(1, 9);
sjsu::experimental::Sd card(spi2, sd_chip_select, sd_card_detect);

// Register the SD card object as a storage drive #0 (defaults to zero)
auto success = sjsu::RegisterFatFsDrive(&card);

// Check if registration was successful.
if (!success)
{
  // Handle error if this is the case...
}
```

## Registering multiple drives
In order to register more than one drive you will need to do two things.

### Step 1. Change SJ2_FAT_DRIVE_COUNT
Within your `project_config.hpp` you will need to define `SJ2_FAT_DRIVE_COUNT`
and set it to the number of storage drives you plan to support in your
application.

### Step 2. Determine a driver number
In order to use another storage device you need to supply the 2nd parameter of
`sjsu::RegisterFatFsDrive()` with the desired drive number. This value defaults
to 0, which is the default drive for ChaN's FAT implementation. Below is an
example.

```C++
// Constructing the SD card object
sjsu::experimental::Sd second_sd_card(/* ...  */);

// Register the SD card object as a storage drive #1
auto success = sjsu::RegisterFatFsDrive(&second_sd_card, 1);

// Check if registration was successful. Will fail if the register number is
// greater than the SJ2_FAT_DRIVE_COUNT value.
if (!success)
{
  // Handle error if this is the case...
}
```

!!! Note
    The maximum number of drives is 10.

## Using FAT FS

FAT FS's API is very similar to
[C's file I/O API](https://en.cppreference.com/w/c/io).
ChaN's documentation describes in detail how to use the the APIs which can be
found [here](http://elm-chan.org/fsw/ff/00index_e.html).

Examples of its usage in SJSU-Dev2 can be found in the `demos/` folder for
projects labeled `fatfs`. A few examples are

- [demos/stm32f10x/fatfs](https://github.com/SJSU-Dev2/SJSU-Dev2/tree/master/demos/stm32f10x/fatfs)
- [demos/lpc40xx/fatfs](https://github.com/SJSU-Dev2/SJSU-Dev2/tree/master/demos/sjtwo/fatfs)

The configuration options for FatFS can be found in
[library/third_party/fatfs/source/ffconf.h](https://github.com/SJSU-Dev2/SJSU-Dev2/tree/master/library/third_party/fatfs/source/ffconf.h)
Long file names are not supported to reduce code size. In the future, this may
be changed to allow configuration of this option.
