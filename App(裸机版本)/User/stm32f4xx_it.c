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
	
extern volatile GUI_TIMER_TIME OS_TimeMS; // emWin 系统时间计数器
extern __IO uint32_t CANRxflag;           // 标志是否接收到数据
extern CanRxMsg RxMessage;                // CAN 接收缓冲区
extern volatile uint8_t StartStopVoltageFlag; // 标志位，判断是否将电压数据存储进SD卡

/**
  * @brief  This function handles TIM interrupt request.
  * @param  None
  * @retval None
  */
uint32_t Measurement=0; //XCP观测量
volatile uint32_t DAQ_Timestamp = 0; // XCP的DAQ 时间戳，单位：10ms

void BASIC_TIM_IRQHandler(void)
{
	static uint8_t counter = 0; // 中断计数器
	static uint8_t CANSendcount = 0; //设置CAN报文发送周期计数器
	
    OS_TimeMS++;  // emWin 系统时间更新
	Measurement++;//XCP观测量
	counter++;
	
    if (TIM_GetITStatus(BASIC_TIM, TIM_IT_Update) != RESET) {
        TIM_ClearITPendingBit(BASIC_TIM, TIM_IT_Update);
    }
	//通过遍历10次中断，实现10ms事件
	if (counter >= 10) 
	{ 
        counter = 0;     // 重置计数器
        DAQ_Timestamp++; // 时间戳增加 1（10ms 单位）
		
		XcpEvent(0);   // 通知 XCP 事件，10ms事件
		CANSendcount++;
    }
	//通过遍历10次10ms事件，设置CAN随机报文发送周期(100ms)
	if (CANSendcount >= 10 && StartStopVoltageFlag==1)
	{
		SendCANEvent();
		CANSendcount = 0;
		// 每当电压信息发送时，则将电压信息储存进SD卡.csv表格
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
    /* 从邮箱中读取报文 */
    CAN_Receive(CANx, CAN_FIFO0, &RxMessage);

    /* 检查是否为 XCP 报文 */
    if ((RxMessage.ExtId == 0x1234) && 
        (RxMessage.IDE == CAN_ID_EXT) && 
        (RxMessage.DLC == 8)) 
    {
        /* 调用 XCP 的接收处理函数 */
        XcpRxHandler(RxMessage.Data);
    } 
    else if ((RxMessage.ExtId == 0x1314) && 
             (RxMessage.IDE == CAN_ID_EXT) && 
             (RxMessage.DLC == 8)) 
    {
        /* 处理原有的 0x1314 报文 */
        CANRxflag = 1;  // 接收成功  
    } 
    else 
    {
        CANRxflag = 0;  // 接收失败
    }
}
/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
