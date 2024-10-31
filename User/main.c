/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   CAN双机通讯实验例程    
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火  STM32 F407 开发板  
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "stm32f4xx.h"
#include "./led/bsp_led.h"
#include "./usart/bsp_debug_usart.h"
#include "./can/bsp_can.h"
#include "./key/bsp_key.h"
#include "./lcd/bsp_ili9341_lcd.h"
#include <stdio.h>

__IO uint32_t flag = 0;		 //用于标志是否接收到数据，在中断函数中赋值
CanTxMsg TxMessage;			     //发送缓冲区
CanRxMsg RxMessage;				 //接收缓冲区

static void Delay ( __IO uint32_t nCount );
void Printf_Charater(void)   ;
void Display_CAN_Voltage(uint8_t* data);

/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{
	LED_GPIO_Config();
	
    /*初始化USART1*/
    Debug_USART_Config();
	
	/*初始化LCD屏幕*/
    ILI9341_Init(); 
	
	/*初始化按键*/
	Key_GPIO_Config();
 
	/*初始化can,在中断接收CAN数据包*/
	CAN_Config();
	
	printf("\r\n 1.使用导线连接好两个CAN讯设备\r\n");
	printf("\r\n 2.使用跳线帽连接好: F407V2底板：3V3 <---> CAN/485_3V3 \r\n");
	printf("\r\n 3.按下开发板的KEY1键，会使用CAN向外发送0-7的数据包，包的扩展ID为0x1314 \r\n");
	printf("\r\n 4.若开发板的CAN接收到扩展ID为0x1314的数据包，会把数据以打印到串口。 \r\n");
	printf("\r\n 5.本例中的can波特率为1MBps，为stm32的can最高速率。 \r\n");
	
	ILI9341_GramScan ( 6 );
    while(1)
	{

		
		/*按一次按键发送一次数据*/
		if(	Key_Scan(KEY1_GPIO_PORT,KEY1_PIN) == KEY_ON)
		{
			LED_BLUE;
			CAN_SetMsg(&TxMessage);
			
			CAN_Transmit(CANx, &TxMessage);
			
			Delay(10000);//等待发送完毕，可使用CAN_TransmitStatus查看状态
			
			LED_GREEN;
			
			printf("\r\n已使用CAN发送数据包！\r\n"); 			
			printf("\r\n发送的报文内容为：\r\n");
			printf("\r\n 扩展ID号ExtId：0x%x \r\n",TxMessage.ExtId);
			CAN_DEBUG_ARRAY(TxMessage.Data,8); 


		}
		
		if(flag==1)
		{		
			LED_GREEN;
			printf("\r\nCAN接收到数据：\r\n");	

			CAN_DEBUG_ARRAY(RxMessage.Data,8); 
			
			Display_CAN_Voltage(RxMessage.Data);
			
			flag=0;
		}
	
	
	}

}


extern uint16_t lcdid;

/**
  * @brief  解析CAN接收的数据为电压值并在LCD上显示
  * @param  data: 指向接收到的8字节数据的指针
  * @retval 无
  */
void Display_CAN_Voltage(uint8_t* data)
{
    // 将变量声明放在函数开头
    uint16_t voltage1, voltage2, voltage3, voltage4;
    char displayStr[32];

    // DBC逻辑解析，每两个字节倒序并转换为电压值
    voltage1 = (data[1] << 8) | data[0];
    voltage2 = (data[3] << 8) | data[2];
    voltage3 = (data[5] << 8) | data[4];
    voltage4 = (data[7] << 8) | data[6];
    
    // 在串口打印解析后的电压值
    printf("当前电压为：\r\n");
    printf("Voltage1: %dmv\r\n", voltage1);
    printf("Voltage2: %dmv\r\n", voltage2);
    printf("Voltage3: %dmv\r\n", voltage3);
    printf("Voltage4: %dmv\r\n", voltage4);

    // 清屏并设置字体和颜色
    ILI9341_Clear(0, 0, LCD_X_LENGTH, LCD_Y_LENGTH);
    LCD_SetFont(&Font16x24);
    LCD_SetColors(WHITE, BLACK);
    
    // 在LCD上显示电压值
    ILI9341_DispStringLine_EN(LINE(0), "Voltage Values:");
    
    snprintf(displayStr, sizeof(displayStr), "V1: %dmV", voltage1);
    ILI9341_DispStringLine_EN(LINE(1), displayStr);

    snprintf(displayStr, sizeof(displayStr), "V2: %dmV", voltage2);
    ILI9341_DispStringLine_EN(LINE(2), displayStr);

    snprintf(displayStr, sizeof(displayStr), "V3: %dmV", voltage3);
    ILI9341_DispStringLine_EN(LINE(3), displayStr);

    snprintf(displayStr, sizeof(displayStr), "V4: %dmV", voltage4);
    ILI9341_DispStringLine_EN(LINE(4), displayStr);
}




/**
  * @brief  简单延时函数
  * @param  nCount ：延时计数值
  * @retval 无
  */	
static void Delay ( __IO uint32_t nCount )
{
  for ( ; nCount != 0; nCount -- );
	
}

/*********************************************END OF FILE**********************/

