#include "GUI.h"
#include "START.h"
#include <stdio.h>  
#include <stdint.h>
#include "DIALOG.h"
#include "bsp_key.h"
#include "bsp_led.h"
#include "bsp_can.h"
#include "Voltage.h"
#include "xcpBasic.h"
#include "stm32f4xx.h"
#include "bsp_basic_tim.h"
#include "bsp_debug_usart.h"
#include "bsp_ili9341_lcd.h"


// 定义 App 的起始地址宏
#define APP_BASE_ADDRESS  0x08020000

// 声明初始化函数
void App_Init(void);

__IO uint32_t CANRxflag = 0;	 //用于标志是否接收到数据，在中断函数中赋值
CanTxMsg TxMessage;			     //发送缓冲区
CanRxMsg RxMessage;				 //接收缓冲区
__IO uint8_t key1_pressed = 0;   // 标记 KEY1 是否已经按下
__IO uint8_t key2_pressed = 0;   // 标记 KEY2 是否已经按下


static void Delay ( __IO uint32_t nCount );

/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{

	/* 更新APP的中断向量表和重启全局中断 */
    App_Init();
	
	/* 初始化LED */
	LED_GPIO_Config();
	
    /* 初始化USART1 */
    Debug_USART_Config();
		
	/* 初始化按键 */
	Key_GPIO_Config();
		
	/* 初始化基本定时器定时 */
	TIMx_Configuration();
	
	/* 初始化can,在中断接收CAN数据包 */
	CAN_Config();
	
	/* 初始化XCP协议栈 */
	XcpInit();

    /* 启用 CRC 校验，用于 emWin 库保护 */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_CRC, ENABLE);  
  	
	/* 初始化LCD屏幕 */
	GUI_Init(); 
		
	/* LCD初始界面 */
	LCD_Start();

	while(1)
	{

		// 调用 GUI_Delay() 以处理窗口消息
		GUI_Delay(10);
		
		/* 检测KEY1按键，按下时显示电压信息 */
		if(	Key_Scan(KEY1_GPIO_PORT,KEY1_PIN) == KEY_ON)
		{
			LED_RED;
			CAN_SetMsg(&TxMessage);
			CAN_Transmit(CANx, &TxMessage);
			
			//等待发送完毕，可使用CAN_TransmitStatus查看状态
			Delay(10000);		
			
			CAN_DEBUG_ARRAY(TxMessage.Data,8); 
			
			// 处理 CAN 数据，解析电压值
            Process_CAN_Voltage(RxMessage.Data);
			
			//将发送电压显示到LCD屏幕
			Voltage();
		}
		
		/* 检测KEY2按键，按下时显示电压信息 */
		if(Key_Scan(KEY2_GPIO_PORT, KEY2_PIN) == KEY_ON)
		{
			LED_PURPLE;
			
			//将接收电压显示到LCD屏幕
			Voltage();
		}
		
		if(CANRxflag==1)
		{			
			// 处理 CAN 数据，解析电压值
            Process_CAN_Voltage(RxMessage.Data);
			
			CANRxflag=0;
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


/**
  * @brief  应用程序初始化函数
  * @param  无
  * @retval 无
  */
void App_Init(void)
{
	#define VECT_TAB_OFFSET  0x0000
	
    // 更新向量表地址为 app 的起始地址
    SCB->VTOR = APP_BASE_ADDRESS | VECT_TAB_OFFSET;

    // 清除 FAULTMASK，开启全局中断
    __set_FAULTMASK(0);
    __enable_irq();
	
	// 指示进入 App
	printf("\r\n----------------------进入到App程序----------------------\r\n");
	
}

