/**************************************************************************************************
  Filename:       hal_lcd.c
  Revised:        $Date: 2010-06-21 17:31:27 -0700 (Mon, 21 Jun 2010) $
  Revision:       $Revision: 22794 $

  Description:    This file contains the interface to the HAL LCD Service.


  Copyright 2007-2010 Texas Instruments Incorporated. All rights reserved.

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

/**************************************************************************************************
 *                                           INCLUDES
 **************************************************************************************************/
#include "hal_types.h"
#include "hal_lcd.h"
#include "OSAL.h"
#include "OnBoard.h"
#include "hal_assert.h"

#if defined (ZTOOL_P1) || defined (ZTOOL_P2)
  #include "DebugTrace.h"
#endif

/**************************************************************************************************
 *                                          CONSTANTS
 **************************************************************************************************/

/**************************************************************
 *
 *  the  picture of set temprature label 
 *
 ****************************************************************/
const uint8 ST_PIC[30] =
{
 0xF8,0x84,0x84,0x84,0x84,0x84,0x84,0x00,0x00,0x00,0x04,0x04,0xFC,0x04,0x04,
 0x20,0x20,0x20,0x20,0x20,0x20,0x1F,0x00,0x00,0x00,0x00,0x00,0x3F,0x00,0x00 
};

/**************************************************************
 *
 *  the  picture of set temprature unit label 
 *
 ****************************************************************/
const uint8 ST_Unit[20] =
{
 0x60,0x90,0x90,0x60,0x80,0x40,0x40,0x40,0x40,0x80,
 0x80,0x80,0x80,0x80,0x8F,0x90,0x90,0x90,0x90,0x88
};

/**************************************************************
 *
 *  the  picture of real temprature unit label 
 *
 ****************************************************************/
const uint8 RT_Unit[22] =
{
0x0C,0x12,0x12,0x0C,0x00,0xF0,0x08,0x08,0x08,0x08,0x10,
0x00,0x00,0x00,0x00,0x00,0x01,0x02,0x02,0x02,0x02,0x01
};

/**************************************************************
 *
 *  the  picture of wifi label 
 *
 ****************************************************************/
const uint8 WiFi[81] =
{
  /*--  调入了一幅图像：C:\Users\William\Desktop\mode\WIFIMOD.bmp  --*/
/*--  宽度x高度=27x24  --*/

0x00,0x80,0xC0,0xE0,0xF0,0x78,0x38,0x3C,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,
0x1C,0x1C,0x1C,0x3C,0x38,0x78,0xF0,0xE0,0xC0,0x80,0x00,0x07,0x07,0x03,0x01,0xE0,
0xF0,0x78,0x3C,0x1E,0x0E,0x0F,0x07,0x87,0x87,0x87,0x07,0x0F,0x0E,0x1E,0x3C,0x78,
0xF0,0xE0,0x01,0x03,0x07,0x07,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
0x8E,0x9F,0xBF,0xBF,0xBF,0x9F,0x8E,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
0x80
};

/**************************************************************
 *
 *  the  picture of wifi_1 label 
 *
 ****************************************************************/
const uint8 WiFi_1[81] =
{
/*--  调入了一幅图像：C:\Users\William\Desktop\mode\WIFIMOD1.bmp  --*/
/*--  宽度x高度=27x24  --*/
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xE0,
0xF0,0x78,0x3C,0x1E,0x0E,0x0F,0x07,0x87,0x87,0x87,0x07,0x0F,0x0E,0x1E,0x3C,0x78,
0xF0,0xE0,0x00,0x00,0x00,0x00,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
0x8E,0x9F,0xBF,0xBF,0xBF,0x9F,0x8E,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
};

/**************************************************************
 *
 *  the  picture of wifi_2 label 
 *
 ****************************************************************/
const uint8 WiFi_2[81] =
{
/*--  调入了一幅图像：C:\Users\William\Desktop\mode\WIFIMOD2.bmp  --*/
/*--  宽度x高度=27x24  --*/
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x80,0x80,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
0x8E,0x9F,0xBF,0xBF,0xBF,0x9F,0x8E,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
0x80
};

/**************************************************************
 *
 *  the  picture of wifi_3 label 
 *
 ****************************************************************/
const uint8 WiFi_3[81] =
{
/*--  调入了一幅图像：C:\Users\William\Desktop\mode\WIFIMOD3.bmp  --*/
/*--  宽度x高度=27x24  --*/
0x00,0x00,0x08,0x18,0x38,0x78,0xF8,0xF8,0xF8,0xF8,0xF8,0x78,0x38,0x18,0x08,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x01,0xFF,0x01,0x00,0x00,0x00,0x60,0xE0,0xC0,0x80,0x00,0x80,0xC0,0xE0,
0x60,0x00,0x00,0x00,0x00,0x00,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0xBF,0x80,
0x80,0x80,0x80,0xB0,0xB8,0x9D,0x8F,0x87,0x8F,0x9D,0xB8,0xB0,0x80,0x80,0x80,0x80,
0x80
};

