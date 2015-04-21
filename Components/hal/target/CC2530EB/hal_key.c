/**************************************************************************************************
  Filename:       hal_key.c
  Revised:        $Date: 2010-09-15 19:02:45 -0700 (Wed, 15 Sep 2010) $
  Revision:       $Revision: 23815 $

  Description:    This file contains the interface to the HAL KEY Service.


  Copyright 2006-2010 Texas Instruments Incorporated. All rights reserved.

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
/*********************************************************************
 NOTE: If polling is used, the hal_driver task schedules the KeyRead()
       to occur every 100ms.  This should be long enough to naturally
       debounce the keys.  The KeyRead() function remembers the key
       hal_key_mode of the previous poll and will only return a non-zero
       value if the key hal_key_mode changes.

 NOTE: If interrupts are used, the KeyRead() function is scheduled
       25ms after the interrupt occurs by the ISR.  This delay is used
       for key debouncing.  The ISR disables any further Key interrupt
       until KeyRead() is executed.  KeyRead() will re-enable Key
       interrupts after executing.  Unlike polling, when interrupts
       are enabled, the previous key hal_key_mode is not remembered.  This
       means that KeyRead() will return the current hal_key_mode of the keys
       (not a change in hal_key_mode of the keys).

 NOTE: If interrupts are used, the KeyRead() fucntion is scheduled by
       the ISR.  Therefore, the joystick movements will only be detected
       during a pushbutton interrupt caused by S1 or the center joystick
       pushbutton.

 NOTE: When a switch like S1 is pushed, the S1 signal goes from a normally
       high hal_key_mode to a low hal_key_mode.  This transition is typically clean.  The
       duration of the low hal_key_mode is around 200ms.  When the signal returns
       to the high hal_key_mode, there is a high likelihood of signal bounce, which
       causes a unwanted interrupts.  Normally, we would set the interrupt
       edge to falling edge to generate an interrupt when S1 is pushed, but
       because of the signal bounce, it is better to set the edge to rising
       edge to generate an interrupt when S1 is released.  The debounce logic
       can then filter out the signal bounce.  The result is that we typically
       get only 1 interrupt per button push.  This mechanism is not totally
       foolproof because occasionally, signal bound occurs during the falling
       edge as well.  A similar mechanism is used to handle the joystick
       pushbutton on the DB.  For the EB, we do not have independent control
       of the interrupt edge for the S1 and center joystick pushbutton.  As
       a result, only one or the other pushbuttons work reasonably well with
       interrupts.  The default is the make the S1 switch on the EB work more
       reliably.

*********************************************************************/

/**************************************************************************************************
 *                                            INCLUDES
 **************************************************************************************************/
#include "hal_mcu.h"
#include "hal_defs.h"
#include "hal_types.h"
#include "hal_board.h"
#include "hal_drivers.h"
#include "hal_adc.h"
#include "hal_key.h"
#include "osal.h"

#if (defined HAL_KEY) && (HAL_KEY == TRUE)

/**************************************************************************************************
 *                                              MACROS
 **************************************************************************************************/

/**************************************************************************************************
 *                                            CONSTANTS
 **************************************************************************************************/
#define HAL_KEY_RISING_EDGE   0
#define HAL_KEY_FALLING_EDGE  1
#define HAL_KEY_DEBOUNCE_VALUE  25


/* CPU port interrupt flats*/ 
#define HAL_KEY_CPU_PORT_0_IF P0IF
#define HAL_KEY_CPU_PORT_2_IF P2IF

/* SW is at P0*///By Peng Tao
#define HAL_KEY_PORT     P0
#define HAL_KEY_1_BIT    BV(3)//KEY1 is at P0.3
#define HAL_KEY_2_BIT    BV(2)//KEY2 is at P0.2
#define HAL_KEY_3_BIT    BV(4)//KEY3 is at P0.4
#define HAL_KEY_4_BIT    BV(1)//KEY4 is at P0.1
#define HAL_KEY_5_BIT    BV(0)//KEY5 is at P0.0
#define HAL_KEY_SEL      P0SEL// register  of selecting the function of  P0  
#define HAL_KEY_DIR      P0DIR// register  of selecting the direction of  P0 


