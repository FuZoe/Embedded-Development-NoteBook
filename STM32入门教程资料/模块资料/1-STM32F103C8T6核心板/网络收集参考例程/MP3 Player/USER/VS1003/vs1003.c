/****************************************Copyright (c)****************************************************
**                                      
**                                 http://www.powermcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               VS1003.c
** Descriptions:            The VS1003 application function
**
**--------------------------------------------------------------------------------------------------------
** Created by:              AVRman
** Created date:            2011-2-27
** Version:                 v1.0
** Descriptions:            The original version
**
**--------------------------------------------------------------------------------------------------------
** Modified by:             
** Modified date:           
** Version:                 
** Descriptions:            
**
*********************************************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "vs1003.h"

/* Private variables ---------------------------------------------------------*/

/* VS1003设置参数 */ 
/* 0 , henh.1 , hfreq.2 , lenh.3 , lfreq 5 ,主音量 */
uint8_t vs1003ram[5] = { 0 , 0 , 0 , 0 , 250 };
		
/*******************************************************************************
* Function Name  : VS1003_SPI_Init
* Description    : VS1003 SPI Init
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static void VS1003_SPI_Init(void)
{
  SPI_InitTypeDef  SPI_InitStructure;

  RCC_APB2PeriphClockCmd( RCC_APB2Periph_SPI1, ENABLE);
  /* SPI1 configuration */
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(SPI1, &SPI_InitStructure);

  /* Enable SPI1  */
  SPI_Cmd(SPI1, ENABLE);
}

/*******************************************************************************
* Function Name  : VS1003_SPI_SetSpeed
* Description    : None
* Input          : - SpeedSet: SPI_SPEED_HIGH 1  SPI_SPEED_LOW 0
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static void VS1003_SPI_SetSpeed( uint8_t SpeedSet)
{
  SPI_InitTypeDef SPI_InitStructure ;
    
  SPI_InitStructure.SPI_Direction=SPI_Direction_2Lines_FullDuplex ;
  SPI_InitStructure.SPI_Mode=SPI_Mode_Master ;
  SPI_InitStructure.SPI_DataSize=SPI_DataSize_8b ;
  SPI_InitStructure.SPI_CPOL=SPI_CPOL_High ;
  SPI_InitStructure.SPI_CPHA=SPI_CPHA_2Edge ;
  SPI_InitStructure.SPI_NSS=SPI_NSS_Soft ;

  if( SpeedSet == SPI_SPEED_LOW )
  {
      SPI_InitStructure.SPI_BaudRatePrescaler=SPI_BaudRatePrescaler_128;
  }
  else 
  {
      SPI_InitStructure.SPI_BaudRatePrescaler=SPI_BaudRatePrescaler_8;
  }
  SPI_InitStructure.SPI_FirstBit=SPI_FirstBit_MSB ;
  SPI_InitStructure.SPI_CRCPolynomial=7 ;
  SPI_Init(SPI1,&SPI_InitStructure);
}

/*******************************************************************************
* Function Name  : VS1003_SPI_ReadWriteByte
* Description    : None
* Input          : - TxData: Write Byte
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static uint8_t VS1003_SPI_ReadWriteByte( uint8_t TxData )
{
  /* Loop while DR register in not emplty */
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);

  /* Send byte through the SPI1 peripheral */
  SPI_I2S_SendData(SPI1, TxData);

  /* Wait to receive a byte */
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);

  /* Return the byte read from the SPI bus */
  return SPI_I2S_ReceiveData(SPI1);
}

/*******************************************************************************
* Function Name  : VS1003_Init
* Description    : VS1003端口初始化
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/	 
void VS1003_Init(void)
{		  
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC , ENABLE);
  /* Configure SPI1 pins: SCK, MISO and MOSI ---------------------------------*/ 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5  | GPIO_Pin_6 | GPIO_Pin_7; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;       
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure); 
  /* XCS */
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_5 | GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  /* DREQ */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(GPIOC, &GPIO_InitStructure); 

  MP3_DCS(1); 
  MP3_CCS(1); 

  VS1003_SPI_Init();
}

/*******************************************************************************
* Function Name  : delay_ms
* Description    : delay time
* Input          : - ms: time
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static void delay_ms(uint32_t ms)
{
    uint32_t len;
    for (;ms > 0; ms --)
        for (len = 0; len < 100; len++ );
}

/*******************************************************************************
* Function Name  : VS1003_WriteReg
* Description    : VS1003写寄存器
* Input          : - reg: 命令地址
*				   - value: 命令数据
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static void VS1003_WriteReg( uint8_t reg,uint16_t value )
{  
   while(  MP3_DREQ == 0 );           /* 等待空闲 */
   
   VS1003_SPI_SetSpeed( SPI_SPEED_LOW );	 
   MP3_DCS(1); 
   MP3_CCS(0); 
   VS1003_SPI_ReadWriteByte(VS_WRITE_COMMAND); /* 发送VS1003的写命令 */
   VS1003_SPI_ReadWriteByte(reg);             
   VS1003_SPI_ReadWriteByte(value>>8);        
   VS1003_SPI_ReadWriteByte(value);	          
   MP3_CCS(1);         
   VS1003_SPI_SetSpeed( SPI_SPEED_HIGH );
} 

