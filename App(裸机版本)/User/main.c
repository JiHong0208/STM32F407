#include "sd.h"
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
static void App_Init(void);
static void BSP_Init(void);
static void Delay ( __IO uint32_t nCount );

__IO uint32_t CANRxflag = 0;	            // 用于标志是否接收到数据，在中断函数中赋值
CanTxMsg TxMessage;			                // 发送缓冲区
CanRxMsg RxMessage;				            // 接收缓冲区
__IO uint8_t key1_pressed = 0;              // 标记 KEY1 是否已经按下
__IO uint8_t key2_pressed = 0;              // 标记 KEY2 是否已经按下
volatile uint8_t StartStopVoltageFlag = 0;  // 标志位，用于暂停或开始电压信息发送和存储
uint8_t VoltageShowCount = 0;				// 标志位，用于保证Voltage()函数只触发一次




/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{

	// 更新APP的中断向量表和重启全局中断
    App_Init();
	
	// 初始化硬件
	BSP_Init();
	
	// LCD初始界面
	LCD_Start();

	while(1)
	{

		// 调用 GUI_Delay() 以处理窗口消息
		GUI_Delay(10);

		// 检测KEY1按键，按下时切换StartStopVoltageFlag，用于暂停或开始电压信息发送和存储
		if(	Key_Scan(KEY1_GPIO_PORT,KEY1_PIN) == KEY_ON)
		{
			LED_GREEN;
			
			// 切换标志位
			StartStopVoltageFlag = !StartStopVoltageFlag;
			
			// 用于保证Voltage()函数只触发一次
			VoltageShowCount++;
			if (VoltageShowCount == 1)
			{
				Voltage();
			}
		}
		
		// 检测KEY2按键，按下时切换StartStopVoltageFlag，用于暂停或开始电压信息发送和存储
		if(Key_Scan(KEY2_GPIO_PORT, KEY2_PIN) == KEY_ON)
		{
			Delay(200);// 按键防抖设计
			LED_PURPLE;
			SD_Function();
			
		}
		
		if(CANRxflag==1)
		{			
			// 处理 CAN 数据，解析电压值
            Process_CAN_Voltage(RxMessage.Data);
			
			CANRxflag = 0;
		}

	}
}

static void Delay ( __IO uint32_t nCount )
{
  for ( ; nCount != 0; nCount -- );
	
}

static void App_Init(void)
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

static void BSP_Init(void)
{
    // 初始化LED
	LED_GPIO_Config();
	
    // 初始化USART1
    Debug_USART_Config();
		
	// 初始化按键
	Key_GPIO_Config();
		
	// 初始化基本定时器定时
	TIMx_Configuration();
	
	// 初始化can,在中断接收CAN数据包
	CAN_Config();
	
	// 初始化XCP协议栈
	XcpInit();

    // 启用 CRC 校验，用于 emWin 库保护 
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_CRC, ENABLE);  
  	
	// 初始化LCD屏幕 
	GUI_Init(); 
	
	// SD卡格式化测试，检测SD卡可用性
	SD_Test();
		
}
