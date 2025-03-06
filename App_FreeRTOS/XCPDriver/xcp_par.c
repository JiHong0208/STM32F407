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
#include <string.h>
#include "bsp_can.h"
#include "xcpBasic.h"
/* implement here parameters for customizing XcpBasic driver (e.g. kXcpStationId) */

V_MEMROM0 vuint8 kXcpEventDirection[] = { (vuint8) DAQ_EVENT_DIRECTION_DAQ,
                                          (vuint8) DAQ_EVENT_DIRECTION_DAQ, (vuint8) DAQ_EVENT_DIRECTION_DAQ
                                        };
V_MEMROM0 vuint8 kXcpEventNameLength[] = { (vuint8)5, (vuint8)5, (vuint8)5 };
V_MEMROM0 vuint8 kXcpEventCycle[] = { (vuint8)1, (vuint8)2, (vuint8)5 };
V_MEMROM0 vuint8 kXcpEventUnit[] = { (vuint8) DAQ_TIMESTAMP_UNIT_10MS,
                                     (vuint8) DAQ_TIMESTAMP_UNIT_10MS, (vuint8) DAQ_TIMESTAMP_UNIT_10MS
                                   };

V_MEMROM0 XcpCharType kXcpEventName_0[] = "10_ms";
V_MEMROM0 XcpCharType kXcpEventName_1[] = "20_ms";
V_MEMROM0 XcpCharType kXcpEventName_2[] = "50_ms";

V_MEMROM0 XcpCharType* kXcpEventName[] = { &kXcpEventName_0[0],
                                           &kXcpEventName_1[0], &kXcpEventName_2[0]
                                         };

vuint8 XcpReceivedBuffer[8];

void XcpRxHandler(const vuint8* Buf)
{
    XcpMemCpy(XcpReceivedBuffer, Buf, 8u);
    XcpCommand((const vuint32*)XcpReceivedBuffer);
}



void ApplXcpSend(vuint8 len, /*MEMORY_ROM*/ BYTEPTR msg)
{
    CanTxMsg TxMessage;

    // ���ݳ��ȼ�飬����Ϊ CAN ���֡���ȣ�8 �ֽڣ�
    if (len > 8) len = 8;

    // ��� CAN ���ͽṹ��
    TxMessage.StdId = 0;                  // ���ʹ�ñ�׼ ID����������Ҫ��ֵ
    TxMessage.ExtId = 0x5678;             // ʹ����չ ID
    TxMessage.RTR = CAN_RTR_DATA;         // ����֡����
    TxMessage.IDE = CAN_ID_EXT;           // ��չ ID ����
    TxMessage.DLC = len;                  // ����֡����
    memcpy(TxMessage.Data, msg, len);     // �����ݿ��������ͻ�����

    // ���� CAN ����
    if (CAN_Transmit(CAN2, &TxMessage) == CAN_TxStatus_NoMailBox)
    {
		
    }
}


/*****************************************************************************/
/*if ApplXcpSend() not OK, you need to call this function to send can message*/
void ApplXcpSendFlush(void)
{
}

void ApplXcpInterruptDisable(void)
{
    
}

void ApplXcpInterruptEnable(void)
{
    
}

