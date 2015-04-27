
#include"EnddeviceVariable.h"
#include "TemperatureControl.h"



uint8 ControlTemperature(uint16 RoomSetValue,uint16 FloorSetValue,
                                  uint16 RoomSampleValue,uint16 FloorSampleValue)
{
  uint8 control = 0;
  bool over_heat = FALSE;
  int16 Error = FloorSetValue * 10 -  FloorSampleValue;
  
  if(Error < -SATURATION)
  {
    over_heat = TRUE; 
  }
  else
  {
    over_heat = FALSE; 
  }
  
  if((POWER) && (!over_heat) && (!Error_Code)) // control the relay only when power on,not overheat
                                               // and no error exist
  {
    Error = RoomSetValue * 10 - RoomSampleValue;
    if(TOKEN)// here is the token  now
    {
      if(Error < -SATURATION)
      {
          control = 0;      
      }
      if(Error > SATURATION)
      {
          control = 1;  
      }
    }
    else// the token is took off
    {
       control = 0;
    } 
  }
  else// not power,overheated or an error occur
  {
      control = 0;
  } 
  
 return control;
}