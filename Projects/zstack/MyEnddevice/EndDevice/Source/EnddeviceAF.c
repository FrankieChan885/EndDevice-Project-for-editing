#include "EnddeviceVariable.h"
#include "EnddeviceAF.h"



/*********************************************************************
 * @fn      EnddeviceApp_HandleAfIncomingMessage
 *
 * @brief   Data message processor callback.  This function processes
 *          any incoming data - probably from other devices.  So, based
 *          on cluster ID, perform the intended action.
 *
 * @param   none
 *
 * @return  none
 */
void EnddeviceApp_HandleAfIncomingMessage( afIncomingMSGPacket_t *pkt )
{  
  uint8 *str = pkt->cmd.Data;
  uint8 len = str[0];
  uint8 id = str[1];
  uint8 command = str[2];
  
 /*****************************for debug **************************/
   //  uint8  n = command /16;
   //  uint8  m = command % 16;
   // HalLcdWriteChar5x8(n,0,50,TRUE);
    // HalLcdWriteChar5x8(m,0,55,TRUE);
/*****************************for debug **************************/
   
 
  // the PANID is changed ,reset the system regardless of id
  if((command == CMD_SET_PANID))// &&  //(pkt->clusterId == COORDINATOR_BROADCAST_CLUSTERID)) 
  {
     if(len == 1)
     {
        _NIB.nwkPanId = str[3];
	    NLME_UpdateNV(0x01);
	    SystemReset();
     }     
  }

  if(id == MyDeviceId)
  {
    //HalLcdWriteDeviceID(pkt->clusterId,TRUE);// for test
    switch (pkt->clusterId)
    {
    case COORDINATOR_COMMAND_CLUSTERID:
     {
       
      switch(command)
      {
        case CMD_SET_ROOM_TEMPER:
             if(len == 1)
             {
              
              // if(POWER)// only when powered on,than we can set the temperature
              // {
                 SetRM_Temperature = str[3];
                 MsgBuf[0] = 1;
                 MsgBuf[1] = MyDeviceId;
                 MsgBuf[2] = CMD_SET_ROOM_TEMPER;
                 MsgBuf[3] = SetRM_Temperature;
                 
                 if(osal_nv_item_init(SET_RM_TEMP_NV,1,NULL) == SUCCESS)
                 {
                     osal_nv_write(SET_RM_TEMP_NV,0,1,&SetRM_Temperature);// load the item to the NV
                     EnddeviceApp_SendP2PMessage(ENDDEVICE_DATA_CLUSTERID,4,MsgBuf);//load sucess send 
                     //message
                 }
                 if(Real_Tem_Mod && POWER)//  room  temperature mode and Power on ,
                  //  we need to diplay the temprerature immediately in LCD
                 {
                   HalLcdWriteSetTemperature(SetRM_Temperature,TRUE);
                 }
         
             }
          break;
        case CMD_SET_FLOOR_TEMPER:// only when powered on,than we can set the temperature
             if(len == 1)
             {
                //if(POWER)
             //   {
                  SetFL_Temperature = str[3];
                  MsgBuf[0] = 1;
                  MsgBuf[1] = MyDeviceId;
                  MsgBuf[2] = CMD_SET_FLOOR_TEMPER;
                  MsgBuf[3] = SetFL_Temperature;
                 if(osal_nv_item_init(SET_FL_TEMP_NV,1,NULL) == SUCCESS)
                 {
                     osal_nv_write(SET_FL_TEMP_NV,0,1,&SetFL_Temperature);// load the item to the NV
                     EnddeviceApp_SendP2PMessage(ENDDEVICE_DATA_CLUSTERID,4,MsgBuf);//load sucess and
                      //send  message   
                 }
                  if(!Real_Tem_Mod && POWER)//  floor  temperature mode
                  {
                   HalLcdWriteSetTemperature(SetFL_Temperature,TRUE);
                  }
               // }
               // else
               // {
               //   ;
               // }
             }
          break;
        case CMD_ROOM_REAL_TEMPER:
          break;
        case CMD_FLOOR_REAL_TEMPER:
          break; 
        case CMD_RELAY_STATE:
          break;
        case CMD_SET_RELAY_STATE:
              if(len == 1)
              {
                if(POWER && (!End_Mod))// only when powered on and the manual mod 
                  //than we can set the Relay   
                {
                  REL_CTL = str[3] & 0x01;
                }
              }
          break;
        case CMD_SET_SLEEP_MODE:
          break;
        case CMD_SET_START_CLOCK:
          break; 
        case CMD_SET_CLOSE_CLOCK:
          break;
        case CMD_SET_NEW_DEVICEID:
          if(len == 1)
          {
            if(!POWER)// only when powered off,that we can change the ID
            {
              //MyDeviceId = debugnum = str[3];
              //if(osal_nv_item_init(MY_DEVICEID_NV,1,NULL) == SUCCESS)
              //{
               //  osal_nv_write(MY_DEVICEID_NV,0,1,&MyDeviceId);
              //}
            }
          }
          break;
        case CMD_SET_PRIORITY:
          break;
        case CMD_SET_MODE:
          if(len == 1)
          {
            if(POWER)// only when powered on,than we can set the Mode
            {
              End_Mod = str[3];
              REL_CTL = 0;
              HalLcdWriteEndMod(End_Mod);// update the mode in LCD
              
              // send mode to the coordinator
              MsgBuf[0] = 1;
	          MsgBuf[1] = MyDeviceId;
	          MsgBuf[2] = CMD_SET_MODE;
	          MsgBuf[3] = End_Mod;
	          EnddeviceApp_SendP2PMessage(ENDDEVICE_DATA_CLUSTERID,4,MsgBuf);
            }
          }
          break; 
        case CMD_SEND_TOKEN:
          if(len == 1)
          TOKEN = TRUE;
          break;
        case CMD_TAKEN_TOKEN:
          if(len == 1)
          TOKEN = FALSE;
          break;
        case CMD_GET_RSSI:
          if(len == 0)
            rssi = pkt->rssi;
          break;
        case CMD_ACK:
          if(len == 1)
          End_Ack = TRUE;
          break;
        case CMD_START_OR_SHUTDOWN:
          if(len == 1)
          {
            POWER = str[3] & 0x01;
              if(POWER) // power on
              {
                REL_CTL = 0;
                Real_Tem_Mod = TRUE;
                
                HalLcdWritePower(FALSE);  // clear the power picture for the ST
                 
                HalLcd_HW_WriteST(TRUE);
                HalLcdWriteSetTemperature(SetRM_Temperature,TRUE);
                HalLcd_HW_WriteSTU(TRUE); 
       
                HalLcdWriteEndMod(End_Mod);
                HalLcdWriteRealTempeMod(TRUE);
              }
              else//power off
              {
                REL_CTL = 0;
                Real_Tem_Mod = TRUE;
                HalLcd_HW_WriteST(FALSE);
                HalLcdWriteSetTemperature(SetRM_Temperature,FALSE);
                HalLcd_HW_WriteSTU(FALSE);// clear the ST for the power picture
                
                HalLcdWritePower(TRUE);  // write the power picture
                
                HalLcdWriteEndMod(FALSE);	
                HalLcdWriteRealTempeMod(TRUE);
             }
             // send the power state to the coordinator
            MsgBuf[0] = 1;
            MsgBuf[1] = MyDeviceId;
            MsgBuf[2] = CMD_START_OR_SHUTDOWN;
            MsgBuf[3] = POWER;
            EnddeviceApp_SendP2PMessage(ENDDEVICE_DATA_CLUSTERID,4,MsgBuf);
         
         }
          break;
        default:
	  break;
       }
       break;
     }
   default :
      break;
   }  
  }
  else // the message is not mine
  {
     ; 
  }
}



