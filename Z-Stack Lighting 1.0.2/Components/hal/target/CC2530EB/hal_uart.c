/**************************************************************************************************
  Filename:       _hal_uart.c
  Revised:        $Date: 2009-06-12 09:16:43 -0700 (Fri, 12 Jun 2009) $
  Revision:       $Revision: 20142 $

  Description: This file contains the interface to the H/W UART driver.


  Copyright 2006-2009 Texas Instruments Incorporated. All rights reserved.

  IMPORTANT: Your use of this Software is limited to those specific rights
  granted under the terms of a software license agreement between the user
  who downloaded the software, his/her employer (which must be your employer)
  and Texas Instruments Incorporated (the "License").  You may not use this
  Software unless you agree to abide by the terms of the License. The License
  limits your use, and you acknowledge, that the Software may not be modified,
  copied or distributed unless embedded on a Texas Instruments microcontroller
  or used solely and exclusively in conjunction with a Texas Instruments radio
  frequency transceiver, which is integrated into your product.  Other than for
  the foregoing purpose, you may not use, reproduce, copy, prepare derivative
  works of, modify, distribute, perform, display or sell this Software and/or
  its documentation for any purpose.

  YOU FURTHER ACKNOWLEDGE AND AGREE THAT THE SOFTWARE AND DOCUMENTATION ARE
  PROVIDED “AS IS” WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED,
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

/*********************************************************************
 * INCLUDES
 */

#include "hal_board_cfg.h"
#include "hal_defs.h"
#include "hal_types.h"
#include "hal_uart.h"

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */

/*********************************************************************
 * GLOBAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */

/*********************************************************************
 * LOCAL FUNCTIONS
 */

#if HAL_UART_DMA
#include "_hal_uart_dma.c"
#endif
#if HAL_UART_ISR
#include "_hal_uart_isr.c"
#endif
#if HAL_UART_USB
#include "_hal_uart_usb.c"
#endif

/******************************************************************************
 * @fn      HalUARTInit
 *
 * @brief   Initialize the UART
 *
 * @param   none
 *
 * @return  none
 *****************************************************************************/
void HalUARTInit(void)
{
#if HAL_UART_DMA
  HalUARTInitDMA();
#endif
#if HAL_UART_ISR
  HalUARTInitISR();
#endif
#if HAL_UART_USB
  HalUARTInitUSB();
#endif
}

/******************************************************************************
 * @fn      HalUARTOpen
 *
 * @brief   Open a port according tp the configuration specified by parameter.
 *
 * @param   port   - UART port
 *          config - contains configuration information
 *
 * @return  Status of the function call
 *****************************************************************************/
uint8 HalUARTOpen(uint8 port, halUARTCfg_t *config)
{
  (void)port;
  (void)config;

#if (HAL_UART_DMA == 1)
  if (port == HAL_UART_PORT_0)  HalUARTOpenDMA(config);
#endif
#if (HAL_UART_DMA == 2)
  if (port == HAL_UART_PORT_1)  HalUARTOpenDMA(config);
#endif
#if (HAL_UART_ISR == 1)
  if (port == HAL_UART_PORT_0)  HalUARTOpenISR(config);
#endif
#if (HAL_UART_ISR == 2)
  if (port == HAL_UART_PORT_1)  HalUARTOpenISR(config);
#endif
#if (HAL_UART_USB)
  HalUARTOpenUSB(config);
#endif
  
  return HAL_UART_SUCCESS;
}

/*****************************************************************************
 * @fn      HalUARTRead
 *
 * @brief   Read a buffer from the UART
 *
 * @param   port - USART module designation
 *          buf  - valid data buffer at least 'len' bytes in size
 *          len  - max length number of bytes to copy to 'buf'
 *
 * @return  length of buffer that was read
 *****************************************************************************/
uint16 HalUARTRead(uint8 port, uint8 *buf, uint16 len)
{
  (void)port;
  (void)buf;
  (void)len;

#if (HAL_UART_DMA == 1)
  if (port == HAL_UART_PORT_0)  return HalUARTReadDMA(buf, len);
#endif
#if (HAL_UART_DMA == 2)
  if (port == HAL_UART_PORT_1)  return HalUARTReadDMA(buf, len);
#endif
#if (HAL_UART_ISR == 1)
  if (port == HAL_UART_PORT_0)  return HalUARTReadISR(buf, len);
#endif
#if (HAL_UART_ISR == 2)
  if (port == HAL_UART_PORT_1)  return HalUARTReadISR(buf, len);
#endif

#if HAL_UART_USB
  return HalUARTRx(buf, len);
#else
  return 0;
#endif
}

