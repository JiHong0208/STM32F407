#include <stdio.h>
#include "bsp_can.h"
#include "xcpBasic.h"
#include "stdlib.h"  // �������������


extern CanTxMsg TxMessage;			     //ͨ��CAN���ͳ�ȥ��ģ���ѹ����

/*
 * ��������CAN_GPIO_Config
 * ����  ��CAN��GPIO ����
 * ����  ����
 * ���  : ��
 * ����  ���ڲ�����
 */
static void CAN_GPIO_Config(void)
{
 	GPIO_InitTypeDef GPIO_InitStructure;
   	

  /* Enable GPIO clock */
  RCC_AHB1PeriphClockCmd(CAN_TX_GPIO_CLK|CAN_RX_GPIO_CLK, ENABLE);
	
	  /* Connect CAN pins to AF9 */
  GPIO_PinAFConfig(CAN_TX_GPIO_PORT, CAN_RX_SOURCE, CAN_AF_PORT);
  GPIO_PinAFConfig(CAN_RX_GPIO_PORT, CAN_TX_SOURCE, CAN_AF_PORT); 

	  /* Configure CAN TX pins */
  GPIO_InitStructure.GPIO_Pin = CAN_TX_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
  GPIO_Init(CAN_TX_GPIO_PORT, &GPIO_InitStructure);
	
	/* Configure CAN RX  pins */
  GPIO_InitStructure.GPIO_Pin = CAN_RX_PIN ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_Init(CAN_RX_GPIO_PORT, &GPIO_InitStructure);


}

/*
 * ��������CAN_NVIC_Config
 * ����  ��CAN��NVIC ����,��1���ȼ��飬0��0���ȼ�
 * ����  ����
 * ���  : ��
 * ����  ���ڲ�����
 */
