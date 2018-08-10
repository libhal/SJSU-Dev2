/*
 * FreeRTOS+FAT Labs Build 160111 (C) 2016 Real Time Engineers ltd.
 * Authors include James Walmsley, Hein Tibosch and Richard Barry
 *
 *******************************************************************************
 ***** NOTE ******* NOTE ******* NOTE ******* NOTE ******* NOTE ******* NOTE ***
 ***                                                                         ***
 ***                                                                         ***
 ***   FREERTOS+FAT IS STILL IN THE LAB:                                     ***
 ***                                                                         ***
 ***   This product is functional and is already being used in commercial    ***
 ***   products.  Be aware however that we are still refining its design,    ***
 ***   the source code does not yet fully conform to the strict coding and   ***
 ***   style standards mandated by Real Time Engineers ltd., and the         ***
 ***   documentation and testing is not necessarily complete.                ***
 ***                                                                         ***
 ***   PLEASE REPORT EXPERIENCES USING THE SUPPORT RESOURCES FOUND ON THE    ***
 ***   URL: http://www.FreeRTOS.org/contact  Active early adopters may, at   ***
 ***   the sole discretion of Real Time Engineers Ltd., be offered versions  ***
 ***   under a license other than that described below.                      ***
 ***                                                                         ***
 ***                                                                         ***
 ***** NOTE ******* NOTE ******* NOTE ******* NOTE ******* NOTE ******* NOTE ***
 *******************************************************************************
 *
 * FreeRTOS+FAT can be used under two different free open source licenses.  The
 * license that applies is dependent on the processor on which FreeRTOS+FAT is
 * executed, as follows:
 *
 * If FreeRTOS+FAT is executed on one of the processors listed under the Special
 * License Arrangements heading of the FreeRTOS+FAT license information web
 * page, then it can be used under the terms of the FreeRTOS Open Source
 * License.  If FreeRTOS+FAT is used on any other processor, then it can be used
 * under the terms of the GNU General Public License V2.  Links to the relevant
 * licenses follow:
 *
 * The FreeRTOS+FAT License Information Page: http://www.FreeRTOS.org/fat_license
 * The FreeRTOS Open Source License: http://www.FreeRTOS.org/license
 * The GNU General Public License Version 2: http://www.FreeRTOS.org/gpl-2.0.txt
 *
 * FreeRTOS+FAT is distributed in the hope that it will be useful.  You cannot
 * use FreeRTOS+FAT unless you agree that you use the software 'as is'.
 * FreeRTOS+FAT is provided WITHOUT ANY WARRANTY; without even the implied
 * warranties of NON-INFRINGEMENT, MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE. Real Time Engineers Ltd. disclaims all conditions and terms, be they
 * implied, expressed, or statutory.
 *
 * 1 tab == 4 spaces!
 *
 * http://www.FreeRTOS.org
 * http://www.FreeRTOS.org/plus
 * http://www.FreeRTOS.org/labs
 *
 */

#ifndef _FF_CONFIG_H_
#define _FF_CONFIG_H_

/* Must be set to either pdFREERTOS_LITTLE_ENDIAN or pdFREERTOS_BIG_ENDIAN,
depending on the endian of the architecture on which FreeRTOS is running. */
#define ffconfigBYTE_ORDER pdFREERTOS_LITTLE_ENDIAN

/* Set to 1 to maintain a current working directory (CWD) for each task that
accesses the file system, allowing relative paths to be used.

Set to 0 not to use a CWD, in which case full paths must be used for each
file access. */
#define ffconfigHAS_CWD 1

/* Set to an index within FreeRTOS's thread local storage array that is free for
use by FreeRTOS+FAT.  FreeRTOS+FAT will use two consecutive indexes from this
that set by ffconfigCWD_THREAD_LOCAL_INDEX.  The number of thread local storage
pointers provided by FreeRTOS is set by configNUM_THREAD_LOCAL_STORAGE_POINTERS
in FreeRTOSConfig.h */
#define ffconfigCWD_THREAD_LOCAL_INDEX 0

/* Set to 1 to include long file name support.  Set to 0 to exclude long
file name support.

If long file name support is excluded then only 8.3 file names can be used.
Long file names will be recognised but ignored.

Users should familiarise themselves with any patent issues that may
potentially exist around the use of long file names in FAT file systems
before enabling long file name support. */
#define ffconfigLFN_SUPPORT 1