/**************************************************************
 *
 *  the  picture of enddevice mode label 
 *
 ****************************************************************/
const uint8 Auto[60] = 
{
/*--  调入了一幅图像：C:\Users\William\Desktop\mode\AUTOMOD.bmp  --*/
/*--  宽度x高度=30x16  --*/
0xF8,0x88,0x88,0x88,0x88,0xF8,0x00,0x00,0xF8,0x00,0x00,0x00,0x00,0xF8,0x00,0x00,
0x00,0x08,0x08,0xF8,0x08,0x08,0x00,0x00,0xF8,0x08,0x08,0x08,0x08,0xF0,0x1F,0x00,
0x00,0x00,0x00,0x1F,0x00,0x00,0x0F,0x10,0x10,0x10,0x10,0x0F,0x00,0x00,0x00,0x00,
0x00,0x1F,0x00,0x00,0x00,0x00,0x0F,0x10,0x10,0x10,0x10,0x0F
};


/**************************************************************
 *
 *  the  picture of temperature mod label 
 *
 ****************************************************************/
const uint8 RT[51] = 
{
/*--  调入了一幅图像：C:\Users\William\Desktop\mode\RTMOD.bmp  --*/
/*--  宽度x高度=17x24  --*/
0xE0,0x20,0x20,0x20,0x20,0x20,0x20,0xE0,0x00,0x00,0x20,0x20,0x20,0xE0,0x20,0x20,
0x20,0xFF,0x08,0x18,0x38,0x68,0xC8,0x88,0x07,0x00,0x00,0x00,0x00,0x00,0xFF,0x00,
0x00,0x00,0x03,0x00,0x00,0x00,0x00,0x00,0x01,0x03,0x00,0x00,0x00,0x00,0x00,0x03,
0x00,0x00,0x00 
};

/**************************************************************
 *
 *  the  picture of temperature mod label 
 *
 ****************************************************************/
const uint8 FT[51] = 
{
/*--  调入了一幅图像：C:\Users\William\Desktop\mode\FTMOD.bmp  --*/
/*--  宽度x高度=17x24  --*/
0xE0,0x20,0x20,0x20,0x20,0x20,0x20,0x00,0x00,0x00,0x20,0x20,0x20,0xE0,0x20,0x20,
0x20,0xF7,0x08,0x08,0x08,0x08,0x08,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x00,
0x00,0x00,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,
0x00,0x00,0x00
};

/**************************************************************
 *
 *  the  picture of relay state label 
 *
 ****************************************************************/
const uint8 Arrow[33] = 
{
/*--  调入了一幅图像：C:\Users\William\Desktop\mode\arrow.bmp  --*/
/*--  宽度x高度=11x24  --*/
0x00,0x00,0x00,0x00,0x80,0xC0,0x80,0x00,0x00,0x00,0x00,0x08,0x0C,0x8E,0xCF,0xEF,
0xFF,0xEF,0xCF,0x8E,0x0C,0x08,0x02,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,
0x02
};


/**************************************************************
 *
 *  the  code of set temperature number  
 *
 ****************************************************************/
const uint8 STCODE[360] = 
{
  /*--  调入了一幅图像：C:\Users\William\Desktop\mode\ST0MOD.bmp  --*/
/*--  宽度x高度=12x24  --*/
0xFE,0xFE,0x06,0x06,0x06,0x06,0x06,0x06,0x06,0x06,0xFA,0xFE,0xFB,0xF1,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0xF3,0xFB,0x9F,0x97,0x98,0x98,0x98,0x98,0x98,0x98,
0x98,0x98,0x93,0x9F,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFC,0xFE,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0xF1,0xFB,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
0x80,0x80,0x87,0x9F,
0x00,0x02,0x06,0x06,0x06,0x06,0x06,0x06,0x06,0x06,0xFA,0xFE,0xF8,0xF8,0x0C,0x0C,
0x0C,0x0C,0x0C,0x0C,0x0C,0x0C,0x0F,0x03,0x87,0x93,0x98,0x98,0x98,0x98,0x98,0x98,
0x98,0x98,0x90,0x80,
0x02,0x06,0x06,0x06,0x06,0x06,0x06,0x06,0x06,0x06,0xFE,0xFE,0x0C,0x0C,0x0C,0x0C,
0x0C,0x0C,0x0C,0x0C,0x0C,0x0C,0xF1,0xFB,0x90,0x98,0x98,0x98,0x98,0x98,0x98,0x98,
0x98,0x98,0x93,0x87,
0xFE,0xFC,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFC,0xFE,0x03,0x01,0x0C,0x0C,
0x0C,0x0C,0x0C,0x0C,0x0C,0x0C,0xFD,0xFB,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
0x80,0x80,0x87,0x9F,
0xFE,0xFA,0x06,0x06,0x06,0x06,0x06,0x06,0x06,0x06,0x02,0x00,0x03,0x03,0x0C,0x0C,
0x0C,0x0C,0x0C,0x0C,0x0C,0x0C,0xFC,0xF8,0x80,0x90,0x98,0x98,0x98,0x98,0x98,0x98,
0x98,0x98,0x93,0x87,
0xFE,0xFE,0x06,0x06,0x06,0x06,0x06,0x06,0x06,0x06,0x02,0x00,0xFF,0xF9,0x0C,0x0C,
0x0C,0x0C,0x0C,0x0C,0x0C,0x0C,0xFC,0xF8,0x87,0x93,0x98,0x98,0x98,0x98,0x98,0x98,
0x98,0x98,0x97,0x9F,
0x02,0x02,0x06,0x06,0x06,0x06,0x06,0x06,0x06,0x06,0xFE,0xFE,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0xF1,0xFB,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
0x80,0x80,0x87,0x9F,
0xFE,0xFE,0x06,0x06,0x06,0x06,0x06,0x06,0x06,0x06,0xFA,0xFE,0xFB,0xF9,0x0C,0x0C,
0x0C,0x0C,0x0C,0x0C,0x0C,0x0C,0xFF,0xFB,0x87,0x93,0x98,0x98,0x98,0x98,0x98,0x98,
0x98,0x98,0x97,0x9F,
0xFE,0xFE,0x06,0x06,0x06,0x06,0x06,0x06,0x06,0x06,0xFE,0xFE,0x03,0x01,0x0C,0x0C,
0x0C,0x0C,0x0C,0x0C,0x0C,0x0C,0xFD,0xFF,0x80,0x90,0x98,0x98,0x98,0x98,0x98,0x98,
0x98,0x98,0x9B,0x9F
};

