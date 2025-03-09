
#include "stm32f4xx_it.h"

#include "FreeRTOS.h"					//FreeRTOSʹ��		  
#include "task.h" 
#include "bsp_can.h"
#include "xcpBasic.h"
#include "bsp_exti.h"

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {}
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {}
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {}
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {}
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{}


/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
extern void xPortSysTickHandler(void);
//systick�жϷ�����
void SysTick_Handler(void)
{	
    #if (INCLUDE_xTaskGetSchedulerState  == 1 )
      if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
      {
    #endif  /* INCLUDE_xTaskGetSchedulerState */  
        xPortSysTickHandler();
    #if (INCLUDE_xTaskGetSchedulerState  == 1 )
      }
    #endif  /* INCLUDE_xTaskGetSchedulerState */
}

/******************************************************************************/
/*                 STM32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f429_439xx.s).                         */
/******************************************************************************/

extern __IO uint32_t CANRxflag;               // ��־�Ƿ���յ�����
extern CanRxMsg RxMessage;                    // CAN ���ջ�����
extern volatile uint8_t StartStopVoltageFlag; // ��־λ���ж��Ƿ񽫵�ѹ���ݴ洢��SD��

/**
  * @brief  This function handles CAN RX interrupt request.
  * @param  None
  * @retval None
  */
void CAN_RX_IRQHandler(void)
{
    /* �������ж�ȡ���� */
    CAN_Receive(CANx, CAN_FIFO0, &RxMessage);

    /* ����Ƿ�Ϊ XCP ���� */
    if ((RxMessage.ExtId == 0x1234) && 
        (RxMessage.IDE == CAN_ID_EXT) && 
        (RxMessage.DLC == 8)) 
    {
        /* ���� XCP �Ľ��մ����� */
        XcpRxHandler(RxMessage.Data);
    } 
    else if ((RxMessage.ExtId == 0x1314) && 
             (RxMessage.IDE == CAN_ID_EXT) && 
             (RxMessage.DLC == 8)) 
    {
        /* ����ԭ�е� 0x1314 ���� */
        CANRxflag = 1;  // ���ճɹ�  
    } 
    else 
    {
        CANRxflag = 0;  // ����ʧ��
    }
}
#if 0
void KEY1_IRQHandler(void)
{
  //ȷ���Ƿ������EXTI Line�ж�
	if(EXTI_GetITStatus(KEY1_INT_EXTI_LINE) != RESET) 
	{
    //����жϱ�־λ
		EXTI_ClearITPendingBit(KEY1_INT_EXTI_LINE);     
	}  
}

void KEY2_IRQHandler(void)
{
  //ȷ���Ƿ������EXTI Line�ж�
	if(EXTI_GetITStatus(KEY2_INT_EXTI_LINE) != RESET) 
	{
    //����жϱ�־λ
		EXTI_ClearITPendingBit(KEY2_INT_EXTI_LINE);     
	}  
}
#endif
/**
  * @}
  */ 

/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
