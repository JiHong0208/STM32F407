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
#ifndef XCP_PAR_H
#define XCP_PAR_H

/* declare here parameters for customizing XcpBasic driver (e.g. kXcpStationId) */
#include "xcp_cfg.h"
#include "xcpBasic.h"

extern vuint8 XcpReceivedBuffer[8];
extern volatile vuint32 Xcp_Ctl;


extern void XcpRxHandler(const vuint8* Buf);
extern V_MEMROM0 vuint8 kXcpEventDirection[];
extern V_MEMROM0 vuint8 kXcpEventNameLength[];
extern V_MEMROM0 vuint8 kXcpEventCycle[];
extern V_MEMROM0 vuint8 kXcpEventUnit[];
extern V_MEMROM0 XcpCharType* kXcpEventName[];

#endif
