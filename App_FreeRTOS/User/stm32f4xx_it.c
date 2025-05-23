#include "stm32f4xx_it.h"
#include "bsp_can.h"
#include "xcpBasic.h"
#include "bsp_led.h"


//FreeRTOS使用	
#include "FreeRTOS.h"	  
#include "task.h" 

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
//systick中断服务函数
void SysTick_Handler(void)
{	
    #if (INCLUDE_xTaskGetSchedulerState  == 1 )
      if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
      {
    #endif  // INCLUDE_xTaskGetSchedulerState 
        xPortSysTickHandler();
    #if (INCLUDE_xTaskGetSchedulerState  == 1 )
      }
    #endif  // INCLUDE_xTaskGetSchedulerState 
}

/******************************************************************************/
/*                 STM32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f429_439xx.s).                         */
/******************************************************************************/

extern __IO uint32_t CANRxflag;               // 标志是否接收到数据
extern CanRxMsg RxMessage;                    // CAN 接收缓冲区

/**
  * @brief  This function handles CAN RX interrupt request.
  * @param  None
  * @retval None
  */
void CAN_RX_IRQHandler(void)
{
    // 从邮箱中读取报文 
    CAN_Receive(CANx, CAN_FIFO0, &RxMessage);

    // 检查是否为 XCP 报文 
    if ((RxMessage.ExtId == 0x1234) && 
        (RxMessage.IDE == CAN_ID_EXT) && 
        (RxMessage.DLC == 8)) 
    {
        // 调用 XCP 的接收处理函数 
        XcpRxHandler(RxMessage.Data);
    } 
    else if ((RxMessage.ExtId == 0x1314) && 
             (RxMessage.IDE == CAN_ID_EXT) && 
             (RxMessage.DLC == 8)) 
    {
        // 处理原有的 0x1314 报文 
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

/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
