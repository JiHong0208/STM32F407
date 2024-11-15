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

// ���� app ����ʼ��ַ��
#define APP_BASE_ADDRESS  0x08020000

// ������ʼ������
void App_Init(void);

__IO uint32_t flag = 0;		 //���ڱ�־�Ƿ���յ����ݣ����жϺ����и�ֵ
CanTxMsg TxMessage;			     //���ͻ�����
CanRxMsg RxMessage;				 //���ջ�����

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

    /* ���� CRC У�飬���� emWin �Ᵽ�� */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_CRC, ENABLE);  
  	
	/* ��ʼ��LCD��Ļ */
	GUI_Init(); 
		
	LCD_Start();      // ��ʾ����
	
	
	while(1)
	{

		// ���� GUI_Delay() �Դ�������Ϣ
		GUI_Delay(10);
		
		/* ���KEY1����������ʱ��ʾ��ѹ��Ϣ */
		if(	Key_Scan(KEY1_GPIO_PORT,KEY1_PIN) == KEY_ON)
		{
			LED_BLUE;
			CAN_SetMsg(&TxMessage);
			CAN_Transmit(CANx, &TxMessage);
			
			Delay(10000);//�ȴ�������ϣ���ʹ��CAN_TransmitStatus�鿴״̬		
			
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
		
		if(flag==1)
		{		
			LED_GREEN;
			printf("\r\nCAN���յ����ݣ�\r\n");	

			CAN_DEBUG_ARRAY(RxMessage.Data,8); 
			
			// ���� CAN ���ݣ�������ѹֵ
            Process_CAN_Voltage(RxMessage.Data);
			
			flag=0;
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
}

