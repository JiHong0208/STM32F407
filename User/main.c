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

static void LCD_Test(void);	
static void Delay ( __IO uint32_t nCount );
void Printf_Charater(void)   ;

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
		LCD_Test();
		
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
			
			Parse_CAN_Data(RxMessage.Data);
			
			flag=0;
		}
	
	
	}

}



extern uint16_t lcdid;

/*用于测试各种液晶的函数*/
void LCD_Test(void)
{
	/*演示显示变量*/
	static uint8_t testCNT = 0;	
	char dispBuff[100];
	
	testCNT++;	
	
	LCD_SetFont(&Font8x16);
	LCD_SetColors(RED,BLACK);

  ILI9341_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);	/* 清屏，显示全黑 */
	/********显示字符串示例*******/
  ILI9341_DispStringLine_EN(LINE(0),"BH 3.2 inch LCD para:");
  ILI9341_DispStringLine_EN(LINE(1),"Image resolution:240x320 px");
  if(lcdid == LCDID_ILI9341)
  {
    ILI9341_DispStringLine_EN(LINE(2),"ILI9341 LCD driver");
  }

  ILI9341_DispStringLine_EN(LINE(3),"XPT2046 Touch Pad driver");
  
	/********显示变量示例*******/
	LCD_SetFont(&Font16x24);
	LCD_SetTextColor(GREEN);

	/*使用c标准库把变量转化成字符串*/
	sprintf(dispBuff,"Count : %d ",testCNT);
  LCD_ClearLine(LINE(4));	/* 清除单行文字 */
	
	/*然后显示该字符串即可，其它变量也是这样处理*/
	ILI9341_DispStringLine_EN(LINE(4),dispBuff);

	/*******显示图形示例******/
	LCD_SetFont(&Font24x32);
  /* 画直线 */
  
  LCD_ClearLine(LINE(4));/* 清除单行文字 */
	LCD_SetTextColor(BLUE);

  ILI9341_DispStringLine_EN(LINE(4),"Draw line:");
  
	LCD_SetTextColor(RED);
  ILI9341_DrawLine(50,170,210,230);  
  ILI9341_DrawLine(50,200,210,240);
  
	LCD_SetTextColor(GREEN);
  ILI9341_DrawLine(100,170,200,230);  
  ILI9341_DrawLine(200,200,220,240);
	
	LCD_SetTextColor(BLUE);
  ILI9341_DrawLine(110,170,110,230);  
  ILI9341_DrawLine(130,200,220,240);
  
  Delay(0xFFFFFF);
  
  ILI9341_Clear(0,16*8,LCD_X_LENGTH,LCD_Y_LENGTH-16*8);	/* 清屏，显示全黑 */
  
  
  /*画矩形*/

  LCD_ClearLine(LINE(4));	/* 清除单行文字 */
	LCD_SetTextColor(BLUE);

  ILI9341_DispStringLine_EN(LINE(4),"Draw Rect:");

	LCD_SetTextColor(RED);
  ILI9341_DrawRectangle(50,200,100,30,1);
	
	LCD_SetTextColor(GREEN);
  ILI9341_DrawRectangle(160,200,20,40,0);
	
	LCD_SetTextColor(BLUE);
  ILI9341_DrawRectangle(170,200,50,20,1);
  
  
  Delay(0xFFFFFF);
	
	ILI9341_Clear(0,16*8,LCD_X_LENGTH,LCD_Y_LENGTH-16*8);	/* 清屏，显示全黑 */

  /* 画圆 */
  LCD_ClearLine(LINE(4));	/* 清除单行文字 */
	LCD_SetTextColor(BLUE);
	
  ILI9341_DispStringLine_EN(LINE(4),"Draw Cir:");

	LCD_SetTextColor(RED);
  ILI9341_DrawCircle(100,200,20,0);
	
	LCD_SetTextColor(GREEN);
  ILI9341_DrawCircle(100,200,10,1);
	
	LCD_SetTextColor(BLUE);
	ILI9341_DrawCircle(140,200,20,0);

  Delay(0xFFFFFF);
  
  ILI9341_Clear(0,16*8,LCD_X_LENGTH,LCD_Y_LENGTH-16*8);	/* 清屏，显示全黑 */

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

