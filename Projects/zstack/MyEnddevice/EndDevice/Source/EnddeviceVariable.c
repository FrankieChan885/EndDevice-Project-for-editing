#include "EnddeviceVariable.h"

const cId_t EnddeviceApp_ClusterList[EnddeviceApp_MAX_CLUSTERS] =
{

 ENDDEVICE_DATA_CLUSTERID,              
 COORDINATOR_COMMAND_CLUSTERID,   
 COORDINATOR_DATA_CLUSTERID,
 COORDINATOR_BROADCAST_CLUSTERID   
};

const SimpleDescriptionFormat_t EnddeviceApp_SimpleDesc =
{
  EnddeviceApp_ENDPOINT,              //  int Endpoint;
  EnddeviceApp_PROFID,                //  uint16 AppProfId[2];
  EnddeviceApp_DEVICEID,              //  uint16 AppDeviceId[2];
  EnddeviceApp_DEVICE_VERSION,        //  int   AppDevVer:4;
  EnddeviceApp_FLAGS,                 //  int   AppFlags:4;
  EnddeviceApp_MAX_CLUSTERS,          //  uint8  AppNumInClusters;
  (cId_t *)EnddeviceApp_ClusterList,  //  uint8 *pAppInClusterList;
  EnddeviceApp_MAX_CLUSTERS,          //  uint8  AppNumInClusters;
  (cId_t *)EnddeviceApp_ClusterList   //  uint8 *pAppInClusterList;
};

// This is the Endpoint/Interface description.  It is defined here, but
// filled-in in EnddeviceApp_Init().  Another way to go would be to fill
// in the structure here and make it a "const" (in code space).  The
// way it's defined in this sample app it is define in RAM.
endPointDesc_t EnddeviceApp_epDesc;

/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */
uint8 EnddeviceApp_TaskID;   // Task ID for internal task/event processing
                          // This variable will be received when
                          // EnddeviceApp_Init() is called.
devStates_t EnddeviceApp_NwkState;

uint8 EnddeviceApp_TransID;  // This is the unique message ID (counter)

afAddrType_t EnddeviceApp_Broadcast_DstAddr;
afAddrType_t EnddeviceApp_Group_DstAddr;
afAddrType_t EnddeviceApp_P2PDstAddr;     //µã¶Ôµã

aps_Group_t EnddeviceApp_Group;

uint8 EnddeviceAppPeriodicCounter = 0;
uint8 EnddeviceAppFlashCounter = 0;
uint16 GroupId = 0x0000;

uint16 PANID = 0;
//uint8 MyNewDeviceId = 1;
uint8 MyDeviceId = 1;// the device ID
bool  End_Mod = FALSE;// false :the device is operated manually, true :automatically
bool  Real_Tem_Mod = TRUE; // the real temperature mod ,true to be the room temperature,
//false to be the floor
bool  POWER = FALSE; //false: the device is powered off ,ture :power on 
bool TOKEN = TRUE; // the TOKEN of the device,false:taken,true:given
bool End_Ack = FALSE; // the ack of the gateway.true:ack ,false :not ack
uint8 MsgBuf[9] =  {0};// the message to be send
uint8 WiFi_Mode = 0;  // the wifi mode 
int8  rssi = 0;    //  the rssi 

uint8 SetRM_Temperature = 22; // the Room Set Temperature
uint8 SetFL_Temperature = 30; // the floor Set Temperature

bool Error = FALSE;// if the NTC is not installed or short curcuit , error is set to true;
bool Error_Last = FALSE;//  error of last time

int16 RM_Temperature = 156; // the Room Temperature
int16 FL_Temperature = 405; // the floor Temperature


bool LINKED = FALSE;
uint8  send_tick  =0;       // for sending message
uint8 sample_tick =0;    // use for sample the temperature

/*****************for debug use******************************/
bool ID_OR_NET = TRUE; // for change the device ID or PANID,true for the device ID,
//false to be another
bool BIT_SEL = FALSE; //  set the bit to be set
uint8  debugnum = 0; // the num for debug use
uint8  debughighnum = 0; // for the high bit
uint8  debuglownum = 0;// for the low bit
bool  DebugMode = FALSE;// for the debug mode  to use
bool  Debug_Blink = FALSE; // for the information in the lcd in debug mode to blink
uint8 Debug_Blink_Count = 0;// for the information in the lcd in debug mode to blink