/* Switch interrupts *///By Peng Tao
#define CPU_ICTL             IEN1  /* CPU interrupt mask register 1 */
#define HAL_KEY_CPU_ICTLBIT  BV(5)
#define HAL_KEY_ICTL         P0IEN /* P0 Interrupt Control register */
#define HAL_KEY_1_ICTLBIT    BV(3)//KEY1 interrupt control bit is at P0IEN.3
#define HAL_KEY_2_ICTLBIT    BV(2)//KEY2 interrupt control bit is at P0IEN.2
#define HAL_KEY_3_ICTLBIT    BV(4)//KEY3 interrupt control bit is at P0IEN.4
#define HAL_KEY_4_ICTLBIT    BV(1)//KEY4 interrupt control bit is at P0IEN.1
#define HAL_KEY_5_ICTLBIT    BV(0)//KEY5 interrupt control bit is at P0IEN.0



/*Switch Port input hal_key_mode(pulldown/pullup or 3-State)*/ //By Peng Tao
#define HAL_KEY_INPUTMODE        P0INP // P0 INPUT MODE SET
#define HAL_KEY_1_INPMODEBIT     BV(3)//KEY1 INPUT MODE SELECTION BIT IS AT P0INP.3 
#define HAL_KEY_2_INPMODEBIT     BV(2)//KEY2 INPUT MODE SELECTION BIT IS AT P0INP.2 
#define HAL_KEY_3_INPMODEBIT     BV(4)//KEY3 INPUT MODE SELECTION BIT IS AT P0INP.4 
#define HAL_KEY_4_INPMODEBIT     BV(1)//KEY4 INPUT MODE SELECTION BIT IS AT P0INP.1 
#define HAL_KEY_5_INPMODEBIT     BV(0)//KEY5 INPUT MODE SELECTION BIT IS AT P0INP.0 

/*selecting the port for pulldown or pullup,when Switch port is select for pulldown/pullup hal_key_mode */ //By Peng Tao
#define HAL_KEY_PULL_DUP    P2INP  // the register for selecting the pulldown or pullup hal_key_mode for  P0,P1,P2
#define HAL_KEY_PULL_DUPBIT BV(5)  // PDUP0 is the P2INP.5


#define MAX_DOUBLE_HIT_GAP   8// the maxium gap of double hit




/**************************************************************************************************
 *                                            TYPEDEFS

 **************************************************************************************************/



/**************************************************************************************************
 *                                        GLOBAL VARIABLES
 **************************************************************************************************/

static halKeyCBack_t pHalKeyProcessFunction;
static uint8 HalKeyConfigured;
bool  Hal_KeyIntEnable;            /* interrupt enable/disable flag */
//uint8 POWER_KEY_COUNT = 0;   

uint8 double_hit_count = 0;// the key was pressing ,ant the hit_count recorde the count 
bool  double_hit = FALSE;  // the key double hit or not;
uint8 key_depressed_gap = 0; // the gap betweet the double hit

static uint8 halKeySavedKeystate;     /* used to store previous key hal_key_mode in polling hal_key_mode */
static uint8 halKeySavedKeybounce = 0; // the keybouced  hal_key_mode last
static uint8 keystate  = 0; // if the key is pressed , the corresponding bit is set to be high

uint8 hal_key_mode = 0x00; // hal_key_mode = 0x00 the normal hal_key_mode;hal_key_mode = 0x01 the shift pressed;
// hal_key_mode = 0x02 the shift depressed,hal_key_mode = 0x03 exit the double_hit hal_key_mode; 
/**************************************************************************************************
 *                                        FUNCTIONS - Local
 **************************************************************************************************/
void halProcessKeyInterrupt(void);



/**************************************************************************************************
 *                                        FUNCTIONS - API
 **************************************************************************************************/


/**************************************************************************************************
 * @fn      HalKeyInit
 *
 * @brief   Initilize Key Service
 *
 * @param   none
 *
 * @return  None
 **************************************************************************************************/
void HalKeyInit( void )
{
  /* Initialize previous key to 0 */
  halKeySavedKeystate = 0;
  HAL_KEY_SEL &= ~(HAL_KEY_1_BIT + HAL_KEY_2_BIT + HAL_KEY_3_BIT + HAL_KEY_4_BIT + HAL_KEY_5_BIT);    /* Set key1,2,3,4,5 pin 3,2,4,1,0 function to GPIO */
  HAL_KEY_DIR &= ~(HAL_KEY_1_BIT + HAL_KEY_2_BIT + HAL_KEY_3_BIT + HAL_KEY_4_BIT + HAL_KEY_5_BIT);    /* Set pin direction to Input */
  CPU_ICTL &= ~(HAL_KEY_CPU_ICTLBIT);  // disable the P0 interrupt 
  HAL_KEY_ICTL &=  ~(HAL_KEY_1_ICTLBIT + HAL_KEY_2_ICTLBIT + HAL_KEY_3_ICTLBIT + 
                     HAL_KEY_4_ICTLBIT + HAL_KEY_5_ICTLBIT); // disable the key's interrupts
  HAL_KEY_INPUTMODE &= ~(HAL_KEY_1_INPMODEBIT + HAL_KEY_2_INPMODEBIT + HAL_KEY_3_INPMODEBIT +
                        HAL_KEY_4_INPMODEBIT + HAL_KEY_5_INPMODEBIT);// SET THE KEY INPUT PIN TO BE PULLDOWN OR PULLUP MODE;
  HAL_KEY_PULL_DUP &=  ~HAL_KEY_PULL_DUPBIT; // SET TEH KEY INPUT PINS TO BE PULLUP MODE;

  
  /* Initialize callback function */
  pHalKeyProcessFunction  = NULL;
  /* Start with key is not configured */
  HalKeyConfigured = FALSE;
}


