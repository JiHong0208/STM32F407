#ifndef      __BSP_ILI9341_LCD_H
#define	     __BSP_ILI9341_LCD_H


#include "stm32f4xx.h"
#include "./font/fonts.h"

/***************************************************************************************
2^26 =0X0400 0000 = 64MB,每个 BANK 有4*64MB = 256MB
64MB:FSMC_Bank1_NORSRAM1:0X6000 0000 ~ 0X63FF FFFF
64MB:FSMC_Bank1_NORSRAM2:0X6400 0000 ~ 0X67FF FFFF
64MB:FSMC_Bank1_NORSRAM3:0X6800 0000 ~ 0X6BFF FFFF
64MB:FSMC_Bank1_NORSRAM4:0X6C00 0000 ~ 0X6FFF FFFF

选择BANK1-BORSRAM3 连接 TFT，地址范围为0X6800 0000 ~ 0X6BFF FFFF
FSMC_A0 接LCD的DC(寄存器/数据选择)脚
寄存器基地址 = 0X6C00 0000
RAM基地址 = 0X6D00 0000 = 0X6C00 0000+2^0*2 = 0X6800 0000 + 0X2 = 0X6800 0002
当选择不同的地址线时，地址要重新计算  
****************************************************************************************/

/******************************* ILI9341 显示屏的 FSMC 参数定义 ***************************/
//FSMC_Bank1_NORSRAM用于LCD命令操作的地址
#define      FSMC_Addr_ILI9341_CMD         ( ( uint32_t ) 0x68000000 )

//FSMC_Bank1_NORSRAM用于LCD数据操作的地址      
#define      FSMC_Addr_ILI9341_DATA        ( ( uint32_t ) 0x68000002 )

//由片选引脚决定的NOR/SRAM块
#define      FSMC_Bank1_NORSRAMx           FSMC_Bank1_NORSRAM3

#define 			ILI9341_LESS_PIXEL	  	240			//液晶屏较短方向的像素宽度
#define 			ILI9341_MORE_PIXEL	 	320			//液晶屏较长方向的像素宽度


/******************************* ILI9341 显示屏8080通讯引脚定义 ***************************/
/******控制信号线******/
#define      FSMC_AF                       GPIO_AF_FSMC
//片选，选择NOR/SRAM块
#define      ILI9341_CS_CLK                RCC_AHB1Periph_GPIOG  
#define      ILI9341_CS_PORT               GPIOG
#define      ILI9341_CS_PIN                GPIO_Pin_10
#define      ILI9341_CS_PinSource          GPIO_PinSource10

//DC引脚，使用FSMC的地址信号控制，本引脚决定了访问LCD时使用的地址
//PF0为FSMC_A0
#define      ILI9341_DC_CLK                RCC_AHB1Periph_GPIOF  
#define      ILI9341_DC_PORT               GPIOF
#define      ILI9341_DC_PIN                GPIO_Pin_0
#define      ILI9341_DC_PinSource          GPIO_PinSource0

//写使能
#define      ILI9341_WR_CLK                RCC_AHB1Periph_GPIOD   
#define      ILI9341_WR_PORT               GPIOD
#define      ILI9341_WR_PIN                GPIO_Pin_5
#define      ILI9341_WR_PinSource          GPIO_PinSource5

//读使能
#define      ILI9341_RD_CLK                RCC_AHB1Periph_GPIOD   
#define      ILI9341_RD_PORT               GPIOD
#define      ILI9341_RD_PIN                GPIO_Pin_4
#define      ILI9341_RD_PinSource          GPIO_PinSource4

//复位引脚
#define      ILI9341_RST_CLK               RCC_AHB1Periph_GPIOF 
#define      ILI9341_RST_PORT              GPIOF
#define      ILI9341_RST_PIN               GPIO_Pin_11

//背光引脚
#define      ILI9341_BK_CLK                RCC_AHB1Periph_GPIOF   
#define      ILI9341_BK_PORT               GPIOF
#define      ILI9341_BK_PIN                GPIO_Pin_9

/********数据信号线***************/
#define      ILI9341_D0_CLK                RCC_AHB1Periph_GPIOD   
#define      ILI9341_D0_PORT               GPIOD
#define      ILI9341_D0_PIN                GPIO_Pin_14
#define      ILI9341_D0_PinSource          GPIO_PinSource14

#define      ILI9341_D1_CLK                RCC_AHB1Periph_GPIOD   
#define      ILI9341_D1_PORT               GPIOD
#define      ILI9341_D1_PIN                GPIO_Pin_15
#define      ILI9341_D1_PinSource          GPIO_PinSource15

#define      ILI9341_D2_CLK                RCC_AHB1Periph_GPIOD   
#define      ILI9341_D2_PORT               GPIOD
#define      ILI9341_D2_PIN                GPIO_Pin_0
#define      ILI9341_D2_PinSource          GPIO_PinSource0

#define      ILI9341_D3_CLK                RCC_AHB1Periph_GPIOD  
#define      ILI9341_D3_PORT               GPIOD
#define      ILI9341_D3_PIN                GPIO_Pin_1
#define      ILI9341_D3_PinSource          GPIO_PinSource1

#define      ILI9341_D4_CLK                RCC_AHB1Periph_GPIOE   
#define      ILI9341_D4_PORT               GPIOE
#define      ILI9341_D4_PIN                GPIO_Pin_7
#define      ILI9341_D4_PinSource          GPIO_PinSource7

#define      ILI9341_D5_CLK                RCC_AHB1Periph_GPIOE   
#define      ILI9341_D5_PORT               GPIOE
#define      ILI9341_D5_PIN                GPIO_Pin_8
#define      ILI9341_D5_PinSource          GPIO_PinSource8

