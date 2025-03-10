#include "sd.h"
#include "ff.h"
#include <string.h>
#include "bsp_can.h"
#include "bsp_rtc.h"
#include "bsp_debug_usart.h"

/**
  *******************************************************************************
  *                              �������
  *******************************************************************************
  */
FATFS fs;               	 // FatFs�ļ�ϵͳ����
FIL fnew;               	 // �ļ�����
FRESULT res_sd;         	 // �ļ��������
UINT fnum;              	 // �ļ��ɹ���д����
extern CanTxMsg TxMessage;	 // ͨ��CAN���ͳ�ȥ��ģ���ѹ����


void SD_MainFunction()
{
    // ���� CAN ���ݣ�������ѹֵ
    Process_CAN_Voltage(TxMessage.Data);

    // ��ȡ��ѹֵ
    uint16_t voltage1 = Get_CAN_Voltage(1);  
    uint16_t voltage2 = Get_CAN_Voltage(2);  
    uint16_t voltage3 = Get_CAN_Voltage(3);  
    uint16_t voltage4 = Get_CAN_Voltage(4);  

    char dateStr[20];  // �洢RTC�����ı� YYYY/MM/DD
    char weekStr[20];  // �洢RTC�����ı�
    char timeStr[20];  // �洢RTCʱ���ı� HH:MM:SS

    RTC_TimeAndDate_Get(dateStr, timeStr, weekStr);

	// ��ȡ������Ϣ�������ļ���
	int year, month, day;
	sscanf(dateStr, "%d-%d-%d", &year, &month, &day);  // ���� YYYY-MM-DD ��ʽ
	char fileName[64];
	snprintf(fileName, sizeof(fileName), "0:STM32F407_BMS_Voltage_%04d%02d%02d.csv", year, month, day);


    // ��ʽ��д������
    char WriteBuffer[128];
    snprintf(WriteBuffer, sizeof(WriteBuffer), 
             "%s,%s,%s,%d,%d,%d,%d\r\n",
             dateStr, weekStr, timeStr, voltage1, voltage2, voltage3, voltage4);

    // 1. ��� SD �������Ƿ�С�� 1GB
    DWORD free_clusters, free_sectors;
    FATFS* fs_ptr;
    
    res_sd = f_getfree("0:", &free_clusters, &fs_ptr);
    if (res_sd == FR_OK) 
	{
        free_sectors = free_clusters * fs_ptr->csize;
        if (free_sectors < (1024 * 1024 * 2)) {  // С�� 1GB������������С512�ֽڣ�
            DeleteOldCSV(year, month, day);
        }
    } 

	// 2. �򿪵���� CSV �ļ�
	res_sd = f_open(&fnew, fileName, FA_OPEN_ALWAYS | FA_WRITE);
	if (res_sd == FR_OK)
	{
		// �ƶ����ļ�ĩβ��ȷ��������׷��д��
		f_lseek(&fnew, f_size(&fnew));

		// ��������ļ�����д���ͷ
		if (f_size(&fnew) == 0) 
		{
			char header[] = "Date,WeekDay,Time,Voltage1(mv),Voltage2(mv),Voltage3(mv),Voltage4(mv)\r\n";
			res_sd = f_write(&fnew, header, strlen(header), &fnum);
		}

		// д���ѹ����
		res_sd = f_write(&fnew, WriteBuffer, strlen(WriteBuffer), &fnum);

		// ȷ������д�� SD ��
		f_sync(&fnew);

		// �ر��ļ�
		f_close(&fnew);
	}
}	

void DeleteOldCSV(int currentYear, int currentMonth, int currentDay)
{
    DIR dir;
    FILINFO fno;
    char path[] = "0:";  // SD����Ŀ¼
    int oldestYear = currentYear; // ��¼��������

    // ��Ŀ¼
    if (f_opendir(&dir, path) == FR_OK)
    {
        while (f_readdir(&dir, &fno) == FR_OK && fno.fname[0])
        {
            int year, month, day;
            // �����ļ��������� STM32F407_BMS_Voltage_20240310.csv
            if (sscanf(fno.fname, "STM32F407_BMS_Voltage_%4d%2d%2d.csv", &year, &month, &day) == 3)
            {
                if (year < oldestYear)
                {
                    oldestYear = year; // ��¼��������
                }
            }
        }
        f_closedir(&dir);
    }

    // ����ҵ��˱ȵ�ǰ��ݸ�������ݣ���ɾ�����и�����ļ�
    if (oldestYear < currentYear)
    {
        if (f_opendir(&dir, path) == FR_OK)
        {
            while (f_readdir(&dir, &fno) == FR_OK && fno.fname[0])
            {
                int year, month, day;
                if (sscanf(fno.fname, "STM32F407_BMS_Voltage_%4d%2d%2d.csv", &year, &month, &day) == 3)
                {
                    if (year == oldestYear)
                    {
                        char oldFileName[64];
                        snprintf(oldFileName, sizeof(oldFileName), "0:%s", fno.fname);
                        f_unlink(oldFileName); // ɾ������ݵ��ļ�
                    }
                }
            }
            f_closedir(&dir);
        }
    }
}

void SD_Check()
{
    printf("\r\n----- ����һ��SD����ʽ�����ԣ����ڼ��SD�������� ------\r\n");
    // �����ļ�ϵͳ���ļ�ϵͳ����ʱ����豸��ʼ��
    res_sd = f_mount(&fs, "0:", 1);
    
    /*----------------------- ��ʽ������ ---------------------------*/  
    /* ���û���ļ�ϵͳ�͸�ʽ�����������ļ�ϵͳ */
    if (res_sd == FR_NO_FILESYSTEM)
    {
        printf("--------- SD����û���ļ�ϵͳ���������и�ʽ�� ---------\r\n");
        /* ��ʽ�� */
        res_sd = f_mkfs("0:", 0, 0);
        
        if (res_sd == FR_OK)
        {
            printf("------------ SD���ѳɹ���ʽ���ļ�ϵͳ ------------\r\n");
            /* ��ʽ������ȡ������ */
            res_sd = f_mount(NULL, "0:", 1);            
            /* ���¹��� */
            res_sd = f_mount(&fs, "0:", 1);
        }
        else
        {
            printf("---------------- ��ʽ��ʧ�� ----------------\r\n");
        }
    }
    else if (res_sd != FR_OK)
    {
        printf("������SD�������ļ�ϵͳʧ�� ������(%d)\r\n", res_sd);
        printf("����������ԭ��SD����ʼ�����ɹ� ������\r\n");
    }
    else
    {
        printf("----------- �ļ�ϵͳ���سɹ������Խ��ж�д���� ----------\r\n");
    }
}

