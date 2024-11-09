#include <stdio.h>  
#include "stm32f4xx.h"
#include "./led/bsp_led.h"
#include "./usart/bsp_debug_usart.h"
#include "./can/bsp_can.h"
#include "./key/bsp_key.h"
#include "./lcd/bsp_ili9341_lcd.h"
#include "./tim/bsp_basic_tim.h"
#include "GUI.h"

__IO uint32_t flag = 0;		 //用于标志是否接收到数据，在中断函数中赋值
CanTxMsg TxMessage;			     //发送缓冲区
CanRxMsg RxMessage;				 //接收缓冲区

static void Delay ( __IO uint32_t nCount );
void Printf_Charater(void);
void Display_Test_Message(void);  // 显示测试信息的函数

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
	
    /*CRC和emWin没有关系，只是他们为了库的保护而做的，这样STemWin的库只能用在ST的芯片上面，别的芯片是无法使用的。 */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_CRC, ENABLE);  
  	
	/*初始化LCD屏幕*/
	GUI_Init(); 
	
	/*初始化按键*/
	Key_GPIO_Config();
 
	/*初始化can,在中断接收CAN数据包*/
	CAN_Config();
	
	/* 初始化基本定时器定时 */
	TIMx_Configuration();
	
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
			
			Display_CAN_Voltage(TxMessage.Data);
			
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

		/* 检测KEY2按键，按下时显示测试信息 */
		if(Key_Scan(KEY2_GPIO_PORT, KEY2_PIN) == KEY_ON)
		{
			Display_Test_Message();  // 按下KEY2时显示测试消息
			Delay(2000);  // 确保消息持续显示一段时间
		}
	}
}

/**
  * @brief  显示测试信息
  * @param  无
  * @retval 无
  */
void Display_Test_Message(void)
{
	// 清除屏幕
	GUI_Clear();
	
	// 设置字体颜色和背景色
	GUI_SetColor(GUI_WHITE);
	GUI_SetBkColor(GUI_BLUE);
	
	// 显示信息
	GUI_DispStringAt("LCD Init Success!", 50, 100);  // 这里的坐标可以根据你的LCD显示需求调整
	GUI_DispStringAt("Press KEY2 to test.", 50, 140);
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
