#include "hal_font.h"
#include "hal_oled.h"
#include "osal.h"

/****************************************************
* B) Command Table per device *
****************************************************/
#define 	ColAddrLo 	0x00
#define 	ColAddrHi 	0x10
#define 	SetAddrMode 	0x20
#define 	SetColAddr 	0x21
#define 	SetPagAddr 	0x22
#define 	DisplayStart 	0x40
#define 	ContCtrlReg 	0x81//
#define 	ChgPump 	0x8D//
#define 	SegRemapOff 	0xA0//
#define 	SegRemapOn 	0xA1//
#define 	ExitEntireD 	0xA4//
#define 	EntEntireD 	0xA5//
#define 	NormalDisp 	0xA6//
#define 	ReverseDisp 	0xA7//
#define 	CmdMuxRatio 	0xA8//
#define 	DisplayOff 	0xAE//
#define 	DisplayOn 	0xAF//
#define 	PageAddr 	0xB0//
#define 	ComRemapOff 	0xC0//-
#define 	ComRemapOn 	0xC8//-
#define 	DispOffset 	0xD3//-
#define 	SetOscFrq 	0xD5
#define 	PreChgPeriod 	0xD9
#define 	ComPin  	0xDA
#define 	VComH   	0xDB
#define 	LcdNOP  	0xE3

#define 	Device SSD1306 			/* device under demo */
#define 	ColNo 		132 		/* number of Column/Seg on LCD glass*/
#define 	RowNo 		64		/* number of Row/Com/Mux */
#define 	PS 		1 		/* fixed to Parallel mode */
#define 	PageNo 		10 		/* Total no of RAM pages */
#define 	IconPage 	10 		/* Icon Page number */
#define 	All0 		6 		/* 3 for all 0, 4 for all 1 */
#define 	All1 		4
#define 	iIntRegValue 	1 		/*Internal Regulator Resistor Ratio Value */
#define 	iContCtrlRegValue 16 		/* Contrast Control Register Value */
#define 	iIntRegValuea 	20 		/*Internal Regulator Resistor Ratio Value */
#define 	iContCtrlRegValuea 16 		/* Contrast Control Register Value */
#define 	iIntRegValueb 	1 		/*Internal Regulator Resistor Ratio Value */
#define 	iContCtrlRegValueb 16 		/* Contrast Control Register Value */
#define 	MSGNo 		16
#define 	MSGLength 	22
#define 	SSLNameNo 	4
#define 	DevicePg 	0 		//RAM page for showing device name
#define 	FeaturePg 	1 		//RAM page for showing feature
#define 	GRAPHICNo 	13
#define 	xlogo 		38
#define 	ylogo 		5
#define 	xsolomon 	91
#define 	ysolomon 	2
#define 	xsystech 	81
#define 	ysystech 	2
#define 	xlimited 	70
#define 	ylimited 	2
#define 	xcc 		16
#define 	ycc 		2
#define 	xpageq 		128
#define 	ypageq 		4
#define 	horizonal 	0
#define 	d_time 		60
////////////////////////////////////////////////////////////////////////////////////////////

static float f_vcc_full   = 3.6;
static float f_vcc_afull  = 3.2;
static float f_vcc_midd   = 2.8;
static float f_vcc_aempty = 2.5;

void WaitUs(uint16 microSecs);
void Wait100Ns(uint16 microSecs);
void Print16_8(unsigned short y,unsigned short x, unsigned char ch[],unsigned short yn);
void PrintCh32_16(unsigned short y,unsigned short x, unsigned char ch,unsigned short yn);
void Print32_16(unsigned short y,unsigned short x, unsigned char ch[],unsigned short yn);
void WriteLCDbyte(unsigned char dat);
void WriteLCD(unsigned char fs, unsigned char da);
void SetRamAddr(unsigned char x ,unsigned char y);
void Rectangle(unsigned char x1,unsigned char y1,unsigned char x2,unsigned char y2);
void WriteLCD(unsigned char fs, unsigned char da);
//void Printico(unsigned char xx, unsigned char yy, unsigned char ico[], unsigned char ww, unsigned char ll);	
void PrintWirelessQuality(unsigned char x);
void PrintBatteryCapacity(float x);		
void LcdPowerON(void);
void LcdPowerOFF(void);

