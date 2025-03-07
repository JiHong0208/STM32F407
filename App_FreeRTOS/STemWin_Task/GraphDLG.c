#include "GraphDLG.h"
#include "stdlib.h"
#include "bsp_debug_usart.h"
#include "bsp_can.h"

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define ID_FRAMEWIN_0 (GUI_ID_USER + 0x00)
#define ID_GRAPH_0    (GUI_ID_USER + 0x01)

#define ID_TEXT_1     (GUI_ID_USER + 0x02)
#define ID_TEXT_2     (GUI_ID_USER + 0x03)
#define ID_TEXT_3     (GUI_ID_USER + 0x04)
#define ID_TEXT_4     (GUI_ID_USER + 0x05)

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/
// ���ݶ�����
GRAPH_DATA_Handle Graphdata1;
GRAPH_DATA_Handle Graphdata2;
GRAPH_DATA_Handle Graphdata3;
GRAPH_DATA_Handle Graphdata4;

extern CanTxMsg TxMessage;			     //ͨ��CAN���ͳ�ȥ��ģ���ѹ����

/*********************************************************************
*
*       _aDialogCreate
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
  { FRAMEWIN_CreateIndirect, "Framewin", ID_FRAMEWIN_0, 0, 0, 240, 320, 0, 0x0, 0 },
  { GRAPH_CreateIndirect, "Graph", ID_GRAPH_0, 10, 85, 220, 200, 0, 0x0, 0 }, 
  { TEXT_CreateIndirect, "V1:", ID_TEXT_1, 10, 20, 100, 20, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "V2:", ID_TEXT_2, 120, 20, 100, 20, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "V3:", ID_TEXT_3, 10, 50, 100, 20, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "V4:", ID_TEXT_4, 120, 50, 100, 20, 0, 0x0, 0 },
};

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/

/**
  * @brief �Ի���ص�����
  * @note ��
  * @param pMsg����Ϣָ��
  * @retval ��
  */
static void _cbDialog(WM_MESSAGE* pMsg) {
  WM_HWIN hItem;
  GRAPH_SCALE_Handle hScaleV;

  switch (pMsg->MsgId) {
  case WM_INIT_DIALOG:
    /* ��ʼ��Framewin�ؼ� */
    hItem = pMsg->hWin;
    FRAMEWIN_SetText(hItem, "Voltage Value:");
    FRAMEWIN_SetFont(hItem, GUI_FONT_20_1);
    FRAMEWIN_SetTextColor(hItem, GUI_WHITE);
    FRAMEWIN_SetClientColor(hItem,GUI_BLACK);

    /* ��ʼ��Graph�ؼ� */
    hItem = WM_GetDialogItem(pMsg->hWin, ID_GRAPH_0);
    GRAPH_SetColor(hItem, GUI_BLACK, GRAPH_CI_BK);
    GRAPH_SetColor(hItem, GUI_WHITE, GRAPH_CI_GRID);
    GRAPH_SetBorder(hItem, 20, 5, 10, 10);  // ����240x320��Ļ�����߾�
    GRAPH_SetGridDistX(hItem, 30);  		// ����X��������
    GRAPH_SetGridDistY(hItem, 30); 		    // ����Y��������
    GRAPH_SetLineStyleH(hItem, GUI_LS_DOT);
    GRAPH_SetLineStyleV(hItem, GUI_LS_DOT);
    GRAPH_SetGridVis(hItem, 1);

    /* ������ֱ�̶ȶ��� */
    hScaleV = GRAPH_SCALE_Create(10, GUI_TA_HCENTER | GUI_TA_LEFT,
                                 GRAPH_SCALE_CF_VERTICAL, 50);
    GRAPH_AttachScale(hItem, hScaleV);
    GRAPH_SCALE_SetFactor(hScaleV, 0.1);

    /* �������ݶ��� */
    Graphdata1 = GRAPH_DATA_YT_Create(GUI_WHITE, 2000, 0, 0);
	Graphdata2 = GRAPH_DATA_YT_Create(GUI_YELLOW, 2000, 0, 0);
	Graphdata3 = GRAPH_DATA_YT_Create(GUI_BLUE, 2000, 0, 0);
	Graphdata4 = GRAPH_DATA_YT_Create(GUI_RED, 2000, 0, 0);
	
    GRAPH_AttachData(hItem, Graphdata1);
	GRAPH_AttachData(hItem, Graphdata2);
	GRAPH_AttachData(hItem, Graphdata3);
	GRAPH_AttachData(hItem, Graphdata4);
	
    break;

  default:
    WM_DefaultProc(pMsg);
    break;
  }
}

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/**
  * @brief �ԶԻ���ʽ��Ӵ����ؼ�
  * @note ��
  * @param ��
  * @retval hWin����Դ���е�һ���ؼ��ľ��
  */
