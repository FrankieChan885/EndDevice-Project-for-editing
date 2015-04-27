#include "OSAL.h"
#include "ZGlobals.h"
#include "ZComDef.h"
#include "OSAL_NV.h"
#include "AF.h"
#include "ZDApp.h"

#include "aps_groups.h"


#include "OnBoard.h"

/* HAL */
#include "hal_adc.h"
#include "hal_lcd.h"
#include "hal_key.h"
#include"MT.h"
#include "MT_UART.h"

// These constants are only for example and should be changed to the
// device's needs
#define EnddeviceApp_ENDPOINT           20

#define EnddeviceApp_PROFID             0x0F08
#define EnddeviceApp_DEVICEID           0x0001
#define EnddeviceApp_DEVICE_VERSION     0
#define EnddeviceApp_FLAGS              0

#define EnddeviceApp_MAX_CLUSTERS       4//2
#define ENDDEVICE_DATA_CLUSTERID        1
#define COORDINATOR_COMMAND_CLUSTERID   2
#define COORDINATOR_DATA_CLUSTERID      3
#define COORDINATOR_BROADCAST_CLUSTERID 4
 

#define CMD_SET_ROOM_TEMPER             1  
#define CMD_SET_FLOOR_TEMPER            2  
#define CMD_ROOM_REAL_TEMPER            3  
#define CMD_FLOOR_REAL_TEMPER           4  
#define CMD_RELAY_STATE                 5   
#define CMD_SET_RELAY_STATE             6    
#define CMD_SET_SLEEP_MODE              7  
#define CMD_SET_START_CLOCK             8  
#define CMD_SET_CLOSE_CLOCK             9  
#define CMD_SET_NEW_DEVICEID            10  
#define CMD_SET_PRIORITY                11  
#define CMD_SET_MODE                    12  
#define CMD_SEND_TOKEN                  13  
#define CMD_TAKEN_TOKEN                 14 
#define CMD_GET_RSSI                    15
#define CMD_SET_PANID                   16
#define CMD_ACK                         254
#define CMD_START_OR_SHUTDOWN           255 


#define SATURATION                      30

 

// Send Message Timeout
#define EnddeviceApp_SEND_PERIODIC_MSG_TIMEOUT   5000     // Every 5 seconds                                                

// Application Events (OSAL) - These are bit weighted definitions.
#define EnddeviceApp_SEND_PERIODIC_MSG_EVT       0x0001
#define EnddeviceApp_SCAN_LCD_MSG_EVT            0x0002
#define EnddeviceApp_RESET_MSG_EVT               0x0004
 
// Flash Command Duration - in milliseconds
#define EnddeviceApp_SCAN_LCD_DURATION 200

#define REL_CTL P1_2 // for the control of the relay-------------Zimuge
/*********************************************************************
 * GLOBAL VARIABLES
 */



// This list should be filled with Application specific Cluster IDs.
extern const cId_t EnddeviceApp_ClusterList[EnddeviceApp_MAX_CLUSTERS];


extern const SimpleDescriptionFormat_t EnddeviceApp_SimpleDesc;

// This is the Endpoint/Interface description.  It is defined here, but
// filled-in in EnddeviceApp_Init().  Another way to go would be to fill
// in the structure here and make it a "const" (in code space).  The
// way it's defined in this sample app it is define in RAM.
extern endPointDesc_t EnddeviceApp_epDesc;

/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */
extern uint8 EnddeviceApp_TaskID;   // Task ID for internal task/event processing
                          // This variable will be received when
                          // EnddeviceApp_Init() is called.
extern devStates_t EnddeviceApp_NwkState;

extern uint8 EnddeviceApp_TransID;  // This is the unique message ID (counter)

extern afAddrType_t EnddeviceApp_Broadcast_DstAddr;
extern afAddrType_t EnddeviceApp_Group_DstAddr;
extern afAddrType_t EnddeviceApp_P2PDstAddr;     //µã¶Ôµã

extern aps_Group_t EnddeviceApp_Group;

extern uint8 EnddeviceAppPeriodicCounter;
extern uint8 EnddeviceAppFlashCounter;
extern uint16 GroupId ;

extern uint16 PANID;
//extern uint8 MyNewDeviceId;
extern uint8 MyDeviceId;// the device ID
extern bool  End_Mod;// false :the device is operated manually, true :automatically
extern bool  Real_Tem_Mod; // the real temperature mod ,true to be the room temperature,
//false to be the floor
extern bool  POWER; //false: the device is powered off ,ture :power on 
extern bool TOKEN; // the TOKEN of the device,false:taken,true:given
extern bool End_Ack; // the ack of the gateway.true:ack ,false :not ack
extern uint8 MsgBuf[9];// the message to be send
extern uint8 WiFi_Mode;  // the wifi mode 
extern int8  rssi;
extern uint8 SetRM_Temperature; // the Room Set Temperature
extern uint8 SetFL_Temperature; // the floor Set Temperature

extern int16 RM_Temperature; // the Room Temperature
extern int16 FL_Temperature; // the floor Temperature

extern bool LINKED; // whether the device linked to the coordinator or not
extern uint8 send_tick;       // for sending message
extern uint8 sample_tick; // the sample time tick


extern bool  ID_OR_NET; // for change the device ID or PANID,true for the device ID,
//false to be another
extern bool   BIT_SEL; //  set the bit to be set,true higher bit ,false to be lower
extern uint8  debugnum; // the num for debug use
extern uint8  debughighnum; // for the high bit
extern uint8  debuglownum;// for the low bit
extern bool  DebugMode;// for the debug mode  to use
extern bool  Debug_Blink; // for the information in the lcd in debug mode to blink
extern uint8 Debug_Blink_Count;// for the information in the lcd in debug mode to blink
extern uint16 Error_Code; // the error code for the error_handle