uint8 CharToValue(uint8 string[2])
{
 uint8 i,result;
 uint8 data[2];
  for(i=0;i<2;i++)
    {
      switch(string[i])
      {
      case 'f':
      case 'F':
       data[i]=15;
       break;
      case 'e':
      case 'E':
       data[i]=14;
       break;
      case 'd':
      case 'D':
       data[i]=13;
       break;
      case 'c':
      case 'C':
       data[i]=12;
       break;
      case 'b':
      case 'B':
       data[i]=11;
       break;
      case 'a':
      case 'A':
       data[i]=10;
       break;
      case '9':
       data[i]=9;
       break;
      case '8':
       data[i]=8;
       break;
      case '7':
       data[i]=7;
       break;
      case '6':
       data[i]=6;
       break;
      case '5':
       data[i]=5;
       break;
      case '4':
       data[i]=4;
       break;
      case '3':
       data[i]=3;
       break;
      case '2':
       data[i]=2;
       break;
      case '1':
       data[i]=1;
       break;
      case '0':
       data[i]=0;
       break;
      }
    }
 result = data[0] * 16 + data[1];
 return result;
}

void HalUARTReadAndFormate(uint8 port, uint8 *buf, uint16 len)
{
  (void)port;
  uint8 i;
  uint8 string[2];
  for(i = 0; i < len; i++)
  {
   HalUARTRead(port, string, 2);
   *buf++ = CharToValue(string);
  }
}

/******************************************************************************
 * @fn      HalUARTWrite
 *
 * @brief   Write a buffer to the UART.
 *
 * @param   port - UART port
 *          buf  - pointer to the buffer that will be written, not freed
 *          len  - length of
 *
 * @return  length of the buffer that was sent
 *****************************************************************************/
uint16 HalUARTWrite(uint8 port, uint8 *buf, uint16 len)
{
  (void)port;
  (void)buf;
  (void)len;

#if (HAL_UART_DMA == 1)
  if (port == HAL_UART_PORT_0)  return HalUARTWriteDMA(buf, len);
#endif
#if (HAL_UART_DMA == 2)
  if (port == HAL_UART_PORT_1)  return HalUARTWriteDMA(buf, len);
#endif
#if (HAL_UART_ISR == 1)
  if (port == HAL_UART_PORT_0)  return HalUARTWriteISR(buf, len);
#endif
#if (HAL_UART_ISR == 2)
  if (port == HAL_UART_PORT_1)  return HalUARTWriteISR(buf, len);
#endif

#if HAL_UART_USB
  HalUARTTx(buf, len);
  return len;
#else
  return 0;
#endif
}

/******************************************************************************
 * @fn      HalUARTSuspend
 *
 * @brief   Suspend UART hardware before entering PM mode 1, 2 or 3.
 *
 * @param   None
 *
 * @return  None
 *****************************************************************************/
void HalUARTSuspend( void )
{
#if HAL_UART_DMA
  HalUARTSuspendDMA();
#endif
#if HAL_UART_ISR
  HalUARTSuspendISR();
#endif
}

/******************************************************************************
 * @fn      HalUARTResume
 *
 * @brief   Resume UART hardware after exiting PM mode 1, 2 or 3.
 *
 * @param   None
 *
 * @return  None
 *****************************************************************************/
void HalUARTResume( void )
{
#if HAL_UART_DMA
  HalUARTResumeDMA();
#endif
#if HAL_UART_ISR
  HalUARTResumeISR();
#endif
}

/***************************************************************************************************
 * @fn      HalUARTPoll
 *
 * @brief   Poll the UART.
 *
 * @param   none
 *
 * @return  none
 *****************************************************************************/
void HalUARTPoll(void)
{
#if HAL_UART_DMA
  HalUARTPollDMA();
#endif
#if HAL_UART_ISR
  HalUARTPollISR();
#endif
#if HAL_UART_USB
  HalUARTPollUSB();
#endif
}

/**************************************************************************************************
 * @fn      Hal_UART_RxBufLen()
 *
 * @brief   Calculate Rx Buffer length - the number of bytes in the buffer.
 *
 * @param   port - UART port
 *
 * @return  length of current Rx Buffer
 **************************************************************************************************/
uint16 Hal_UART_RxBufLen( uint8 port )
{
  (void)port;

#if (HAL_UART_DMA == 1)
  if (port == HAL_UART_PORT_0)  return HalUARTRxAvailDMA();
#endif
#if (HAL_UART_DMA == 2)
  if (port == HAL_UART_PORT_1)  return HalUARTRxAvailDMA();
#endif
#if (HAL_UART_ISR == 1)
  if (port == HAL_UART_PORT_0)  return HalUARTRxAvailISR();
#endif
#if (HAL_UART_ISR == 2)
  if (port == HAL_UART_PORT_1)  return HalUARTRxAvailISR();
#endif
#if HAL_UART_USB
  return HalUARTRxAvailUSB();
#else
  return 0;
#endif
}

/******************************************************************************
******************************************************************************/
