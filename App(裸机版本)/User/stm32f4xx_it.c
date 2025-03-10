/**
  ******************************************************************************
  * @file    FMC_SDRAM/stm32f4xx_it.c 
  * @author  MCD Application Team
  * @version V1.0.1
  * @date    11-November-2013
  * @brief   Main Interrupt Service Routines.
  *         This file provides template for all exceptions handler and
  *         peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "sd.h"
#include "GUI.h"
#include "bsp_can.h"
#include "xcpBasic.h"
#include "stm32f4xx_it.h"
#include "bsp_basic_tim.h"


/** @addtogroup STM32F429I_DISCOVERY_Examples
  * @{
  */

/** @addtogroup FMC_SDRAM
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

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
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{}

/**
  * @brief  This function handles PendSV_Handler exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{}

/******************************************************************************/
/*                 STM32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f429_439xx.s).                         */
/******************************************************************************/
/**
  * @}
  */ 
	/**
  * @brief  This function handles TIM interrupt request.
  * @param  None
  * @retval None
  */
	
extern volatile GUI_TIMER_TIME OS_TimeMS; // emWin ϵͳʱ�������
extern __IO uint32_t CANRxflag;           // ��־�Ƿ���յ�����
extern CanRxMsg RxMessage;                // CAN ���ջ�����
extern volatile uint8_t StartStopVoltageFlag; // ��־λ���ж��Ƿ񽫵�ѹ���ݴ洢��SD��

/**
  * @brief  This function handles TIM interrupt request.
  * @param  None
  * @retval None
  */
uint32_t Measurement=0; //XCP�۲���
volatile uint32_t DAQ_Timestamp = 0; // XCP��DAQ ʱ�������λ��10ms

void BASIC_TIM_IRQHandler(void)
{
	static uint8_t counter = 0; // �жϼ�����
	static uint8_t CANSendcount = 0; //����CAN���ķ������ڼ�����
	
    OS_TimeMS++;  // emWin ϵͳʱ�����
	Measurement++;//XCP�۲���
	counter++;
	
    if (TIM_GetITStatus(BASIC_TIM, TIM_IT_Update) != RESET) {
        TIM_ClearITPendingBit(BASIC_TIM, TIM_IT_Update);
    }
	//ͨ������10���жϣ�ʵ��10ms�¼�
	if (counter >= 10) 
	{ 
        counter = 0;     // ���ü�����
        DAQ_Timestamp++; // ʱ������� 1��10ms ��λ��
		
		XcpEvent(0);   // ֪ͨ XCP �¼���10ms�¼�
		CANSendcount++;
    }
	//ͨ������10��10ms�¼�������CAN������ķ�������(100ms)
	if (CANSendcount >= 10 && StartStopVoltageFlag==1)
	{
		SendCANEvent();
		CANSendcount = 0;
		// ÿ����ѹ��Ϣ����ʱ���򽫵�ѹ��Ϣ�����SD��.csv���
		//SD_Function();
	}


}

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
/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
