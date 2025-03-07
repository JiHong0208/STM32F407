// FreeRTOSͷ�ļ�
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"

// ������Ӳ��bspͷ�ļ�
#include "bsp_debug_usart.h"
#include "bsp_ili9341_lcd.h"
#include "bsp_sram.h"	  
#include "bsp_led.h"
#include "bsp_key.h"
#include "bsp_adc.h"
#include "bsp_can.h"
#include "bsp_rtc.h"
#include "sd.h"

// STemWINͷ�ļ�
#include "GUI.h"
#include "DIALOG.h"
#include "START.h"

// XCPDriverͷ�ļ�
#include "xcpBasic.h"

/**************************** ������ ********************************/
/* 
 * ��������һ��ָ�룬����ָ��һ�����񣬵����񴴽���֮�����;�����һ��������
 * �Ժ�����Ҫ��������������Ҫͨ�������������������������������Լ�����ô
 * ����������ΪNULL��
 */
/* ���������� */
static TaskHandle_t AppTaskCreate_Handle = NULL;
/* CAN ������ */
static TaskHandle_t CAN_Task_Handle = NULL;
/* XCP_Driver ������ */
static TaskHandle_t XCP_Driver_Task_Handle = NULL;
/* GUI ������ */
static TaskHandle_t GUI_Task_Handle = NULL;
/* SD_Card ������ */
static TaskHandle_t SD_Card_Task_Handle = NULL;

/********************************** �ں˶����� *********************************/
/*
 * �ź�������Ϣ���У��¼���־�飬�����ʱ����Щ�������ں˵Ķ���Ҫ��ʹ����Щ�ں�
 * ���󣬱����ȴ����������ɹ�֮��᷵��һ����Ӧ�ľ����ʵ���Ͼ���һ��ָ�룬������
 * �ǾͿ���ͨ��������������Щ�ں˶���
 *
 * �ں˶���˵���˾���һ��ȫ�ֵ����ݽṹ��ͨ����Щ���ݽṹ���ǿ���ʵ��������ͨ�ţ�
 * �������¼�ͬ���ȸ��ֹ��ܡ�������Щ���ܵ�ʵ��������ͨ��������Щ�ں˶���ĺ���
 * ����ɵ�
 * 
 */
SemaphoreHandle_t CanReadySem_Handle   = NULL;// �ź������

 
// ���� App ����ʼ��ַ��
#define APP_BASE_ADDRESS  0x08020000

// ����ȫ�ֱ���
volatile uint32_t DAQ_Timestamp = 0; 		// XCP��DAQʱ�������λ��10ms
__IO uint32_t CANRxflag = 0;	            // ���ڱ�־�Ƿ���յ����ݣ����жϺ����и�ֵ
CanTxMsg TxMessage;			                // ���ͻ�����
CanRxMsg RxMessage;				            // ���ջ�����



// ������ʼ������
static void App_Init(void);                     // ����Bootloader��ת��App
static void BSP_Init(void);						// ���ڳ�ʼ�����������Դ

	
static void AppTaskCreate(void);				// ���ڴ�������
static void CAN_Task(void* parameter);			// CAN_Task����ʵ�� 
static void GUI_Task(void* parameter);			// GUI_Task����ʵ�� 
static void XCP_Driver_Task(void* parameter);   // XCP_Driver����ʵ��
static void SD_Card_Task(void* parameter);      // SD_Card����ʵ��

/**
  * @brief  ������
  * @param  ��
  * @retval ��
  * @note   
  */
int main(void)
{	
	BaseType_t xReturn = pdPASS;// ����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS
  
    // ����APP���ж������������ȫ���ж�
	App_Init();

    // ��ʼ��Ӳ��
	BSP_Init();
	
    // ��ʼ��Ӳ��,����AppTaskCreate����
	xReturn = xTaskCreate((TaskFunction_t )AppTaskCreate,  		// ������ں��� 
                        (const char*    )"AppTaskCreate",		// ��������
                        (uint16_t       )512,  			 		// ����ջ��С 
                        (void*          )NULL,			 		// ������ں������� 
                        (UBaseType_t    )1, 				    // ��������ȼ� 
                        (TaskHandle_t*  )&AppTaskCreate_Handle);// ������ƿ�ָ�� 
						
    // �����������            
	if(pdPASS == xReturn)
		vTaskStartScheduler();   // �������񣬿�������
	else
	return -1;  
  
	while(1);   // ��������ִ�е�����    
}


/**
  * @brief  AppTaskCreate
  * @param  ��
  * @retval ��
  * @note   Ϊ�˷���������е����񴴽����������������������
  */
