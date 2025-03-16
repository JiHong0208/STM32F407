#include "stm32f4xx.h"
#include "fonts.h"
#include "bsp_key.h"
#include "bsp_led.h"
#include "bsp_ili9341_lcd.h"
#include "stm32f4xx_gpio.h"
#include "bsp_debug_usart.h"

#define APP_ADDRESS 0x08020000

typedef void (*pFunction)(void);
pFunction Jump_To_Application;
uint32_t JumpAddress;

static void JumpToApp(void);
static void BSP_Init(void);
static void LCD_Start(void);
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

	// ��ʼ��LCD����ʾ����
	LCD_Start();
	
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
	ILI9341_Init();
    LED_GPIO_Config();
    Key_GPIO_Config();
    Debug_USART_Config();
}

static void LCD_Start(void)
{
	// ��������ʾȫ�ڱ���
	ILI9341_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);
	
	// ����������ɫΪ��ɫ������Ϊ��ɫ
	LCD_SetColors(WHITE,BLACK);
	
	// ��ʾ��ʼ����Ϣ
	LCD_SetFont(&Font16x24);
	ILI9341_DispStringLine_EN(LINE(1),"STM32F407_BMS");
	
	LCD_SetFont(&Font8x16);
	ILI9341_DispStringLine_EN(LINE(5),"This is Bootloader.");
	ILI9341_DispStringLine_EN(LINE(8),"Press Key1 jump to App.");
	ILI9341_DispStringLine_EN(LINE(15),"           Author:ZhiFengSu");
	ILI9341_DispStringLine_EN(LINE(16),"           Graduation:FJUT");

	
}

static void Delay(__IO uint32_t nCount) {
    for (; nCount != 0; nCount--);
}




