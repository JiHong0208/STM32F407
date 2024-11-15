/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   ��1.8.0�汾�⽨�Ĺ���ģ��
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��  STM32 F407 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
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
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main(void) {
    // ��ʼ��ϵͳ��Ӳ��
    SystemInit();
    LED_GPIO_Config();
    Key_GPIO_Config();

    // ָʾ���� Bootloader
    LED_RED;

    // ������� KEY1 ����
    while (1) {
        // ��ⰴ�� KEY1�������������ת�� APP
        if (Key_Scan(KEY1_GPIO_PORT, KEY1_PIN) == KEY_ON) {
            LED_GREEN;
            Delay(100000);  // ��ʱһ��ʱ�䣬��ȷ���ȶ�
            JumpToApp();
        }
        
        // ��ȫ�������²�����������ִ����ѭ�������� LED ״̬
        LED_BLUE;
        Delay(500000);  // ���Ը�����Ҫ������ʱʱ�䣬ȷ�������Լ��
    }
}

void JumpToApp(void) {
    // ��� APP �����Ƿ���Ч
    if (((*(__IO uint32_t*)APP_ADDRESS) & 0x2FFE0000) == 0x20000000) {
        // �����ж�
        __disable_irq();

        // ��������ջָ��
        __set_MSP(*(__IO uint32_t*)APP_ADDRESS);

        // ��ȡ��λ��������ַ����ת
        JumpAddress = *(__IO uint32_t*)(APP_ADDRESS + 4);
        Jump_To_Application = (pFunction)JumpAddress;
        Jump_To_Application();
    }
}

static void Delay(__IO uint32_t nCount) {
    for (; nCount != 0; nCount--);
}


/*********************************************END OF FILE**********************/