unsigned char ContrastValue = 0xff;  //�Աȶ�

/**************************************************************************************************
* @fn      WaitUs
*
* @brief   wait for x us. @ 32MHz MCU clock it takes 32 "nop"s for 1 us delay.
*
* @param   x us. range[0-65536]
*
* @return  None
**************************************************************************************************/
void WaitUs(uint16 microSecs)
{
  while(microSecs--)
  {
    /* 32 NOPs == 1 usecs */
    asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop");
    asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop");
    asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop");
    asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop");
    asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop");
    asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop");
    asm("nop"); asm("nop");
  }
}

/**************************************************************************************************
* @fn      WaitUs
*
* @brief   wait for x us. @ 32MHz MCU clock it takes 32 "nop"s for 1 us delay.
*
* @param   x us. range[0-65536]
*
* @return  None
**************************************************************************************************/
void Wait100Ns(uint16 microSecs)
{
  while(microSecs--)
  {
    /* 32 NOPs == 1 usecs */
    asm("nop");asm("nop");asm("nop");
  }
}

//******************************************************************************
//��������void WriteLCDbyte(unsigned char dat)
//���룺����
//�������
//����������LCDд���ݣ�һ��д��8λ
//******************************************************************************
void WriteLCDbyte(unsigned char dat)
{
  
  unsigned char ii;		
  
  for(ii = 0 ; ii < 8; ii++)
  {
    LCD_SCLK = 0;
    if(dat & 0x80) 	
      LCD_SDIN = 1;
    else		
      LCD_SDIN = 0;
    dat <<= 1;
    LCD_SCLK = 1;	
  }
}

//*****************************************************************************
//*****************************************************************************
//��������WriteLCD(unsigned char fs, unsigned char da)
//����������fs��0Ϊ���1Ϊ����   da����д����
//*****************************************************************************
void WriteLCD(unsigned char fs, unsigned char da)
{
  unsigned char ii;	
  LCD_CS = 0;
  LCD_SCLK = 0;
  if(fs)
    LCD_SDIN = 1;	
  else
    LCD_SDIN = 0;
  LCD_SCLK = 1;	
  
  for(ii = 0 ; ii < 8; ii++)
  {
    LCD_SCLK = 0;
    if(da & 0x80) 	
      LCD_SDIN = 1;
    else		
      LCD_SDIN = 0;
    da <<= 1;
    LCD_SCLK = 1;	
  }
  
  LCD_CS = 1;
}


void SetRamAddr(unsigned char x ,unsigned char y)
{
  unsigned char temp;
  
  temp = 0x0f & x;
  WriteLCD(COMMAND , PageAddr|temp);
  
  temp = 0x0f & (y >> 4);
  WriteLCD(COMMAND , ColAddrHi|temp);
  temp = 0x0f & y;
  WriteLCD(COMMAND , ColAddrLo|temp);
}

/*******************************************************************************
//��������void SetContrast(unsigned char Gain, unsigned char Step)
//���ܣ�lcd�Աȶ��趨
//���룺Gain,�Ҷ� Step���Աȶ�
//�������
********************************************************************************/
void SetContrast(unsigned char Step)
{
  WriteLCD(COMMAND , ContCtrlReg);//--set contrast control register
  WriteLCD(COMMAND , Step);
}