#define      ILI9341_D6_CLK                RCC_AHB1Periph_GPIOE   
#define      ILI9341_D6_PORT               GPIOE
#define      ILI9341_D6_PIN                GPIO_Pin_9
#define      ILI9341_D6_PinSource          GPIO_PinSource9

#define      ILI9341_D7_CLK                RCC_AHB1Periph_GPIOE  
#define      ILI9341_D7_PORT               GPIOE
#define      ILI9341_D7_PIN                GPIO_Pin_10
#define      ILI9341_D7_PinSource          GPIO_PinSource10

#define      ILI9341_D8_CLK                RCC_AHB1Periph_GPIOE   
#define      ILI9341_D8_PORT               GPIOE
#define      ILI9341_D8_PIN                GPIO_Pin_11
#define      ILI9341_D8_PinSource          GPIO_PinSource11

#define      ILI9341_D9_CLK                RCC_AHB1Periph_GPIOE   
#define      ILI9341_D9_PORT               GPIOE
#define      ILI9341_D9_PIN                GPIO_Pin_12
#define      ILI9341_D9_PinSource          GPIO_PinSource12

#define      ILI9341_D10_CLK                RCC_AHB1Periph_GPIOE   
#define      ILI9341_D10_PORT               GPIOE
#define      ILI9341_D10_PIN                GPIO_Pin_13
#define      ILI9341_D10_PinSource          GPIO_PinSource13

#define      ILI9341_D11_CLK                RCC_AHB1Periph_GPIOE   
#define      ILI9341_D11_PORT               GPIOE
#define      ILI9341_D11_PIN                GPIO_Pin_14
#define      ILI9341_D11_PinSource          GPIO_PinSource14

#define      ILI9341_D12_CLK                RCC_AHB1Periph_GPIOE   
#define      ILI9341_D12_PORT               GPIOE
#define      ILI9341_D12_PIN                GPIO_Pin_15
#define      ILI9341_D12_PinSource          GPIO_PinSource15

#define      ILI9341_D13_CLK                RCC_AHB1Periph_GPIOD   
#define      ILI9341_D13_PORT               GPIOD
#define      ILI9341_D13_PIN                GPIO_Pin_8
#define      ILI9341_D13_PinSource          GPIO_PinSource8

#define      ILI9341_D14_CLK                RCC_AHB1Periph_GPIOD   
#define      ILI9341_D14_PORT               GPIOD
#define      ILI9341_D14_PIN                GPIO_Pin_9
#define      ILI9341_D14_PinSource          GPIO_PinSource9

#define      ILI9341_D15_CLK                RCC_AHB1Periph_GPIOD   
#define      ILI9341_D15_PORT               GPIOD
#define      ILI9341_D15_PIN                GPIO_Pin_10
#define      ILI9341_D15_PinSource          GPIO_PinSource10

/*************************************** 调试预用 ******************************************/
#define      DEBUG_DELAY()               Delay(0x5000)

/***************************** ILI934 显示区域的起始坐标和总行列数 ***************************/
#define      ILI9341_DispWindow_X_Star		    0     //起始点的X坐标
#define      ILI9341_DispWindow_Y_Star		    0     //起始点的Y坐标

#define 			ILI9341_LESS_PIXEL	  		240			//液晶屏较短方向的像素宽度
#define 			ILI9341_MORE_PIXEL	 		320			//液晶屏较长方向的像素宽度

//根据液晶扫描方向而变化的XY像素宽度
//调用ILI9341_GramScan函数设置方向时会自动更改
extern uint16_t LCD_X_LENGTH,LCD_Y_LENGTH; 

//液晶屏扫描模式
//参数可选值为0-7
extern uint8_t LCD_SCAN_MODE;

/******************************* 定义 ILI934 显示屏常用颜色 ********************************/
#define      BACKGROUND		                BLACK   //默认背景颜色

#define      WHITE		 		           0xFFFF	   //白色
#define      BLACK                         0x0000	   //黑色 
#define      GREY                          0xF7DE	   //灰色 
#define      BLUE                          0x001F	   //蓝色 
#define      BLUE2                         0x051F	   //浅蓝色 
#define      RED                           0xF800	   //红色 
#define      MAGENTA                       0xF81F	   //红紫色，洋红色 
#define      GREEN                         0x07E0	   //绿色 
#define      CYAN                          0x7FFF	   //蓝绿色，青色 
#define      YELLOW                        0xFFE0	   //黄色 
#define      BRED                          0xF81F
#define      GRED                          0xFFE0
#define      GBLUE                         0x07FF



/******************************* 定义 ILI934 常用命令 ********************************/
#define      CMD_SetCoordinateX		 		    0x2A	     //设置X坐标
#define      CMD_SetCoordinateY		 		    0x2B	     //设置Y坐标
#define      CMD_SetPixel		 		          0x2C	     //填充像素




/********************************** 声明 ILI934 函数 ***************************************/
void                     ILI9341_Init                    ( void );
void                     ILI9341_Rst                     ( void );
void 					 ILI9341_InvertDisplay			 (uint8_t enable);
void                     ILI9341_BackLed_Control         ( FunctionalState enumState );
void                     ILI9341_GramScan                ( uint8_t ucOtion );
void                     ILI9341_OpenWindow              ( uint16_t usX, uint16_t usY, uint16_t usWidth, uint16_t usHeight );
void                     ILI9341_Clear                   ( uint16_t usX, uint16_t usY, uint16_t usWidth, uint16_t usHeight );

#endif /* __BSP_ILI9341_ILI9341_H */