static void AppTaskCreate(void)
{
	BaseType_t xReturn = pdPASS;// ����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS 
	
	taskENTER_CRITICAL();//�����ٽ���
	
	// ����ScreenShotSem�ź���
	CanReadySem_Handle = xSemaphoreCreateBinary();
	
	if (CanReadySem_Handle != NULL)
    {
        printf("CanReadySem�ź��������ɹ���\r\n");
    }
	
	// ��һ�� CAN ����
	xReturn = xTaskCreate((TaskFunction_t)CAN_Task,						 		// ������ں��� 
											 (const char*    )"CAN_Task",		// �������� 
											 (uint16_t       )128,       		// ����ջ��С 
											 (void*          )NULL,      		// ������ں������� 
											 (UBaseType_t    )5,         		// ��������ȼ� 
											 (TaskHandle_t   )&CAN_Task_Handle);// ������ƿ�ָ�� 
	if(pdPASS == xReturn)
		printf("����CAN_Task����ɹ���\r\n");
	
	// �ڶ��� XCP_Driver ����
    xReturn = xTaskCreate((TaskFunction_t)XCP_Driver_Task,						// ������ں��� 
											 (const char*      )"XCP_Driver_Task",// ��������
											 (uint16_t         )256,     		// ����ջ��С
											 (void*            )NULL,    		// ������ں�������
											 (UBaseType_t      )4,       		// ��������ȼ�
											 (TaskHandle_t     )&XCP_Driver_Task_Handle);// ������ƿ�ָ��
	if(pdPASS == xReturn)
		printf("����XCP_Driver_Task����ɹ���\r\n");
	
	// ������ GUI_Task����
	xReturn = xTaskCreate((TaskFunction_t)GUI_Task,						 		// ������ں��� 
											 (const char*    )"GUI_Task",		// �������� 
											 (uint16_t       )1024,       		// ����ջ��С 
											 (void*          )NULL,      		// ������ں������� 
											 (UBaseType_t    )2,         		// ��������ȼ� 
											 (TaskHandle_t   )&GUI_Task_Handle);// ������ƿ�ָ�� 
											 
	if(pdPASS == xReturn)
		printf("����GUI_Task����ɹ���\r\n");
	
	// ���ĸ� SD_Card_Task����
	xReturn = xTaskCreate((TaskFunction_t)SD_Card_Task,						 	// ������ں��� 
											 (const char*    )"SD_Card_Task",	// �������� 
											 (uint16_t       )256,       		// ����ջ��С 
											 (void*          )NULL,      		// ������ں������� 
											 (UBaseType_t    )3,         		// ��������ȼ� 
											 (TaskHandle_t   )&SD_Card_Task_Handle);// ������ƿ�ָ�� 
											 
	if(pdPASS == xReturn)
		printf("����SD_Card_Task����ɹ���\r\n");
	
	vTaskDelete(AppTaskCreate_Handle);// ɾ��AppTaskCreate����
	
	taskEXIT_CRITICAL();// �˳��ٽ���
}

/**
  * @brief CAN��������
  * @note ��
  * @param ��
  * @retval ��
  */
static void CAN_Task(void* parameter)
{
	// CAN ������ɳ�ʼ����֪ͨ XCP ��������ź���
    xSemaphoreGive(CanReadySem_Handle);
	
	while(1)
	{
		SendCANEvent();
		vTaskDelay(100);
	}
}

/**
  * @brief XCP_Driver��������
  * @note ��
  * @param ��
  * @retval ��
  */
static void XCP_Driver_Task(void* parameter)
{
    // �ȴ� CAN ����׼����
    xSemaphoreTake(CanReadySem_Handle, portMAX_DELAY);  // ����ֱ�� CAN ����֪ͨ
	
	while(1)
	{
		DAQ_Timestamp++; // ʱ�������(10ms ��λ)
		XcpEvent(0);     // ֪ͨ XCP �¼���10ms�¼�
		vTaskDelay(10);
	}
}

/**
  * @brief GUI��������
  * @note ��
  * @param ��
  * @retval ��
  */
static void GUI_Task(void* parameter)
{
	// ��ʼ��LCD��Ļ 
	GUI_Init(); 
	
	// ��LCD�����
	ILI9341_BackLed_Control ( ENABLE );
	
	while(1)
	{
		MainTask();
		//LCD_Start();                 
	}
}

/**
  * @brief SD_Card��������
  * @note ��
  * @param ��
  * @retval ��
  */
static void SD_Card_Task(void* parameter)
{
	// �ȴ� CAN ����׼����
	xSemaphoreTake(CanReadySem_Handle, portMAX_DELAY);  // ����ֱ�� CAN ����֪ͨ	
	
	while(1)
	{
		SD_MainFunction();
		vTaskDelay(100);
	}
}

static void App_Init(void)
{
	#define VECT_TAB_OFFSET  0x0000
	
    // �����������ַΪ app ����ʼ��ַ
    SCB->VTOR = APP_BASE_ADDRESS | VECT_TAB_OFFSET;

    // ��� FAULTMASK������ȫ���ж�
    __set_FAULTMASK(0);
    __enable_irq();
	
	// ָʾ���� App
	printf("\r\n----------------------���뵽App����----------------------\r\n");
	
}

/**
  * @brief �弶�����ʼ��
  * @note ���а����ϵĳ�ʼ�����ɷ��������������
  * @param ��
  * @retval ��
  */
static void BSP_Init(void)
{
	// ��ʼ��LED
	LED_GPIO_Config();
	
    // ��ʼ��USART1
    Debug_USART_Config();
		
	// ��ʼ������
	Key_GPIO_Config();
		
	// ��ʼ��CAN,���жϽ���CAN���ݰ�
	CAN_Config();
	
	// ��ʼ��XCPЭ��ջ
	XcpInit();

    // ���� CRC У�飬���� emWin �Ᵽ�� 
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_CRC, ENABLE);  
  	
	// SD����ʽ�����ԣ����SD��������
	SD_Check();
	
	// SRAM��ʼ��
    FSMC_SRAM_Init();
  
    // ADC��ʼ��
	//Rheostat_Init();
	
	// RTCʱ�ӳ�ʼ��
	RTC_CLK_Config();
	
	// RTC���蹦�ܳ�ʼ��
	RTC_Initialize();

	/*
	 * STM32�ж����ȼ�����Ϊ4����4bit��������ʾ��ռ���ȼ�����ΧΪ��0~15
	 * ���ȼ�����ֻ��Ҫ����һ�μ��ɣ��Ժ������������������Ҫ�õ��жϣ�
	 * ��ͳһ��������ȼ����飬ǧ��Ҫ�ٷ��飬�мɡ�
	 */
	NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 );
}

/********************************END OF FILE****************************/
