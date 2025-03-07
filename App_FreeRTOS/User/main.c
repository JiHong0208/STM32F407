// FreeRTOS头文件
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"

// 开发板硬件bsp头文件
#include "bsp_debug_usart.h"
#include "bsp_ili9341_lcd.h"
#include "bsp_sram.h"	  
#include "bsp_led.h"
#include "bsp_key.h"
#include "bsp_adc.h"
#include "bsp_can.h"
#include "bsp_rtc.h"
#include "sd.h"

// STemWIN头文件
#include "GUI.h"
#include "DIALOG.h"
#include "START.h"

// XCPDriver头文件
#include "xcpBasic.h"

/**************************** 任务句柄 ********************************/
/* 
 * 任务句柄是一个指针，用于指向一个任务，当任务创建好之后，它就具有了一个任务句柄
 * 以后我们要想操作这个任务都需要通过这个任务句柄，如果是自身的任务操作自己，那么
 * 这个句柄可以为NULL。
 */
/* 创建任务句柄 */
static TaskHandle_t AppTaskCreate_Handle = NULL;
/* CAN 任务句柄 */
static TaskHandle_t CAN_Task_Handle = NULL;
/* XCP_Driver 任务句柄 */
static TaskHandle_t XCP_Driver_Task_Handle = NULL;
/* GUI 任务句柄 */
static TaskHandle_t GUI_Task_Handle = NULL;
/* SD_Card 任务句柄 */
static TaskHandle_t SD_Card_Task_Handle = NULL;

/********************************** 内核对象句柄 *********************************/
/*
 * 信号量，消息队列，事件标志组，软件定时器这些都属于内核的对象，要想使用这些内核
 * 对象，必须先创建，创建成功之后会返回一个相应的句柄。实际上就是一个指针，后续我
 * 们就可以通过这个句柄操作这些内核对象。
 *
 * 内核对象说白了就是一种全局的数据结构，通过这些数据结构我们可以实现任务间的通信，
 * 任务间的事件同步等各种功能。至于这些功能的实现我们是通过调用这些内核对象的函数
 * 来完成的
 * 
 */
SemaphoreHandle_t CanReadySem_Handle   = NULL;// 信号量句柄

 
// 定义 App 的起始地址宏
#define APP_BASE_ADDRESS  0x08020000

// 定义全局变量
volatile uint32_t DAQ_Timestamp = 0; 		// XCP的DAQ时间戳，单位：10ms
__IO uint32_t CANRxflag = 0;	            // 用于标志是否接收到数据，在中断函数中赋值
CanTxMsg TxMessage;			                // 发送缓冲区
CanRxMsg RxMessage;				            // 接收缓冲区



// 声明初始化函数
static void App_Init(void);                     // 用于Bootloader跳转到App
static void BSP_Init(void);						// 用于初始化板载相关资源

	
static void AppTaskCreate(void);				// 用于创建任务
static void CAN_Task(void* parameter);			// CAN_Task任务实现 
static void GUI_Task(void* parameter);			// GUI_Task任务实现 
static void XCP_Driver_Task(void* parameter);   // XCP_Driver任务实现
static void SD_Card_Task(void* parameter);      // SD_Card任务实现

/**
  * @brief  主函数
  * @param  无
  * @retval 无
  * @note   
  */
int main(void)
{	
	BaseType_t xReturn = pdPASS;// 定义一个创建信息返回值，默认为pdPASS
  
    // 更新APP的中断向量表和重启全局中断
	App_Init();

    // 初始化硬件
	BSP_Init();
	
    // 初始化硬件,创建AppTaskCreate任务
	xReturn = xTaskCreate((TaskFunction_t )AppTaskCreate,  		// 任务入口函数 
                        (const char*    )"AppTaskCreate",		// 任务名字
                        (uint16_t       )512,  			 		// 任务栈大小 
                        (void*          )NULL,			 		// 任务入口函数参数 
                        (UBaseType_t    )1, 				    // 任务的优先级 
                        (TaskHandle_t*  )&AppTaskCreate_Handle);// 任务控制块指针 
						
    // 启动任务调度            
	if(pdPASS == xReturn)
		vTaskStartScheduler();   // 启动任务，开启调度
	else
	return -1;  
  
	while(1);   // 正常不会执行到这里    
}


/**
  * @brief  AppTaskCreate
  * @param  无
  * @retval 无
  * @note   为了方便管理，所有的任务创建函数都放在这个函数里面
  */
