//###########################################################################
// LCD12864����dsp28335 ���ּ���ʾ����
//###########################################################################

#include "DSP28x_Project.h" // Device Headerfile and Examples Include File
#include "usr/lcd12864.h"
#include "usr/keyint.h"
#include "usr/adcint.h"
#include "usr/controlCenter.h"

/***
 * TODO: ʱ��߶�  &&  ��ѹ�߶�  &&  ������ʽ
 *       Ƶ�ʲ���  ==> ��������
 */

/******************************************************************************************/


void main(void)
{
    all_Sys_Init();
    lcd_Toast("    ��ӭʹ��      ����ʾ����~!\0");
    //lcd_Sin_test();
    for(;;){
        if(DISPLAY_CURVE){
            if(adc_Over()){
                Lcd_ClearTXT();
                lcd_Draw_Sample();
//                DELAY_LOOP();
                adc_Restart();
            }
        }
        if(!DISPLAY_CURVE)
                   {
                       Lcd_ClearTXT();
                       Lcd_ClearBMP();
                       lcd_PutStr(0, 0, "Ƶ��:\0");
                       DELAY_LOOP();
                   }

    }
}