/*******************************************************************************
//��������void InitDisplay(void)
//���ܣ�lcd�趨Ϊ������ʾ״̬
//���룺��
//�������
********************************************************************************/
void InitDisplay(void)
{
//  P0DIR |= 0x25;
//  P1DIR |= 0x0C;
  P0DIR |= 0x00;
  P1DIR |= 0xE4;
  LCD_RST = 0;
  WaitUs(10000);
  LCD_RST = 1;
//  LCD_DC =1;
  WriteLCD(COMMAND , DisplayOff);//--turn off oled panel
  WriteLCD(COMMAND , ColAddrLo);//---set low column address
  WriteLCD(COMMAND , ColAddrHi);//---set high column address
  WriteLCD(COMMAND , DisplayStart);//--set start line address
  WriteLCD(COMMAND , ContCtrlReg);//--set contrast control register
  WriteLCD(COMMAND , ContrastValue);
  WriteLCD(COMMAND , SegRemapOn);//--set segment re-map 95 to 0
  WriteLCD(COMMAND , NormalDisp);//--set normal display
  WriteLCD(COMMAND , ComRemapOn);//--set remap on
  WriteLCD(COMMAND , CmdMuxRatio);//--set multiplex ratio(1 to 64)
  WriteLCD(COMMAND , 0x3f);//--1/64 duty
  WriteLCD(COMMAND , DispOffset);//-set display offset
  WriteLCD(COMMAND , 0x00);//-not offset
  WriteLCD(COMMAND , SetOscFrq);//--set display clock divide ratio/oscillator frequency
  WriteLCD(COMMAND , 0xF0);//--set divide ratio
  WriteLCD(COMMAND , PreChgPeriod);//--set pre-charge period
  WriteLCD(COMMAND , 0xf1);
  WriteLCD(COMMAND , ComPin);//--set com pins hardware configuration
  WriteLCD(COMMAND , 0x12);
  WriteLCD(COMMAND , VComH);//--set vcomh
  WriteLCD(COMMAND , 0x40);
  WriteLCD(COMMAND , ChgPump);//--set Charge Pump enable/disable
  WriteLCD(COMMAND , 0x14);//--set(0x10) disable
  WriteLCD(COMMAND , DisplayOn);//--turn on oled panel
}


/*******************************************************************************
//��������void contrastctrl(unsigned char start,stop)
//���ܣ�lcd�Աȶȵ���
//���룺��
//�������
********************************************************************************/
void contrastctrl(unsigned char start, unsigned char stop)
{
  unsigned char i;
  if (start < stop)
  {
    for (i=start; i<stop; i+=1)
    {
      SetContrast(i); //slowly turn on display
      WaitUs(10000);
    }
  }
  else
  {
    for (i=start; i>stop; i-=1)
    {
      SetContrast(i); //slowly turn off display
      WaitUs(10000);
    }
  }
}

void ClearScreen(void)
{
  unsigned char x,y;
  for(x = 0;x < 8 ;x++){
    for(y = 0 ; y < 128 ; y++){	  		
      SetRamAddr(x , y);			
      WriteLCD(DATA , 0x00);
    }		
  }
}

void ClearRow16(unsigned char rowbegin,unsigned char rowend)
{
  unsigned char x,y;
  for(x = rowbegin;x <= rowend ;x++){
    for(y = 0 ; y < 128 ; y++){	  		
      SetRamAddr(x , y);			
      WriteLCD(DATA , 0x00);
    }		
  }
}

/*******************************************************************************
//��������void Print6(unsigned char xx, unsigned char yy, unsigned char ch1[], unsigned char yn)
//���ܣ���ʾ6*8�ַ���
//���룺xx ,yy ����,ch1����ʾ���ַ���,yn�Ƿ񷴺�
//�������
********************************************************************************/
void Print6(unsigned char xx, unsigned char yy, unsigned char ch1[], unsigned char yn)		
{
  unsigned char ii = 0;
  unsigned char bb = 0;
  unsigned int index = 0 ;	
  
  while(ch1[bb] != '\0')
  {
    index = (unsigned int)(ch1[bb] - 0x20);
    index = (unsigned int)index*6;		
    for(ii=0;ii<6;ii++)
    {
      SetRamAddr(xx , yy);
      if(yn == 0)
      {
        WriteLCD(DATA, ~FontSystem6x8[index]);
        
      }
      else
      {
        WriteLCD(DATA, FontSystem6x8[index]);
      }		
      index += 1;
      yy += 1;
    }		
    bb += 1;
  }
}