/**************************************************************************************************
 * @fn      HalKeyConfig
 *
 * @brief   Configure the Key serivce
 *
 * @param   interruptEnable - TRUE/FALSE, enable/disable interrupt
 *          cback - pointer to the CallBack function
 *
 * @return  None
 **************************************************************************************************/
void HalKeyConfig (bool interruptEnable, halKeyCBack_t cback)
{
  /* Enable/Disable Interrupt or */
  Hal_KeyIntEnable = interruptEnable;

  /* Register the callback fucntion */
  pHalKeyProcessFunction = cback;

  /* Determine if interrupt is enable or not */
  if (Hal_KeyIntEnable)
  {
    /* Do this only after the hal_key is configured - to work with sleep stuff */
    if (HalKeyConfigured == TRUE)
    {
      osal_stop_timerEx(Hal_TaskID, HAL_KEY_EVENT);  /* Cancel polling if active */
    }
  }
  else    /* Interrupts NOT enabled */
  {
    osal_set_event(Hal_TaskID, HAL_KEY_EVENT);
  }

  /* Key now is configured */
  HalKeyConfigured = TRUE;
}


/**************************************************************************************************
 * @fn      HalKeyRead
 *
 * @brief   Read the current hal_key_mode of a key
 *
 * @param   None
 *
 * @return  keys - current keys status
 **************************************************************************************************/
uint8 HalKeyRead ( void )
{
  uint8 keys = 0;
  if(!(HAL_KEY_PORT & HAL_KEY_1_BIT)) // KEY 1 Pressed
  {
     keys |= HAL_KEY_DEC;
  }
  
  if(!(HAL_KEY_PORT & HAL_KEY_2_BIT)) // KEY 2 Pressed
  {
     keys |= HAL_KEY_INC;
  }
  
  if(!(HAL_KEY_PORT & HAL_KEY_3_BIT)) // KEY 3 Pressed
  {
     keys |= HAL_KEY_PWR;
  }
  
  if(!(HAL_KEY_PORT & HAL_KEY_4_BIT)) // KEY 4 Pressed
  {
     keys |= HAL_KEY_FLR;
  }
  
  
  if(!(HAL_KEY_PORT & HAL_KEY_5_BIT)) // KEY 5 Pressed
  {
     keys |= HAL_KEY_AIR;
  }
  
  return keys;
}

/**************************************************************************************************
 * @fn      HalKeyBounce
 *
 * @brief   judge wether the keys  bounce or not
 *
 * @param   None
 *
 * @return  the keys bounce
 **************************************************************************************************/

uint8 HalKeyBounce( void )
{
  uint8 keys = 0;
  if(halKeySavedKeystate & HAL_KEY_DEC) // KEY 1 was Pressed
  {
     if(!(keystate & HAL_KEY_DEC))// KEY 1 Bounce now/depressd
     keys |= HAL_KEY_DEC;
  }
  
  if(halKeySavedKeystate & HAL_KEY_INC) // KEY 2  was Pressed 
  {
     if(!(keystate & HAL_KEY_INC))// KEY 2 Bounc now/depressd
     keys |= HAL_KEY_INC;
  }
  
  if(halKeySavedKeystate & HAL_KEY_PWR) // KEY 3  was Pressed
  {
     if(!(keystate & HAL_KEY_PWR))// KEY 3 Bounce now/depressd
     keys |= HAL_KEY_PWR;
  }
  
 if(halKeySavedKeystate & HAL_KEY_FLR) // KEY 4 was Pressed
  {
     if(!(keystate & HAL_KEY_FLR))// KEY 4 Bounce now/depressd
     keys |= HAL_KEY_FLR;
  }
  
  
 if(halKeySavedKeystate & HAL_KEY_AIR) // KEY 5 was Pressed
  {
     if(!(keystate & HAL_KEY_AIR))// KEY 5 Bounce now/depressd
     keys |= HAL_KEY_AIR;
  }
  return keys;
}
/**************************************************************************************************
 * @fn      HalKeyPoll
 *
 * @brief   Called by hal_driver to poll the keys
 *
 * @param   None
 *
 * @return  None
 **************************************************************************************************/
