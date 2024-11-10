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
		
		/*��һ�ΰ�������һ������*/
		if(	Key_Scan(KEY1_GPIO_PORT,KEY1_PIN) == KEY_ON)
		{
			LED_BLUE;
			CAN_SetMsg(&TxMessage);
			CAN_Transmit(CANx, &TxMessage);
			
			Delay(10000);//�ȴ�������ϣ���ʹ��CAN_TransmitStatus�鿴״̬		
			
			CAN_DEBUG_ARRAY(TxMessage.Data,8); 
			
			// ���� CAN ���ݣ�������ѹֵ
            Process_CAN_Voltage(RxMessage.Data);
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

		/* ���KEY2����������ʱ��ʾ������Ϣ */
		if(Key_Scan(KEY2_GPIO_PORT, KEY2_PIN) == KEY_ON)
		{
			LED_PURPLE;
			
			// �����Ļ��������ʾ�ɵ�����
            GUI_Clear();
            // ��ʱ��ȷ����ʾ����
            GUI_Delay(10);
			Voltage();
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