//*******************************************************************************
//��������void Printn8(unsigned char xx ,unsigned char yy , unsigned long no,unsigned char yn,unsigned char le)
//���ܣ���ʾ8*8һ���޷�������
//���룺xx , yy��Ļ����λ��,no����ʾ���� yn=0������ʾ yn=1������ʾ  le��Чλ
//�������
//*******************************************************************************
void Printn8(unsigned char xx ,unsigned char yy , unsigned long no,unsigned char yn,unsigned char le)
{
  unsigned char ch2[6];
  unsigned char ii;
  
  for(ii = 1 ; ii <= le ;){
    ch2[le - ii] = no % 10 + 0x30;
    no /= 10;
    ii += 1;
  }
  ch2[le] = '\0';
  Print16_8(xx ,yy ,ch2 ,yn);
}

/*******************************************************************************
//��������void Print16_8(unsigned short y,unsigned short x, unsigned char ch,unsigned short yn)
//���ܣ���ʾ16*8�ַ�
//���룺xx ,yy ����,ch����ʾ���ַ�,yn�Ƿ񷴺�
//�������
********************************************************************************/
void PrintCh16_8(unsigned short y,unsigned short x, unsigned char ch,unsigned short yn)
{
  unsigned char i,wm;
  unsigned short adder;
  
  adder = (ch - 0x20) * 16;
  for(i=0;i<2;i++)
  {
    for(wm = 0;wm < 8;wm++)
    {
      SetRamAddr(y , x);
      if(yn == 0)
      {
        WriteLCD(DATA, ~Font16X8[adder]);
      }
      else
      {
        WriteLCD(DATA, Font16X8[adder]);
      }
      adder += 1;
      x += 1;
    }
    y += 1;
    x -= 8;
  }
}

/*******************************************************************************
//��������void Print32_16(unsigned short y,unsigned short x, unsigned char ch,unsigned short yn)
//���ܣ���ʾ32*16�ַ�
//���룺xx ,yy ����,ch����ʾ���ַ�,yn�Ƿ񷴺�
//�������
********************************************************************************/
void PrintCh32_16(unsigned short y,unsigned short x, unsigned char ch,unsigned short yn)
{
  unsigned char i,wm;
  unsigned short adder;
  
  adder = (ch - 0x20) * 64;
  for(i=0;i<4;i++)
  {
    for(wm = 0;wm <16;wm++)
    {
      SetRamAddr(y , x);
      if(yn == 0)
      {
        WriteLCD(DATA, ~Font32X16[adder]);
      }
      else
      {
        WriteLCD(DATA, Font32X16[adder]);
      }
      adder += 1;
      x += 1;
    }
    y += 1;
    x -= 16;
  }
}

/*******************************************************************************
//��������void Print16_8(unsigned short y,unsigned short x, unsigned char ch[],unsigned short yn)
//���ܣ���ʾ16*8�ַ���
//���룺xx ,yy ����,ch1����ʾ���ַ���,yn�Ƿ񷴺�
//�������
********************************************************************************/
void Print16_8(unsigned short y,unsigned short x, unsigned char ch[],unsigned short yn)
{
  unsigned char ii = 0;
  while(ch[ii] != '\0')
  {
    PrintCh16_8(y,x,ch[ii],yn);
    ii += 1;
    x += 8;
  }
  
}

/*******************************************************************************
//��������void Print32_16(unsigned short y,unsigned short x, unsigned char ch[],unsigned short yn)
//���ܣ���ʾ32*16�ַ���
//���룺xx ,yy ����,ch1����ʾ���ַ���,yn�Ƿ񷴺�
//�������
********************************************************************************/
void Print32_16(unsigned short y,unsigned short x, unsigned char ch[],unsigned short yn)
{
  unsigned char ii = 0;
  while(ch[ii] != '\0')
  {
    PrintCh32_16(y,x,ch[ii],yn);
    ii += 1;
    x += 16;
  }
  
}

