#include "DSP28x_Project.h"
#include <string.h>
#include "lcd12864.h"

#define uint8   unsigned char
#define uint32  unsigned int
#define int8    char
#define int32   int
#define uchar unsigned char
#define uint unsigned int

#define BASIC_SET 0x30               //����ָ�  00110000
#define EXTEND_SET 0x34              //��չָ�  00110100
#define DRAW_ON 0x36                 //��ͼ��ʾ��  00110110
#define DRAW_OFF 0x34                //��ͼ��ʾ��  00110100
#define execute_72us delay_Loop(400) //ִ��ʱ��
#define SCLK_quarter delay_Loop(40)  //�ķ�֮һSCLKʱ�� SCLK��LCD����ͨ��ʱ��
#define SCLK_half delay_Loop(80)     //����֮һSCLKʱ�� SCLK��LCD����ͨ��ʱ��

#define CS_1 GpioDataRegs.GPADAT.bit.GPIO5 = 1      //GPIO5��ΪCS
#define CS_0 GpioDataRegs.GPADAT.bit.GPIO5 = 0      //GPIO5��ΪCS
#define SCLK_1 GpioDataRegs.GPADAT.bit.GPIO4 = 1    //GPIO4��ΪSCLK
#define SCLK_0 GpioDataRegs.GPADAT.bit.GPIO4 = 0    //GPIO4��ΪSCLK
#define SDATA_OUT GpioCtrlRegs.GPADIR.bit.GPIO2 = 1 //GPIO2��ΪSDATA
#define SDATA_IN GpioCtrlRegs.GPADIR.bit.GPIO2 = 0  //GPIO2��ΪSDATA
#define SDATA GpioDataRegs.GPADAT.bit.GPIO2
#define RST_1 GpioDataRegs.GPADAT.bit.GPIO3 = 1 //GPIO3��RST
#define RST_0 GpioDataRegs.GPADAT.bit.GPIO3 = 0 //GPIO3��RST

