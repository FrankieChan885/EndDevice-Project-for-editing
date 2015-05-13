/**************************************************************************************************
  Filename:       EnddeviceApp.c
  Revised:        $Date: 2009-03-18 15:56:27 -0700 (Wed, 18 Mar 2009) $
  Revision:       $Revision: 19453 $

  Description:    Enddevice Application (no Profile).
**************************************************************************************************/

/*********************************************************************
  
 
*********************************************************************/

/*********************************************************************
 * INCLUDES
 */
#include "EnddeviceVariable.h"
#include "EnddeviceApp.h"
#include "EnddeviceAF.h"
#include "Enddevicekey.h"
#include "EnddeviceLCD.h"
#include "EnddeviceSampleADC.h"
#include "EnddeviceError.h"
#include "NTC_Table.h"
#include "TemperatureControl.h"



//#include"MT_UART.c"
/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * LOCAL FUNCTIONS
 */

/*********************************************************************
 * NETWORK LAYER CALLBACKS
 */

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn      EnddeviceApp_Init
 *
 * @brief   Initialization function for the Generic App Task.
 *          This is called during initialization and should contain
 *          any application specific initialization (ie. hardware
 *          initialization/setup, table initialization, power up
 *          notificaiton ... ).
 *
 * @param   task_id - the ID assigned by OSAL.  This ID should be
 *                    used to send messages and set timers.
 *
 * @return  none
 */
void EnddeviceApp_Init( uint8 task_id )
{ 
  //set the TaskID, 
  EnddeviceApp_TaskID = task_id;
  EnddeviceApp_NwkState = DEV_INIT;
  EnddeviceApp_TransID = 0;
 // PANID = _NIB.nwkPanId;
  P1SEL &= ~(BV(2));// set the P1_2 to be the GPIO to control the relay
  P1DIR |= BV(2);   // set to be output pin
  REL_CTL = 0; // open the relay,shut down the heater

  Enddevice_Init_NV();
 // HalLcdWriteEndMod(End_Mod);
  
  HalLcdWriteRealTempeMod(Real_Tem_Mod);
  HalLcdWritePower(TRUE);  // write the power picture
  
 // HalLcdWriteSetTemperature(SetRM_Temperature);
 // HalLcdWriteRealTemperature(RM_Temperature);

#if defined ( HOLD_AUTO_START )
  // HOLD_AUTO_START is a compile option that will surpress ZDApp
  //  from starting the device and wait for the application to
  //  start the device.
   ZDOInitDevice(0);
#endif

  // Setup for the  broadcast address
  // Broadcast to everyone--------------------by Zimuge
  EnddeviceApp_Broadcast_DstAddr.addrMode = (afAddrMode_t)AddrBroadcast;
  EnddeviceApp_Broadcast_DstAddr.endPoint = EnddeviceApp_ENDPOINT;
  EnddeviceApp_Broadcast_DstAddr.addr.shortAddr = 0xFFFF;

  // Setup for the group  address - Group 1
  // send message to the group--------------------by Zimuge
  EnddeviceApp_Group_DstAddr.addrMode = (afAddrMode_t)afAddrGroup;
  EnddeviceApp_Group_DstAddr.endPoint = EnddeviceApp_ENDPOINT;
  EnddeviceApp_Group_DstAddr.addr.shortAddr = GroupId;
  
  // Setup for the point-2-point address -0x0000(coordinator)
  // send message to the coordinator--------------------by Zimuge
  EnddeviceApp_P2PDstAddr.addrMode = (afAddrMode_t)afAddr16Bit;
  EnddeviceApp_P2PDstAddr.endPoint = EnddeviceApp_ENDPOINT;
  EnddeviceApp_P2PDstAddr.addr.shortAddr = 0x0000;
  
  // Fill out the endpoint description.
  EnddeviceApp_epDesc.endPoint = EnddeviceApp_ENDPOINT;
  EnddeviceApp_epDesc.task_id = &EnddeviceApp_TaskID;
  EnddeviceApp_epDesc.simpleDesc
            = (SimpleDescriptionFormat_t *)&EnddeviceApp_SimpleDesc;
  EnddeviceApp_epDesc.latencyReq = noLatencyReqs;

  
  // Register the endpoint description with the AF
  afRegister( &EnddeviceApp_epDesc );

  // Register for all key events - This app will handle all key events
 // register the key for the EnddeviceApp, so that the key's  callback 
 //function can send the message to the osal,then the Enddevice can handle the event!!---------by Zimuge
  RegisterForKeys( EnddeviceApp_TaskID );

  // By default, all devices start out in Group 1
  EnddeviceApp_Group.ID = GroupId;//0x0001
  osal_memcpy( EnddeviceApp_Group.name, "Group 0", 7  );
  aps_AddGroup( EnddeviceApp_ENDPOINT, &EnddeviceApp_Group );
  
  // Start scan the LCD periodic
  osal_start_timerEx(EnddeviceApp_TaskID,
                     EnddeviceApp_SCAN_LCD_MSG_EVT ,
                     EnddeviceApp_SCAN_LCD_DURATION);
  
}