/*******************************************************************************
//��������void Print16(unsigned short y,unsigned short x,unsigned char ch[],unsigned short yn)
//���ܣ�����Ļ����ʾ����
//���룺x ,y ����,ch[]����ʾ�ĺ���,yn�Ƿ񷴺�
//�������
********************************************************************************/
void Print16(unsigned short y,unsigned short x,unsigned char ch[],unsigned short yn)
{
  unsigned char wm ,ii = 0;
  unsigned short adder;
  
  wm = 0;
  adder = 1;
  while(FontNew8X16_Index[wm] > 128)
  {
    if(FontNew8X16_Index[wm] == ch[ii])
    {
      if(FontNew8X16_Index[wm + 1] == ch[ii + 1])
      {
        adder = wm * 16;
        break;
      }
    }
    wm += 2;				//�ҵ������������е�λ��
  }
  SetRamAddr(y , x);
  
  if(adder != 1)					//�ҵ����֣���ʾ����	
  {
    
    for(wm = 0;wm < 16;wm++)
    {
      SetRamAddr(y , x);
      if(yn == 0)
      {
        WriteLCD(DATA, ~FontNew16X16[adder]);
      }
      else
      {
        WriteLCD(DATA, FontNew16X16[adder]);
      }
      adder += 1;
      x += 1;
    }
    y += 1;
    x -=16;
    
    for(wm = 0;wm < 16;wm++)
    {
      SetRamAddr(y , x);
      if(yn == 0)
      {
        WriteLCD(DATA, ~FontNew16X16[adder]);
      }
      else
      {
        WriteLCD(DATA, FontNew16X16[adder]);
      }
      adder += 1;
      x += 1;
    }
    for(wm = 0;wm < 2;wm++)
    {
      SetRamAddr(y , x);
      if(yn == 0)
      {
        WriteLCD(DATA, 0xff);
      }
      else
      {
        WriteLCD(DATA, 0x00);
      }
      x += 1;
    }
    
    
  }
  else						//�Ҳ�������ʾ�ո�			
  {
    ii += 1;
    
    for(wm = 0;wm < 16;wm++)
    {
      SetRamAddr(y , x);
      if(yn == 0)
      {
        WriteLCD(DATA, 0xff);
      }
      else
      {
        WriteLCD(DATA, 0x00);
      }
      x += 1;
    }
    y += 1;
    x -= 16;
    for(wm = 0;wm < 16;wm++)
    {
      SetRamAddr(y , x);
      if(yn == 0)
      {
        WriteLCD(DATA, 0xff);
      }
      else
      {
        WriteLCD(DATA, 0x00);
      }
      x += 1;
    }
  }
}


/*******************************************************************************
//��������void Print(unsigned char y, unsigned char x, unsigned char ch[], unsigned short yn)
//���ܣ�ʵ�ֺ��ּ���ĸ�����ʾ
//���룺x ,y ����,ch[]����ʾ�ĺ��ֻ���ĸ,yn�Ƿ񷴺�
//�������
********************************************************************************/
void Print(unsigned char y, unsigned char x, unsigned char ch[], unsigned short yn)
{
  unsigned char ch2[3];
  unsigned char ii;
  ii = 0;
  while(ch[ii] != '\0')
  {
    if(ch[ii] > 128)
    {
      ch2[0] = ch[ii];
      ch2[1] = ch[ii + 1];
      ch2[2] = '\0';			//����Ϊ�����ֽ�
      Print16(y , x , ch2 , yn);	//��ʾ����
      x += 16;
      ii += 2;
    }
    else
    {
      ch2[0] = ch[ii];	
      ch2[1] = '\0';			//��ĸռһ���ֽ�
      Print16_8(y , x , ch2 , yn);	//��ʾ��ĸ
      x += 8;
      ii += 1;
    }
  }
}

/*******************************************************************************
//��������void Rectangle(unsigned char x1,unsigned char y1,unsigned char x2,unsigned char y2)
//���ܣ���ֱ�ߺ�����������Ŀǰֻ�ܻ�ˮƽ�ʹ�ֱ��
//���룺x1,y1(��һ����)   x2,y2�ڶ�����
//�������
********************************************************************************/
void Rectangle(unsigned char x1,unsigned char y1,unsigned char x2,unsigned char y2)
{
  
  unsigned char ii;
  
  for(ii=x1; ii<x2; ii++)
  {		
    SetRamAddr(y1,ii);
    WriteLCD(DATA , 0x08);	//������
    SetRamAddr(y2,ii);
    WriteLCD(DATA , 0x08);
  }
  SetRamAddr(y1,x1);
  WriteLCD(DATA , 0xF0);
  SetRamAddr(y1,x2);
  WriteLCD(DATA , 0xf0);
  
  for(ii = y1+1;ii<y2;ii++)
  {		
    SetRamAddr(ii,x1);
    WriteLCD(DATA , 0xFF);
    SetRamAddr(ii,x2);//������
    WriteLCD(DATA , 0xFF);
  }
  
  SetRamAddr(y2,x1);
  WriteLCD(DATA , 0x0F);
  SetRamAddr(y2,x2);
  WriteLCD(DATA , 0x0F);
}