/*******************************************************************************
* Function Name  : VS1003_ReadReg
* Description    : VS1003读寄存器
* Input          : - reg: 命令地址
* Output         : None
* Return         : - value: 数据
* Attention		 : None
*******************************************************************************/
uint16_t VS1003_ReadReg( uint8_t reg)
{ 
   uint16_t value;
    
   while(  MP3_DREQ == 0 );           /* 等待空闲 */
   VS1003_SPI_SetSpeed( SPI_SPEED_LOW );
   MP3_DCS(1);     
   MP3_CCS(0);     
   VS1003_SPI_ReadWriteByte(VS_READ_COMMAND);/* 发送VS1003的读命令 */
   VS1003_SPI_ReadWriteByte( reg );   
   value = VS1003_SPI_ReadWriteByte(0xff);		
   value = value << 8;
   value |= VS1003_SPI_ReadWriteByte(0xff); 
   MP3_CCS(1);   
   VS1003_SPI_SetSpeed( SPI_SPEED_HIGH );
   return value; 
} 

/*******************************************************************************
* Function Name  : VS1003_GetBitrate
* Description    : 得到mp3&wma的波特率
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
uint16_t VS1003_GetBitrate(void)
{
	uint16_t head0;
	uint16_t head1;            
    const uint16_t bitrate[2][16]={ {0,8,16,24,32,40,48,56,64,80,96,112,128,144,160,0}, 
                                    {0,32,40,48,56,64,80,96,112,128,160,192,224,256,320,0}}; 

    head0 = VS1003_ReadReg(SPI_HDAT0); 
    head1 = VS1003_ReadReg(SPI_HDAT1);

    switch( head1 )
    {        
        case 0x7665:	  /* WAV格式 */
		     return 0;    
        case 0X4D54:	  /* MIDI格式 */
		     return 1;    
        case 0X574D:      /* WMA格式 */
             head1 = head0 * 2 / 25;
             if( ( head1 % 10 ) > 5 )
			 {
			     return head1 / 10 + 1;
			 }
             else 
			 {
			     return head1 / 10;
			 }
        default:          /* MP3格式 */
            head1 >>= 3;
            head1 = head1 & 0x03; 
            if( head1 == 3 )
			{
			    head1 = 1;
			}
            else 
			{
			    head1 = 0;
			}
            return bitrate[head1][head0>>12];
    } 
}  

/*******************************************************************************
* Function Name  : VS1003_ResetDecodeTime
* Description    : None
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/                       
void VS1003_ResetDecodeTime(void)
{
   VS1003_WriteReg(SPI_DECODE_TIME,0x0000);
   VS1003_WriteReg(SPI_DECODE_TIME,0x0000); /* 操作两次 */
}

/*******************************************************************************
* Function Name  : VS1003_GetDecodeTime
* Description    : None
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/    
uint16_t VS1003_GetDecodeTime(void)
{ 
   return VS1003_ReadReg(SPI_DECODE_TIME);   
} 

/*******************************************************************************
* Function Name  : VS1003_SoftReset
* Description    : VS1003软复位
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void VS1003_SoftReset(void)
{
   uint8_t retry; 	
   			   
   while(  MP3_DREQ == 0 );               /* 等待软件复位结束 */
   VS1003_SPI_ReadWriteByte(0xff);        /* 启动传输 */
   retry = 0;
   while( VS1003_ReadReg(0) != 0x0804 )   /* 软件复位,新模式 */  
   {
	   VS1003_WriteReg(SPI_MODE,0x0804);  /* 软件复位,新模式 */
	   delay_ms(2);                       /* 等待至少1.35ms */
	   if( retry++ > 100 )
	   { 
	      break; 
	   }
   }	 	

   while(  MP3_DREQ ==0 );   /* 等待软件复位结束 */

   retry = 0;

   while( VS1003_ReadReg(SPI_CLOCKF) != 0X9800 ) /* 设置vs1003的时钟,3倍频 ,1.5xADD */ 
   {
	   VS1003_WriteReg(SPI_CLOCKF,0X9800);       /* 设置vs1003的时钟,3倍频 ,1.5xADD */
	   if( retry++ > 100 )
	   { 
	      break; 
	   }
   }		   
   retry = 0;

   while( VS1003_ReadReg(SPI_AUDATA) != 0XBB81 ) /* 设置vs1003的时钟,3倍频 ,1.5xADD */
   {
	   VS1003_WriteReg(SPI_AUDATA,0XBB81);
	   if( retry++ > 100 )
	   { 
	      break; 
	   }   
   }

   VS1003_WriteReg(11,0x2020);	 /* 音量 */			 
   VS1003_ResetDecodeTime();     /* 复位解码时间 */	     
   /* 向vs1003发送4个字节无效数据，用以启动SPI发送 */	
   MP3_DCS(0);  /* 选中数据传输 */
   VS1003_SPI_ReadWriteByte(0XFF);
   VS1003_SPI_ReadWriteByte(0XFF);
   VS1003_SPI_ReadWriteByte(0XFF);
   VS1003_SPI_ReadWriteByte(0XFF);
   MP3_DCS(1);  /* 取消数据传输 */
   delay_ms(20);
} 