/**************************************************************
 *
 *  the  code of real temperature number  
 *
 ****************************************************************/
const uint8 RTCODE[850] = 
{
/*--  调入了一幅图像：C:\Users\William\Desktop\mode\RT0MOD.bmp  --*/
/*--  宽度x高度=17x40  --*/
0xC0,0xA0,0x60,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0x60,0xA0,
0xC0,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFE,
0xFF,0xFF,0xE3,0xF7,0xE3,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0xE3,0xF7,0xE3,0xFF,0xFF,0x7F,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
0x80,0x3F,0x7F,0xFF,0x01,0x02,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,
0x03,0x03,0x03,0x02,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xC0,
0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,
0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0xC1,0xF7,0xE3,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x7F,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x01,0x03,
0x00,0x20,0x60,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0x60,0xA0,
0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,
0xFF,0xFF,0xE0,0xE0,0xDC,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,
0x0B,0x07,0x03,0xFF,0xFF,0x7F,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
0x80,0x00,0x00,0x00,0x01,0x02,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,
0x03,0x03,0x03,0x02,0x00,
0x20,0x60,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0x60,0x20,0x80,
0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,
0xFF,0xFF,0x08,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,
0xEB,0xF7,0xE3,0x00,0x00,0x00,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
0x00,0x3F,0x7F,0xFF,0x02,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,
0x03,0x03,0x02,0x00,0x00,
0xE0,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xC0,
0xE0,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,
0xFF,0xFF,0x03,0x07,0x1D,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,
0xCB,0xF7,0xE3,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x7F,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x01,0x03,
0xC0,0xA0,0x60,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0x60,0x20,
0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x03,0x07,0x1F,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,
0xC8,0xE0,0xE0,0x00,0x00,0x00,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
0x80,0x7F,0xFF,0xFF,0x00,0x02,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,
0x03,0x03,0x03,0x02,0x01,
0xC0,0xA0,0x60,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0x60,0x20,
0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0xE3,0xF7,0xFF,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,
0xE8,0xF0,0xE0,0xFF,0x7F,0x3F,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
0x80,0x7F,0xFF,0xFF,0x00,0x02,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,
0x03,0x03,0x03,0x02,0x01,
0x20,0x60,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0x60,0x20,0xA0,
0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,
0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0xC3,0xF7,0xE3,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x7F,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x01,0x03,
0xC0,0xA0,0x60,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0x60,0xA0,
0xC0,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFE,
0xFF,0xFF,0xE3,0xF7,0xFF,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,
0xEB,0xF7,0xE3,0xFF,0x7F,0x3F,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
0x80,0x7F,0xFF,0xFF,0x00,0x02,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,
0x03,0x03,0x03,0x02,0x01,
0xC0,0xA0,0x60,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0x60,0xA0,
0xC0,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,
0xFF,0xFF,0x03,0x07,0x1F,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,
0xEB,0xF7,0xE3,0x00,0x00,0x00,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
0x80,0x3F,0x7F,0xFF,0x00,0x02,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,
0x03,0x03,0x03,0x02,0x00
};