/*********************************************************************
 * @fn      EnddeviceApp_SendP2PMessage
 *
 * @brief   Send the PointToPoint message.
 *
 * @param   ClusterID,the clusterID of the message,
            len,the length of the message,
            str,the pointer of the message.
 *
 * @return  none
 */
void EnddeviceApp_SendP2PMessage(uint16 ClusterID,uint16 len,uint8 * str)
{
  
  if(AF_DataRequest(&EnddeviceApp_P2PDstAddr,&EnddeviceApp_epDesc,
                    ClusterID,
                    len,
                    str,
                    &EnddeviceApp_TransID,
                    AF_DISCV_ROUTE,
                    AF_DEFAULT_RADIUS ) == afStatus_SUCCESS)
  {
    
  }
  else
  {
    
    
  } 
}


/*********************************************************************
 * @fn      EnddeviceApp_SendGroupMessage
 *
 * @brief   Send the Group message .
 *
 * @param   ClusterID,the clusterID of the message,
            len,the length of the message,
            str,the pointer of the message.
 *
 * @return  none
 */
void EnddeviceApp_SendGroupMessage(uint16 ClusterID,uint16 len,uint8 * str)
{
  if ( AF_DataRequest( &EnddeviceApp_Group_DstAddr, &EnddeviceApp_epDesc,
                       ClusterID,
                       len,
                       str,
                       &EnddeviceApp_TransID,
                       AF_DISCV_ROUTE,
                       AF_DEFAULT_RADIUS ) == afStatus_SUCCESS )
  {
  }
  else
  {
    // Error occurred in request to send.
  }
}


/*********************************************************************
 * @fn      EnddeviceApp_SendBroadcastMessage
 *
 * @brief   Send the Broadcast message.
 *
* @param   ClusterID,the clusterID of the message,
           len,the length of the message,
           str,the pointer of the message.
 *
 * @return  none
 */
void EnddeviceApp_SendBroadcastMessage(uint16 ClusterID,uint16 len,uint8 * str)
{
  if ( AF_DataRequest( &EnddeviceApp_Broadcast_DstAddr, &EnddeviceApp_epDesc,
                       ClusterID,
                       len,
                       str,//(uint8*)&EnddeviceAppPeriodicCounter,
                       &EnddeviceApp_TransID,
                       AF_DISCV_ROUTE,
                       AF_DEFAULT_RADIUS ) == afStatus_SUCCESS )
  {
  }
  else
  {
    // Error occurred in request to send.
  }
}