/*******************************************************************************
* Function Name  : VS1003_Reset
* Description    : VS1003复位
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void VS1003_Reset(void)
{
   delay_ms(20);
   VS1003_SPI_ReadWriteByte(0XFF);
   MP3_DCS(1);              /* 取消数据传输 */
   MP3_CCS(1);              /* 取消数据传输 */

   while(  MP3_DREQ ==0 );  /* 等待DREQ为高 */
   delay_ms(20);			 
}

/*******************************************************************************
* Function Name  : VS1003_SineTest
* Description    : VS1003正弦测试
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void VS1003_SineTest(void)
{	
   VS1003_WriteReg(0x0b,0X2020);	  /* 设置音量 */	 
   VS1003_WriteReg(SPI_MODE,0x0820); /* 进入vs1003的测试模式 */	    
   while(  MP3_DREQ == 0 );  /* 等待DREQ为高 */
 	
   /* 向vs1003发送正弦测试命令：0x53 0xef 0x6e n 0x00 0x00 0x00 0x00 */
   MP3_DCS(0);/* 选中数据传输 */
   VS1003_SPI_ReadWriteByte(0x53);
   VS1003_SPI_ReadWriteByte(0xef);
   VS1003_SPI_ReadWriteByte(0x6e);
   VS1003_SPI_ReadWriteByte(0x24);
   VS1003_SPI_ReadWriteByte(0x00);
   VS1003_SPI_ReadWriteByte(0x00);
   VS1003_SPI_ReadWriteByte(0x00);
   VS1003_SPI_ReadWriteByte(0x00);
   delay_ms(100);
   MP3_DCS(1); 
   /* 退出正弦测试 */
   MP3_DCS(0);/* 选中数据传输 */
   VS1003_SPI_ReadWriteByte(0x45);
   VS1003_SPI_ReadWriteByte(0x78);
   VS1003_SPI_ReadWriteByte(0x69);
   VS1003_SPI_ReadWriteByte(0x74);
   VS1003_SPI_ReadWriteByte(0x00);
   VS1003_SPI_ReadWriteByte(0x00);
   VS1003_SPI_ReadWriteByte(0x00);
   VS1003_SPI_ReadWriteByte(0x00);
   delay_ms(100);
   MP3_DCS(1);		 

   /* 再次进入正弦测试并设置n值为0x44，即将正弦波的频率设置为另外的值 */
   MP3_DCS(0);/* 选中数据传输 */      
   VS1003_SPI_ReadWriteByte(0x53);
   VS1003_SPI_ReadWriteByte(0xef);
   VS1003_SPI_ReadWriteByte(0x6e);
   VS1003_SPI_ReadWriteByte(0x44);
   VS1003_SPI_ReadWriteByte(0x00);
   VS1003_SPI_ReadWriteByte(0x00);
   VS1003_SPI_ReadWriteByte(0x00);
   VS1003_SPI_ReadWriteByte(0x00);
   delay_ms(100);
   MP3_DCS(1);
   /* 退出正弦测试 */
   MP3_DCS(0);/* 选中数据传输 */      
   VS1003_SPI_ReadWriteByte(0x45);
   VS1003_SPI_ReadWriteByte(0x78);
   VS1003_SPI_ReadWriteByte(0x69);
   VS1003_SPI_ReadWriteByte(0x74);
   VS1003_SPI_ReadWriteByte(0x00);
   VS1003_SPI_ReadWriteByte(0x00);
   VS1003_SPI_ReadWriteByte(0x00);
   VS1003_SPI_ReadWriteByte(0x00);
   delay_ms(100);
   MP3_DCS(1);	 
}	 