//  the ASCII CODE for NUM
const uint8 ASCII_CODE[90] = {
0x3e,0x51,0x49,0x45,0x3e,//-0-
0x00,0x42,0x7f,0x40,0x00,//-1-
0x42,0x61,0x51,0x49,0x46,//-2-
0x21,0x41,0x45,0x4b,0x31,//-3-
0x18,0x14,0x12,0x7f,0x10,//-4-
0x27,0x45,0x45,0x45,0x39,//-5-
0x3c,0x4a,0x49,0x49,0x30,//-6-
0x01,0x71,0x09,0x05,0x03,//-7-
0x36,0x49,0x49,0x49,0x36,//-8-
0x06,0x49,0x49,0x29,0x1e,//-9-
0x7e,0x11,0x11,0x11,0x7e,//-A-
0x7f,0x49,0x49,0x49,0x36,//-B-
0x3e,0x41,0x41,0x41,0x22,//-C-
0x7f,0x41,0x41,0x22,0x1c,//-D-
0x7f,0x49,0x49,0x49,0x41,//-E-
0x7f,0x09,0x09,0x09,0x01,//-F-
0x7f,0x04,0x08,0x10,0x7f,//-N-
0x00,0x36,0x36,0x00,0x00,//-:-
};


// the power off picture
const uint8 POWER_PIC[57] =
{
/*--  调入了一幅图像：C:\Users\William\Desktop\毕业设计\图形界面设计\Enddevice\mode\Power.bmp  --*/
/*--  宽度x高度=19x24  --*/
0x80,0xC0,0xE0,0xF0,0xF0,0x70,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x70,0xF0,0xF0,
0xE0,0xC0,0x80,0xFF,0xFF,0xFF,0x01,0x00,0x00,0x00,0x00,0x7F,0x7F,0x7F,0x00,0x00,
0x00,0x00,0x01,0xFF,0xFF,0xFF,0x87,0x8F,0x9F,0xBC,0xB8,0xB8,0xB8,0xB8,0xB8,0xB8,
0xB8,0xB8,0xB8,0xB8,0xB8,0xBC,0x9F,0x8F,0x87
};


// the error picture
const uint8 ERROR_PIC[340] = 
{
  /*--  调入了一幅图像：C:\Users\William\Desktop\毕业设计\图形界面设计\Enddevice\mode\error.bmp  --*/
/*--  宽度x高度=68x40  --*/
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0xF8,0xF8,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x08,0x00,
0x00,0x00,0xF8,0xF8,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0xE0,0xE0,0x00,0x00,0x00,
0xF8,0xF8,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0xE0,0xE0,0x00,0x00,0x00,0xF0,0xF0,
0x18,0x18,0x18,0x18,0x18,0x18,0x18,0xF0,0xF0,0x00,0x00,0x00,0xF8,0xF8,0x18,0x18,
0x18,0x18,0x18,0x18,0x18,0xE0,0xE0,0x00,0xF7,0xFB,0x18,0x18,0x18,0x18,0x18,0x18,
0x18,0x00,0x00,0x00,0x00,0x00,0xF7,0xFB,0x18,0xD8,0xD8,0x98,0x18,0x18,0x18,0x07,
0x07,0x00,0x00,0x00,0xF7,0xFB,0x18,0xD8,0xD8,0x98,0x18,0x18,0x18,0x07,0x07,0x00,
0x00,0x00,0xF7,0xE7,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xE3,0xF7,0x00,0x00,0x00,
0xF7,0xFB,0x18,0xD8,0xD8,0x98,0x18,0x18,0x18,0x07,0x07,0x00,0x0F,0x17,0x18,0x18,
0x18,0x18,0x18,0x18,0x18,0x18,0x10,0x00,0x00,0x00,0x1F,0x0F,0x00,0x00,0x00,0x01,
0x03,0x06,0x0E,0x1C,0x18,0x00,0x00,0x00,0x1F,0x0F,0x00,0x00,0x00,0x01,0x03,0x06,
0x0E,0x1C,0x18,0x00,0x00,0x00,0x07,0x1F,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x07,
0x07,0x00,0x00,0x00,0x1F,0x0F,0x00,0x00,0x00,0x01,0x03,0x06,0x0E,0x1C,0x18,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00
};

/*************************************************************************************************
                                           MACROS
 **************************************************************************************************/




#if (HAL_LCD == TRUE)
/**************************************************************************************************
 *                                       LOCAL VARIABLES
 **************************************************************************************************/

/**************************************************************************************************
 *                                       FUNCTIONS - API
 **************************************************************************************************/
void HalLcd_HW_WaitUs(uint16 i);
void HalLcd_HW_WriteCommand(uint8 cmd);
void HalLcd_HW_WriteData(uint8 data);
void HalLcd_HW_WriteAddr(uint8 page,uint8 col);
void HalLcd_HW_Clear(void);
void HalLcd_HW_LoadPicture(const uint8 *  pic ,uint8 height, uint8 width,uint8 start_x, 
                           uint8 start_y, bool clear);
