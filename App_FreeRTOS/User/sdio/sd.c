#include "sd.h"
#include "ff.h"
#include <string.h>
#include "bsp_can.h"
#include "bsp_rtc.h"
#include "bsp_debug_usart.h"

/**
  *******************************************************************************
  *                              定义变量
  *******************************************************************************
  */
FATFS fs;                // FatFs文件系统对象
FIL fnew;                // 文件对象
FRESULT res_sd;          // 文件操作结果
UINT fnum;               // 文件成功读写数量
extern CanTxMsg TxMessage;			     //通过CAN发送出去的模拟电压数据


void SD_MainFunction()
{
    // 处理 CAN 数据，解析电压值
    Process_CAN_Voltage(TxMessage.Data);

    // 获取电压值
    uint16_t voltage1 = Get_CAN_Voltage(1);  
    uint16_t voltage2 = Get_CAN_Voltage(2);  
    uint16_t voltage3 = Get_CAN_Voltage(3);  
    uint16_t voltage4 = Get_CAN_Voltage(4);  

    char dateStr[20];  // 存储RTC日期文本 YYYY/MM/DD
    char weekStr[20];  // 存储RTC星期文本
    char timeStr[20];  // 存储RTC时钟文本 HH:MM:SS

    RTC_TimeAndDate_Get(dateStr, timeStr, weekStr);

	// 提取日期信息并构造文件名
	int year, month, day;
	sscanf(dateStr, "%d-%d-%d", &year, &month, &day);  // 解析 YYYY-MM-DD 格式
	char fileName[64];
	snprintf(fileName, sizeof(fileName), "0:STM32F407_BMS_Voltage_%04d%02d%02d.csv", year, month, day);


    // 格式化写入内容
    char WriteBuffer[128];
    snprintf(WriteBuffer, sizeof(WriteBuffer), 
             "%s,%s,%s,%d,%d,%d,%d\r\n",
             dateStr, weekStr, timeStr, voltage1, voltage2, voltage3, voltage4);

    // 1. 检查 SD 卡容量是否小于 1GB
    DWORD free_clusters, free_sectors;
    FATFS* fs_ptr;
    
    res_sd = f_getfree("0:", &free_clusters, &fs_ptr);
    if (res_sd == FR_OK) 
	{
        free_sectors = free_clusters * fs_ptr->csize;
        if (free_sectors < (1024 * 1024 * 2)) {  // 小于 1GB（假设扇区大小512字节）
            DeleteOldCSV(year, month, day);
        }
    } 

    // 2. 打开当天的 CSV 文件
    res_sd = f_open(&fnew, fileName, FA_OPEN_ALWAYS );
    if (res_sd == FR_OK)
    {
        /* 移动到文件末尾，确保新数据追加写入 */
        f_lseek(&fnew, f_size(&fnew));

        /* 如果是新文件，则写入表头 */
        if (f_size(&fnew) == 0) {
            char header[] = "Date,WeekDay,Time,Voltage1(mv),Voltage2(mv),Voltage3(mv),Voltage4(mv)\r\n";
            f_write(&fnew, header, strlen(header), &fnum);
        }

        /* 写入电压数据 */
        f_write(&fnew, WriteBuffer, strlen(WriteBuffer), &fnum);

        /* 关闭文件 */
        f_close(&fnew);
    }

}

void DeleteOldCSV(int currentYear, int currentMonth, int currentDay)
{
    // 计算一年前的日期
    int oldYear = currentYear - 1;
    int oldMonth = currentMonth;
    int oldDay = currentDay;

    // 构造一年前的文件名
    char oldFileName[64];
    snprintf(oldFileName, sizeof(oldFileName), "0:STM32F407_BMS_Voltage_%04d%02d%02d.csv", oldYear, oldMonth, oldDay);

    // 删除文件
    res_sd = f_unlink(oldFileName);
}

void SD_Check()
{
    printf("\r\n----- 这是一个SD卡格式化测试，用于检测SD卡可用性 ------\r\n");
    // 挂载文件系统，文件系统挂载时会对设备初始化
    res_sd = f_mount(&fs, "0:", 1);
    
    /*----------------------- 格式化测试 ---------------------------*/  
    /* 如果没有文件系统就格式化创建创建文件系统 */
    if (res_sd == FR_NO_FILESYSTEM)
    {
        printf("--------- SD卡还没有文件系统，即将进行格式化 ---------\r\n");
        /* 格式化 */
        res_sd = f_mkfs("0:", 0, 0);
        
        if (res_sd == FR_OK)
        {
            printf("------------ SD卡已成功格式化文件系统 ------------\r\n");
            /* 格式化后，先取消挂载 */
            res_sd = f_mount(NULL, "0:", 1);            
            /* 重新挂载 */
            res_sd = f_mount(&fs, "0:", 1);
        }
        else
        {
            printf("---------------- 格式化失败 ----------------\r\n");
        }
    }
    else if (res_sd != FR_OK)
    {
        printf("！！！SD卡挂载文件系统失败 ！！！(%d)\r\n", res_sd);
        printf("！！！可能原因：SD卡初始化不成功 ！！！\r\n");
    }
    else
    {
        printf("----------- 文件系统挂载成功，可以进行读写测试 ----------\r\n");
    }
}