void HalKeyPoll (void)
{
    
    uint8 keysbounce = 0;
    keystate = HalKeyRead();// read the key hal_key_mode now
    keysbounce = HalKeyBounce();// read the key bounce now
    
  /* If interrupts are not enabled, previous key status and current key status
   * are compared to find out if a key has changed status.
   */
  if (!Hal_KeyIntEnable)
  {
    
    // exit the double_hit mode  
    if(hal_key_mode == 0x03)
    {
      (pHalKeyProcessFunction) (keysbounce, HAL_KEY_STATE_EIXT_DOUBLE_HIT);
      hal_key_mode = 0x00;
    }
    
    /* Store the current keys for comparation next time */
    halKeySavedKeystate = keystate;
      
     /* Invoke Callback if new keys were depressed */
    if (keysbounce && (pHalKeyProcessFunction))
    {
        /***********************whether the double_hit mode************/
        if( (keysbounce == halKeySavedKeybounce) &&
            (
                (keysbounce & HAL_KEY_DEC) || 
                (keysbounce & HAL_KEY_INC)
            ) && 
            (hal_key_mode == 0x00) // only can double hit in the normal mode 
           )// the same key depressed 
        {
          //if the gap is less than the MAX_DOUBLE_HIT_GAP, the key double hit
          if(key_depressed_gap < MAX_DOUBLE_HIT_GAP)
          {
            double_hit = TRUE;
            //double_hit_count++; //and start the count for next double hit 
          }
        }
        /***********************whether the double_hit mode************/
        
        
      // only save the kesbounce when the key depressed
      halKeySavedKeybounce = keysbounce;
      if(keystate & HAL_KEY_PWR)  // the shift is pressed ,another key is depressed 
      {
        (pHalKeyProcessFunction) (keysbounce, HAL_KEY_STATE_SHIFT);
         hal_key_mode = 0x01; 
      }
      else // not the shift hal_key_mode or exit the shift hal_key_mode 
      {
        if(hal_key_mode == 0x01)// in the shift hal_key_mode, the shift depressed 
         //will create the shiftevent(exit the shift hal_key_mode)
        { 
          //keys &= ~HAL_KEY_PWR;
          hal_key_mode = 0x00;
          (pHalKeyProcessFunction) (keysbounce, HAL_KEY_STATE_SHIFTBOUNCE);
        }
        else
        {
          (pHalKeyProcessFunction) (keysbounce, HAL_KEY_STATE_NORMAL);
        }
      }
    }
    
  }
  else
  {
    ; /* Key interrupt handled here */
  }
  
  // the maxium gap between the depressed
  if(key_depressed_gap >= MAX_DOUBLE_HIT_GAP)
  {
   // exit the double_hit mode only when the double_hit
  // change from TRUE to FALSE
    if(double_hit) 
    {
      hal_key_mode = 0x03;  
    }
    key_depressed_gap = MAX_DOUBLE_HIT_GAP - 1;
    double_hit = FALSE;//the gap is the maxium value,can never be double hit
  }
  
  if(keysbounce)// clear the gap between the key depressed ,recount the gap
  {
     key_depressed_gap = 0; 
  }
  key_depressed_gap++;//  the (double_hit_gap * 100) ms  between the keys
  // depressed
}

/**************************************************************************************************
 * @fn      HalKeyEnterSleep
 *
 * @brief  - Get called to enter sleep hal_key_mode
 *
 * @param
 *
 * @return
 **************************************************************************************************/
void HalKeyEnterSleep ( void )
{
  ;
}

/**************************************************************************************************
 * @fn      HalKeyExitSleep
 *
 * @brief   - Get called when sleep is over
 *
 * @param
 *
 * @return  - return saved keys
 **************************************************************************************************/
uint8 HalKeyExitSleep ( void )
{
  return ( HalKeyRead () );
}

#else

void HalKeyInit(void){}
void HalKeyConfig(bool interruptEnable, halKeyCBack_t cback){}
uint8 HalKeyRead(void){ return 0;}
void HalKeyPoll(void){}

#endif /* HAL_KEY */





/**************************************************************************************************
**************************************************************************************************/