void HalLcd_HW_WriteST(bool power);
void HalLcd_HW_WriteSTU(bool power);
void HalLcd_HW_WriteRTU(void);
void HalLcdWriteWiFiMod(uint8 mod);
void HalLcdWriteEndMod(bool MOD);
void HalLcdWriteRealTempeMod(bool  mod);
void HalLcdWritePower(bool POWER);
void HalLcdWriteError(bool clear);

void HalLcdWriteSetTemperature(uint8 temper,bool power);
void HalLcdWriteRealTemperature(uint16  temper);
void HalLcdWriteRelayState(bool  state);
void HalLcdWriteDebug(uint8 num,bool shift,bool sign,bool clear,bool blink);
void HalLcdWriteChar5x8(uint8 chr,uint8 page,uint8 col,bool clear);
//void HalLcdWriteString5x8(uint8* str,uint8 len, uint8 page, uint8 col);
void LoadBackground(void);
#endif //LCD

/**************************************************************************************************
 * @fn      HalLcdInit
 *
 * @brief   Initilize LCD Service
 *
 * @param   init - pointer to void that contains the initialized value
 *
 * @return  None
 **************************************************************************************************/

void HalLcdInit(void)
{
#if (HAL_LCD == TRUE)
  /*config the I/O---------------by Zimuge*/
    P1SEL &= ~(BV(3) + BV(5) + BV(6) + BV(7));
    P1DIR |= (BV(3) + BV(5) + BV(6) + BV(7));
    P2SEL &= ~BV(0);
    P2DIR |= BV(0);
    LEDD = 0; //open the backlight
    HalLcd_HW_WriteCommand(0xE2);// software rest
    HalLcd_HW_WaitUs(2);
   
    HalLcd_HW_WriteCommand(0x2C);// open the boost circuit
    HalLcd_HW_WaitUs(2);
    HalLcd_HW_WriteCommand(0x2E);// open the voltage management circuit
    HalLcd_HW_WaitUs(2);
    HalLcd_HW_WriteCommand(0x2F);// open the voltage follower
    HalLcd_HW_WaitUs(2);
    
    HalLcd_HW_WriteCommand(0x24);// select the contrast roughly
    HalLcd_HW_WriteCommand(0x81);// select the contrast smoothly
    HalLcd_HW_WriteCommand(0x12);
    HalLcd_HW_WriteCommand(0xA2);// select the bias voltage radio 1/9
    
    HalLcd_HW_WriteCommand(0xC8);// scan the LCD from top to bottom
    
    HalLcd_HW_WriteCommand(0xA0);// the address will automatically increase one by one
    
    HalLcd_HW_WriteCommand(0x40);// select the begining of the column to display the char
    
    HalLcd_HW_WriteCommand(0xAF);// open the lcd
    
   // HalLcd_HW_WriteCommand(0xA5);
    HalLcd_HW_Clear();
    LoadBackground();

#endif
}

/**************************************************************************************************
 *                                    HARDWARE LCD
 **************************************************************************************************/
#if (HAL_LCD == TRUE)

/**************************************************************************************************
 * @fn      HalLcd_HW_WaitUs
 *
 * @brief   wait for x us. @ 32MHz MCU clock it takes 32 "nop"s for 1 us delay.
 *
 * @param   x us. range[0-65536]
 *
 * @return  None
 **************************************************************************************************/
void HalLcd_HW_WaitUs(uint16 microSecs)
{
  while(microSecs--)
  {
    /* 32 NOPs == 1 usecs */
    asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop");
    asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop");
    asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop");
    asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop");
    asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop");
    asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop");
    asm("nop"); asm("nop");
  }
}

/**************************************************************************************************
 * @fn      HalLcdWriteCommand to the LCD
 *
 * @brief   Write a command to the LCD
 *
 * @param  cmd-the command to be wrote
 *         
 *
 * @return  None
 **************************************************************************************************/
void HalLcd_HW_WriteCommand(uint8 cmd)
{
   uint8 i = 0;
   LCD_CS = 0; // to select the LCD
   HalLcd_HW_WaitUs(1);
   LCD_RS = 0;// write the command
   HalLcd_HW_WaitUs(1);
   for(i = 0;i < 8;i++)
   {
       LCD_CLK = 0;
       asm("nop");// a delay for waiting the peripheral
      // HalLcd_HW_WaitUs(2);
       if(cmd & 0x80)
       {
         LCD_DI = 1;
       }
       else
       {
         LCD_DI = 0; 
       }
       //asm("nop");// a delay to set the data
      // asm("nop");
      // HalLcd_HW_WaitUs(2);
       cmd = cmd << 1;
       LCD_CLK = 1;
       asm("nop");// a delay to hold the data
       asm("nop"); 
       HalLcd_HW_WaitUs(1);
   }
   LCD_CS = 1;
   
}

/**************************************************************************************************
 * @fn      HalLcdWriteData to the LCD
 *
 * @brief   Write a data to the LCD
 *
 * @param  data-the data to be wrote
 *         
 *
 * @return  None
 **************************************************************************************************/
