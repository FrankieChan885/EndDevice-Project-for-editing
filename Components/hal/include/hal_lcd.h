/**************************************************************************************************
  Filename:       hal_lcd.h
  Revised:        $Date: 2007-07-06 10:42:24 -0700 (Fri, 06 Jul 2007) $
  Revision:       $Revision: 13579 $

  Description:    This file contains the interface to the LCD Service.


  Copyright 2005-2007 Texas Instruments Incorporated. All rights reserved.

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
  PROVIDED “AS IS?WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED,
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

/**************************************************************************************************
 *                                         CONSTANTS
*************************************************************************************************/
/*
  LCD pins

  //control
  P1.3 - LCD backlight control bit
  P1.5 - LCD_CLK
  P1.6 - LCD_DI
  P1.7 - LCD_RS
  P2.0 - LCD_CS
*/
#define LEDD    P1_3 
#define LCD_CLK P1_5
#define LCD_DI  P1_6
#define LCD_RS  P1_7
#define LCD_CS  P2_0  
 

  
/**************************************************************************************************
 *                                          MACROS
 **************************************************************************************************/


/**************************************************************************************************
 *                                         TYPEDEFS
 **************************************************************************************************/


/**************************************************************************************************
 *                                     GLOBAL VARIABLES
 **************************************************************************************************/


/**************************************************************************************************
 *                                     FUNCTIONS - API
 **************************************************************************************************/

/*
 * Initialize LCD Service
 *///
extern void HalLcdInit(void);

/*
 * Write a string to the LCD
 */
/*extern void HalLcdWriteString5x8( uint8* str, uint8 len, uint8 page, uint8 col);*/

/*
 * Write a value to the LCD
 */
extern void HalLcdWriteChar5x8(uint8 chr,uint8 page,uint8 col,bool clear);
/*
 * Write a value to the LCD
 */
//extern void HalLcd_HW_WriteCommand(uint8 * cmd);
/*Load a picture with the height x width size,and the start point is (x,y).*///
extern void HalLcd_HW_LoadPicture(const uint8 * pic ,uint8 height, uint8 width,uint8 start_x,
                                  uint8 start_y, bool clear);

/* Load the wifi lable according to the mod *///
extern void HalLcdWriteWiFiMod(uint8 mod);

/* load the power picture according to the power*/
extern void HalLcdWritePower(bool POWER);

extern void HalLcdWriteError(bool clear);

/* Load the set temperature according to the temper */
extern void HalLcdWriteSetTemperature(uint8 temper,bool power);

/* Load the enddevice's mode of automatic or Manul lable according to the MOD ,ture to be auto
-tomatic ,flase to be manual *///
extern void HalLcdWriteEndMod(bool MOD);

/* Load the real tempreature mode  according to the mod ,ture to be room temperature
false to be floor temperature*///
extern void HalLcdWriteRealTempeMod(bool  mod);

/* Load the real tempreature according to the tempr */
extern void HalLcdWriteRealTemperature(uint16  temper);


/* Load the state of the relay ,ture the relay is closed ,false the relay is open*/ 
extern void HalLcdWriteRelayState(bool  state);

/*write the Debug information */
extern void HalLcdWriteDebug(uint8 num,bool shift,bool sign,bool clear,bool blink);

/*write the st*/
extern void HalLcd_HW_WriteST(bool power);

/*write the st unit*/
extern void HalLcd_HW_WriteSTU(bool power);

/**************************************************************************************************
**************************************************************************************************/

#ifdef __cplusplus
}
#endif

#endif