unsigned char AC_TABLE[] = {
    0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, //��һ�к���λ��
    0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, //�ڶ��к���λ��
    0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f, //�����к���λ��
    0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f, //�����к���λ��
};
static Uint16 frambuff[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int sin[] = {32, 33, 35, 36, 38, 39, 41, 42, 44, 45, 47, 48, 49, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 60, 61, 62, 62, 63, 63, 63, 63, 63, 64, 63, 63, 63, 63, 63, 62, 62, 61, 60, 60, 59, 58, 57, 56, 55, 54, 53, 52, 51, 49, 48, 47, 45, 44, 42, 41, 39, 38, 36, 35, 33, 32, 30, 28, 27, 25, 24, 22, 21, 19, 18, 16, 15, 14, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 3, 2, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 2, 3, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 14, 15, 16, 18, 19, 21, 22, 24, 25, 27, 28, 30};

#define uchar unsigned char
#define uint unsigned int

extern Uint16 SampleTable[];
int votage_Level=0;

Uint16 Key = 1;
void put_point(uchar x,uchar y);

void Delay(uint MS)
{
      uchar us, usn;
      while (MS != 0) //for 12M
      {
            usn = 2;
            while (usn != 0)
            {
                  us = 0xf5;
                  while (us != 0)
                  {
                        us--;
                  };
                  usn--;
            }
            MS--;
      }
}
void DelayKey(uint Second, uint MS100)
{ //���뾫ȷ��0.1S,����","
      uint i;
      for (i = 0; i < Second * 100 + MS100 * 10; i++)
      {
            if (Key == 0)
            {
                  Delay(20);
                  while (Key == 0)
                  {
                        Delay(20);
                  }
                  break;
            }
            else
                  Delay(10);
      }
}

void delay_Loop(Uint32 x)
{
      Uint32 i;
      for (i = 0; i < x; i++)
      {
      }
}

Uint16 read_byte(void)
{
      Uint16 i = 0;
      Uint16 data = 0;
      SDATA_IN;
      data = 0;
      for (i = 0; i <= 7; i++)
      {
            SCLK_0;
            SCLK_quarter;
            SCLK_1;
            SCLK_quarter;
            if (SDATA)
            {
                  data = (data | (0x01 << (7 - i))); //ע�⣡
            }
            SCLK_quarter;
            SCLK_0;
            SCLK_quarter;
      }
      return data;
}

void write_byte(Uint16 x)
{
      Uint16 i = 0;
      SDATA_OUT;
      SCLK_quarter;
      for (i = 0; i <= 7; i++)
      {
            SCLK_0;
            SCLK_quarter;
            SDATA = (0x1 & ((x) >> (7 - i))); //�ȰѸ�λ�Ƴ�Ȼ����
            SDATA = (0x1 & ((x) >> (7 - i)));
            SCLK_quarter;
            SCLK_1; //��ʱ��ĳ������������
            SCLK_half;
            SCLK_0;
            SCLK_quarter;
      }
      SDATA = 0;
}

Uint16 lcd_read_data(void)
{
      Uint16 tmp1, tmp2, data;
      CS_1;
      write_byte(0xFE); //0xFA ==11111110 b ��������MCU��LCD ����������
      tmp1 = read_byte();
      tmp2 = read_byte();
      CS_0;
      execute_72us;
      data = (tmp1 & 0xF0);                 //����λ
      data = (data | ((tmp2 & 0xF0) >> 4)); //����λ
      return data;
}

void lcd_write_data(Uint16 data)
{
      Uint16 tmp1, tmp2;
      CS_1;
      delay_Loop(40);
      tmp1 = (data & 0xF0);        //����λ
      tmp2 = ((data & 0x0F) << 4); //����λ����
      write_byte(0xFA);            //0xFA ==11111010 b ��������MCU��LCD ����������
      write_byte(tmp1);
      write_byte(tmp2);
      delay_Loop(40);
      CS_0;
      execute_72us;
}

void lcd_write_cmd(Uint16 data) //lcd ����д
{
      Uint16 tmp1, tmp2;
      CS_1;
      delay_Loop(40);
      tmp1 = (data & 0xF0);        //����λ
      tmp2 = ((data & 0x0F) << 4); //����λ����
      write_byte(0xF8);            //0xF8 ==11111000 b ��������MCU��LCD ������ָ��
      write_byte(tmp1);
      write_byte(tmp2);
      delay_Loop(40);
      CS_0;
      execute_72us;
}

void lcd_gpio_init(void)
{
      EALLOW;
      GpioCtrlRegs.GPAMUX1.all = 0x00000000; // All GPIO
      GpioCtrlRegs.GPAMUX2.all = 0x00000000; // All GPIO
      GpioCtrlRegs.GPAMUX1.all = 0x00000000; // All GPIO
      GpioCtrlRegs.GPADIR.all = 0x0000003C;  // GPIO2 3 4 5����Ϊ���
      GpioCtrlRegs.GPAPUD.all = 0xFFFFFFC3;  // GPIO2 3 4 5����Ϊ�ڲ�����
      EDIS;
}

/******************************************************************************************
* ��������    ��LcmInit
******************************************************************************************/
void LcmInit(void)
{
      lcd_write_cmd(0x30); //8BitMCU,����ָ���
      lcd_write_cmd(0x03); //AC��0,���ı�DDRAM����
      lcd_write_cmd(0x0C); //��ʾON,�α�OFF,�α�λ����OFF
      lcd_write_cmd(0x01); //����,AC��0
      lcd_write_cmd(0x06); //д��ʱ,�α����ƶ�
}

/******************************************************************************************
* ��������    ��LcmClearTXT
* ��������    ���ı�����RAM����
******************************************************************************************/
void Lcd_ClearTXT(void)
{
      uchar i;
      lcd_write_cmd(0x30); //8BitMCU,����ָ���
      lcd_write_cmd(0x80); //AC����ʼλ
      for (i = 0; i < 64; i++)
            lcd_write_data(0x20);
}

/******************************************************************************************
* ��������    ��LcmClearBMP
* ��������    ��ͼ������RAM����
******************************************************************************************/
void Lcd_ClearBMP(void)
{
      uchar i, j;
      lcd_write_cmd(0x34);     //8Bit����ָ�,��ʹ��36HҲҪд����
      lcd_write_cmd(0x36);     //��ͼON,����ָ�����36H���ܿ���ͼ
      for (i = 0; i < 32; i++) //12864ʵ��Ϊ256x32
      {
            lcd_write_cmd(0x80 | i); //��λ��
            lcd_write_cmd(0x80);     //��λ��
            for (j = 0; j < 32; j++) //256/8=32 byte
                  lcd_write_data(0);
      }
}
/******************************************************************************************
* ��������    ��PutStr
******************************************************************************************/
void lcd_PutStr(uchar row, uchar col, char *puts)
{
      const char endc[] = {"\0"};
      lcd_write_cmd(0x30);                    //8BitMCU,����ָ���
      lcd_write_cmd(AC_TABLE[8 * row + col]); //��ʼλ��
      strcat(puts, endc);
      while (*puts != '\0') //�ж��ַ����Ƿ���ʾ���
      {
            if (col == 8) //�жϻ���
            {             //�����ж�,���Զ��ӵ�һ�е�������
                  col = 0;
                  row++;
            }
            if (row == 4)
                  row = 0; //һ����ʾ��,�ص������Ͻ�
            lcd_write_cmd(AC_TABLE[8 * row + col]);
            lcd_write_data(*puts); //һ������Ҫд����
            puts++;
            lcd_write_data(*puts);
            puts++;
            col++;
      }
}

/******************************************************************************************
* ��������    ��PutBMP
******************************************************************************************/
void lcd_PutBMP(Uint16 *puts)
{
      uint x = 0;
      uchar i, j;
      lcd_write_cmd(0x34);     //8Bit����ָ�,��ʹ��36HҲҪд����
      lcd_write_cmd(0x36);     //��ͼON,����ָ�����36H���ܿ���ͼ

      //                if(puts[x]==0x0000)
      //                {
      //                    break;
      //                }


//      for (i = 0; i < 512; i++) //12864ʵ��Ϊ256x32
//      {
//          //                if(puts[x]==0x0000)
//          //                {
//          //                    break;
//          //                }
//            lcd_write_cmd(0x80 | (i/32)); //��λ��
//            lcd_write_cmd(0x80 | (i%32));     //��λ��
//            lcd_write_data(((puts[i] & 0xFF00) >> 8));
//            lcd_write_data((puts[i] & 0x00FF));
////            DELAY_LOOP();
//
//      }


      for (i = 0; i < 32; i++) //12864ʵ��Ϊ256x32
      {
            lcd_write_cmd(0x80 | i); //��λ��
            lcd_write_cmd(0x80);     //��λ��
            for (j = 0; j < 16; j++) //256/16=16 byte
            {                        //��λ��ÿ���Զ�����

                  lcd_write_data(((puts[x] & 0xFF00) >> 8));
                  lcd_write_data((puts[x] & 0x00FF));
                  x++;
            }
      }
}

void lcd_Clean_Screnn_With_Buffer(Uint16 *puts)
{
      uint x = 0;
      uchar i, j;
      lcd_write_cmd(0x34);     //8Bit����ָ�,��ʹ��36HҲҪд����
      lcd_write_cmd(0x36);     //��ͼON,����ָ�����36H���ܿ���ͼ
      for (i = 0; i < 32; i++) //12864ʵ��Ϊ256x32
      {
            lcd_write_cmd(0x80 | i); //��λ��
            lcd_write_cmd(0x80);     //��λ��
            for (j = 0; j < 16; j++) //256/16=16 byte
            {                        //��λ��ÿ���Զ�����
                if(puts[x]==0x0000)
                {
                    break;
                }
                  lcd_write_data(((puts[x] & 0x0000) >> 8));
                  lcd_write_data((puts[x] & 0x0000));
                  x++;
            }
      }
}

int xytobuffRange(int x,int y)
{
    y = 63 - y;
          int x_byte = x / 16;

          int down = y / 32;
          int buffrange = (y % 32) * 16 + (8 * down + x_byte);
          return buffrange;
}

void lcd_PutPointtoBuffer(int x, int y)
{
    int bits = (0x8000) >> (x % 16);
    int buffrange = xytobuffRange(x, y);
      frambuff[buffrange] = frambuff[buffrange] | bits;
}

void lcd_Init()
{
      lcd_gpio_init();
      delay_Loop(632000); //80ms����ʱ �ϵ���ʱ
      RST_1;
      RST_1;
      delay_Loop(632000);
      LcmInit();
      Lcd_ClearTXT();
      Lcd_ClearBMP();
}

void lcd_Clear_frambuff()
{
      int i = 0;
      for (i = 0; i < 512; i++)
      {
            frambuff[i] = 0;
      }
}

void lcd_Update()
{
      lcd_PutBMP(frambuff);
      lcd_Clear_frambuff();
}

void lcd_Sin_test()
{
      int i = 0;
      for (i = 0; i <= 127; i++)
      {
            lcd_PutPointtoBuffer(i, sin[i]);
            put_point(i, 13);
      }
//      lcd_Update();
}

void lcd_Toast(char *puts)
{
    lcd_PutStr(0,0,puts);
    int i=0;
    for(;i<TOAST_TIME;i++){
        delay_Loop(65535);
    }
    Lcd_ClearTXT();
}

void DELAY_LOOP()
{int i=0;
    for(;i<TOAST_TIME;i++){
        delay_Loop(65535);
    }
}
void lcd_Draw_Sample(void)
{
//    votage_Level=0;
    /****
     * TODO: do add votage_level function here:
     */
    //lcd_Clean_Screnn_With_Buffer(frambuff);
    int j=0;
//    for(j=0;j<128;j++){
//        lcd_PutPointtoBuffer(j,SampleTable[j]>>(6+votage_Level));
//    }
//    lcd_Update();
    for (j=0;j<128;j++)
    {
        put_point(j,SampleTable[j]>>(6+votage_Level));
    }
    lcd_Clear_frambuff();
}

void put_point(uchar x,uchar y)
{
    lcd_PutPointtoBuffer(x, y);
    uint bt=0;
    uchar x_adr,y_adr,h_bit,l_bit;
    y_adr=0x80+y%32;            //����Y��ĵ�ַ��ӦΪ��������64�������ж�32���࣬��Y����31ʱ��Y���������°����ġ�
    if(y>31) //����X��ĵ�ַ��Y����31ʱX�ĵ�ַ���°�������0X88��ʼ��С��31ʱX�ĵ�ַ�����ϰ�������0X80��ʼ
        x_adr=0x88+x/16;
    else
        x_adr=0x80+x/16;
    bt=0x8000>>(x%16); //������㵽�������ĸ���
    lcd_write_cmd(0x34);
    lcd_write_cmd(0x34);
    lcd_write_cmd(y_adr);     //��ȡ���ݵ�ʱ��Ҫ��д����ȡ���ݵĵ�ַ
    lcd_write_cmd(x_adr);
    //lcd_read_data();         //��ȡ�ĵ�һ���ֽڲ�Ҫ��
    int buffrange=xytobuffRange(x, y);
    bt=bt|frambuff[buffrange];
    h_bit=bt>>8;
    l_bit=bt;
    lcd_write_cmd(y_adr);     //д����ʱ������д���ַ����Ϊ��ַ�Ѿ��ı䡣
    lcd_write_cmd(x_adr);
    lcd_write_data(h_bit);
    lcd_write_data(l_bit);
    lcd_write_cmd(0x36); //����ʾ
    lcd_write_cmd(0x30);     //ת�ػ���ָ�
}