/* Only used when ffconfigLFN_SUPPORT is set to 1.

Set to 1 to include a file's short name when listing a directory, i.e. when
calling findfirst()/findnext().  The short name will be stored in the
'pcShortName' field of FF_DirEnt_t.

Set to 0 to only include a file's long name. */
#define ffconfigINCLUDE_SHORT_NAME 0

/* Set to 1 to recognise and apply the case bits used by Windows XP+ when
using short file names - storing file names such as "readme.TXT" or
"SETUP.exe" in a short-name entry.  This is the recommended setting for
maximum compatibility.

Set to 0 to ignore the case bits. */
#define ffconfigSHORTNAME_CASE 1

/* Only used when ffconfigLFN_SUPPORT is set to 1.

Set to 1 to use UTF-16 (wide-characters) for file and directory names.

Set to 0 to use either 8-bit ASCII or UTF-8 for file and directory names
(see the ffconfigUNICODE_UTF8_SUPPORT). */
#define ffconfigUNICODE_UTF16_SUPPORT 0

/* Only used when ffconfigLFN_SUPPORT is set to 1.

Set to 1 to use UTF-8 encoding for file and directory names.

Set to 0 to use either 8-bit ASCII or UTF-16 for file and directory
names (see the ffconfig_UTF_16_SUPPORT setting). */
#define	ffconfigUNICODE_UTF8_SUPPORT 0

/* Set to 1 to include FAT12 support.

Set to 0 to exclude FAT12 support.

FAT16 and FAT32 are always enabled. */
#define	ffconfigFAT12_SUPPORT 0

/* When writing and reading data, i/o becomes less efficient if sizes other
than 512 bytes are being used.  When set to 1 each file handle will
allocate a 512-byte character buffer to facilitate "unaligned access". */
#define	ffconfigOPTIMISE_UNALIGNED_ACCESS	1

/* Input and output to a disk uses buffers that are only flushed at the
following times:

- When a new buffer is needed and no other buffers are available.
- When opening a buffer in READ mode for a sector that has just been changed.
- After creating, removing or closing a file or a directory.

Normally this is quick enough and it is efficient.  If
ffconfigCACHE_WRITE_THROUGH is set to 1 then buffers will also be flushed each
time a buffer is released - which is less efficient but more secure. */
#define	ffconfigCACHE_WRITE_THROUGH	1

/* In most cases, the FAT table has two identical copies on the disk,
allowing the second copy to be used in the case of a read error.  If

Set to 1 to use both FATs - this is less efficient but more	secure.

Set to 0 to use only one FAT - the second FAT will never be written to. */
#define	ffconfigWRITE_BOTH_FATS	1

/* Set to 1 to have the number of free clusters and the first free cluster
to be written to the FS info sector each time one of those values changes.

Set to 0 not to store these values in the FS info sector, making booting
slower, but making changes faster. */
#define	ffconfigWRITE_FREE_COUNT 1

/* Set to 1 to maintain file and directory time stamps for creation, modify
and last access.

Set to 0 to exclude	time stamps.

If time support is used, the following function must be supplied:

	time_t FreeRTOS_time( time_t *pxTime );

FreeRTOS_time has the same semantics as the standard time() function. */
#define	ffconfigTIME_SUPPORT 1

/* Set to 1 if the media is removable (such as a memory card).

Set to 0 if the media is not removable.

When set to 1 all file handles will be "invalidated" if the media is
extracted.  If set to 0 then file handles will not be invalidated.
In that case the user will have to confirm that the media is still present
before every access. */
#define	ffconfigREMOVABLE_MEDIA	1

/* Set to 1 to determine the disk's free space and the disk's first free
cluster when a disk is mounted.

Set to 0 to find these two values when they	are first needed.  Determining
the values can take some time. */
#define	ffconfigMOUNT_FIND_FREE	1

/* Set to 1 to 'trust' the contents of the 'ulLastFreeCluster' and
ulFreeClusterCount fields.

Set to 0 not to 'trust' these fields.*/
#define	ffconfigFSINFO_TRUSTED 1

/* Set to 1 to store recent paths in a cache, enabling much faster access
when the path is deep within a directory structure at the expense of
additional RAM usage.

//Set to 0 to not use a path cache. */
#define	ffconfigPATH_CACHE 0