/*********************************************************************
 * @fn      EnddeviceApp_ProcessEvent
 *
 * @brief   Generic Application Task event processor.  This function
 *          is called to process all events for the task.  Events
 *          include timers, messages and any other user defined events.
 *
 * @param   task_id  - The OSAL assigned task ID.
 * @param   events - events to process.  This is a bit map and can
 *                   contain more than one event.
 *
 * @return  none
 */
uint16 EnddeviceApp_ProcessEvent( uint8 task_id, uint16 events )
{
  afIncomingMSGPacket_t *MSGpkt;
  (void)task_id;  // Intentionally unreferenced parameter

  if ( events & SYS_EVENT_MSG )
  {
    MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( EnddeviceApp_TaskID );
    while ( MSGpkt )
    {
      switch (MSGpkt->hdr.event)
      {
        // Received when a key is pressed
        case KEY_CHANGE:
          EnddeviceApp_HandleKeys( ((keyChange_t *)MSGpkt)->state, ((keyChange_t *)MSGpkt)->keys );
          break;

        // Received when a messages is received (OTA) for this endpoint
        case AF_INCOMING_MSG_CMD:
          EnddeviceApp_HandleAfIncomingMessage( MSGpkt );
          break;

        // Received whenever the device changes state in the network
        case ZDO_STATE_CHANGE:
          EnddeviceApp_NwkState = (devStates_t)(MSGpkt->hdr.status);
          if((EnddeviceApp_NwkState == DEV_END_DEVICE))
          {
            PANID = _NIB.nwkPanId;
            debugnum = MyDeviceId;
            LINKED = TRUE; // for the wifi mode , the device is connected to the net
           
            // register the device id in the gateway
            MsgBuf[FRAME_LEN] = 0x01;
            MsgBuf[FRAME_ROOMID] = MyDeviceId;
            MsgBuf[FRAME_CMD] = CMD_SET_NEW_DEVICEID;
            MsgBuf[FRAME_PARAH] = MyDeviceId;
            EnddeviceApp_SendP2PMessage(ENDDEVICE_DATA_CLUSTERID,4,MsgBuf);
            
            
             // send the initial SetRM_Temperature parameter
             MsgBuf[FRAME_LEN] = 1;
             MsgBuf[FRAME_ROOMID] = MyDeviceId;
            MsgBuf[FRAME_CMD] = CMD_SET_ROOM_TEMPER;
            MsgBuf[FRAME_PARAH] = SetRM_Temperature;
            EnddeviceApp_SendP2PMessage(ENDDEVICE_DATA_CLUSTERID,4,MsgBuf);
                
          // send the initial SetFL_Temperature parameter
          MsgBuf[FRAME_LEN] = 1;
          MsgBuf[FRAME_ROOMID] = MyDeviceId;
          MsgBuf[FRAME_CMD] = CMD_SET_FLOOR_TEMPER;
          MsgBuf[FRAME_PARAH] = SetFL_Temperature;
          EnddeviceApp_SendP2PMessage(ENDDEVICE_DATA_CLUSTERID,4,MsgBuf);
                
          // send the initial POWER parameter
          MsgBuf[FRAME_LEN] = 1;
          MsgBuf[FRAME_ROOMID] = MyDeviceId;
          MsgBuf[FRAME_CMD] = CMD_START_OR_SHUTDOWN;
          MsgBuf[FRAME_PARAH] = POWER;
          EnddeviceApp_SendP2PMessage(ENDDEVICE_DATA_CLUSTERID,4,MsgBuf);
               
          // send the initial mod parameter
          MsgBuf[FRAME_LEN] = 1;
          MsgBuf[FRAME_ROOMID] = MyDeviceId;
          MsgBuf[FRAME_CMD] = CMD_SET_MODE;
          MsgBuf[FRAME_PARAH] = End_Mod;
          EnddeviceApp_SendP2PMessage(ENDDEVICE_DATA_CLUSTERID,4,MsgBuf);
           // Start sending the periodic message in a regular interval.
           osal_start_timerEx( EnddeviceApp_TaskID,
                              EnddeviceApp_SEND_PERIODIC_MSG_EVT,
                          EnddeviceApp_SEND_PERIODIC_MSG_TIMEOUT ); 
          }
          else
          {
            LINKED = FALSE; //  no existing on the network
          }
       break;
     default:
       break;
     }

      // Release the memory
      osal_msg_deallocate( (uint8 *)MSGpkt );
      // Next - if one is available
      MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive(EnddeviceApp_TaskID);
    }

    // return unprocessed events
    return (events ^ SYS_EVENT_MSG);
  }

  // Send a message out - This event is generated by a timer
  //  (setup in EnddeviceApp_Init()).
  if ( events & EnddeviceApp_SEND_PERIODIC_MSG_EVT )
  {
    if(send_tick == 0)
    {
        //get the rssi
        MsgBuf[FRAME_LEN] = 0x00;
        MsgBuf[FRAME_ROOMID] = MyDeviceId;
        MsgBuf[FRAME_CMD] = CMD_GET_RSSI;
        EnddeviceApp_SendP2PMessage(ENDDEVICE_DATA_CLUSTERID,3,MsgBuf);   
    }
        
    if(send_tick == 2)
    { 
       // send the room real temperature   
      MsgBuf[FRAME_LEN] = 2;
      MsgBuf[FRAME_ROOMID] = MyDeviceId;
      MsgBuf[FRAME_CMD] = CMD_ROOM_REAL_TEMPER;
      MsgBuf[FRAME_PARAH] = (RM_Temperature >> 8) & 0x00ff; 
      MsgBuf[FRAME_PARAL] = (RM_Temperature) & 0x00ff;
      EnddeviceApp_SendP2PMessage(ENDDEVICE_DATA_CLUSTERID,5,MsgBuf);     
    
      // send the floor real temperature
      MsgBuf[FRAME_LEN] = 2;
      MsgBuf[FRAME_ROOMID] = MyDeviceId;
      MsgBuf[FRAME_CMD] = CMD_FLOOR_REAL_TEMPER;
      MsgBuf[FRAME_PARAH] = (FL_Temperature >> 8) & 0x00ff; 
      MsgBuf[FRAME_PARAL] = (FL_Temperature) & 0x00ff;
      EnddeviceApp_SendP2PMessage(ENDDEVICE_DATA_CLUSTERID,5,MsgBuf); 
    }
    
    
    if(send_tick == 3)
    {
      //get the rssi
        MsgBuf[FRAME_LEN] = 0x00;
        MsgBuf[FRAME_ROOMID] = MyDeviceId;
        MsgBuf[FRAME_CMD] = CMD_GET_RSSI;
        EnddeviceApp_SendP2PMessage(ENDDEVICE_DATA_CLUSTERID,3,MsgBuf);
    }
    
      
    if(send_tick == 4)
    {
      //control the temperature in the mode auotmatically
      if(End_Mod)
      {
        REL_CTL = ControlTemperature(SetRM_Temperature,SetFL_Temperature,\
                                     RM_Temperature,FL_Temperature);
      }
      // send relay state
      MsgBuf[FRAME_LEN] = 1;
      MsgBuf[FRAME_ROOMID] = MyDeviceId;
      MsgBuf[FRAME_CMD] = CMD_RELAY_STATE;
      MsgBuf[FRAME_PARAH] = REL_CTL;
      EnddeviceApp_SendP2PMessage(ENDDEVICE_DATA_CLUSTERID,4,MsgBuf);
      send_tick = 0;
    }
    
    send_tick++;
   
    // restart the timer
    osal_start_timerEx(EnddeviceApp_TaskID,
                     EnddeviceApp_SEND_PERIODIC_MSG_EVT ,
                     1000 + (osal_rand() & 0x000f)); 
    
    return (events ^ EnddeviceApp_SEND_PERIODIC_MSG_EVT);
  }
  
  // Scan the LCD
  if ( events & EnddeviceApp_SCAN_LCD_MSG_EVT)
  {
    if(sample_tick == 10) // sample the NTC ADC every 2 seconds
    {
 
       uint16 RM_ADC_Result = TemperADCSampleAndAverage(10,0);
       uint16 FL_ADC_Result = TemperADCSampleAndAverage(10,1);
       
       //  detect the NTC error
        Error_Code = NTC_Error_Detect(RM_ADC_Result,FL_ADC_Result);
        
        HandleError(Error_Code);    
        if(!Error_Code)// no error detected  
        {
           RM_Temperature = (uint16)EnddeviceApp_LookupTemp(RM_ADC_Result << 1);
           FL_Temperature = (uint16)EnddeviceApp_LookupTemp(FL_ADC_Result << 1);
        }
       sample_tick = 0;  
    }   
    sample_tick++;
    
    
    EnddeviceApp_HandleLCD();
    
    osal_start_timerEx(EnddeviceApp_TaskID,
                     EnddeviceApp_SCAN_LCD_MSG_EVT ,
                     
                     EnddeviceApp_SCAN_LCD_DURATION); 
    
    return (events ^ EnddeviceApp_SCAN_LCD_MSG_EVT);
    
  }
  
  //  reset the system
  if(events & EnddeviceApp_RESET_MSG_EVT)
  {
     SystemReset();  
  }
  return 0;
}

 void Enddevice_Init_NV(void)
 {
     /***********************************************************************
      *
      *  Load MyDeviceId from the NV if exist ,write MyDeviceId if not exist
      *
      *************************************************************************/
      // the item didn't exist and was initialized successfully
      if(osal_nv_item_init(MY_DEVICEID_NV,1,NULL) == NV_ITEM_UNINIT)
      {
        osal_nv_write(MY_DEVICEID_NV,0,1,&MyDeviceId); // write the item
      }
      // the item existed already
      else if(osal_nv_item_init(MY_DEVICEID_NV,1,NULL) == SUCCESS)
      {
        osal_nv_read(MY_DEVICEID_NV,0,1,&MyDeviceId);// load the item to the RAM
      }
      else;
      
      
      /***********************************************************************
      *
      *  Load SetRM_ from the NV if exist ,write MyDeviceId if not exist
      *
      *************************************************************************/
      if(osal_nv_item_init(SET_RM_TEMP_NV,1,NULL) == NV_ITEM_UNINIT)
      {
        osal_nv_write(SET_RM_TEMP_NV,0,1,&SetRM_Temperature); // write the item
      }
      // the item existed already
      else if(osal_nv_item_init(SET_RM_TEMP_NV,1,NULL) == SUCCESS)
      {
        osal_nv_read(SET_RM_TEMP_NV,0,1,&SetRM_Temperature);// load the item to the RAM
      }
      else;
      
    
      /***********************************************************************
      *
      *  Load MyDeviceId from the NV if exist ,write MyDeviceId if not exist
      *
      *************************************************************************/
      if(osal_nv_item_init(SET_FL_TEMP_NV,1,NULL) == NV_ITEM_UNINIT)
      {
        osal_nv_write(SET_FL_TEMP_NV,0,1,&SetFL_Temperature); // write the item
      }
      // the item existed already
      else if(osal_nv_item_init(SET_FL_TEMP_NV,1,NULL) == SUCCESS)
      {
        osal_nv_read(SET_FL_TEMP_NV,0,1,&SetFL_Temperature);// load the item to the RAM
      }
      else;
 }
