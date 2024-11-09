#include <stdio.h>  
#include "stm32f4xx.h"
#include "./led/bsp_led.h"
#include "./usart/bsp_debug_usart.h"
#include "./can/bsp_can.h"
#include "./key/bsp_key.h"
#include "./lcd/bsp_ili9341_lcd.h"
#include "./tim/bsp_basic_tim.h"
#include "GUI.h"

__IO uint32_t flag = 0;		 //���ڱ�־�Ƿ���յ����ݣ����жϺ����и�ֵ
CanTxMsg TxMessage;			     //���ͻ�����
CanRxMsg RxMessage;				 //���ջ�����

static void Delay ( __IO uint32_t nCount );
void Printf_Charater(void);
void Display_Test_Message(void);  // ��ʾ������Ϣ�ĺ���

/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main(void)
{
	LED_GPIO_Config();
	
    /*��ʼ��USART1*/
    Debug_USART_Config();
	
    /*CRC��emWinû�й�ϵ��ֻ������Ϊ�˿�ı��������ģ�����STemWin�Ŀ�ֻ������ST��оƬ���棬���оƬ���޷�ʹ�õġ� */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_CRC, ENABLE);  
  	
	/*��ʼ��LCD��Ļ*/
	GUI_Init(); 
	
	/*��ʼ������*/
	Key_GPIO_Config();
 
	/*��ʼ��can,���жϽ���CAN���ݰ�*/
	CAN_Config();
	
	/* ��ʼ��������ʱ����ʱ */
	TIMx_Configuration();
	
	printf("\r\n 1.ʹ�õ������Ӻ�����CANѶ�豸\r\n");
	printf("\r\n 2.ʹ������ñ���Ӻ�: F407V2�װ壺3V3 <---> CAN/485_3V3 \r\n");
	printf("\r\n 3.���¿������KEY1������ʹ��CAN���ⷢ��0-7�����ݰ���������չIDΪ0x1314 \r\n");
	printf("\r\n 4.���������CAN���յ���չIDΪ0x1314�����ݰ�����������Դ�ӡ�����ڡ� \r\n");
	printf("\r\n 5.�����е�can������Ϊ1MBps��Ϊstm32��can������ʡ� \r\n");
	
	ILI9341_GramScan ( 6 );
  

	while(1)
	{
		/*��һ�ΰ�������һ������*/
		if(	Key_Scan(KEY1_GPIO_PORT,KEY1_PIN) == KEY_ON)
		{
			LED_BLUE;
			CAN_SetMsg(&TxMessage);
			CAN_Transmit(CANx, &TxMessage);
			
			Delay(10000);//�ȴ�������ϣ���ʹ��CAN_TransmitStatus�鿴״̬
			
			Display_CAN_Voltage(TxMessage.Data);
			
			printf("\r\n��ʹ��CAN�������ݰ���\r\n"); 			
			printf("\r\n���͵ı�������Ϊ��\r\n");
			printf("\r\n ��չID��ExtId��0x%x \r\n",TxMessage.ExtId);
			CAN_DEBUG_ARRAY(TxMessage.Data,8); 
		}
		
		if(flag==1)
		{		
			LED_GREEN;
			printf("\r\nCAN���յ����ݣ�\r\n");	

			CAN_DEBUG_ARRAY(RxMessage.Data,8); 
			
			Display_CAN_Voltage(RxMessage.Data);
			
			flag=0;
		}

		/* ���KEY2����������ʱ��ʾ������Ϣ */
		if(Key_Scan(KEY2_GPIO_PORT, KEY2_PIN) == KEY_ON)
		{
			Display_Test_Message();  // ����KEY2ʱ��ʾ������Ϣ
			Delay(2000);  // ȷ����Ϣ������ʾһ��ʱ��
		}
	}
}

/**
  * @brief  ��ʾ������Ϣ
  * @param  ��
  * @retval ��
  */
void Display_Test_Message(void)
{
	// �����Ļ
	GUI_Clear();
	
	// ����������ɫ�ͱ���ɫ
	GUI_SetColor(GUI_WHITE);
	GUI_SetBkColor(GUI_BLUE);
	
	// ��ʾ��Ϣ
	GUI_DispStringAt("LCD Init Success!", 50, 100);  // �����������Ը������LCD��ʾ�������
	GUI_DispStringAt("Press KEY2 to test.", 50, 140);
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
