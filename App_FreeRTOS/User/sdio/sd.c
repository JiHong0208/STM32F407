#include "sd.h"
#include "ff.h"
#include <string.h>
#include "bsp_can.h"
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

void SD_MainFunction()
{
    // 获取电压值
    uint16_t voltage1 = Get_CAN_Voltage(1);  
    uint16_t voltage2 = Get_CAN_Voltage(2);  
    uint16_t voltage3 = Get_CAN_Voltage(3);  
    uint16_t voltage4 = Get_CAN_Voltage(4);  
    
    // 格式化写入内容，删除时间信息
    char WriteBuffer[128];
    snprintf(WriteBuffer, sizeof(WriteBuffer), 
             "Voltage1(mv),Voltage2(mv),Voltage3(mv),Voltage4(mv)\r\n"
             ",%d,%d,%d,%d\r\n",
             voltage1, voltage2, voltage3, voltage4);

    // 确保文件系统已挂载
    res_sd = f_mount(&fs, "0:", 1);
    if (res_sd != FR_OK) {
        printf("SD卡挂载失败! 错误代码：%d\r\n", res_sd);
        return;
    }

    /* 打开文件，如果文件不存在则创建它，否则追加写入 */
    res_sd = f_open(&fnew, "0:STM32F407_BMS_Voltage.csv", FA_CREATE_ALWAYS | FA_WRITE);
    if (res_sd == FR_OK)
    {
        /* 写入电压数据 */
        f_write(&fnew, WriteBuffer, strlen(WriteBuffer), &fnum);
        
        /* 关闭文件 */
        f_close(&fnew);
    }
    else
    {
        printf("文件打开失败! 错误代码：%d\r\n", res_sd);
    }

    /* 取消挂载文件系统 */
    f_mount(NULL, "0:", 1);
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
            while (1);
        }
    }
    else if (res_sd != FR_OK)
    {
        printf("！！！SD卡挂载文件系统失败 ！！！(%d)\r\n", res_sd);
        printf("！！！可能原因：SD卡初始化不成功 ！！！\r\n");
        while (1);
    }
    else
    {
        printf("-------- 文件系统挂载成功，可以进行读写测试 --------\r\n");
    }
}
