#include "EnddeviceVariable.h"
#include "hal_lcd.h"
#include "EnddeviceError.h"



/**********************defines*****************************/
#define  NO_ERROR                       0x0000      // no error
#define  NTC_FLOOR_NORMAL_ROOM_BROKEN   0x0002      // the floor ntc normal and the room ntc broken:0 + 2
#define  NTC_FLOOR_NORMAL_ROOM_SHORT    0x0003      // the floor ntc normal and the room ntc short :0 + 3

#define  NTC_FLOOR_BROKEN_ROOM_NORMAL   0x0008      // the floor ntc brokencircuit only  8 + 0
#define  NTC_FLOOR_BROKEN_ROOM_BROKEN   0x000A      // the floor ntc broken and the room ntc broken:8 + 2
#define  NTC_FLOOR_BROKEN_ROOM_SHORT    0x000B      // the floor ntc broken and the room ntc short :8 + 3

#define  NTC_FLOOR_SHORT_ROOM_NORMAL    0x000C      // the floor ntc short and the room ntc normal: C + 0
#define  NTC_FLOOR_SHORT_ROOM_BROKEN    0x000D      // the floor ntc short and the room ntc broken: C + 2
#define  NTC_FLOOR_SHORT_ROOM_SHORT     0x000F     // the floor ntc short and the room ntc short : C + 3


/**********************Variables*****************************/
uint16 Error_Code_Saved = 0x0000;



/**********************Local Functions*****************************/



/*********************************************************************
 * @fn      NTC_Error_Detect
 *
 * @brief   detect the ntc error
 *         
 * @param   Room_ADC_Result  - the room ntc ADC sample value.
 *
 * @param  Floor_ADC_Result - the floor ntc ADC sample value
 *                   
 *
 * @return  the error code
 **********************************************************************/
uint16 NTC_Error_Detect(uint16 Room_ADC_Result,uint16 Floor_ADC_Result)
{
  uint16 error = 0x0000;// the bit1 is set if there is an error on room ntc£¬reset  if no error
      //the bit0 is set if the NTC is broken circuit , reset if the ntc short circuit.
      // samely the bit3 and bit2, if bit3 is set the error occur on the floor ntc,and etc...
   if(Room_ADC_Result >= 1700)
   {
      error |= 0x0002; // room ntc broken xx_10
   }
   else if(Room_ADC_Result <= 150)
   {
      error |= 0x0003;// room ntc short circuit xx_11
   }
   else 
   {
     
      error |= 0x0000;// no error with the room ntc xx_00
   }
   
   if(Floor_ADC_Result >= 1700)
   {
      error |= 0x0008;// floor ntc broken 10_xx
   }
   else if(Floor_ADC_Result <= 150)
   {
      error |= 0x000C;// floor ntc shortcircuit 11_xx
   }
   else
   {
      error |= 0x0000;// no error occur   00_xx 
   }
   
   return error;
}


/*********************************************************************
 * @fn      HandleError
 *
 * @brief  handle the error 
 *         
 * @param  Error_Code-the error code
 *                   
 * @return none
 *
 ***********************************************************************/
void HandleError(uint16 Error_Code)
{
  
 
  switch(Error_Code)// handle the error here
  {
  case NO_ERROR:
    break;
  default:
     REL_CTL = 0;
    break;
  }
  // display the error picture here
  if(Error_Code && (!Error_Code_Saved)) // load error picture if enter the error state
  {
     HalLcdWriteError(TRUE);
  }
  else if(Error_Code_Saved && (!Error_Code))// clear error picture and the error code when 
                                             //leave the error state
  {
     HalLcdWriteError(FALSE);
     HalLcdWriteErrorCode(Error_Code,FALSE); 
     HalLcd_HW_WriteAddr(2,30);
     for(uint8 i = 0; i < 30; i++)
     {
        HalLcd_HW_WriteData(0x80); 
     }
  }
  else;
  
  
  if(Error_Code && (Error_Code_Saved != Error_Code))// write the error code if the code changed
  {
    HalLcdWriteErrorCode(Error_Code,TRUE); 
  }
  else;
  Error_Code_Saved = Error_Code;
}