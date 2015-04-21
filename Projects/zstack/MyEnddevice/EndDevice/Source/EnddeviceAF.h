



//处理其他设备发送过来的数据
extern void EnddeviceApp_HandleAfIncomingMessage( afIncomingMSGPacket_t *pckt );
//用广播的方式发送数据
extern void EnddeviceApp_SendBroadcastMessage(uint16 ClusterID,uint16 len,uint8 * str);
//用组播的方式发送数据
extern void EnddeviceApp_SendGroupMessage(uint16 ClusterID,uint16 len,uint8 * str);
//用点对点的方式发送数据
extern void EnddeviceApp_SendP2PMessage(uint16 ClusterID,uint16 len,uint8 * str);