static void AppTaskCreate(void)
{
	BaseType_t xReturn = pdPASS;// 定义一个创建信息返回值，默认为pdPASS 
	
	taskENTER_CRITICAL();//进入临界区
	
	// 创建ScreenShotSem信号量
	CanReadySem_Handle = xSemaphoreCreateBinary();
	
	if (CanReadySem_Handle != NULL)
    {
        printf("CanReadySem信号量创建成功！\r\n");
    }
	
	// 第一个 CAN 任务
	xReturn = xTaskCreate((TaskFunction_t)CAN_Task,						 		// 任务入口函数 
											 (const char*    )"CAN_Task",		// 任务名称 
											 (uint16_t       )128,       		// 任务栈大小 
											 (void*          )NULL,      		// 任务入口函数参数 
											 (UBaseType_t    )5,         		// 任务的优先级 
											 (TaskHandle_t   )&CAN_Task_Handle);// 任务控制块指针 
	if(pdPASS == xReturn)
		printf("创建CAN_Task任务成功！\r\n");
	
	// 第二个 XCP_Driver 任务
    xReturn = xTaskCreate((TaskFunction_t)XCP_Driver_Task,						// 任务入口函数 
											 (const char*      )"XCP_Driver_Task",// 任务名称
											 (uint16_t         )256,     		// 任务栈大小
											 (void*            )NULL,    		// 任务入口函数参数
											 (UBaseType_t      )4,       		// 任务的优先级
											 (TaskHandle_t     )&XCP_Driver_Task_Handle);// 任务控制块指针
	if(pdPASS == xReturn)
		printf("创建XCP_Driver_Task任务成功！\r\n");
	
	// 第三个 GUI_Task任务
	xReturn = xTaskCreate((TaskFunction_t)GUI_Task,						 		// 任务入口函数 
											 (const char*    )"GUI_Task",		// 任务名称 
											 (uint16_t       )1024,       		// 任务栈大小 
											 (void*          )NULL,      		// 任务入口函数参数 
											 (UBaseType_t    )2,         		// 任务的优先级 
											 (TaskHandle_t   )&GUI_Task_Handle);// 任务控制块指针 
											 
	if(pdPASS == xReturn)
		printf("创建GUI_Task任务成功！\r\n");
	
	// 第四个 SD_Card_Task任务
	xReturn = xTaskCreate((TaskFunction_t)SD_Card_Task,						 	// 任务入口函数 
											 (const char*    )"SD_Card_Task",	// 任务名称 
											 (uint16_t       )256,       		// 任务栈大小 
											 (void*          )NULL,      		// 任务入口函数参数 
											 (UBaseType_t    )3,         		// 任务的优先级 
											 (TaskHandle_t   )&SD_Card_Task_Handle);// 任务控制块指针 
											 
	if(pdPASS == xReturn)
		printf("创建SD_Card_Task任务成功！\r\n");
	
	vTaskDelete(AppTaskCreate_Handle);// 删除AppTaskCreate任务
	
	taskEXIT_CRITICAL();// 退出临界区
}

/**
  * @brief CAN任务主体
  * @note 无
  * @param 无
  * @retval 无
  */
static void CAN_Task(void* parameter)
{
	// CAN 任务完成初始化，通知 XCP 任务，添加信号量
    xSemaphoreGive(CanReadySem_Handle);
	
	while(1)
	{
		SendCANEvent();
		vTaskDelay(100);
	}
}

/**
  * @brief XCP_Driver任务主体
  * @note 无
  * @param 无
  * @retval 无
  */
static void XCP_Driver_Task(void* parameter)
{
    // 等待 CAN 任务准备好
    xSemaphoreTake(CanReadySem_Handle, portMAX_DELAY);  // 阻塞直到 CAN 任务通知
	
	while(1)
	{
		DAQ_Timestamp++; // 时间戳增加(10ms 单位)
		XcpEvent(0);     // 通知 XCP 事件，10ms事件
		vTaskDelay(10);
	}
}

/**
  * @brief GUI任务主体
  * @note 无
  * @param 无
  * @retval 无
  */
static void GUI_Task(void* parameter)
{
	// 初始化LCD屏幕 
	GUI_Init(); 
	
	// 开LCD背光灯
	ILI9341_BackLed_Control ( ENABLE );
	
	while(1)
	{
		MainTask();
		//LCD_Start();                 
	}
}

/**
  * @brief SD_Card任务主体
  * @note 无
  * @param 无
  * @retval 无
  */
static void SD_Card_Task(void* parameter)
{
	// 等待 CAN 任务准备好
	xSemaphoreTake(CanReadySem_Handle, portMAX_DELAY);  // 阻塞直到 CAN 任务通知	
	
	while(1)
	{
		SD_MainFunction();
		vTaskDelay(100);
	}
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

/**
  * @brief 板级外设初始化
  * @note 所有板子上的初始化均可放在这个函数里面
  * @param 无
  * @retval 无
  */
static void BSP_Init(void)
{
	// 初始化LED
	LED_GPIO_Config();
	
    // 初始化USART1
    Debug_USART_Config();
		
	// 初始化按键
	Key_GPIO_Config();
		
	// 初始化CAN,在中断接收CAN数据包
	CAN_Config();
	
	// 初始化XCP协议栈
	XcpInit();

    // 启用 CRC 校验，用于 emWin 库保护 
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_CRC, ENABLE);  
  	
	// SD卡格式化测试，检测SD卡可用性
	SD_Check();
	
	// SRAM初始化
    FSMC_SRAM_Init();
  
    // ADC初始化
	//Rheostat_Init();
	
	// RTC时钟初始化
	RTC_CLK_Config();
	
	// RTC外设功能初始化
	RTC_Initialize();

	/*
	 * STM32中断优先级分组为4，即4bit都用来表示抢占优先级，范围为：0~15
	 * 优先级分组只需要分组一次即可，以后如果有其他的任务需要用到中断，
	 * 都统一用这个优先级分组，千万不要再分组，切忌。
	 */
	NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 );
}

/********************************END OF FILE****************************/
