



//���������豸���͹���������
extern void EnddeviceApp_HandleAfIncomingMessage( afIncomingMSGPacket_t *pckt );
//�ù㲥�ķ�ʽ��������
extern void EnddeviceApp_SendBroadcastMessage(uint16 ClusterID,uint16 len,uint8 * str);
//���鲥�ķ�ʽ��������
extern void EnddeviceApp_SendGroupMessage(uint16 ClusterID,uint16 len,uint8 * str);
//�õ�Ե�ķ�ʽ��������
extern void EnddeviceApp_SendP2PMessage(uint16 ClusterID,uint16 len,uint8 * str);