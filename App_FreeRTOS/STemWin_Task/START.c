#include "DIALOG.h"
#include "stdlib.h"
#include "bsp_rtc.h"
#include "bsp_key.h"

/*********************************************************************
*       Defines
**********************************************************************/
#define ID_FRAMEWIN_0  (GUI_ID_USER + 0x00)
#define ID_TEXT_0      (GUI_ID_USER + 0x01)
#define ID_TEXT_1      (GUI_ID_USER + 0x02)
#define ID_TEXT_2      (GUI_ID_USER + 0x03)
#define ID_TEXT_3      (GUI_ID_USER + 0x04)
#define ID_TEXT_4      (GUI_ID_USER + 0x05)
#define ID_TEXT_5      (GUI_ID_USER + 0x06)
#define ID_TEXT_6      (GUI_ID_USER + 0x07)

extern __IO uint8_t key1_pressed_flag;  //ͨ��key1��ѹ��־������ѭ��

/*********************************************************************
*       Static data
**********************************************************************/
static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
    { FRAMEWIN_CreateIndirect, "FRAMEWIN", ID_FRAMEWIN_0, 0, 0, 240, 320, 0, 0x0, 0 },
    { TEXT_CreateIndirect, "Date:", ID_TEXT_0, 0, 10, 240, 30, 0, 0x64, 0 },
    { TEXT_CreateIndirect, "Weekday:", ID_TEXT_1, 0, 50, 240, 30, 0, 0x64, 0 },
    { TEXT_CreateIndirect, "Time:", ID_TEXT_2, 0, 120, 240, 50, 0, 0x64, 0 },  
    { TEXT_CreateIndirect, "Welcome to FJUT", ID_TEXT_3, 0, 180, 240, 30, 0, 0x64, 0 },
    { TEXT_CreateIndirect, "Author: ZhiFengSu", ID_TEXT_4, 0, 220, 240, 30, 0, 0x64, 0 },
    { TEXT_CreateIndirect, "Graduation Design", ID_TEXT_5, 0, 250, 240, 30, 0, 0x64, 0 },
};

/*********************************************************************
*       Static code
**********************************************************************/
static void _cbDialog(WM_MESSAGE * pMsg) {
    WM_HWIN hItem;

    switch (pMsg->MsgId) {
    case WM_INIT_DIALOG:
        /* ��ʼ��Framewin�ؼ� */
        hItem = pMsg->hWin;
        FRAMEWIN_SetText(hItem, "LCD_Start");
        FRAMEWIN_SetFont(hItem, GUI_FONT_20_1);
        FRAMEWIN_SetTextColor(hItem, GUI_WHITE);
        FRAMEWIN_SetClientColor(hItem, GUI_MAKE_COLOR(0xC4B8B6));

        hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_0);
        TEXT_SetFont(hItem, GUI_FONT_16_ASCII);
        TEXT_SetTextAlign(hItem, GUI_TA_CENTER | GUI_TA_VCENTER);
        TEXT_SetTextColor(hItem, GUI_BLACK);
        
        hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_1);
        TEXT_SetFont(hItem, GUI_FONT_16_ASCII);
        TEXT_SetTextAlign(hItem, GUI_TA_CENTER | GUI_TA_VCENTER);
        TEXT_SetTextColor(hItem, GUI_BLACK);
        
        hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_2);
        TEXT_SetFont(hItem, GUI_FONT_32_ASCII);  
        TEXT_SetTextAlign(hItem, GUI_TA_CENTER | GUI_TA_VCENTER);
        TEXT_SetTextColor(hItem, GUI_WHITE);  
        
        hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_3);
        TEXT_SetFont(hItem, GUI_FONT_16_ASCII); 
        TEXT_SetTextAlign(hItem, GUI_TA_CENTER | GUI_TA_VCENTER);
        TEXT_SetTextColor(hItem, GUI_BLACK);

        hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_4);
        TEXT_SetFont(hItem, GUI_FONT_16_ASCII); 
        TEXT_SetTextAlign(hItem, GUI_TA_CENTER | GUI_TA_VCENTER);
        TEXT_SetTextColor(hItem, GUI_BLACK);

        hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_5);
        TEXT_SetFont(hItem, GUI_FONT_16_ASCII); 
        TEXT_SetTextAlign(hItem, GUI_TA_CENTER | GUI_TA_VCENTER);
        TEXT_SetTextColor(hItem, GUI_BLACK);

        break;

    default:
        WM_DefaultProc(pMsg);
        break;
    }
}

/*********************************************************************
*       Public code
**********************************************************************/
WM_HWIN CreateFramewin1(void);
WM_HWIN CreateFramewin1(void) {
    WM_HWIN hWin;

    hWin = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);
    return hWin;
}

/**
  * @brief  ���������������Ը���ʱ���������ʾ
  * @note ��
  * @param ��
  * @retval ��
  */
void LCD_Start(void) 
{
    WM_HWIN hItem;

    hItem = CreateFramewin1();   // ��������

    while (1)
    {
        char dateStr[20];  	   // �洢RTC�����ı�
        char weekStr[20];  	   // �洢RTC�����ı�
        char timeStr[20];  	   // �洢RTCʱ���ı�
		char displayDate[30];  // �洢��ǰ׺�������ı�
		char displayWeek[30];  // �洢��ǰ׺�������ı�
		char displayTime[30];  // �洢��ǰ׺��ʱ���ı�

		RTC_TimeAndDate_Get(dateStr, timeStr, weekStr);
		sprintf(displayDate, "Date: %s", dateStr);
		sprintf(displayWeek, "WeekDay: %s", weekStr);
		sprintf(displayTime, "Time: %s", timeStr);
        
        
        hItem = WM_GetDialogItem(WM_HBKWIN, ID_TEXT_0);
        TEXT_SetText(hItem, displayDate);  // ���������ı�
        TEXT_SetTextColor(hItem, GUI_BLACK);
        
        hItem = WM_GetDialogItem(WM_HBKWIN, ID_TEXT_1);
        TEXT_SetText(hItem, displayWeek);  // ���������ı�
        TEXT_SetTextColor(hItem, GUI_BLACK);
        
        hItem = WM_GetDialogItem(WM_HBKWIN, ID_TEXT_2);
        TEXT_SetText(hItem, displayTime);  // ����ʱ���ı�
        TEXT_SetTextColor(hItem, GUI_WHITE);  
        
		GUI_Delay(1000); // ��ʱ1�룬ģ�����ڸ���ʱ�������
		
    }
}