void HalLcd_HW_WriteData(uint8 data)
{
   uint8 i = 0;
   LCD_CS = 0; // to select the LCD
   HalLcd_HW_WaitUs(1);
   LCD_RS = 1;// write data
   HalLcd_HW_WaitUs(1);
   for(i = 0;i < 8;i++)
   {
       LCD_CLK = 0;
       asm("nop");// a delay for waiting the peripheral
      // HalLcd_HW_WaitUs(1);
       if(data & 0x80)
       {
         LCD_DI = 1;
       }
       else
       {
         LCD_DI = 0; 
       }
       asm("nop");// a delay to set the data
       asm("nop");
       //HalLcd_HW_WaitUs(1);
       data = data << 1;
       LCD_CLK = 1;
      // HalLcd_HW_WaitUs(1);
       asm("nop");// a delay to hold the data
       asm("nop"); 
   }
   LCD_CS = 1;//
  // 
}

/**************************************************************************************************
 * @fn      HalLcd_HW_WriteAddr
 *
 * @brief   the address that to be wrote
 *
 * @param   page-the page of the lcd
 *          col - the column of the lcd
 *
 * @return  None
 **************************************************************************************************/
void HalLcd_HW_WriteAddr(uint8 page,uint8 col)
{
   HalLcd_HW_WriteCommand(page + 0xb0);// set the page address
   HalLcd_HW_WriteCommand(((col >> 4) & 0x0f) + 0x10);//set the column high four bit address
   HalLcd_HW_WriteCommand(col & 0x0f);//set the column low four  bit address
}

/**************************************************************************************************
 * @fn      HalLcd_HW_Clear
 *
 * @brief   Clear the HW LCD
 *
 * @param   None
 *
 * @return  None
 **************************************************************************************************/
void HalLcd_HW_Clear(void)
{
  uint8 i = 0;
  uint8 page = 0;
  for(page = 0 ;page < 8; page++)
  {
    HalLcd_HW_WriteAddr(page,0);
    for(i = 0; i < 128; i++)
    {
      HalLcd_HW_WriteData(0x00);
    }  
  }
}

/**************************************************************************************************
 * @fn      HalLcdWriteChar
 *
 * @brief   Write a string to the LCD
 *
 * @param   chr  - char that will be displayed 
 *        
 *          page - the place to display
 *          
 *          col  - the column to display

            clear - true write the char,false clear the area
 * @return  None
 **************************************************************************************************/

void HalLcdWriteChar5x8( uint8 chr,uint8 page,uint8 col,bool clear)
{
  
   HalLcd_HW_WriteAddr(page,col);
   uint8 i = 0;
   uint8 data = 0;
   for(i = 0; i < 5; i++)
   {
      data =  ASCII_CODE[5 * chr + i];
      if(clear)
        HalLcd_HW_WriteData(data);
      else
        HalLcd_HW_WriteData(0x00);
   }
}

/**************************************************************************************************
 * @fn      HalLcdWriteString
 *
 * @brief   Write a string to the LCD
 *
 * @param   title  - Title that will be displayed 
 *        
 *          len   - the lenght of the string
 *
 * @return  None
 **************************************************************************************************/

/*void HalLcdWriteString5x8( uint8* str, uint8 len, uint8 page, uint8 col)
{
   uint8 i = 0;
   for(i = 0; i < len; i++)
   {
     HalLcdWriteChar5x8(str[i],page,col);
     col += 5;
     if(col >= 125)
     {
       col = 0;
       page++;
       if(page >= 8)
       {
         page = 0;
       }
     }
     
   }
}*/
/**************************************************************************************************
 * @fn      HalLcd_HW_LoadPicture
 *
 * @brief   Load a picture with the height x width size,and the start point is (x,y). 
 *
 * @param   pic  - The picture to be displayed 
 *        
 *          height - the height of the picture
 *
 *          width - the width of the picture
 *
 *          x - the x coordinate of the start point
 *
 *          y - the y coordinate of the start point
 *
 *          clear - FALSE to clear the area,TRUE to wite the area;
 * @return  None
 **************************************************************************************************/

void HalLcd_HW_LoadPicture(const uint8 * pic,uint8 height, uint8 width,uint8 start_x, 
                           uint8 start_y,bool clear)
{
   uint8 start_page = start_y / 8;
   uint8 start_col = start_x;// the address of the start point
   uint8 page_count = height / 8; // the count of page that the picture occupies
   uint8 i = 0, j = 0;
   for(i = 0; i < page_count; i++)
   {
     HalLcd_HW_WriteAddr(i + start_page,start_col);
     for(j = 0; j < width; j++)
     {
        if(clear)
        HalLcd_HW_WriteData(pic[i * width + j]);
        else
        HalLcd_HW_WriteData(0x00);
     }
     
   }
}

/************************************************************** 
 * @fn      HalLcd_HW_WriteST
 *
 * @brief   Load the set temperature label picture
 *
 * @param   power - ture to load ,false to clear
 *
 * @return  None  
 *
**************************************************************/