WM_HWIN CreateFramewin2(void);
WM_HWIN CreateFramewin2(void) {
  WM_HWIN hWin;

  hWin = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);
  return hWin;
}

/**
  * @brief GUI������
  * @note ��
  * @param ��
  * @retval ��
  */
void MainTask(void)
{
  WM_HWIN hItem;
  CreateFramewin2();  						 // ��������


  while(1)
  {
	// ���� CAN ���ݣ�������ѹֵ
    Process_CAN_Voltage(TxMessage.Data);
	  
	uint16_t voltage1 = Get_CAN_Voltage(1);  // ��ȡ��ѹ1����λ������
    uint16_t voltage2 = Get_CAN_Voltage(2);  // ��ȡ��ѹ2����λ������
    uint16_t voltage3 = Get_CAN_Voltage(3);  // ��ȡ��ѹ3����λ������
    uint16_t voltage4 = Get_CAN_Voltage(4);  // ��ȡ��ѹ4����λ������

    // ������ת��Ϊ����
    float voltage1V = voltage1 / 1000.0f;
    float voltage2V = voltage2 / 1000.0f;
    float voltage3V = voltage3 / 1000.0f;
    float voltage4V = voltage4 / 1000.0f;
	  
    // Ϊÿ����������µĵ�ѹֵ����Ҫ����Y��̶����ӽ���ת��
    GRAPH_DATA_YT_AddValue(Graphdata1, (voltage1V)*10);
    GRAPH_DATA_YT_AddValue(Graphdata2, (voltage2V)*10);
    GRAPH_DATA_YT_AddValue(Graphdata3, (voltage3V)*10);
    GRAPH_DATA_YT_AddValue(Graphdata4, (voltage4V)*10);
	  
	char voltageText[20];  // �洢��ѹֵ�ı�
	// ���µ�ѹ�ı���ʾ
    sprintf(voltageText, "V1: %.2fV", voltage1V);
    hItem = WM_GetDialogItem(WM_HBKWIN, ID_TEXT_1);  // ��ȡ��ѹ1���ı��ؼ�
    TEXT_SetText(hItem, voltageText);
	TEXT_SetTextColor(hItem, GUI_WHITE); 			 // �����ı���ɫΪ��ɫ

    sprintf(voltageText, "V2: %.2fV", voltage2V);
    hItem = WM_GetDialogItem(WM_HBKWIN, ID_TEXT_2);  // ��ȡ��ѹ2���ı��ؼ�
    TEXT_SetText(hItem, voltageText);
	TEXT_SetTextColor(hItem, GUI_YELLOW);  		     // �����ı���ɫΪ��ɫ

    sprintf(voltageText, "V3: %.2fV", voltage3V);
    hItem = WM_GetDialogItem(WM_HBKWIN, ID_TEXT_3);  // ��ȡ��ѹ3���ı��ؼ�
    TEXT_SetText(hItem, voltageText);
	TEXT_SetTextColor(hItem, GUI_BLUE);  			 // �����ı���ɫΪ��ɫ

    sprintf(voltageText, "V4: %.2fV", voltage4V);
    hItem = WM_GetDialogItem(WM_HBKWIN, ID_TEXT_4);  // ��ȡ��ѹ4���ı��ؼ�
    TEXT_SetText(hItem, voltageText);
	TEXT_SetTextColor(hItem, GUI_RED);  			 // �����ı���ɫΪ��ɫ
	  
    GUI_Delay(100);  // ÿ 100 ms���һ������
  }
}

/*************************** End of file ****************************/