static void CAN_NVIC_Config(void)
{
   	NVIC_InitTypeDef NVIC_InitStructure;
	/* Configure one bit for preemption priority */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	/*�ж�����*/
	NVIC_InitStructure.NVIC_IRQChannel = CAN_RX_IRQ;	   //CAN RX0�ж�
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;		   //��ռ���ȼ�0
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;			   //�����ȼ�Ϊ0
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/*
 * ��������CAN_Mode_Config
 * ����  ��CAN��ģʽ ����
 * ����  ����
 * ���  : ��
 * ����  ���ڲ�����
 */
static void CAN_Mode_Config(void)
{
	CAN_InitTypeDef        CAN_InitStructure;
	/************************CANͨ�Ų�������**********************************/
	/* Enable CAN clock */
  RCC_APB1PeriphClockCmd(CAN_CLK, ENABLE);

	/*CAN�Ĵ�����ʼ��*/
	CAN_DeInit(CANx);
	CAN_StructInit(&CAN_InitStructure);

	/*CAN��Ԫ��ʼ��*/
	CAN_InitStructure.CAN_TTCM=DISABLE;			   //MCR-TTCM  �ر�ʱ�䴥��ͨ��ģʽʹ��
	CAN_InitStructure.CAN_ABOM=ENABLE;			   //MCR-ABOM  �Զ����߹��� 
	CAN_InitStructure.CAN_AWUM=ENABLE;			   //MCR-AWUM  ʹ���Զ�����ģʽ
	CAN_InitStructure.CAN_NART=DISABLE;			   //MCR-NART  ��ֹ�����Զ��ش�	  DISABLE-�Զ��ش�
	CAN_InitStructure.CAN_RFLM=DISABLE;			   //MCR-RFLM  ����FIFO ����ģʽ  DISABLE-���ʱ�±��ĻḲ��ԭ�б���  
	CAN_InitStructure.CAN_TXFP=DISABLE;			   //MCR-TXFP  ����FIFO���ȼ� DISABLE-���ȼ�ȡ���ڱ��ı�ʾ�� 
	CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;  //��������ģʽ
	CAN_InitStructure.CAN_SJW=CAN_SJW_2tq;		   //BTR-SJW ����ͬ����Ծ��� 2��ʱ�䵥Ԫ
	 
	/* ss=1 bs1=4 bs2=2 λʱ����Ϊ(1+4+2) �����ʼ�Ϊʱ������tq*(1+4+2)  */
	CAN_InitStructure.CAN_BS1=CAN_BS1_4tq;		   //BTR-TS1 ʱ���1 ռ����4��ʱ�䵥Ԫ
	CAN_InitStructure.CAN_BS2=CAN_BS2_2tq;		   //BTR-TS1 ʱ���2 ռ����2��ʱ�䵥Ԫ	
	
	/* CAN Baudrate = 1 MBps (1MBps��Ϊstm32��CAN�������) (CAN ʱ��Ƶ��Ϊ APB 1 = 42 MHz) */
	CAN_InitStructure.CAN_Prescaler =6;		   ////BTR-BRP �����ʷ�Ƶ��  ������ʱ�䵥Ԫ��ʱ�䳤�� 42/(1+4+2)/6=1 Mbps
	CAN_Init(CANx, &CAN_InitStructure);
}

/*
 * ��������CAN_Filter_Config
 * ����  ��CAN�Ĺ����� ����
 * ����  ����
 * ���  : ��
 * ����  ���ڲ�����
 */
static void CAN_Filter_Config(void)
{
	CAN_FilterInitTypeDef  CAN_FilterInitStructure;

	/*CANɸѡ����ʼ��*/
	CAN_FilterInitStructure.CAN_FilterNumber=14;						//ɸѡ����14
	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask;	//����������ģʽ
	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit;	//ɸѡ��λ��Ϊ����32λ��
	/* ʹ��ɸѡ�������ձ�־�����ݽ��бȶ�ɸѡ����չID�������µľ����������ǵĻ��������FIFO0�� */

	CAN_FilterInitStructure.CAN_FilterIdHigh = ((((uint32_t)0x1314u << 3) | ((uint32_t)0x00000004u) | ((uint32_t)0x00000000u)) & 0xFFFF0000u) >> 16;//Ҫɸѡ��ID��λ 
	CAN_FilterInitStructure.CAN_FilterIdLow = ((((uint32_t)0x1314u << 3) | (uint32_t)CAN_ID_EXT | (uint32_t)CAN_RTR_DATA) & 0xFFFFu);//Ҫɸѡ��ID��λ 
//	CAN_FilterInitStructure.CAN_FilterMaskIdHigh= 0xFFFF;			//ɸѡ����16λÿλ����ƥ��
//	CAN_FilterInitStructure.CAN_FilterMaskIdLow= 0xFFFF;	
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh= 0x0000;			//ɸѡ����16λÿλ����ƥ��
	CAN_FilterInitStructure.CAN_FilterMaskIdLow= 0x0000;		    //ɸѡ����16λÿλ����ƥ��
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0 ;				//ɸѡ����������FIFO0
	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;			//ʹ��ɸѡ��
	CAN_FilterInit(&CAN_FilterInitStructure);
	/*CANͨ���ж�ʹ��*/
	CAN_ITConfig(CANx, CAN_IT_FMP0, ENABLE);
}


/*
 * ��������CAN_Config
 * ����  ����������CAN�Ĺ���
 * ����  ����
 * ���  : ��
 * ����  ���ⲿ����
 */
void CAN_Config(void)
{
  CAN_GPIO_Config();
  CAN_NVIC_Config();
  CAN_Mode_Config();
  CAN_Filter_Config();   
}


/**
  * @brief  ��ʼ�� Rx Message���ݽṹ��
  * @param  RxMessage: ָ��Ҫ��ʼ�������ݽṹ��
  * @retval None
  */
void Init_RxMes(CanRxMsg *RxMessage)
{
  uint8_t ubCounter = 0;

	/*�ѽ��սṹ������*/
  RxMessage->StdId = 0x00;
  RxMessage->ExtId = 0x00;
  RxMessage->IDE = CAN_ID_STD;
  RxMessage->DLC = 0;
  RxMessage->FMI = 0;
  for (ubCounter = 0; ubCounter < 8; ubCounter++)
  {
    RxMessage->Data[ubCounter] = 0x00;
  }
}

/**
  * @brief  ��16λ��ѹֵת��Ϊ�����ֽڣ����ֽ���ǰ�����ֽ��ں�
  * @param  voltage: 16λ��ѹֵ
  * @param  lowByte: ���ֽ�
  * @param  highByte: ���ֽ�
  * @retval ��
  */
void VoltageToBytes(uint16_t voltage, uint8_t *lowByte, uint8_t *highByte)
{
    *lowByte = voltage & 0xFF;          // ȡ��8λ
    *highByte = (voltage >> 8) & 0xFF;  // ȡ��8λ
}

/*
 * ��������CAN_SetMsg
 * ����  ��CANͨ�ű�����������,����һ����������Ϊ0-7�����ݰ�
 * ����  �����ͱ��Ľṹ��
 * ���  : ��
 * ����  ���ⲿ����
 */	 
void CAN_SetMsg(CanTxMsg *TxMessage)
{	  
	uint8_t ubCounter = 0;
	
    uint8_t data[8];
	
	// �������������ѹֵ��������15000��
    uint16_t voltages[4];
    for (ubCounter = 0; ubCounter < 4; ubCounter++)
    {
        voltages[ubCounter] = rand() % 15001;  // ����0-15000�������ѹֵ
    }

    // �������ѹֵת��ΪCAN��������
    for (ubCounter = 0; ubCounter < 4; ubCounter++)
    {
        VoltageToBytes(voltages[ubCounter], &data[ubCounter * 2], &data[ubCounter * 2 + 1]);
    }
	
    //TxMessage.StdId=0x00;						 
    TxMessage->ExtId = 0x520;					  //ʹ�õ���չID����ID����Ϊ0x520
    TxMessage->IDE = CAN_ID_EXT;				  //��չģʽ
    TxMessage->RTR = CAN_RTR_DATA;			      //���͵�������
    TxMessage->DLC = 8;						      //���ݳ���Ϊ8�ֽ�
	
	/*�����úõ����ݷ��ͳ�ȥ*/
	for (ubCounter = 0; ubCounter < 8; ubCounter++)
	{
		TxMessage->Data[ubCounter] = data[ubCounter];
	}
}

/**********************************************�����CAN���Ľ�������**********************************************/
// ����ȫ�־�̬�������ڴ洢��ѹֵ
static uint16_t g_voltage1 = 0;
static uint16_t g_voltage2 = 0;
static uint16_t g_voltage3 = 0;
static uint16_t g_voltage4 = 0;

/**
  * @brief  ����CAN���ģ�����ȫ�ֵ�ѹ����
  * @param  data: ָ����յ���8�ֽ�CAN���ݵ�ָ��
  * @retval ��
  */
void Process_CAN_Voltage(uint8_t* data)
{
    // ���������еĵ�ѹֵ��ÿ�����ֽڵ���ת��
	g_voltage1 = (uint16_t)(((uint16_t)data[1] << 8) | (uint16_t)data[0]);
	g_voltage2 = (uint16_t)(((uint16_t)data[3] << 8) | (uint16_t)data[2]);
	g_voltage3 = (uint16_t)(((uint16_t)data[5] << 8) | (uint16_t)data[4]);
	g_voltage4 = (uint16_t)(((uint16_t)data[7] << 8) | (uint16_t)data[6]);


}

/**
  * @brief  ��ȡ������ĵ�ѹֵ
  * @param  voltageIndex: ��ѹ��ţ�1~4��
  * @retval ��Ӧ��ѹֵ����λmV
  */
uint16_t Get_CAN_Voltage(uint8_t voltageIndex)
{
    uint16_t voltage = 0;

    switch (voltageIndex)
    {
        case 1:
            voltage = g_voltage1;
            break;
        case 2:
            voltage = g_voltage2;
            break;
        case 3:
            voltage = g_voltage3;
            break;
        case 4:
            voltage = g_voltage4;
            break;
        default:
            voltage = 0; // ����������0��ʾ��Ч��ѹ
            break;
    }

    return voltage;
}

/**
  * @brief  ��Ƭ����������λ��PCAN����������ģ��ڻ�����ʱ���ж��п������÷�������
  */
void SendCANEvent(void)
{
	CAN_SetMsg(&TxMessage);
	CAN_Transmit(CANx, &TxMessage);
	
	//�Ƿ�CAN������Ϣ��ӡ��������
	#if 1
	CAN_DEBUG_ARRAY(TxMessage.Data,8); 
	#endif
	
	// ���� CAN ���ݣ�������ѹֵ
    Process_CAN_Voltage(TxMessage.Data);
    
}
/**************************END OF FILE************************************/