void HalLcd_HW_WriteST(bool power)
{
    HalLcd_HW_LoadPicture(ST_PIC,16,15,61,0,power); 
}

/************************************************************** 
 * @fn      HalLcd_HW_WriteSTU
 *
 * @brief   Load the set temperature unit label picture
 *
 * @param   power - power - ture to load ,false to clear
 *
 * @return  None  
 *
**************************************************************/
void HalLcd_HW_WriteSTU(bool power)
{
  if(power)
  HalLcd_HW_LoadPicture((uint8 *)ST_Unit,16,10,112,8,TRUE);
  else
  {
    HalLcd_HW_LoadPicture((uint8 *)ST_Unit,16,10,112,8,FALSE);
    HalLcd_HW_WriteAddr(2,112);
    for(uint8 i = 0; i < 10; i++)
    {
      HalLcd_HW_WriteData(0x80);
    }
  }
}

/************************************************************** 
 * @fn      HalLcd_HW_WriteRTU
 *
 * @brief   Load the real temperature unit label picture
 *
 * @param   None
 *
 * @return  None  
 *
**************************************************************/
void HalLcd_HW_WriteRTU(void)
{
  HalLcd_HW_LoadPicture(RT_Unit,16,11,111,48,TRUE);
}

/************************************************************** 
 * @fn      HalLcdWriteWiFiMod
 *
 * @brief   Load the wifi mod picture according to the mod
 *
 * @param   mod - the mode of the wifi
 *
 * @return  None  
 *
**************************************************************/
void HalLcdWriteWiFiMod(uint8 mod)
{
  switch (mod)
  {
  case 0:
    HalLcd_HW_LoadPicture(WiFi,24,27,1,0,TRUE);
    break;
  case 1:
    HalLcd_HW_LoadPicture(WiFi_1,24,27,1,0,TRUE);
    break;
  case 2:
    HalLcd_HW_LoadPicture(WiFi_2,24,27,1,0,TRUE);
    break;
  case 3:
    HalLcd_HW_LoadPicture(WiFi_3,24,27,1,0,TRUE);
    break;
  default:
    break;
  }
}

/************************************************************** 
 * @fn      HalLcdWriteSetTemperature
 *
 * @brief   Load the set temperature label picture
 *
 * @param   temper - the temperature to be display
 *
 *          power - true to load the picture,false to clear       
 *
 * @return  None  
 *
**************************************************************/
void HalLcdWriteSetTemperature(uint8 temper,bool power)
{
  uint8 ten = temper / 10;
  uint8 one = temper % 10;
  if(power)
  {
    // write the higher bit of the temperatue
    HalLcd_HW_LoadPicture(&STCODE[ten * 36],24,12,81,0,TRUE);
    // write the low bit of the temperatue
    HalLcd_HW_LoadPicture(&STCODE[one * 36],24,12,98,0,TRUE);
  }
  else
  {
    // write the higher bit of the temperatue
    HalLcd_HW_LoadPicture(&STCODE[ten * 36],24,12,81,0,FALSE);
    HalLcd_HW_WriteAddr(2,81);// reload the line
    for(uint8 i = 0; i < 12; i++)
    {
       HalLcd_HW_WriteData(0x80);
    }
    // write the low bit of the temperatue
    HalLcd_HW_LoadPicture(&STCODE[one * 36],24,12,98,0,FALSE);
    HalLcd_HW_WriteAddr(2,98);  // reload the line 
    for(uint8 i = 0; i < 12; i++)
    {
       HalLcd_HW_WriteData(0x80);
    }
    
  }
  
  
}

/************************************************************** 
 * @fn      HalLcdWriteEndMod
 *
 * @brief   Load the enddevice mode picture
 *
 * @param   MOD - ture to be automatic,false to be manual
 *
 * @return  None  
 *
**************************************************************/
void HalLcdWriteEndMod(bool MOD)
{
   if(MOD)
   HalLcd_HW_LoadPicture(Auto,16,30,2,24,TRUE);
   else
   HalLcd_HW_LoadPicture(Auto,16,30,2,24,FALSE);
}

/************************************************************** 
 * @fn      HalLcdWriteRealTempeMod
 *
 * @brief   Load the real temperature mod label picture
 *
 * @param   mod - True the temperature is the room temperature, false to be the 
 * floor's
 *
 * @return  None  
 *
**************************************************************/
void HalLcdWriteRealTempeMod(bool  mod)
{
  if(mod)
    HalLcd_HW_LoadPicture(RT,24,17,8,40,TRUE); 
  else
    HalLcd_HW_LoadPicture(FT,24,17,8,40,TRUE); 
}
/************************************************************** 
 * @fn      HalLcdWriteRealTemperature
 *
 * @brief   Load the real temperature picture
 *
 * @param   temper-temperature to display
 *
 * @return  None  
 *
**************************************************************/
void HalLcdWriteRealTemperature(uint16 temper)
{
  uint8 hun = temper / 100;
  uint8 ten = temper % 100 / 10;
  uint8 one = temper % 10;
  HalLcd_HW_LoadPicture(&RTCODE[hun * 85],40,17,39,24,TRUE);
  // write the higher bit of the temperatue
  HalLcd_HW_LoadPicture(&RTCODE[ten * 85],40,17,64,24,TRUE);
  // write the middle bit of the temperatue
  HalLcd_HW_LoadPicture(&RTCODE[one * 85],40,17,90,24,TRUE);
  // write the lower bit of the temperature
}