/*******************************************************************************
* Function Name  : VS1003_RamTest
* Description    : VS1003 RAM测试
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/																			 
void VS1003_RamTest(void)
{
   volatile uint16_t value;
     
   VS1003_Reset();     
   VS1003_WriteReg(SPI_MODE,0x0820); /* 进入vs1003的测试模式 */
   while(  MP3_DREQ == 0 );          /* 等待DREQ为高 */
   MP3_DCS(0);	       			     /* xDCS = 1，选择vs1003的数据接口 */
   VS1003_SPI_ReadWriteByte(0x4d);
   VS1003_SPI_ReadWriteByte(0xea);
   VS1003_SPI_ReadWriteByte(0x6d);
   VS1003_SPI_ReadWriteByte(0x54);
   VS1003_SPI_ReadWriteByte(0x00);
   VS1003_SPI_ReadWriteByte(0x00);
   VS1003_SPI_ReadWriteByte(0x00);
   VS1003_SPI_ReadWriteByte(0x00);
   delay_ms(50);  
   MP3_DCS(1);
   value = VS1003_ReadReg(SPI_HDAT0); /* 如果得到的值为0x807F，则表明完好 */
}     
		 				
/*******************************************************************************
* Function Name  : VS1003_SetVol
* Description    : 设定vs1003播放的音量和高低音
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/   
void VS1003_SetVol(void)
{
   uint8_t i;
   uint16_t bass=0;  /* 暂存音调寄存器值 */
   uint16_t volt=0;  /* 暂存音量值 */
   uint8_t  vset=0;  /* 暂存音量值 */ 
		 
   vset = 255 - vs1003ram[4]; /* 取反得到最大值 */ 
   volt =vset;
   volt <<= 8;
   volt += vset;/* 得到音量设置后大小 */
   /* 0,henh.1,hfreq.2,lenh.3,lfreq */      
   for( i = 0; i < 4; i++ )
   {
       bass <<= 4;
       bass += vs1003ram[i]; 
   }     
   VS1003_WriteReg(SPI_BASS,bass);/* BASS */   
   VS1003_WriteReg(SPI_VOL,volt); /* 设音量 */ 
}    

/*******************************************************************************
* Function Name  : VS1003_WriteData
* Description    : VS1003写数据
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/  
void VS1003_WriteData( uint8_t * buf)
{		
   uint8_t  count = 32;

   MP3_DCS(0);   
   while( count-- )
   {
	  VS1003_SPI_ReadWriteByte( *buf++ );
   }
   MP3_DCS(1);  
   MP3_CCS(1);  
} 

/*******************************************************************************
* Function Name  : VS1003_Record_Init
* Description    : VS1003录音初始化
* Input          : None
* Output         : None
* Return         : None
* Attention		 : VS1003录音初始化成功之后可以听到耳机有回放
*                  此时不能用喇叭容易产生自激
*******************************************************************************/ 
void VS1003_Record_Init(void)
{
  uint8_t retry; 	

  /* 配置时钟 */
  while( VS1003_ReadReg(SPI_CLOCKF) != 0x9800 )   
  {
	  VS1003_WriteReg(SPI_CLOCKF,0x9800);   
	  delay_ms(2);                        /* 等待至少1.35ms */
	  if( retry++ > 100 )
	  { 
	      break; 
	  }
  }	

  while( VS1003_ReadReg(SPI_BASS) != 0x0000 )   
  {
	  VS1003_WriteReg(SPI_CLOCKF,0x0000);   
	  delay_ms(2);                        /* 等待至少1.35ms */
	  if( retry++ > 100 )
	  { 
	      break; 
	  }
  }	

  /* Set sample rate divider=12 */ 
  while( VS1003_ReadReg(SPI_AICTRL0) != 0x0012 )   
  {
	  VS1003_WriteReg(SPI_AICTRL0,0x0012);   
	  delay_ms(2);                        /* 等待至少1.35ms */
	  if( retry++ > 100 )
	  { 
	      break; 
	  }
  }	

  /* AutoGain OFF, reclevel 0x1000 */ 
  while( VS1003_ReadReg(SPI_AICTRL1) != 0x1000 )   
  {
	  VS1003_WriteReg(SPI_AICTRL1,0x1000);   
	  delay_ms(2);                        /* 等待至少1.35ms */
	  if( retry++ > 100 )
	  { 
	      break; 
	  }
  }	

  /* RECORD,NEWMODE,RESET */ 
  while( VS1003_ReadReg(SPI_MODE) != 0x1804 )   
  {
	  VS1003_WriteReg(SPI_MODE,0x1804);   
	  delay_ms(2);                        /* 等待至少1.35ms */
	  if( retry++ > 100 )
	  { 
	      break; 
	  }
  }	

  while( VS1003_ReadReg(SPI_CLOCKF) != 0x9800 )   
  {
	  VS1003_WriteReg(SPI_CLOCKF,0x9800);   
	  delay_ms(2);                        /* 等待至少1.35ms */
	  if( retry++ > 100 )
	  { 
	      break; 
	  }
  }	
}

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/

