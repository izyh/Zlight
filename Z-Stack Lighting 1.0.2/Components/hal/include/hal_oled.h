/**************************************************************************************************
  Filename:       hal_oled.h
  Revised:        $Date: 2012-07-06 10:42:24 -0700 (Fri, 06 Jul 2012) $
  Revision:       $Revision: 13579 $

  Description:    This file contains the interface to the OLED Service.


  Copyright 2005-2007 Corsso Incorporated. All rights reserved.

  IMPORTANT: Your use of this Software is limited to those specific rights
  granted under the terms of a software license agreement between the user
  who downloaded the software, his/her employer (which must be your employer)
  and Texas Instruments Incorporated (the "License").  You may not use this
  Software unless you agree to abide by the terms of the License. The License
  limits your use, and you acknowledge, that the Software may not be modified,
  copied or distributed unless embedded on a Texas Instruments microcontroller
  or used solely and exclusively in conjunction with a Crosso radio
  frequency transceiver, which is integrated into your product.  Other than for
  the foregoing purpose, you may not use, reproduce, copy, prepare derivative
  works of, modify, distribute, perform, display or sell this Software and/or
  its documentation for any purpose.

  YOU FURTHER ACKNOWLEDGE AND AGREE THAT THE SOFTWARE AND DOCUMENTATION ARE
  PROVIDED 揂S IS?WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED,
  INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY, TITLE,
  NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL
  TEXAS INSTRUMENTS OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER CONTRACT,
  NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR OTHER
  LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
  INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE
  OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT
  OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
  (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.

  Should you have any questions regarding your right to use this Software,
  contact Texas Instruments Incorporated at www.TI.com.
**************************************************************************************************/


#ifndef HAL_LCD_H
#define HAL_LCD_H

#ifdef __cplusplus
extern "C"
{
#endif

/**************************************************************************************************
 *                                          INCLUDES
 **************************************************************************************************/
#include "hal_board.h"

#include <ioCC2530.h>
#include <string.h>
#include "hal_types.h"





/**************************************************************************************************
 *                                          MACROS
 **************************************************************************************************/


/**************************************************************************************************
 *                                         TYPEDEFS
 **************************************************************************************************/
#define 	DATA    	1
#define 	COMMAND		0

#define	LCD_SCLK     	P1_5
#define	LCD_SDIN    	P1_6
#define	LCD_CS		    P1_2
#define	LCD_RST 	    P1_7
  
#define HAL_LCD_LINE_1      0x01
#define HAL_LCD_LINE_2      0x02

/*
   This to support LCD with extended number of lines (more than 2).
   Don't use these if LCD doesn't support more than 2 lines
*/
#define HAL_LCD_LINE_3      0x03
#define HAL_LCD_LINE_4      0x04
#define HAL_LCD_LINE_5      0x05
#define HAL_LCD_LINE_6      0x06
#define HAL_LCD_LINE_7      0x07
#define HAL_LCD_LINE_8      0x08

/* Max number of chars on a single LCD line */
#define HAL_LCD_MAX_CHARS   16
#define HAL_LCD_MAX_BUFF    25

/**************************************************************************************************
 *                                     GLOBAL VARIABLES
 **************************************************************************************************/


/**************************************************************************************************
 *                                     FUNCTIONS - API
 **************************************************************************************************/

extern void WaitUs(uint16 microSecs); // 等待微秒
extern void InitDisplay(void);				// 显示初始化
extern void ClearScreen(void);        // 全屏清除
extern void ClearRow16(uint8 rowbegin,uint8 rowend);     // 行清除
extern void Printn(uint8 xx ,uint8 yy , uint32 no,uint8 yn,uint8 le); //
extern void Printn8(uint8 xx ,uint8 yy , uint32 no,uint8 yn,uint8 le);//
extern void Print6(uint8 xx, uint8 yy, uint8 ch1[], uint8 yn);//
extern void Print16_8(uint16 y,uint16 x, uint8 ch[],uint16 yn);//打印16*8点阵字符（英文）
extern void Print32_16(uint16 y,uint16 x, uint8 ch[],uint16 yn);//打印32*16点阵字符（英文）
extern void Print16(uint16 y,uint16 x,uint8 ch[],uint16 yn);//打印16*16点阵字符（中文）
extern void Print(uint8 y, uint8 x, uint8 ch[], uint16 yn);//打印16*8 16*16中英文混合字符
extern void ClearCol(uint8 Begin , uint8 End);//清除列
extern void Rectangle(uint8 x1,uint8 y1,uint8 x2,uint8 y2);//绘制矩形
extern void DoSetContrast(void);//
extern void SetContrast(uint8 Step);//设置对比度
extern void SetRamAddr (uint8 Page, uint8 Col);//设置显存地址
extern void LoadICO(void);//载入图像
extern void TurnOnDisp(void);//打开显示
extern void WriteLCD(uint8 fs, uint8 da);//屏幕写数据
extern void contrastctrl(uint8 start, uint8 stop);//对比度渐变
extern void LCDInit(void);
extern void PrintCh16_8(uint16 y,uint16 x, uint8 ch,uint16 yn);
extern void Printico(uint8 xx, uint8 yy, uint8 ico[], uint8 ww, uint8 ll);	
extern void PrintWirelessQuality(uint8 x);
extern void PrintBatteryCapacity(float x);
extern void LcdPowerON(void);
extern void LcdPowerOFF(void);

extern uint8 ContrastValue;  //对比度

/* 以下为 TI LCD 接口函数 */

/*
 * Initialize LCD Service
 */
extern void HalLcdInit(void);

/*
 * Write a string to the LCD
 */
extern void HalLcdWriteString ( char *str, uint8 option);

/*
 * Write a value to the LCD
 */
extern void HalLcdWriteValue ( uint32 value, const uint8 radix, uint8 option);

/*
 * Write a value to the LCD
 */
extern void HalLcdWriteScreen( char *line1, char *line2 );

/*
 * Write a string followed by a value to the LCD
 */
extern void HalLcdWriteStringValue( char *title, uint16 value, uint8 format, uint8 line );

/*
 * Write a string followed by 2 values to the LCD
 */
extern void HalLcdWriteStringValueValue( char *title, uint16 value1, uint8 format1, uint16 value2, uint8 format2, uint8 line );

/*
 * Write a percentage bar to the LCD
 */
extern void HalLcdDisplayPercentBar( char *title, uint8 value );


/**************************************************************************************************
**************************************************************************************************/

#ifdef __cplusplus
}
#endif

#endif