/************************************************************** 
 * @fn      HalLcdWriteRelayState
 *
 * @brief   Load the relay state picture
 *
 * @param   state - ture the relay is closed,false the relay is opened
 *
 * @return  None  
 *
**************************************************************/
void HalLcdWriteRelayState(bool  state)
{
  if(state)
    HalLcd_HW_LoadPicture(Arrow,24,11,112,24,TRUE); 
  else
    HalLcd_HW_LoadPicture(Arrow,24,11,112,24,FALSE); 
}

/************************************************************** 
 * @fn      HalLcdWriteDeviceID
 *
 * @brief   Load the DeviceID
 *
 * @param   num - the num to display
 * 
 *          shift - true to blink the highbit , false lowerbit
 *         
 *          sign - true display D,false display N
 *
 *          blink - blink the char displayed
 *          
 * @return  None  
 *
**************************************************************/
void HalLcdWriteDebug(uint8 num,bool shift,bool sign,bool clear,bool blink)
{
  uint8 ten = num / 16;
  uint8 one = num % 16;
  if(sign)// display the devicce id tab:D:
  {
   HalLcdWriteChar5x8(13,1,30,clear);
   HalLcdWriteChar5x8(17,1,35,clear);
  }
  else// display the PANID tab: N:
  {
   HalLcdWriteChar5x8(16,1,30,clear);
   HalLcdWriteChar5x8(17,1,35,clear);
  }
  
  if(shift)
  {
     HalLcdWriteChar5x8(ten,1,40,blink & clear);
     HalLcdWriteChar5x8(one,1,45,clear);
  }
  else
  {
     HalLcdWriteChar5x8(ten,1,40,clear );
     HalLcdWriteChar5x8(one,1,45,blink & clear);
  }
}
/************************************************************** 
 * @fn      LoadBackground
 *
 * @brief   Load the back ground picture
 *
 * @param   None
 *
 * @return  None  
 *
**************************************************************/
void LoadBackground(void)
{
     uint8 j = 0;
     // write the line
     HalLcd_HW_WriteAddr(2,0);
     for(j = 0; j < 128; j++)
     {
       HalLcd_HW_WriteData(0x80);
     }
     
     
     // write the point
     HalLcd_HW_WriteAddr(6,84);
     for(j = 0; j < 3; j++)
     {
       HalLcd_HW_WriteData(0x80);
     }
     HalLcd_HW_WriteAddr(7,84);
     for(j = 0; j < 3; j++)
     {
       HalLcd_HW_WriteData(0x03);
     }
     //HalLcd_HW_WriteST();
    // HalLcd_HW_WriteSTU();
     HalLcd_HW_WriteRTU();
}


/************************************************************** 
 * @fn      HalLcdWritePower
 *
 * @brief   Load the power picture
 *
 * @param   POWER-TRUE-power on,false-power off 
 *
 * @return  None  
 *
**************************************************************/
void HalLcdWritePower(bool clear)
{
  if(!clear) //remove the picture
  {
       HalLcd_HW_LoadPicture(POWER_PIC,24,19,81,0,FALSE); 
       HalLcd_HW_WriteAddr( 2,81);
       for(uint8 i = 0; i < 19;i++)
       {
         HalLcd_HW_WriteData(0x80);
       }
  }
  else
  {
       HalLcd_HW_LoadPicture( POWER_PIC,24,19,81,0,TRUE);
  }
   
}

/************************************************************** 
 * @fn      HalLcdWriteError
 *
 * @brief   Load the Error picture when the NTC is not break down
 *
 * @param   clear-TRUE-error occur,false-normal 
 *
 * @return  None  
 *
**************************************************************/
void HalLcdWriteError(bool clear)
{
  if(clear)
  {
    HalLcd_HW_LoadPicture(ERROR_PIC,40,68,39,24,TRUE);
  }
  else
  {
     HalLcd_HW_LoadPicture(ERROR_PIC,40,68,39,24,FALSE);
     HalLcd_HW_WriteAddr(6,84);
     
     // write the point
     uint8 j = 0;
     for(j = 0; j < 3; j++)
     {
       HalLcd_HW_WriteData(0x80);
     }
     HalLcd_HW_WriteAddr(7,84);
     for(j = 0; j < 3; j++)
     {
       HalLcd_HW_WriteData(0x03);
     }
  }

}

#endif
/**************************************************************************************************
**************************************************************************************************/



