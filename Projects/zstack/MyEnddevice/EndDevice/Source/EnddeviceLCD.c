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
    if(!Error_Code)// the error doesn't exist
    {
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
 }
  