/* Only used if ffconfigPATH_CACHE is 1.

Sets the maximum number of paths that can exist in the patch cache at any
one time. */
#define	ffconfigPATH_CACHE_DEPTH 8

/* Set to 1 to calculate a HASH value for each existing short file name.
Use of HASH values can improve performance when working with large
directories, or with files that have a similar name.

Set to 0 not to calculate a HASH value. */
#define	ffconfigHASH_CACHE	1

/* Only used if ffconfigHASH_CACHE is set to 1

Set to CRC8 or CRC16 to use 8-bit or 16-bit HASH values respectively. */
#define	ffconfigHASH_FUNCTION CRC16

/*_RB_ Not in FreeRTOSFFConfigDefaults.h. */
#define ffconfigHASH_CACHE_DEPTH 64

/* Set to 1 to add a parameter to ff_mkdir() that allows an entire directory
tree to be created in one go, rather than having to create one directory in
the tree at a time.  For example mkdir( "/etc/settings/network", pdTRUE );.

Set to 0 to use the normal mkdir() semantics (without the additional
parameter). */
#define	ffconfigMKDIR_RECURSIVE	 0

/* Set to a function that will be used for all dynamic memory allocations.
Setting to pvPortMalloc() will use the same memory allocator as FreeRTOS. */
#define ffconfigMALLOC( size )	pvPortMalloc( size )

/* Set to a function that matches the above allocator defined with
ffconfigMALLOC.  Setting to vPortFree() will use the same memory free
function as	FreeRTOS. */
#define ffconfigFREE( ptr )  vPortFree( ptr )

/* Set to 1 to calculate the free size and volume size as a 64-bit number.

Set to 0 to calculate these values as a 32-bit number. */
#define	ffconfig64_NUM_SUPPORT	0

/* Defines the maximum number of partitions (and also logical partitions)
that can be recognised. */
#define	ffconfigMAX_PARTITIONS 1

/* Defines how many drives can be combined in total.  Should be set to at
least 2. */
#define	ffconfigMAX_FILE_SYS 2

/* In case the low-level driver returns an error 'FF_ERR_DRIVER_BUSY',
the library will pause for a number of ms, defined in
ffconfigDRIVER_BUSY_SLEEP_MS before re-trying. */
#define	ffconfigDRIVER_BUSY_SLEEP_MS 20

/* Set to 1 to include the ff_fprintf() function.

Set to 0 to exclude the ff_fprintf() function.

ff_fprintf() is quite a heavy function because it allocates RAM and
brings in a lot of string and variable argument handling code.  If
ff_fprintf() is not being used then the code size can be reduced by setting
ffconfigFPRINTF_SUPPORT to 0. */
#define ffconfigFPRINTF_SUPPORT	1

/* ff_fprintf() will allocate a buffer of this size in which it will create
its formatted string.  The buffer will be freed before the function
exits. */
#define ffconfigFPRINTF_BUFFER_LENGTH 128

/* Set to 1 to inline some internal memory access functions.

Set to 0 to not inline the memory access functions. */
#define	ffconfigINLINE_MEMORY_ACCESS		1

/* Officially the only criteria to determine the FAT type (12, 16, or 32
bits) is the total number of clusters:
if( ulNumberOfClusters  <  4085 ) : Volume is FAT12
if( ulNumberOfClusters  < 65525 ) : Volume is FAT16
if( ulNumberOfClusters >= 65525 ) : Volume is FAT32
Not every formatted device follows the above rule.

Set to 1 to perform additional checks over and above inspecting the
number of clusters on a disk to determine the FAT type.

Set to 0 to only look at the number of clusters on a disk to determine the
FAT type. */
#define	ffconfigFAT_CHECK 1

/* Sets the maximum length for file names, including the path.
Note that the value of this define is directly related to the maximum stack
use of the +FAT library. In some API's, a character buffer of size
'ffconfigMAX_FILENAME' will be declared on stack. */
#define	ffconfigMAX_FILENAME 250

/* Include the recursive function ff_deltree().  The use of recursion does not
conform with the coding standard, so use this function with care! */
#define ffconfigUSE_DELTREE					1

#endif /* _FF_CONFIG_H_ */

