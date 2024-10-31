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

static void Delay ( __IO uint32_t nCount );
void Printf_Charater(void)   ;
void Display_CAN_Voltage(uint8_t* data);

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
			
			Display_CAN_Voltage(RxMessage.Data);
			
			flag=0;
		}
	
	
	}

}


extern uint16_t lcdid;

/**
  * @brief  ����CAN���յ�����Ϊ��ѹֵ����LCD����ʾ
  * @param  data: ָ����յ���8�ֽ����ݵ�ָ��
  * @retval ��
  */
void Display_CAN_Voltage(uint8_t* data)
{
    // �������������ں�����ͷ
    uint16_t voltage1, voltage2, voltage3, voltage4;
    char displayStr[32];

    // DBC�߼�������ÿ�����ֽڵ���ת��Ϊ��ѹֵ
    voltage1 = (data[1] << 8) | data[0];
    voltage2 = (data[3] << 8) | data[2];
    voltage3 = (data[5] << 8) | data[4];
    voltage4 = (data[7] << 8) | data[6];
    
    // �ڴ��ڴ�ӡ������ĵ�ѹֵ
    printf("��ǰ��ѹΪ��\r\n");
    printf("Voltage1: %dmv\r\n", voltage1);
    printf("Voltage2: %dmv\r\n", voltage2);
    printf("Voltage3: %dmv\r\n", voltage3);
    printf("Voltage4: %dmv\r\n", voltage4);

    // �����������������ɫ
    ILI9341_Clear(0, 0, LCD_X_LENGTH, LCD_Y_LENGTH);
    LCD_SetFont(&Font16x24);
    LCD_SetColors(WHITE, BLACK);
    
    // ��LCD����ʾ��ѹֵ
    ILI9341_DispStringLine_EN(LINE(0), "Voltage Values:");
    
    snprintf(displayStr, sizeof(displayStr), "V1: %dmV", voltage1);
    ILI9341_DispStringLine_EN(LINE(1), displayStr);

    snprintf(displayStr, sizeof(displayStr), "V2: %dmV", voltage2);
    ILI9341_DispStringLine_EN(LINE(2), displayStr);

    snprintf(displayStr, sizeof(displayStr), "V3: %dmV", voltage3);
    ILI9341_DispStringLine_EN(LINE(3), displayStr);

    snprintf(displayStr, sizeof(displayStr), "V4: %dmV", voltage4);
    ILI9341_DispStringLine_EN(LINE(4), displayStr);
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

