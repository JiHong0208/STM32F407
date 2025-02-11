#include "stm32f4xx.h"
#include "bsp_debug_usart.h"
#include "bsp_key.h"
#include "bsp_led.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_usart.h"
#include "stm32f4xx_rcc.h"

#define APP_ADDRESS 0x08020000

typedef void (*pFunction)(void);
pFunction Jump_To_Application;
uint32_t JumpAddress;

static void JumpToApp(void);
static void BSP_Init(void);
static void Delay(__IO uint32_t nCount);

/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void) 
{
    // 初始化硬件
	BSP_Init();

	// 指示进入 Bootloader
    LED_BLUE;
	printf("\r\n------------------进入到Bootloader程序------------------\r\n");
	
    // 持续监测 KEY1 按键
    while (1) 
	{
        // 检测按键 KEY1，如果按下则跳转到 APP
        if (Key_Scan(KEY1_GPIO_PORT, KEY1_PIN) == KEY_ON) 
		{
            Delay(100000);  // 延时一段时间，以确保稳定
            JumpToApp();
        }
        
        Delay(500000);  // 确保周期性检查
    }
}

static void JumpToApp(void) {
    // 检查 APP 分区是否有效
    if (((*(__IO uint32_t*)APP_ADDRESS) & 0x2FFE0000) == 0x20000000) 
	{

        // 禁用中断
        __disable_irq();

        // 设置主堆栈指针
        __set_MSP(*(__IO uint32_t*)APP_ADDRESS);

        // 获取复位处理函数地址并跳转
        JumpAddress = *(__IO uint32_t*)(APP_ADDRESS + 4);
        Jump_To_Application = (pFunction)JumpAddress;
        Jump_To_Application();
		
		
    }
}

static void BSP_Init(void)
{
	// 初始化系统和硬件
    SystemInit();
    LED_GPIO_Config();
    Key_GPIO_Config();
    Debug_USART_Config();
}

static void Delay(__IO uint32_t nCount) {
    for (; nCount != 0; nCount--);
}




