
#include "EnddeviceVariable.h"
#include "EnddeviceSampleADC.h"
#include "hal_adc.h"

/*********************************************************************
 * @fn      TemperADCSampleAndAverage
 *
 * @brief   This function sample the ADC sampletimes and return the average value
 *
 * @param   sampletimes-the times sampled, 
 *           
 *          chanel-which kind of ADC value you want to return
 *
 *          0-room temperature,1-floor temperature
 *
 * @return  the average value
 *******************************************************************************/

uint16 TemperADCSampleAndAverage(uint8 sampletimes,uint8 chanel)
 {
        uint8 i = 0;
        uint32 Result_Sum = 0; // average result
        uint16 Result_Max = 0x0000;
        uint16 Result_Min = 0xffff;
        uint16 ADC_Result = 0x0000;
        for(i = 0; i < sampletimes; i++)
        {
          if(chanel == 0)
          {
            ADC_Result = HalSampleRoomTemperatureADC();
          }
          else if(chanel == 1)
          {
            ADC_Result = HalSampleFloorTemperatureADC();
          }
          else;
    /******************find the minium and maxium value of the RM ADC results**********/
          if(ADC_Result < Result_Min)
          {
            Result_Min = ADC_Result;
          }
          if(ADC_Result > Result_Max)
          {
            Result_Max = ADC_Result;
          } 
          /******************find the minium and maxium value of the FL ADC results**********/ 
          
          Result_Sum += ADC_Result;
        } 
        
        Result_Sum = Result_Sum - Result_Max - Result_Min;
        Result_Sum /= (sampletimes - 2); // average result
        return Result_Sum;
 }