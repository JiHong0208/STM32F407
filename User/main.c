/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   CAN˫��ͨѶʵ������    
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
#include "./led/bsp_led.h"
#include "./usart/bsp_debug_usart.h"
#include "./can/bsp_can.h"
#include "./key/bsp_key.h"
#include "./lcd/bsp_ili9341_lcd.h"
#include <stdio.h>

__IO uint32_t flag = 0;		 //���ڱ�־�Ƿ���յ����ݣ����жϺ����и�ֵ
CanTxMsg TxMessage;			     //���ͻ�����
CanRxMsg RxMessage;				 //���ջ�����

static void LCD_Test(void);	
static void Delay ( __IO uint32_t nCount );
void Printf_Charater(void)   ;

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
	
	/*��ʼ��LCD��Ļ*/
    ILI9341_Init(); 
	
	/*��ʼ������*/
	Key_GPIO_Config();
 
	/*��ʼ��can,���жϽ���CAN���ݰ�*/
	CAN_Config();
	
	printf("\r\n 1.ʹ�õ������Ӻ�����CANѶ�豸\r\n");
	printf("\r\n 2.ʹ������ñ���Ӻ�: F407V2�װ壺3V3 <---> CAN/485_3V3 \r\n");
	printf("\r\n 3.���¿������KEY1������ʹ��CAN���ⷢ��0-7�����ݰ���������չIDΪ0x1314 \r\n");
	printf("\r\n 4.���������CAN���յ���չIDΪ0x1314�����ݰ�����������Դ�ӡ�����ڡ� \r\n");
	printf("\r\n 5.�����е�can������Ϊ1MBps��Ϊstm32��can������ʡ� \r\n");
	
	ILI9341_GramScan ( 6 );
    while(1)
	{
		LCD_Test();
		
		/*��һ�ΰ�������һ������*/
		if(	Key_Scan(KEY1_GPIO_PORT,KEY1_PIN) == KEY_ON)
		{
			LED_BLUE;
			CAN_SetMsg(&TxMessage);
			
			CAN_Transmit(CANx, &TxMessage);
			
			Delay(10000);//�ȴ�������ϣ���ʹ��CAN_TransmitStatus�鿴״̬
			
			LED_GREEN;
			
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
			
			Parse_CAN_Data(RxMessage.Data);
			
			flag=0;
		}
	
	
	}

}



extern uint16_t lcdid;

/*���ڲ��Ը���Һ���ĺ���*/
void LCD_Test(void)
{
	/*��ʾ��ʾ����*/
	static uint8_t testCNT = 0;	
	char dispBuff[100];
	
	testCNT++;	
	
	LCD_SetFont(&Font8x16);
	LCD_SetColors(RED,BLACK);

  ILI9341_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);	/* ��������ʾȫ�� */
	/********��ʾ�ַ���ʾ��*******/
  ILI9341_DispStringLine_EN(LINE(0),"BH 3.2 inch LCD para:");
  ILI9341_DispStringLine_EN(LINE(1),"Image resolution:240x320 px");
  if(lcdid == LCDID_ILI9341)
  {
    ILI9341_DispStringLine_EN(LINE(2),"ILI9341 LCD driver");
  }

  ILI9341_DispStringLine_EN(LINE(3),"XPT2046 Touch Pad driver");
  
	/********��ʾ����ʾ��*******/
	LCD_SetFont(&Font16x24);
	LCD_SetTextColor(GREEN);

	/*ʹ��c��׼��ѱ���ת�����ַ���*/
	sprintf(dispBuff,"Count : %d ",testCNT);
  LCD_ClearLine(LINE(4));	/* ����������� */
	
	/*Ȼ����ʾ���ַ������ɣ���������Ҳ����������*/
	ILI9341_DispStringLine_EN(LINE(4),dispBuff);

	/*******��ʾͼ��ʾ��******/
	LCD_SetFont(&Font24x32);
  /* ��ֱ�� */
  
  LCD_ClearLine(LINE(4));/* ����������� */
	LCD_SetTextColor(BLUE);

  ILI9341_DispStringLine_EN(LINE(4),"Draw line:");
  
	LCD_SetTextColor(RED);
  ILI9341_DrawLine(50,170,210,230);  
  ILI9341_DrawLine(50,200,210,240);
  
	LCD_SetTextColor(GREEN);
  ILI9341_DrawLine(100,170,200,230);  
  ILI9341_DrawLine(200,200,220,240);
	
	LCD_SetTextColor(BLUE);
  ILI9341_DrawLine(110,170,110,230);  
  ILI9341_DrawLine(130,200,220,240);
  
  Delay(0xFFFFFF);
  
  ILI9341_Clear(0,16*8,LCD_X_LENGTH,LCD_Y_LENGTH-16*8);	/* ��������ʾȫ�� */
  
  
  /*������*/

  LCD_ClearLine(LINE(4));	/* ����������� */
	LCD_SetTextColor(BLUE);

  ILI9341_DispStringLine_EN(LINE(4),"Draw Rect:");

	LCD_SetTextColor(RED);
  ILI9341_DrawRectangle(50,200,100,30,1);
	
	LCD_SetTextColor(GREEN);
  ILI9341_DrawRectangle(160,200,20,40,0);
	
	LCD_SetTextColor(BLUE);
  ILI9341_DrawRectangle(170,200,50,20,1);
  
  
  Delay(0xFFFFFF);
	
	ILI9341_Clear(0,16*8,LCD_X_LENGTH,LCD_Y_LENGTH-16*8);	/* ��������ʾȫ�� */

  /* ��Բ */
  LCD_ClearLine(LINE(4));	/* ����������� */
	LCD_SetTextColor(BLUE);
	
  ILI9341_DispStringLine_EN(LINE(4),"Draw Cir:");

	LCD_SetTextColor(RED);
  ILI9341_DrawCircle(100,200,20,0);
	
	LCD_SetTextColor(GREEN);
  ILI9341_DrawCircle(100,200,10,1);
	
	LCD_SetTextColor(BLUE);
	ILI9341_DrawCircle(140,200,20,0);

  Delay(0xFFFFFF);
  
  ILI9341_Clear(0,16*8,LCD_X_LENGTH,LCD_Y_LENGTH-16*8);	/* ��������ʾȫ�� */

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

/*********************************************END OF FILE**********************/

