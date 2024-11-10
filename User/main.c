#include <stdio.h>  
#include "stm32f4xx.h"
#include "./led/bsp_led.h"
#include "./usart/bsp_debug_usart.h"
#include "./can/bsp_can.h"
#include "./key/bsp_key.h"
#include "./lcd/bsp_ili9341_lcd.h"
#include "./tim/bsp_basic_tim.h"
#include "GUI.h"
#include "DIALOG.h"
#include "START.h"
#include "Voltage.h"

__IO uint32_t flag = 0;		 //用于标志是否接收到数据，在中断函数中赋值
CanTxMsg TxMessage;			     //发送缓冲区
CanRxMsg RxMessage;				 //接收缓冲区

static void Delay ( __IO uint32_t nCount );

/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{
	LED_GPIO_Config();
	
    /* 初始化USART1 */
    Debug_USART_Config();
		
	/* 初始化按键 */
	Key_GPIO_Config();
		
	/* 初始化基本定时器定时 */
	TIMx_Configuration();
	
	/* 初始化can,在中断接收CAN数据包 */
	CAN_Config();

    /* 启用 CRC 校验，用于 emWin 库保护 */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_CRC, ENABLE);  
  	
	/* 初始化LCD屏幕 */
	GUI_Init(); 
		
	LCD_Start();      // 显示窗口
	
	while(1)
	{
		// 调用 GUI_Delay() 以处理窗口消息
		GUI_Delay(10);
		
		/*按一次按键发送一次数据*/
		if(	Key_Scan(KEY1_GPIO_PORT,KEY1_PIN) == KEY_ON)
		{
			LED_BLUE;
			CAN_SetMsg(&TxMessage);
			CAN_Transmit(CANx, &TxMessage);
			
			Delay(10000);//等待发送完毕，可使用CAN_TransmitStatus查看状态		
			
			CAN_DEBUG_ARRAY(TxMessage.Data,8); 
			
			// 处理 CAN 数据，解析电压值
            Process_CAN_Voltage(RxMessage.Data);
		}
		
		if(flag==1)
		{		
			LED_GREEN;
			printf("\r\nCAN接收到数据：\r\n");	

			CAN_DEBUG_ARRAY(RxMessage.Data,8); 
			
			// 处理 CAN 数据，解析电压值
            Process_CAN_Voltage(RxMessage.Data);
			
			flag=0;
		}

		/* 检测KEY2按键，按下时显示测试信息 */
		if(Key_Scan(KEY2_GPIO_PORT, KEY2_PIN) == KEY_ON)
		{
			LED_PURPLE;
			
			// 清空屏幕，避免显示旧的内容
            GUI_Clear();
            // 延时，确保显示更新
            GUI_Delay(10);
			Voltage();
		}
	}
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
