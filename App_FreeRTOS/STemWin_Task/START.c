#include "DIALOG.h"
#include "stdlib.h"
//#include "RTC_Driver.h"  // 假设RTC时钟驱动已实现

/*********************************************************************
*       Defines
**********************************************************************/
#define ID_WINDOW_0    (GUI_ID_USER + 0x00)
#define ID_TEXT_0      (GUI_ID_USER + 0x01)
#define ID_TEXT_1      (GUI_ID_USER + 0x02)
#define ID_TEXT_2      (GUI_ID_USER + 0x03)
#define ID_TEXT_3      (GUI_ID_USER + 0x04)

/*********************************************************************
*       Static data
**********************************************************************/
static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
  { WINDOW_CreateIndirect, "Window", ID_WINDOW_0, 0, 0, 240, 320, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "Hello!", ID_TEXT_0, 70, 30, 100, 30, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "-FJUT", ID_TEXT_1, 90, 80, 80, 30, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "-ZhiFengSu", ID_TEXT_2, 60, 120, 140, 30, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "Time: ", ID_TEXT_3, 60, 200, 140, 30, 0, 0x64, 0 },
};

/*********************************************************************
*       Static code
**********************************************************************/
static void _cbDialog(WM_MESSAGE * pMsg) {
  WM_HWIN hItem;

  switch (pMsg->MsgId) {
  case WM_INIT_DIALOG:
    hItem = pMsg->hWin;
    WINDOW_SetBkColor(hItem, GUI_BLACK);

    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_0);
    TEXT_SetFont(hItem, GUI_FONT_24_ASCII);
    TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
    TEXT_SetTextColor(hItem, GUI_WHITE);
    
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_1);
    TEXT_SetFont(hItem, GUI_FONT_24_ASCII);
    TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
    TEXT_SetTextColor(hItem, GUI_WHITE);
    
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_2);
    TEXT_SetFont(hItem, GUI_FONT_24_ASCII);
    TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
    TEXT_SetTextColor(hItem, GUI_WHITE);
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
  * @brief  主任务函数，周期性更新时间显示
  * @note 无
  * @param 无
  * @retval 无
  */
void LCD_Start(void) 
{
  WM_HWIN hItem;
  hItem = CreateFramewin1();   // 创建窗口
  GUI_SetOrientation(GUI_ROTATION_180);  // 设置屏幕旋转180度

  while (1)
  {
	char timeStr[20];// 存储RTC时钟文本
	//sprintf(timeStr,RTCTime);
	hItem = WM_GetDialogItem(WM_HBKWIN, ID_TEXT_3);
    TEXT_SetText(hItem, timeStr);
    TEXT_SetTextColor(hItem, GUI_WHITE);
	  
    GUI_Delay(100);  // 每100ms更新一次
  }
}
