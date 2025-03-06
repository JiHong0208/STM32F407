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
#include "stm32f4xx_it.h"

#include "FreeRTOS.h"					//FreeRTOSʹ��		  
#include "task.h" 
#include "bsp_adc.h"
#include "bsp_can.h"
#include "xcpBasic.h"

/** @addtogroup STM32F407I_DISCOVERY_Examples
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
extern __IO uint16_t ADC_ConvertedValue;
extern __IO uint32_t CANRxflag;               // ��־�Ƿ���յ�����
extern CanRxMsg RxMessage;                    // CAN ���ջ�����
extern volatile uint8_t StartStopVoltageFlag; // ��־λ���ж��Ƿ񽫵�ѹ���ݴ洢��SD��

/**
  * @brief  ADC ת������жϷ������
  * @param  None
  * @retval None
  */
void ADC_IRQHandler(void)
{
	uint32_t ulReturn;
	/* �����ٽ�� */
	ulReturn = taskENTER_CRITICAL_FROM_ISR();
	
	if(ADC_GetITStatus(RHEOSTAT_ADC,ADC_IT_EOC)==SET)
	{
		/* ��ȡADC��ת��ֵ */
		ADC_ConvertedValue = ADC_GetConversionValue(RHEOSTAT_ADC);
	}
	ADC_ClearITPendingBit(RHEOSTAT_ADC,ADC_IT_EOC);
	
	/* �˳��ٽ�� */
	taskEXIT_CRITICAL_FROM_ISR(ulReturn);
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

/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
