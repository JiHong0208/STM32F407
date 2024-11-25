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


// ���� App ����ʼ��ַ��
#define APP_BASE_ADDRESS  0x08020000

// ������ʼ������
void App_Init(void);

__IO uint32_t CANRxflag = 0;	 //���ڱ�־�Ƿ���յ����ݣ����жϺ����и�ֵ
CanTxMsg TxMessage;			     //���ͻ�����
CanRxMsg RxMessage;				 //���ջ�����
__IO uint8_t key1_pressed = 0;   // ��� KEY1 �Ƿ��Ѿ�����
__IO uint8_t key2_pressed = 0;   // ��� KEY2 �Ƿ��Ѿ�����


static void Delay ( __IO uint32_t nCount );

/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main(void)
{

	/* ����APP���ж������������ȫ���ж� */
    App_Init();
	
	/* ��ʼ��LED */
	LED_GPIO_Config();
	
    /* ��ʼ��USART1 */
    Debug_USART_Config();
		
	/* ��ʼ������ */
	Key_GPIO_Config();
		
	/* ��ʼ��������ʱ����ʱ */
	TIMx_Configuration();
	
	/* ��ʼ��can,���жϽ���CAN���ݰ� */
	CAN_Config();
	
	/* ��ʼ��XCPЭ��ջ */
	XcpInit();

    /* ���� CRC У�飬���� emWin �Ᵽ�� */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_CRC, ENABLE);  
  	
	/* ��ʼ��LCD��Ļ */
	GUI_Init(); 
		
	/* LCD��ʼ���� */
	LCD_Start();

	while(1)
	{

		// ���� GUI_Delay() �Դ�������Ϣ
		GUI_Delay(10);
		
		/* ���KEY1����������ʱ��ʾ��ѹ��Ϣ */
		if(	Key_Scan(KEY1_GPIO_PORT,KEY1_PIN) == KEY_ON)
		{
			LED_RED;
			CAN_SetMsg(&TxMessage);
			CAN_Transmit(CANx, &TxMessage);
			
			//�ȴ�������ϣ���ʹ��CAN_TransmitStatus�鿴״̬
			Delay(10000);		
			
			CAN_DEBUG_ARRAY(TxMessage.Data,8); 
			
			// ���� CAN ���ݣ�������ѹֵ
            Process_CAN_Voltage(RxMessage.Data);
			
			//�����͵�ѹ��ʾ��LCD��Ļ
			Voltage();
		}
		
		/* ���KEY2����������ʱ��ʾ��ѹ��Ϣ */
		if(Key_Scan(KEY2_GPIO_PORT, KEY2_PIN) == KEY_ON)
		{
			LED_PURPLE;
			
			//�����յ�ѹ��ʾ��LCD��Ļ
			Voltage();
		}
		
		if(CANRxflag==1)
		{			
			// ���� CAN ���ݣ�������ѹֵ
            Process_CAN_Voltage(RxMessage.Data);
			
			CANRxflag=0;
		}

	}
}


/**
  * @brief  ����ʱ����
  * @param  nCount ����ʱ����ֵ
  * @retval ��
  */	
static void Delay ( __IO uint32_t nCount )
{
  for ( ; nCount != 0; nCount -- );
	
}


/**
  * @brief  Ӧ�ó����ʼ������
  * @param  ��
  * @retval ��
  */
void App_Init(void)
{
	#define VECT_TAB_OFFSET  0x0000
	
    // �����������ַΪ app ����ʼ��ַ
    SCB->VTOR = APP_BASE_ADDRESS | VECT_TAB_OFFSET;

    // ��� FAULTMASK������ȫ���ж�
    __set_FAULTMASK(0);
    __enable_irq();
	
	// ָʾ���� App
	printf("\r\n----------------------���뵽App����----------------------\r\n");
	
}

