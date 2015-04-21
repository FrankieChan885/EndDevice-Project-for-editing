#include "EnddeviceVariable.h"
#include "NTC_Table.h"

uint16 EnddeviceApp_LookupTemp( uint16 adcDat);

const unsigned int ntcTableAdcDat[NTC_TABLE_SIZE] = {
	0xDA8,0xD8A,0xD6B,0xD4C,0xD2B,
	0xD0A,0xCE7,0xCC4,0xCA0,0xC7B,
	0xC55,0xC2E,0xC07,0xBDE,0xBB6,
	0xB8C,0xB62,0xB37,0xB0B,0xADF,
	0xAB3,0xA86,0xA58,0xA2B,0x9FD,
	0x9CF,0x9A0,0x972,0x943,0x915,
	0x8E6,0x8B8,0x88A,0x85C,0x82E,
	0x800,0x7D3,0x7A6,0x779,0x74D,
	0x721,0x6F6,0x6CB,0x6A1,0x677,
	0x64E,0x626,0x5FE,0x5D7,0x5B1,
	0x58B,0x566,0x542,0x51E,0x4FB,
	0x4D9,0x4B8,0x497,0x477,0x458,
	0x43A,0x41C,0x3FF,0x3E3,0x3C7,
	0x3AC,0x392,0x378,0x35F,0x347,
	0x330,0x319,0x302,0x2ED,0x2D8,
	0x2C3,0x2AF,0x29C,0x289,0x277,
	0x265,0x254,0x243,0x233,0x223,
	0x214
};

const int ntcTableTemp[NTC_TABLE_SIZE] = {
    -100, -90, -80, -70, -60, -50, -40, -30, -20, -10, 
	 0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 
	10, 110, 120, 130, 140, 150, 160, 170, 180, 190, 
	200, 210, 220, 230, 240, 250, 260, 270, 280, 290, 
	300, 310, 320, 330, 340, 350, 360, 370, 380, 390, 
	400, 410, 420, 430, 440, 450, 460, 470, 480, 490, 
	500, 510, 520, 530, 540, 550, 560, 570, 580, 590, 
	600, 610, 620, 630, 640, 650, 660, 670, 680, 690, 
	700, 710, 720, 730, 740, 750
};



uint16 EnddeviceApp_LookupTemp( uint16 adcDat )
{
    uint16 temp;
	uint8 mid, left = 0, right = NTC_TABLE_SIZE - 1;
	if (adcDat == ntcTableAdcDat[0])
	{
		return ntcTableTemp[0];
	}
	else if (adcDat == ntcTableAdcDat[NTC_TABLE_SIZE - 1])
	{
		return ntcTableTemp[NTC_TABLE_SIZE - 1];
	}
	else
	{
		do
		{
			mid = (left + right) >> 1;
			if (adcDat < ntcTableAdcDat[mid])
			{
				left = mid + 1;
			}
			else if (adcDat > ntcTableAdcDat[mid])
			{
				right = mid - 1;
			}
			else
			{
				return ntcTableTemp[mid];
            }
		}while(left < right); 
        //mid = (left + right) >> 1;// right == left ,right == left -1,mid == right
		temp = ntcTableTemp[right];
		if(ntcTableAdcDat[right] >= adcDat)
        {
          temp += (ntcTableAdcDat[right] - adcDat) * 10 / (ntcTableAdcDat[right] - ntcTableAdcDat[right + 1]);
        }
        else
        {
          temp -= (adcDat - ntcTableAdcDat[right]) * 10 / (ntcTableAdcDat[right - 1] - ntcTableAdcDat[right]);
        }
        
        return temp;
	}   
}


