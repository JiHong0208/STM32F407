#include <stdio.h>
#include "bsp_can.h"
#include "xcpBasic.h"
#include "stdlib.h"  // 用于随机数生成


extern CanTxMsg TxMessage;			     //通过CAN发送出去的模拟电压数据

/*
 * 函数名：CAN_GPIO_Config
 * 描述  ：CAN的GPIO 配置
 * 输入  ：无
 * 输出  : 无
 * 调用  ：内部调用
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
 * 函数名：CAN_NVIC_Config
 * 描述  ：CAN的NVIC 配置,第1优先级组，0，0优先级
 * 输入  ：无
 * 输出  : 无
 * 调用  ：内部调用
 */
static void CAN_NVIC_Config(void)
{
   	NVIC_InitTypeDef NVIC_InitStructure;
	/* Configure one bit for preemption priority */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	/*中断设置*/
	NVIC_InitStructure.NVIC_IRQChannel = CAN_RX_IRQ;	   //CAN RX0中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;		   //抢占优先级0
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;			   //子优先级为0
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/*
 * 函数名：CAN_Mode_Config
 * 描述  ：CAN的模式 配置
 * 输入  ：无
 * 输出  : 无
 * 调用  ：内部调用
 */
static void CAN_Mode_Config(void)
{
	CAN_InitTypeDef        CAN_InitStructure;
	/************************CAN通信参数设置**********************************/
	/* Enable CAN clock */
  RCC_APB1PeriphClockCmd(CAN_CLK, ENABLE);

	/*CAN寄存器初始化*/
	CAN_DeInit(CANx);
	CAN_StructInit(&CAN_InitStructure);

	/*CAN单元初始化*/
	CAN_InitStructure.CAN_TTCM=DISABLE;			   //MCR-TTCM  关闭时间触发通信模式使能
	CAN_InitStructure.CAN_ABOM=ENABLE;			   //MCR-ABOM  自动离线管理 
	CAN_InitStructure.CAN_AWUM=ENABLE;			   //MCR-AWUM  使用自动唤醒模式
	CAN_InitStructure.CAN_NART=DISABLE;			   //MCR-NART  禁止报文自动重传	  DISABLE-自动重传
	CAN_InitStructure.CAN_RFLM=DISABLE;			   //MCR-RFLM  接收FIFO 锁定模式  DISABLE-溢出时新报文会覆盖原有报文  
	CAN_InitStructure.CAN_TXFP=DISABLE;			   //MCR-TXFP  发送FIFO优先级 DISABLE-优先级取决于报文标示符 
	CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;  //正常工作模式
	CAN_InitStructure.CAN_SJW=CAN_SJW_2tq;		   //BTR-SJW 重新同步跳跃宽度 2个时间单元
	 
	/* ss=1 bs1=4 bs2=2 位时间宽度为(1+4+2) 波特率即为时钟周期tq*(1+4+2)  */
	CAN_InitStructure.CAN_BS1=CAN_BS1_4tq;		   //BTR-TS1 时间段1 占用了4个时间单元
	CAN_InitStructure.CAN_BS2=CAN_BS2_2tq;		   //BTR-TS1 时间段2 占用了2个时间单元	
	
	/* CAN Baudrate = 1 MBps (1MBps已为stm32的CAN最高速率) (CAN 时钟频率为 APB 1 = 42 MHz) */
	CAN_InitStructure.CAN_Prescaler =6;		   ////BTR-BRP 波特率分频器  定义了时间单元的时间长度 42/(1+4+2)/6=1 Mbps
	CAN_Init(CANx, &CAN_InitStructure);
}

/*
 * 函数名：CAN_Filter_Config
 * 描述  ：CAN的过滤器 配置
 * 输入  ：无
 * 输出  : 无
 * 调用  ：内部调用
 */
static void CAN_Filter_Config(void)
{
	CAN_FilterInitTypeDef  CAN_FilterInitStructure;

	/*CAN筛选器初始化*/
	CAN_FilterInitStructure.CAN_FilterNumber=14;						//筛选器组14
	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask;	//工作在掩码模式
	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit;	//筛选器位宽为单个32位。
	/* 使能筛选器，按照标志的内容进行比对筛选，扩展ID不是如下的就抛弃掉，是的话，会存入FIFO0。 */

	CAN_FilterInitStructure.CAN_FilterIdHigh = ((((uint32_t)0x1314u << 3) | ((uint32_t)0x00000004u) | ((uint32_t)0x00000000u)) & 0xFFFF0000u) >> 16;//要筛选的ID高位 
	CAN_FilterInitStructure.CAN_FilterIdLow = ((((uint32_t)0x1314u << 3) | (uint32_t)CAN_ID_EXT | (uint32_t)CAN_RTR_DATA) & 0xFFFFu);//要筛选的ID低位 
//	CAN_FilterInitStructure.CAN_FilterMaskIdHigh= 0xFFFF;			//筛选器高16位每位必须匹配
//	CAN_FilterInitStructure.CAN_FilterMaskIdLow= 0xFFFF;	
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh= 0x0000;			//筛选器高16位每位必须匹配
	CAN_FilterInitStructure.CAN_FilterMaskIdLow= 0x0000;		    //筛选器低16位每位必须匹配
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0 ;				//筛选器被关联到FIFO0
	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;			//使能筛选器
	CAN_FilterInit(&CAN_FilterInitStructure);
	/*CAN通信中断使能*/
	CAN_ITConfig(CANx, CAN_IT_FMP0, ENABLE);
}


/*
 * 函数名：CAN_Config
 * 描述  ：完整配置CAN的功能
 * 输入  ：无
 * 输出  : 无
 * 调用  ：外部调用
 */
void CAN_Config(void)
{
  CAN_GPIO_Config();
  CAN_NVIC_Config();
  CAN_Mode_Config();
  CAN_Filter_Config();   
}


/**
  * @brief  初始化 Rx Message数据结构体
  * @param  RxMessage: 指向要初始化的数据结构体
  * @retval None
  */
void Init_RxMes(CanRxMsg *RxMessage)
{
  uint8_t ubCounter = 0;

	/*把接收结构体清零*/
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
  * @brief  将16位电压值转换为两个字节（低字节在前，高字节在后）
  * @param  voltage: 16位电压值
  * @param  lowByte: 低字节
  * @param  highByte: 高字节
  * @retval 无
  */
void VoltageToBytes(uint16_t voltage, uint8_t *lowByte, uint8_t *highByte)
{
    *lowByte = voltage & 0xFF;          // 取低8位
    *highByte = (voltage >> 8) & 0xFF;  // 取高8位
}

/*
 * 函数名：CAN_SetMsg
 * 描述  ：CAN通信报文内容设置,设置一个数据内容为0-7的数据包
 * 输入  ：发送报文结构体
 * 输出  : 无
 * 调用  ：外部调用
 */	 
void CAN_SetMsg(CanTxMsg *TxMessage)
{	  
	uint8_t ubCounter = 0;
	
    uint8_t data[8];
	
	// 生成四组随机电压值（不超过15000）
    uint16_t voltages[4];
    for (ubCounter = 0; ubCounter < 4; ubCounter++)
    {
        voltages[ubCounter] = rand() % 15001;  // 生成0-15000的随机电压值
    }

    // 将四组电压值转换为CAN报文数据
    for (ubCounter = 0; ubCounter < 4; ubCounter++)
    {
        VoltageToBytes(voltages[ubCounter], &data[ubCounter * 2], &data[ubCounter * 2 + 1]);
    }
	
    //TxMessage.StdId=0x00;						 
    TxMessage->ExtId = 0x520;					  //使用的扩展ID，将ID设置为0x520
    TxMessage->IDE = CAN_ID_EXT;				  //扩展模式
    TxMessage->RTR = CAN_RTR_DATA;			      //发送的是数据
    TxMessage->DLC = 8;						      //数据长度为8字节
	
	/*将设置好的数据发送出去*/
	for (ubCounter = 0; ubCounter < 8; ubCounter++)
	{
		TxMessage->Data[ubCounter] = data[ubCounter];
	}
}

/**********************************************自添加CAN报文解析函数**********************************************/
// 定义全局静态变量用于存储电压值
static uint16_t g_voltage1 = 0;
static uint16_t g_voltage2 = 0;
static uint16_t g_voltage3 = 0;
static uint16_t g_voltage4 = 0;

/**
  * @brief  解析CAN报文，更新全局电压变量
  * @param  data: 指向接收到的8字节CAN数据的指针
  * @retval 无
  */
void Process_CAN_Voltage(uint8_t* data)
{
    // 解析报文中的电压值，每两个字节倒序转换
	g_voltage1 = (uint16_t)(((uint16_t)data[1] << 8) | (uint16_t)data[0]);
	g_voltage2 = (uint16_t)(((uint16_t)data[3] << 8) | (uint16_t)data[2]);
	g_voltage3 = (uint16_t)(((uint16_t)data[5] << 8) | (uint16_t)data[4]);
	g_voltage4 = (uint16_t)(((uint16_t)data[7] << 8) | (uint16_t)data[6]);


}

/**
  * @brief  获取解析后的电压值
  * @param  voltageIndex: 电压序号（1~4）
  * @retval 对应电压值，单位mV
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
            voltage = 0; // 错误处理，返回0表示无效电压
            break;
    }

    return voltage;
}

/**
  * @brief  单片机周期向上位机PCAN发送随机报文，在基本定时器中断中可以设置发送周期
  */
void SendCANEvent(void)
{
	CAN_SetMsg(&TxMessage);
	CAN_Transmit(CANx, &TxMessage);
	
	//是否将CAN报文信息打印到串口中
	#if 1
	CAN_DEBUG_ARRAY(TxMessage.Data,8); 
	#endif
	
	// 处理 CAN 数据，解析电压值
    Process_CAN_Voltage(TxMessage.Data);
    
}
/**************************END OF FILE************************************/