/*******************************************************************************
//��������void LoadICO(void)
//���ܣ���һ��ָ��ָ���ͼ��
//���룺
//�������
********************************************************************************/
void LoadICO(void)
{
  
  unsigned char i,j;
  for(i=0; i<8; i++)
  {
    SetRamAddr(i , 0);
    for(j = 0;j < 128;j++)
    {
      WriteLCD(DATA, ICO[j+i*128]);
    }
  }
}


/*******************************************************************************
//��������void Printn(unsigned char xx ,unsigned char yy , unsigned long no,unsigned char yn,unsigned char le)
//���ܣ���ʾһ��6*8�޷�������
//���룺xx , yy��Ļ����λ��,no����ʾ���� yn=0������ʾ yn=1������ʾ  le��Чλ
//�������
********************************************************************************/
void Printn(unsigned char xx ,unsigned char yy , unsigned long no,unsigned char yn,unsigned char le)
{
  unsigned char ch2[6];
  unsigned char ii;
  
  for(ii = 1 ; ii <= le ;)
  {
    ch2[le - ii] = no % 10 + 0x30;
    no /= 10;
    ii += 1;
  }
  ch2[le] = '\0';
  Print6(xx ,yy ,ch2 ,yn);
}

/*******************************************************************************
//��������void LcdPowerON(void)
//���ܣ���LCD��Դ
//���룺��
//�������
********************************************************************************/
void LcdPowerON(void)
{
  WriteLCD(COMMAND,DisplayOn);
}

/*******************************************************************************
//��������void LcdPowerOFF(void)
//���ܣ��ر�LCD��Դ
//���룺��
//�������
********************************************************************************/
void LcdPowerOFF(void)
{
  WriteLCD(COMMAND,DisplayOff);
}

/*******************************************************************************
//��������void PrintWirelessQuality(x)
//���ܣ���ʾ�����ź�ǿ��
//���룺xΪ�ź�ǿ������
//�������
********************************************************************************/
void PrintWirelessQuality(unsigned char x)		
{
  unsigned char i,k,j;
  if (x>=30)
    k=36*4;           // 100%
  else if (x>=10)
    k=36*3;
  else if (x>=3)
    k=36*2;
  else if (x>=1)
    k=36*1;
  else 
    k=0;
  
  for(i=0; i<2; i++)
  {
    SetRamAddr(i , 105);
    for(j = 0;j < 123 -105;j++)
    {
      WriteLCD(DATA, Sig[j+i*18+k]);
    }
  }
}


/*******************************************************************************
//��������void PrintBatteryCapacity(x)
//���ܣ���ʾ�������
//���룺xΪ�ź�ǿ������
//�������
********************************************************************************/
void PrintBatteryCapacity(float x)		
{
  unsigned char i,k,j;
  if (x>=f_vcc_full)
    k=28*4;           // 100%
  else if (x>=f_vcc_afull)
    k=28*3;
  else if (x>=f_vcc_midd)
    k=28*2;
  else if (x>=f_vcc_aempty)
    k=28*1;
  else 
    k=0;
  
  for(i=0; i<2; i++)
  {
    SetRamAddr(i , 88);
    for(j = 0;j <14;j++)
    {
      WriteLCD(DATA, Pwr[j+i*14+k]);
    }
  }
}

