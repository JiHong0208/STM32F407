/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   用1.8.0版本库建的工程模板
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
#include "./key/bsp_key.h"
#include "./led/bsp_led.h"

#define APP_ADDRESS 0x08020000

typedef void (*pFunction)(void);
pFunction Jump_To_Application;
uint32_t JumpAddress;

void JumpToApp(void);
static void Delay(__IO uint32_t nCount);

/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void) {
    // 初始化系统和硬件
    SystemInit();
    LED_GPIO_Config();
    Key_GPIO_Config();

    // 指示进入 Bootloader
    LED_RED;

    // 持续监测 KEY1 按键
    while (1) {
        // 检测按键 KEY1，如果按下则跳转到 APP
        if (Key_Scan(KEY1_GPIO_PORT, KEY1_PIN) == KEY_ON) {
            LED_GREEN;
            Delay(100000);  // 延时一段时间，以确保稳定
            JumpToApp();
        }
        
        // 在全速运行下不阻塞，继续执行主循环，保持 LED 状态
        LED_BLUE;
        Delay(500000);  // 可以根据需要调整延时时间，确保周期性检查
    }
}

void JumpToApp(void) {
    // 检查 APP 分区是否有效
    if (((*(__IO uint32_t*)APP_ADDRESS) & 0x2FFE0000) == 0x20000000) {
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

static void Delay(__IO uint32_t nCount) {
    for (; nCount != 0; nCount--);
}


/*********************************************END OF FILE**********************/

