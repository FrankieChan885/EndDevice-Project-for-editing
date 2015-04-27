

#define NTC_TABLE_SIZE 86

extern const unsigned int ntcTableAdcDat[NTC_TABLE_SIZE];

extern const int ntcTableTemp[NTC_TABLE_SIZE];

/*************************lookup for the temperature according to the adcDat********/
extern uint16 EnddeviceApp_LookupTemp( uint16 adcDat);