/*
* Initialize LCD Service
*/
void HalLcdInit(void){
#if (HAL_LCD == TRUE)
  InitDisplay();
  ClearScreen();
  HalLcdWriteString( "SampleApp", HAL_LCD_LINE_1 );
  LoadICO();
//  contrastctrl(255,0);
//  contrastctrl(0,255);
#endif
}

void HalLcdWriteString ( char *str, uint8 option)
{
#if (HAL_LCD == TRUE)
  unsigned char str_tmp[30];
  unsigned int i=0;
  while (*(str+i)!=NULL) 
  {
    str_tmp[i]=*(str+i);
    i++;
  }
  
  str_tmp[i]=NULL;
  
  if (option==HAL_LCD_LINE_1) 
  {
    ClearRow16(0,1);
    Print16_8(0, 0, str_tmp, 1);
  }
  else if (option==HAL_LCD_LINE_2)
  {
    ClearRow16(2,3);
    Print16_8(2, 0, str_tmp, 1);
  }
#endif
}

void HalLcdWriteValue ( uint32 value, const uint8 radix, uint8 option)
{
#if (HAL_LCD == TRUE)
  uint8 buf[HAL_LCD_MAX_BUFF];
  
  _ltoa( value, &buf[0], radix );
  HalLcdWriteString( (char*)buf, option );
#endif
}

/*
* Write a value to the LCD
*/
void HalLcdWriteScreen( char *line1, char *line2 )
{
#if (HAL_LCD == TRUE)
  HalLcdWriteString( line1, 1 );
  HalLcdWriteString( line2, 2 );
#endif
  
}

/*
* Write a string followed by a value to the LCD
*/
void HalLcdWriteStringValue( char *title, uint16 value, uint8 format, uint8 line )
{
#if (HAL_LCD == TRUE)
  uint8 tmpLen;
  uint8 buf[HAL_LCD_MAX_BUFF];
  uint32 err;
  
  tmpLen = (uint8)osal_strlen( (char*)title );
  osal_memcpy( buf, title, tmpLen );
  buf[tmpLen] = ' ';
  err = (uint32)(value);
  _ltoa( err, &buf[tmpLen+1], format );
  HalLcdWriteString( (char*)buf, line );	
#endif
  
}

/*
* Write a string followed by 2 values to the LCD
*/
void HalLcdWriteStringValueValue( char *title, uint16 value1, uint8 format1, uint16 value2, uint8 format2, uint8 line )
{
#if (HAL_LCD == TRUE)
  
  uint8 tmpLen;
  uint8 buf[HAL_LCD_MAX_BUFF];
  uint32 err;
  
  tmpLen = (uint8)osal_strlen( (char*)title );
  if ( tmpLen )
  {
    osal_memcpy( buf, title, tmpLen );
    buf[tmpLen++] = ' ';
  }
  
  err = (uint32)(value1);
  _ltoa( err, &buf[tmpLen], format1 );
  tmpLen = (uint8)osal_strlen( (char*)buf );
  
  buf[tmpLen++] = ',';
  buf[tmpLen++] = ' ';
  err = (uint32)(value2);
  _ltoa( err, &buf[tmpLen], format2 );
  
  HalLcdWriteString( (char *)buf, line );		
  
#endif
  
}

/*
* Write a percentage bar to the LCD
*/
void HalLcdDisplayPercentBar( char *title, uint8 value )
{
  #if (HAL_LCD == TRUE)

  uint8 percent;
  uint8 leftOver;
  uint8 buf[17];
  uint32 err;
  uint8 x;

  /* Write the title: */
  HalLcdWriteString( title, HAL_LCD_LINE_1 );

  if ( value > 100 )
    value = 100;

  /* convert to blocks */
  percent = (uint8)(value / 10);
  leftOver = (uint8)(value % 10);

  /* Make window */
  osal_memcpy( buf, "[          ]  ", 15 );

  for ( x = 0; x < percent; x ++ )
  {
    buf[1+x] = '>';
  }

  if ( leftOver >= 5 )
    buf[1+x] = '+';

  err = (uint32)value;
  _ltoa( err, (uint8*)&buf[13], 10 );

  HalLcdWriteString( (char*)buf, HAL_LCD_LINE_2 );

#endif

}