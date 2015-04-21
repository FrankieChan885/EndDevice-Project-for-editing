#include "EnddeviceVariable.h"
#include "EnddeviceLCD.h"
#include "EnddeviceAF.h"
#include "NTC_Table.h"
#include "EnddeviceSampleADC.h"

void Enddevice_HandleLCD(void);

/*********************************************************************
 * @fn      EnddeviceApp_HandleLCD
 *
 * @brief  scan the LCD
 *                   
 * @param   none
 *
 * @return  none
 */

void EnddeviceApp_HandleLCD(void)
{
    // get the next rssi and sample the temperature every 2s
    if(sample_tick == 10)
    {
 
       uint16 RM_ADC_Result = TemperADCSampleAndAverage(10,0);
       uint16 FL_ADC_Result = TemperADCSampleAndAverage(10,1);
       RM_Temperature = (uint16)EnddeviceApp_LookupTemp(RM_ADC_Result << 1);
       FL_Temperature = (uint16)EnddeviceApp_LookupTemp(FL_ADC_Result << 1);
        
   
        // an error occur
        if((RM_ADC_Result >= 1700) || 
           (FL_ADC_Result >= 1700) ||
           (RM_ADC_Result <= 150)  ||
           (FL_ADC_Result <= 150)
          )
       {
            Error = TRUE; 
            REL_CTL = 0;
       }
       else
       {
            Error = FALSE; 
       }
       sample_tick = 0;
    } 
       
    sample_tick++;
     
    if(rssi >= -55)
      WiFi_Mode = 0;
    else if(rssi >= -85)
      WiFi_Mode = 1;
    else
      WiFi_Mode = 2;
    
    
    // write the wifi mode    
    if(LINKED)
    {
      HalLcdWriteWiFiMod(WiFi_Mode);
    }
    else
    {
      HalLcdWriteWiFiMod(3);// not linked to the zigbee net
    }
   
   // write the debug information
   if(DebugMode)
   { 
      PANID = _NIB.nwkPanId;
      
      // write the PANID of the net
      uint8 first = PANID / 4096;
      uint8 second = PANID / 256 % 16;
      uint8 third = PANID / 16 % 16;
      uint8 fourth = PANID % 16;
      HalLcdWriteChar5x8(first,0,30,TRUE);
      HalLcdWriteChar5x8(second,0,35,TRUE);
      HalLcdWriteChar5x8(third,0,40,TRUE);
      HalLcdWriteChar5x8(fourth,0,45,TRUE);
      
      // write the change content of deviceID or netID
      if(Debug_Blink_Count++ == 3)
      {
        Debug_Blink_Count = 0;
        Debug_Blink ^= 1; // for the blink function
        HalLcdWriteDebug(debugnum,BIT_SEL,ID_OR_NET,TRUE,Debug_Blink);
      }
      
    }
    
    
    // write the relay state
    HalLcdWriteRelayState(REL_CTL);
 
    
    // write the real temperatrue
    if(!Error)// the error doesn't exist
    {
        if(Error_Last)// the error has been removed  for the fisrst time
        {
          HalLcdWriteError(FALSE);
        }
        if(!POWER)
        {
           HalLcdWriteRealTemperature(RM_Temperature);
        }
        else
        {
          if(Real_Tem_Mod)
          {
            HalLcdWriteRealTemperature(RM_Temperature);//SetRM_Temperature * 10);
          }
          else
          {
            HalLcdWriteRealTemperature(FL_Temperature);
          }
        }
    }
    else// the error exists
    {
      if(!Error_Last)//the error occur for the fisrst time
      {
        HalLcdWriteError(TRUE);
      }
    }
    
    Error_Last = Error;
}




