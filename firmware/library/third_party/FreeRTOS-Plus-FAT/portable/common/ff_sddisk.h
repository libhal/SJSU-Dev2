/*
 * FreeRTOS+FAT Labs Build 160919 (C) 2016 Real Time Engineers ltd.
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

#ifndef __SDDISK_H__

#define __SDDISK_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "ff_headers.h"


/* Return non-zero if the SD-card is present.
The parameter 'pxDisk' may be null, unless device locking is necessary. */
BaseType_t FF_SDDiskDetect( FF_Disk_t *pxDisk );

/* Create a RAM disk, supplying enough memory to hold N sectors of 512 bytes each */
FF_Disk_t *FF_SDDiskInit( const char *pcName );

BaseType_t FF_SDDiskReinit( FF_Disk_t *pxDisk );

/* Unmount the volume */
BaseType_t FF_SDDiskUnmount( FF_Disk_t *pDisk );

/* Mount the volume */
BaseType_t FF_SDDiskMount( FF_Disk_t *pDisk );

/* Release all resources */
BaseType_t FF_SDDiskDelete( FF_Disk_t *pDisk );

/* Show some partition information */
BaseType_t FF_SDDiskShowPartition( FF_Disk_t *pDisk );

/* Flush changes from the driver's buf to disk */
void FF_SDDiskFlush( FF_Disk_t *pDisk );

/* Format a given partition on an SD-card. */
BaseType_t FF_SDDiskFormat( FF_Disk_t *pxDisk, BaseType_t aPart );

/* Return non-zero if an SD-card is detected in a given slot. */
BaseType_t FF_SDDiskInserted( BaseType_t xDriveNr );

/* _RB_ Temporary function - ideally the application would not need the IO
manageer structure, just a handle to a disk. */
FF_IOManager_t *sddisk_ioman( FF_Disk_t *pxDisk );

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __SDDISK_H__ */
