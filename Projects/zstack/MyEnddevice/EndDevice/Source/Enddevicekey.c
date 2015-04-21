#include "EnddeviceVariable.h"
#include "Enddevicekey.h"
#include "EnddeviceAF.h"

/*********************************************************************
 * @fn      EnddeviceApp_HandleKeys
 *
 * @brief   Handles all key events for this device.
 *
 * @param   shift - true if in shift/alt.
 * @param   keys - bit field for key events. Valid entries:
 *
 * @return  none
 */
void EnddeviceApp_HandleKeys( uint8 shift, uint8 keys)
{
   /*********************************************************
   *          
   *                  the shift is not pressed
   *
  **************************************************************/
  if(shift == 0x00) /// 
  {
    
    /****respond key1 only when powered on**********************************/
    if ( (keys & HAL_KEY_DEC) && (POWER))
    {
      if(End_Mod)// the enddevice is automatically operated
      {
        if(Real_Tem_Mod) // the enddevice now control the room temperature,
                      //decline the set temperature
        {
          SetRM_Temperature--;
          if(SetRM_Temperature <= 16)
            SetRM_Temperature = 16;
          if(SetRM_Temperature >= 30)
            SetRM_Temperature = 30;
        }
        else // the enddevice now control the floor temperature,
             // decline the set temperature
        {
          SetFL_Temperature--;
          if(SetFL_Temperature <= 16)
            SetFL_Temperature = 16;
          if(SetFL_Temperature >= 50)
            SetFL_Temperature =50;
		// send the messge to the coordinator
        }
      }
      else // the enddevice is manually operated
      {
        REL_CTL = 0; // open the relay, shutdown the heater
      }
    }
  
    /**************respond key2 only when powered on**********************/
    if ( (keys & HAL_KEY_INC) &&  (POWER))
    {
       if(End_Mod)// the enddevice is in the auto mode
      {
        if(Real_Tem_Mod) // the enddevice now control the room temperature,
                        //decline the set temperature
        {
          SetRM_Temperature++;
          if(SetRM_Temperature <= 16)
            SetRM_Temperature = 16;
          if(SetRM_Temperature >= 30)
            SetRM_Temperature = 30;
        }
        else // the enddevice now control the floor temperature,
             // decline the set temperature
        {
          SetFL_Temperature++;
          if(SetFL_Temperature <= 16)
            SetFL_Temperature = 16;
          if(SetFL_Temperature >= 50)
            SetFL_Temperature =50;
        }
      }
      else // the enddevice is in the manual mode
      {
        REL_CTL = 1; // open the relay, shutdown the heater
      }
    }
    
  /*******************************key3**********************************/
    if ( keys & HAL_KEY_PWR )
    {
          POWER ^= 1;
          REL_CTL = 0;
          Real_Tem_Mod = TRUE; 
    }
  
  /**************respond key4 only when powered on**********************/
    if ( (keys & HAL_KEY_FLR) &&  (POWER) )
    { 
        Real_Tem_Mod ^= 1;// switch the Temperature to be displayed
    }
    
  /************respond key5 only when powered on**********************/
    if ( (keys & HAL_KEY_AIR) &&  (POWER))
    {
        End_Mod ^= 1;// switch the enddeivece mode
        REL_CTL = 0;
    }
	
  }
  
  /*********************************************************
   *          
   *                  the shift is  pressed
   *
  **************************************************************/
  else if(shift == 0x01 && (!POWER))///////the shift is pressed only
   // in power off mode
  { 
    debughighnum = debugnum / 16;
    debuglownum  = debugnum % 16;
    if((keys & HAL_KEY_AIR) && (DebugMode))// selecting which to be changed the ID or NETID
    {
      ID_OR_NET ^= 1; 
      BIT_SEL = FALSE;    // set the low bit to be changed
    }
    
    if((keys & HAL_KEY_FLR) && (DebugMode))
    {
       BIT_SEL ^= 1; // switch between the low or high bit
    }
    
    if(keys & HAL_KEY_DEC)
    {
      if(!DebugMode)  // enter the debug mode
      {
        DebugMode = TRUE;
      }
      else// in the debug mode to  decline the corresponding  bit num
      {
        if(BIT_SEL)
        {
          debughighnum--;
           if(debughighnum <= 0)
           {
            debughighnum = 0;
           }
           if(debughighnum > 15)
           {
            debughighnum = 0;
           }
        }
        else
        {
           debuglownum--;
           if(debuglownum <= 0)
           {
            debuglownum = 0;
           }
           if(debuglownum > 15)
           {
            debuglownum = 0;
           }
        }
      } 
    }
    
    // in the debug mode to  increase the corresponding  bit num
    if((keys & HAL_KEY_INC) && (DebugMode)) 
    {
       if(BIT_SEL)
      {
        debughighnum++;
         if(debughighnum <= 0)
         {
          debughighnum = 0;
         }
         if(debughighnum > 15)
         {
          debughighnum = 0;
         }
      }
      else
      {
         debuglownum++;
         if(debuglownum <= 0)
         {
          debuglownum = 0;
         }
         if(debuglownum > 15)
         {
          debuglownum = 0;
         }
      }
    }
    debugnum = debughighnum * 16 + debuglownum; 
  }
  
  /*********************************************************
   *          
   *                  the shift is  depressed
   *
  **************************************************************/
  else if(shift == 0x02)
  {
    DebugMode = FALSE;
    if(ID_OR_NET)
    {
      MyDeviceId = debugnum;
      MsgBuf[0] = 1;
      MsgBuf[1] = MyDeviceId;
      MsgBuf[2] = CMD_SET_NEW_DEVICEID;
      MsgBuf[3] = MyDeviceId ;
      
      
      //MyDeviceId = MyDeviceId;
      // the item is initialized already
      if(osal_nv_item_init(MY_DEVICEID_NV,1,NULL) == SUCCESS)
      {
         osal_nv_write(MY_DEVICEID_NV,0,1,&MyDeviceId);
         EnddeviceApp_SendP2PMessage(ENDDEVICE_DATA_CLUSTERID,4,MsgBuf);
        // HalLcdWriteChar5x8(17,0,50,TRUE);
      }
      else if(osal_nv_item_init(MY_DEVICEID_NV,1,NULL) ==  NV_ITEM_UNINIT)
      {
         osal_nv_write(MY_DEVICEID_NV,0,1,&MyDeviceId);
         EnddeviceApp_SendP2PMessage(ENDDEVICE_DATA_CLUSTERID,4,MsgBuf);
      }
      else;
        
      
    }
    else
    {
      PANID = debugnum;
      _NIB.nwkPanId = PANID;
      NLME_UpdateNV(0x01);
      // one second later ,restart
      osal_start_timerEx(EnddeviceApp_TaskID,
                     EnddeviceApp_RESET_MSG_EVT ,
                    2000);   
    }
    
    uint8 first = PANID / 4096;
    uint8 second = PANID / 256 % 16;
    uint8 third = PANID / 16 % 16;
    uint8 fourth = PANID % 16;
    // clear the debug information
    HalLcdWriteDebug(debugnum,BIT_SEL,ID_OR_NET,FALSE,Debug_Blink);
    HalLcdWriteChar5x8(first,0,30,FALSE);
    HalLcdWriteChar5x8(second,0,35,FALSE);
    HalLcdWriteChar5x8(third,0,40,FALSE);
    HalLcdWriteChar5x8(fourth,0,45,FALSE);
    keys = 0;
  }
  
  
  /*********************************************************
   *          
   *                  exit the double_hit mode
   *
  **************************************************************/
  
  else if(shift == 0x03)
  {
    // write the room set temperature  to NV and sent to the coordinator
    MsgBuf[0] = 1;
    MsgBuf[1] = MyDeviceId;
    MsgBuf[2] = CMD_SET_ROOM_TEMPER;
    MsgBuf[3] = SetRM_Temperature;
    if(osal_nv_item_init(SET_RM_TEMP_NV,1,NULL) == SUCCESS)
    { 
       osal_nv_write(SET_RM_TEMP_NV,0,1,&SetRM_Temperature);
    }
    EnddeviceApp_SendP2PMessage(ENDDEVICE_DATA_CLUSTERID,4,MsgBuf);
    
    // write the room set temperature  to NV and sent to the coordinator
    MsgBuf[0] = 1;
    MsgBuf[1] = MyDeviceId;
    MsgBuf[2] = CMD_SET_FLOOR_TEMPER;
    MsgBuf[3] = SetFL_Temperature;
    if(osal_nv_item_init(SET_FL_TEMP_NV,1,NULL) == SUCCESS)
    { 
       osal_nv_write(SET_FL_TEMP_NV,0,1,&SetFL_Temperature);
    }
    EnddeviceApp_SendP2PMessage(ENDDEVICE_DATA_CLUSTERID,4,MsgBuf);
  }
  else;
   
         
  /*********************************************************
   *          
   *   after the  depressed, the related things to do
   *
  **************************************************************/       
      
  if(keys & HAL_KEY_PWR) // the  power key  is depressed 
  {
     if(POWER)
     {
        HalLcdWritePower(FALSE);  // clear the power picture for the ST
        
	    HalLcd_HW_WriteST(TRUE);
        HalLcdWriteSetTemperature(SetRM_Temperature,TRUE);
	    HalLcd_HW_WriteSTU(TRUE); 
	    HalLcdWriteEndMod(End_Mod);
        HalLcdWriteRealTempeMod(TRUE);
     }
     else
     {
        HalLcd_HW_WriteST(FALSE);
        HalLcdWriteSetTemperature(SetRM_Temperature,FALSE);
        HalLcd_HW_WriteSTU(FALSE);
        
        HalLcdWritePower(TRUE);  // write the power picture when powered off
        
        HalLcdWriteEndMod(FALSE);
        
        HalLcdWriteRealTempeMod(TRUE);	
     }
     
      MsgBuf[0] = 1;
      MsgBuf[1] = MyDeviceId;
      MsgBuf[2] = CMD_START_OR_SHUTDOWN;
      MsgBuf[3] = POWER;
      EnddeviceApp_SendP2PMessage(ENDDEVICE_DATA_CLUSTERID,4,MsgBuf);
   }
   if((keys & HAL_KEY_AIR))// the  mode key  is depressed 
   {
	if(POWER) 
	{
        if(End_Mod) // automaticlly
        {
          HalLcdWriteEndMod(TRUE);
        }
        else
        {
          HalLcdWriteEndMod(FALSE);
        }
              // send message mode message to the gateway
         MsgBuf[0] = 1;
         MsgBuf[1] = MyDeviceId;
         MsgBuf[2] = CMD_SET_MODE;
         MsgBuf[3] = End_Mod;
         EnddeviceApp_SendP2PMessage(ENDDEVICE_DATA_CLUSTERID,4,MsgBuf);
     }
     else
     {
       ;	 
     }
   }
  
	 if(keys & HAL_KEY_FLR) // the  real temperatrue mode key  is depressed 
	 {
		 if(POWER)
		 {
			if(Real_Tem_Mod)
			{
				HalLcdWriteSetTemperature(SetRM_Temperature,TRUE);
				HalLcdWriteRealTempeMod(TRUE);
			}	
			else
			{
				HalLcdWriteSetTemperature(SetFL_Temperature,TRUE);
				HalLcdWriteRealTempeMod(FALSE);
			}
		 }
		 else
		 {
			;
		 }
	 }	
  
	  if(keys & HAL_KEY_DEC)// the  dec key  is depressed 
	  {
		 if(POWER)// power on
		 {
			if(End_Mod)// automatically
			{
				if(Real_Tem_Mod)// set room temperatrue
				{
                   HalLcdWriteSetTemperature(SetRM_Temperature,TRUE);
                    // not in the double_hit mode ,send the message immediately
                   if(!double_hit)
                   {
                     MsgBuf[0] = 1;
                     MsgBuf[1] = MyDeviceId;
                     MsgBuf[2] = CMD_SET_ROOM_TEMPER;
                     MsgBuf[3] = SetRM_Temperature;
                     if(osal_nv_item_init(SET_RM_TEMP_NV,1,NULL) == SUCCESS)
                     { 
                       osal_nv_write(SET_RM_TEMP_NV,0,1,&SetRM_Temperature);
                     }
                     EnddeviceApp_SendP2PMessage(ENDDEVICE_DATA_CLUSTERID,4,MsgBuf);
                   }
                   else;
				}
                else// set floor temperatrue
				{
					HalLcdWriteSetTemperature(SetFL_Temperature,TRUE);
                     // not in the double_hit mode ,send the message immediately
                    if(!double_hit)
                    {
                      MsgBuf[0] = 1;
					  MsgBuf[1] = MyDeviceId;
					  MsgBuf[2] = CMD_SET_FLOOR_TEMPER;
					  MsgBuf[3] = SetFL_Temperature;
                      if(osal_nv_item_init(SET_FL_TEMP_NV,1,NULL) == SUCCESS)
                      {
                        osal_nv_write(SET_FL_TEMP_NV,0,1,&SetFL_Temperature); 
                      }
					  EnddeviceApp_SendP2PMessage(ENDDEVICE_DATA_CLUSTERID,4,MsgBuf);
                    }
				}					
			}
			else // manually
			{
				;
			}
		 }			 
		 else
		 {
			;  
		 }  
	  }
	  
	  if(keys & HAL_KEY_INC)// the  inc key  is depressed 
	  {
		 if(POWER)// power on
		 {
			if(End_Mod)// automatically
			{
				if(Real_Tem_Mod)// set room temperatrue
				{
                    HalLcdWriteSetTemperature(SetRM_Temperature,TRUE);
                    // not in the double_hit mode ,send the message immediately
                    if(!double_hit)
                    {
                         MsgBuf[0] = 1;
                         MsgBuf[1] = MyDeviceId;
                         MsgBuf[2] = CMD_SET_ROOM_TEMPER;
                         MsgBuf[3] = SetRM_Temperature;
                         if(osal_nv_item_init(SET_RM_TEMP_NV,1,NULL) == SUCCESS)
                         { 
                           osal_nv_write(SET_RM_TEMP_NV,0,1,&SetRM_Temperature);
                         }
                         EnddeviceApp_SendP2PMessage(ENDDEVICE_DATA_CLUSTERID,4,MsgBuf);
                   }
                   else;
				}
                else// set floor temperatrue
				{
					HalLcdWriteSetTemperature(SetFL_Temperature,TRUE);
                    // not in the double_hit mode ,send the message immediately
                    if(!double_hit)
                    {
                      MsgBuf[0] = 1;
					  MsgBuf[1] = MyDeviceId;
					  MsgBuf[2] = CMD_SET_FLOOR_TEMPER;
					  MsgBuf[3] = SetFL_Temperature;
                      
                      if(osal_nv_item_init(SET_FL_TEMP_NV,1,NULL) == SUCCESS)
                      {
                        osal_nv_write(SET_FL_TEMP_NV,0,1,&SetFL_Temperature); 
                      }
					  EnddeviceApp_SendP2PMessage(ENDDEVICE_DATA_CLUSTERID,4,MsgBuf);
                    }
				}					
			}
			else // manually
			{
				;
			}
		 }			 
		 else
		 {
			;  
		 }  
	  }
	  
	  
	
}