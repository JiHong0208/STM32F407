#include "sd.h"
#include "ff.h"
#include <string.h>
#include "bsp_can.h"
#include "bsp_debug_usart.h"

/**
  ******************************************************************************
  *                              �������
  ******************************************************************************
  */
FATFS fs;													// FatFs�ļ�ϵͳ����
FIL fnew;													// �ļ�����
FRESULT res_sd;                                             // �ļ��������
UINT fnum;            					                    // �ļ��ɹ���д����



void SD_Function()
{
    // ��ȡ��ѹֵ
    uint16_t voltage1 = Get_CAN_Voltage(1);  
    uint16_t voltage2 = Get_CAN_Voltage(2);  
    uint16_t voltage3 = Get_CAN_Voltage(3);  
    uint16_t voltage4 = Get_CAN_Voltage(4);  
    
    // ��ʽ��д�����ݣ�Ԥ��ʱ��λ
    char WriteBuffer[128];
    snprintf(WriteBuffer, sizeof(WriteBuffer), 
             "Time,Voltage1(mv),Voltage2(mv),Voltage3(mv),Voltage4(mv)\r\n"
             ",%d,%d,%d,%d\r\n",
             voltage1, voltage2, voltage3, voltage4);

    // �����ļ�ϵͳ
    res_sd = f_mount(&fs, "0:", 1);
    
    /* ���ļ�������ļ��������򴴽���������׷��д�� */
    res_sd = f_open(&fnew, "0:STM32F407_BMS_Voltage.csv", FA_CREATE_ALWAYS | FA_WRITE);
    if (res_sd == FR_OK)
    {
        /* д���ѹ���� */
        f_write(&fnew, WriteBuffer, strlen(WriteBuffer), &fnum);
        
        /* �ر��ļ� */
        f_close(&fnew);
    }

    /* ȡ�������ļ�ϵͳ */
    f_mount(NULL, "0:", 1);
}


void SD_Test()
{
	printf("\r\n------ ����һ��SD����ʽ�����ԣ����ڼ��SD�������� ------\r\n");
	//�����ļ�ϵͳ���ļ�ϵͳ����ʱ����豸��ʼ��
	res_sd = f_mount(&fs,"0:",1);// f_mount�ĵ�һ��������ʾ�ļ�ϵͳ���󣬵ڶ���������ʾ�洢�豸��ţ�������������ʾ�Ƿ����
	
	/*----------------------- ��ʽ������ ---------------------------*/  
	/* ���û���ļ�ϵͳ�͸�ʽ�����������ļ�ϵͳ */
	if(res_sd == FR_NO_FILESYSTEM)
	{
		printf("*** SD����û���ļ�ϵͳ���������и�ʽ��... ***\r\n");
    /* ��ʽ�� */
		res_sd=f_mkfs("0:",0,0);							
		
		if(res_sd == FR_OK)
		{
			printf("*** SD���ѳɹ���ʽ���ļ�ϵͳ�� ***\r\n");
      /* ��ʽ������ȡ������ */
			res_sd = f_mount(NULL,"0:",1);			
      /* ���¹���	*/			
			res_sd = f_mount(&fs,"0:",1);
		}
		else
		{
			printf("*** ��ʽ��ʧ�ܡ� ***\r\n");
			while(1);
		}
	}
	else if(res_sd!=FR_OK)
  {
    printf("����SD�������ļ�ϵͳʧ�ܡ�(%d)\r\n",res_sd);
    printf("��������ԭ��SD����ʼ�����ɹ���\r\n");
		while(1);
  }
  else
  {
    printf("*** �ļ�ϵͳ���سɹ������Խ��ж�д���� ***\r\n");
  }
  
}
