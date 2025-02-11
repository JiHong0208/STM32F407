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
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main(void) 
{
    // ��ʼ��Ӳ��
	BSP_Init();

	// ָʾ���� Bootloader
    LED_BLUE;
	printf("\r\n------------------���뵽Bootloader����------------------\r\n");
	
    // ������� KEY1 ����
    while (1) 
	{
        // ��ⰴ�� KEY1�������������ת�� APP
        if (Key_Scan(KEY1_GPIO_PORT, KEY1_PIN) == KEY_ON) 
		{
            Delay(100000);  // ��ʱһ��ʱ�䣬��ȷ���ȶ�
            JumpToApp();
        }
        
        Delay(500000);  // ȷ�������Լ��
    }
}

static void JumpToApp(void) {
    // ��� APP �����Ƿ���Ч
    if (((*(__IO uint32_t*)APP_ADDRESS) & 0x2FFE0000) == 0x20000000) 
	{

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

static void BSP_Init(void)
{
	// ��ʼ��ϵͳ��Ӳ��
    SystemInit();
    LED_GPIO_Config();
    Key_GPIO_Config();
    Debug_USART_Config();
}

static void Delay(__IO uint32_t nCount) {
    for (; nCount != 0; nCount--);
}




