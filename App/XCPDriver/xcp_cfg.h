/*----------------------------------------------------------------------------
| Project: XCP_Delivery
|
|  Description:   Implementation of the XCP Protocol Layer
|                 XCP V1.0 slave device driver
|                 Basic Version
|
|-----------------------------------------------------------------------------
| Copyright (c) by Vector Informatik GmbH.  All rights reserved.
----------------------------------------------------------------------------*/

#if !defined(XCP_CFG_H)
#define XCP_CFG_H

/* define here configuration parameters for customizing XcpBasic driver */
#if 0
#else
#ifndef vuint8
typedef unsigned char vuint8;
#endif

#ifndef vuint16
typedef unsigned short int vuint16;
#endif

#ifndef vuint32
typedef unsigned int vuint32;
#endif
#endif
#ifndef XcpCharType
typedef unsigned char XcpCharType;
#endif


#define CAL_RAMPAGEADDR                 0x00000000u
#define CAL_REFPAGEADDR                 0x00000000u

#define XCP_CALIBRATION_FLASH_START     0x08FD0000
#define XCP_CALIBRATION_FLASH_SIZE      (64u * 1024u)
#define XCP_CALIBRATION_WRITEPAGE_SIZE  32u

#define kXcpMaxCTO                      8u
#define kXcpMaxDTO                      8u
#define XCP_TRANSPORT_LAYER_VERSION     0x0100
#define XCP_ENABLE_CALIBRATION
#define XCP_ENABLE_PARAMETER_CHECK
//#define XCP_ENABLE_SEED_KEY
#define XCP_ENABLE_GET_ID_GENERIC

#define XCP_DISABLE_CHECKSUM
#define XCP_ENABLE_CUSTOM_CRC

//#define XCP_ENABLE_PAGE_COPY
#define V_ENABLE_USE_DUMMY_STATEMENT

#define XCP_CPUTYPE_BIGENDIAN
#define V_MEMROM0
#if 1
#define XCP_ENABLE_DAQ
#define kXcpDaqMemSize  2048u
#define XCP_DISABLE_DAQ_HDR_ODT_DAQ
#define XCP_ENABLE_DAQ_PROCESSOR_INFO
#define kXcpMaxEvent    2u
#define XCP_ENABLE_DAQ_RESOLUTION_INFO
#define XCP_DISABLE_DAQ_EVENT_INFO
#define XCP_DISABLE_PROGRAM
#define XCP_ENABLE_PROGRAMMING_WRITE_PROTECTION
#else
#define XCP_DISABLE_DAQ
#endif

#define XCP_DISABLE_SEND_QUEUE

#if 1
#define XCP_ENABLE_COMM_MODE_INFO
#else
#define XCP_DISABLE_COMM_MODE_INFO
#endif

#define XCP_DISABLE_SERV_TEXT
#define XCP_DISABLE_SERV_TEXT_PUTCHAR
#define XCP_DISABLE_SERV_TEXT_PRINT

#if 1
#define XCP_ENABLE_CALIBRATION_PAGE
#else
#define XCP_DISABLE_CALIBRATION_PAGE
#endif

#define XCP_DISABLE_UNALIGNED_MEM_ACCESS

#if !defined(MEMORY_ROM)
#define MEMORY_ROM const
#endif


#if defined ( XCP_ENABLE_PROGRAM )
extern vuint8 ApplXcpProgramStart(void);
#if defined ( XCP_ENABLE_PROGRAMMING_WRITE_PROTECTION )
extern vuint8 ApplXcpCheckProgramAccess(vuint8* address, vuint32 size);
#endif
extern vuint8 ApplXcpFlashClear(vuint8* address, vuint32 size);
extern vuint8 ApplXcpFlashProgram(const vuint8* data, vuint8* address, vuint8 size);
extern void ApplXcpReset(void);
#endif

#endif /* __XCP_CFG_H__ */
