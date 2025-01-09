/****************************************Copyright (c)**************************************************                         
**
**                                 http://www.powermcu.com
**
**--------------File Info-------------------------------------------------------------------------------
** File name:			GLCD.c
** Descriptions:		Has been tested SSD1289、ILI9320、R61505U、SSD1298、ST7781、SPFD5408B、ILI9325、ILI9328、
**						HX8346A、HX8347A
**------------------------------------------------------------------------------------------------------
** Created by:			AVRman
** Created date:		2012-3-10
** Version:					1.3
** Descriptions:		The original version
**
**------------------------------------------------------------------------------------------------------
** Modified by:			Paolo Bernardi
** Modified date:		03/01/2020
** Version:					2.0
** Descriptions:		simple arrangement for screen usage
********************************************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "GLCD.h" 
#include "AsciiLib.h"


/* Private variables ---------------------------------------------------------*/
static uint8_t LCD_Code;

/* Private define ------------------------------------------------------------*/
#define  ILI9320    0  /* 0x9320 */
#define  ILI9325    1  /* 0x9325 */
#define  ILI9328    2  /* 0x9328 */
#define  ILI9331    3  /* 0x9331 */
#define  SSD1298    4  /* 0x8999 */
#define  SSD1289    5  /* 0x8989 */
#define  ST7781     6  /* 0x7783 */
#define  LGDP4531   7  /* 0x4531 */
#define  SPFD5408B  8  /* 0x5408 */
#define  R61505U    9  /* 0x1505 0x0505 */
#define  HX8346A		10 /* 0x0046 */  
#define  HX8347D    11 /* 0x0047 */
#define  HX8347A    12 /* 0x0047 */	
#define  LGDP4535   13 /* 0x4535 */  
#define  SSD2119    14 /* 3.5 LCD 0x9919 */




/*******************************************************************************/

/*******************************************************************************
* Function Name  : Lcd_Configuration
* Description    : Configures LCD Control lines
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static void LCD_Configuration(void)
{
	/* Configure the LCD Control pins */
	
	/* EN = P0.19 , LE = P0.20 , DIR = P0.21 , CS = P0.22 , RS = P0.23 , RS = P0.23 */
	/* RS = P0.23 , WR = P0.24 , RD = P0.25 , DB[0.7] = P2.0...P2.7 , DB[8.15]= P2.0...P2.7 */  
	LPC_GPIO0->FIODIR   |= 0x03f80000;
	LPC_GPIO0->FIOSET    = 0x03f80000;
}

/*******************************************************************************
* Function Name  : LCD_Send
* Description    : LCD写数据
* Input          : - byte: byte to be sent
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static __attribute__((always_inline)) void LCD_Send (uint16_t byte) 
{
	LPC_GPIO2->FIODIR |= 0xFF;          /* P2.0...P2.7 Output */
	LCD_DIR(1)		   				    				/* Interface A->B */
	LCD_EN(0)	                        	/* Enable 2A->2B */
	LPC_GPIO2->FIOPIN =  byte;          /* Write D0..D7 */
	LCD_LE(1)                         
	LCD_LE(0)														/* latch D0..D7	*/
	LPC_GPIO2->FIOPIN =  byte >> 8;     /* Write D8..D15 */
}

/*******************************************************************************
* Function Name  : wait_delay
* Description    : Delay Time
* Input          : - nCount: Delay Time
* Output         : None
* Return         : None
* Return         : None
* Attention		 : None 
*******************************************************************************/
static void wait_delay(int count)
{
	while(count--);
}

/*******************************************************************************
* Function Name  : LCD_Read
* Description    : LCD读数据
* Input          : - byte: byte to be read
* Output         : None
* Return         : 返回读取到的数据
* Attention		 : None
*******************************************************************************/
static __attribute__((always_inline)) uint16_t LCD_Read (void) 
{
	uint16_t value;
	
	LPC_GPIO2->FIODIR &= ~(0xFF);              /* P2.0...P2.7 Input */
	LCD_DIR(0);		   				           				 /* Interface B->A */
	LCD_EN(0);	                               /* Enable 2B->2A */
	wait_delay(30);							   						 /* delay some times */
	value = LPC_GPIO2->FIOPIN0;                /* Read D8..D15 */
	LCD_EN(1);	                               /* Enable 1B->1A */
	wait_delay(30);							   						 /* delay some times */
	value = (value << 8) | LPC_GPIO2->FIOPIN0; /* Read D0..D7 */
	LCD_DIR(1);
	return  value;
}

/*******************************************************************************
* Function Name  : LCD_WriteIndex
* Description    : LCD写寄存器地址
* Input          : - index: 寄存器地址
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static __attribute__((always_inline)) void LCD_WriteIndex(uint16_t index)
{
	LCD_CS(0);
	LCD_RS(0);
	LCD_RD(1);
	LCD_Send( index ); 
	wait_delay(22);	
	LCD_WR(0);  
	wait_delay(1);
	LCD_WR(1);
	LCD_CS(1);
}

/*******************************************************************************
* Function Name  : LCD_WriteData
* Description    : LCD写寄存器数据
* Input          : - index: 寄存器数据
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static __attribute__((always_inline)) void LCD_WriteData(uint16_t data)
{				
	LCD_CS(0);
	LCD_RS(1);   
	LCD_Send( data );
	LCD_WR(0);     
	wait_delay(1);
	LCD_WR(1);
	LCD_CS(1);
}

/*******************************************************************************
* Function Name  : LCD_ReadData
* Description    : 读取控制器数据
* Input          : None
* Output         : None
* Return         : 返回读取到的数据
* Attention		 : None
*******************************************************************************/
static __attribute__((always_inline)) uint16_t LCD_ReadData(void)
{ 
	uint16_t value;
	
	LCD_CS(0);
	LCD_RS(1);
	LCD_WR(1);
	LCD_RD(0);
	value = LCD_Read();
	
	LCD_RD(1);
	LCD_CS(1);
	
	return value;
}

/*******************************************************************************
* Function Name  : LCD_WriteReg
* Description    : Writes to the selected LCD register.
* Input          : - LCD_Reg: address of the selected register.
*                  - LCD_RegValue: value to write to the selected register.
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static __attribute__((always_inline)) void LCD_WriteReg(uint16_t LCD_Reg,uint16_t LCD_RegValue)
{ 
	/* Write 16-bit Index, then Write Reg */  
	LCD_WriteIndex(LCD_Reg);         
	/* Write 16-bit Reg */
	LCD_WriteData(LCD_RegValue);  
}

/*******************************************************************************
* Function Name  : LCD_WriteReg
* Description    : Reads the selected LCD Register.
* Input          : None
* Output         : None
* Return         : LCD Register Value.
* Attention		 : None
*******************************************************************************/
static __attribute__((always_inline)) uint16_t LCD_ReadReg(uint16_t LCD_Reg)
{
	uint16_t LCD_RAM;
	
	/* Write 16-bit Index (then Read Reg) */
	LCD_WriteIndex(LCD_Reg);
	/* Read 16-bit Reg */
	LCD_RAM = LCD_ReadData();      	
	return LCD_RAM;
}

/*******************************************************************************
* Function Name  : LCD_SetCursor
* Description    : Sets the cursor position.
* Input          : - Xpos: specifies the X position.
*                  - Ypos: specifies the Y position. 
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static void LCD_SetCursor(uint16_t Xpos,uint16_t Ypos)
{
    #if  ( DISP_ORIENTATION == 90 ) || ( DISP_ORIENTATION == 270 )
	
 	uint16_t temp = Xpos;

			 Xpos = Ypos;
			 Ypos = ( MAX_X - 1 ) - temp;  

	#elif  ( DISP_ORIENTATION == 0 ) || ( DISP_ORIENTATION == 180 )
		
	#endif

  switch( LCD_Code )
  {
     default:		 /* 0x9320 0x9325 0x9328 0x9331 0x5408 0x1505 0x0505 0x7783 0x4531 0x4535 */
          LCD_WriteReg(0x0020, Xpos );     
          LCD_WriteReg(0x0021, Ypos );     
	      break; 

     case SSD1298: 	 /* 0x8999 */
     case SSD1289:   /* 0x8989 */
	      LCD_WriteReg(0x004e, Xpos );      
          LCD_WriteReg(0x004f, Ypos );          
	      break;  

     case HX8346A: 	 /* 0x0046 */
     case HX8347A: 	 /* 0x0047 */
     case HX8347D: 	 /* 0x0047 */
	      LCD_WriteReg(0x02, Xpos>>8 );                                                  
	      LCD_WriteReg(0x03, Xpos );  

	      LCD_WriteReg(0x06, Ypos>>8 );                           
	      LCD_WriteReg(0x07, Ypos );    
	
	      break;     
     case SSD2119:	 /* 3.5 LCD 0x9919 */
	      break; 
  }
}

/*******************************************************************************
* Function Name  : LCD_Delay
* Description    : Delay Time
* Input          : - nCount: Delay Time
* Output         : None
* Return         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static void delay_ms(uint16_t ms)    
{ 
	uint16_t i,j; 
	for( i = 0; i < ms; i++ )
	{ 
		for( j = 0; j < 1141; j++ );
	}
} 


/*******************************************************************************
* Function Name  : LCD_Initializtion
* Description    : Initialize TFT Controller.
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void LCD_Initialization(void)
{
	uint16_t DeviceCode;
	
	LCD_Configuration();
	delay_ms(100);
	DeviceCode = LCD_ReadReg(0x0000);		/* 读取屏ID	*/	
	
	if( DeviceCode == 0x9325 || DeviceCode == 0x9328 )	
	{
		LCD_Code = ILI9325;
		LCD_WriteReg(0x00e7,0x0010);      
		LCD_WriteReg(0x0000,0x0001);  	/* start internal osc */
		LCD_WriteReg(0x0001,0x0100);     
		LCD_WriteReg(0x0002,0x0700); 	/* power on sequence */
		LCD_WriteReg(0x0003,(1<<12)|(1<<5)|(1<<4)|(0<<3) ); 	/* importance */
		LCD_WriteReg(0x0004,0x0000);                                   
		LCD_WriteReg(0x0008,0x0207);	           
		LCD_WriteReg(0x0009,0x0000);         
		LCD_WriteReg(0x000a,0x0000); 	/* display setting */        
		LCD_WriteReg(0x000c,0x0001);	/* display setting */        
		LCD_WriteReg(0x000d,0x0000); 			        
		LCD_WriteReg(0x000f,0x0000);
		/* Power On sequence */
		LCD_WriteReg(0x0010,0x0000);   
		LCD_WriteReg(0x0011,0x0007);
		LCD_WriteReg(0x0012,0x0000);                                                                 
		LCD_WriteReg(0x0013,0x0000);                 
		delay_ms(50);  /* delay 50 ms */		
		LCD_WriteReg(0x0010,0x1590);   
		LCD_WriteReg(0x0011,0x0227);
		delay_ms(50);  /* delay 50 ms */		
		LCD_WriteReg(0x0012,0x009c);                  
		delay_ms(50);  /* delay 50 ms */		
		LCD_WriteReg(0x0013,0x1900);   
		LCD_WriteReg(0x0029,0x0023);
		LCD_WriteReg(0x002b,0x000e);
		delay_ms(50);  /* delay 50 ms */		
		LCD_WriteReg(0x0020,0x0000);                                                            
		LCD_WriteReg(0x0021,0x0000);           
		delay_ms(50);  /* delay 50 ms */		
		LCD_WriteReg(0x0030,0x0007); 
		LCD_WriteReg(0x0031,0x0707);   
		LCD_WriteReg(0x0032,0x0006);
		LCD_WriteReg(0x0035,0x0704);
		LCD_WriteReg(0x0036,0x1f04); 
		LCD_WriteReg(0x0037,0x0004);
		LCD_WriteReg(0x0038,0x0000);        
		LCD_WriteReg(0x0039,0x0706);     
		LCD_WriteReg(0x003c,0x0701);
		LCD_WriteReg(0x003d,0x000f);
		delay_ms(50);  /* delay 50 ms */		
		LCD_WriteReg(0x0050,0x0000);        
		LCD_WriteReg(0x0051,0x00ef);   
		LCD_WriteReg(0x0052,0x0000);     
		LCD_WriteReg(0x0053,0x013f);
		LCD_WriteReg(0x0060,0xa700);        
		LCD_WriteReg(0x0061,0x0001); 
		LCD_WriteReg(0x006a,0x0000);
		LCD_WriteReg(0x0080,0x0000);
		LCD_WriteReg(0x0081,0x0000);
		LCD_WriteReg(0x0082,0x0000);
		LCD_WriteReg(0x0083,0x0000);
		LCD_WriteReg(0x0084,0x0000);
		LCD_WriteReg(0x0085,0x0000);
		  
		LCD_WriteReg(0x0090,0x0010);     
		LCD_WriteReg(0x0092,0x0000);  
		LCD_WriteReg(0x0093,0x0003);
		LCD_WriteReg(0x0095,0x0110);
		LCD_WriteReg(0x0097,0x0000);        
		LCD_WriteReg(0x0098,0x0000);  
		/* display on sequence */    
		LCD_WriteReg(0x0007,0x0133);
		
		LCD_WriteReg(0x0020,0x0000);  /* 行首址0 */                                                          
		LCD_WriteReg(0x0021,0x0000);  /* 列首址0 */     
	}

    delay_ms(50);   /* delay 50 ms */	
}

/*******************************************************************************
* Function Name  : LCD_Clear
* Description    : 将屏幕填充成指定的颜色，如清屏，则填充 0xffff
* Input          : - Color: Screen Color
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void LCD_Clear(uint16_t Color)
{
	uint32_t index;
	
	if( LCD_Code == HX8347D || LCD_Code == HX8347A )
	{
		LCD_WriteReg(0x02,0x00);                                                  
		LCD_WriteReg(0x03,0x00);  
		                
		LCD_WriteReg(0x04,0x00);                           
		LCD_WriteReg(0x05,0xEF);  
		                 
		LCD_WriteReg(0x06,0x00);                           
		LCD_WriteReg(0x07,0x00);    
		               
		LCD_WriteReg(0x08,0x01);                           
		LCD_WriteReg(0x09,0x3F);     
	}
	else
	{	
		LCD_SetCursor(0,0); 
	}	

	LCD_WriteIndex(0x0022);
	for( index = 0; index < MAX_X * MAX_Y; index++ )
	{
		LCD_WriteData(Color);
	}
}

/******************************************************************************
* Function Name  : LCD_BGR2RGB
* Description    : RRRRRGGGGGGBBBBB 改为 BBBBBGGGGGGRRRRR 格式
* Input          : - color: BRG 颜色值  
* Output         : None
* Return         : RGB 颜色值
* Attention		 : 内部函数调用
*******************************************************************************/
static uint16_t LCD_BGR2RGB(uint16_t color)
{
	uint16_t  r, g, b, rgb;
	
	b = ( color>>0 )  & 0x1f;
	g = ( color>>5 )  & 0x3f;
	r = ( color>>11 ) & 0x1f;
	
	rgb =  (b<<11) + (g<<5) + (r<<0);
	
	return( rgb );
}

/******************************************************************************
* Function Name  : LCD_GetPoint
* Description    : 获取指定座标的颜色值
* Input          : - Xpos: Row Coordinate
*                  - Xpos: Line Coordinate 
* Output         : None
* Return         : Screen Color
* Attention		 : None
*******************************************************************************/
uint16_t LCD_GetPoint(uint16_t Xpos,uint16_t Ypos)
{
	uint16_t dummy;
	
	LCD_SetCursor(Xpos,Ypos);
	LCD_WriteIndex(0x0022);  
	
	switch( LCD_Code )
	{
		case ST7781:
		case LGDP4531:
		case LGDP4535:
		case SSD1289:
		case SSD1298:
             dummy = LCD_ReadData();   /* Empty read */
             dummy = LCD_ReadData(); 	
 		     return  dummy;	      
	    case HX8347A:
	    case HX8347D:
             {
		        uint8_t red,green,blue;
				
				dummy = LCD_ReadData();   /* Empty read */

		        red = LCD_ReadData() >> 3; 
                green = LCD_ReadData() >> 2; 
                blue = LCD_ReadData() >> 3; 
                dummy = (uint16_t) ( ( red<<11 ) | ( green << 5 ) | blue ); 
		     }	
	         return  dummy;

        default:	/* 0x9320 0x9325 0x9328 0x9331 0x5408 0x1505 0x0505 0x9919 */
             dummy = LCD_ReadData();   /* Empty read */
             dummy = LCD_ReadData(); 	
 		     return  LCD_BGR2RGB( dummy );
	}
}

/******************************************************************************
* Function Name  : LCD_SetPoint
* Description    : 在指定座标画点
* Input          : - Xpos: Row Coordinate
*                  - Ypos: Line Coordinate 
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void LCD_SetPoint(uint16_t Xpos,uint16_t Ypos,uint16_t point)
{
	if( Xpos >= MAX_X || Ypos >= MAX_Y )
	{
		return;
	}
	LCD_SetCursor(Xpos,Ypos);
	LCD_WriteReg(0x0022,point);
}

/******************************************************************************
* Function Name  : LCD_DrawLine
* Description    : Bresenham's line algorithm
* Input          : - x1: A点行座标
*                  - y1: A点列座标 
*				   - x2: B点行座标
*				   - y2: B点列座标 
*				   - color: 线颜色
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/	 
void LCD_DrawLine( uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1 , uint16_t color )
{
    short dx,dy;      /* 定义X Y轴上增加的变量值 */
    short temp;       /* 起点 终点大小比较 交换数据时的中间变量 */

    if( x0 > x1 )     /* X轴上起点大于终点 交换数据 */
    {
	    temp = x1;
		x1 = x0;
		x0 = temp;   
    }
    if( y0 > y1 )     /* Y轴上起点大于终点 交换数据 */
    {
		temp = y1;
		y1 = y0;
		y0 = temp;   
    }
  
	dx = x1-x0;       /* X轴方向上的增量 */
	dy = y1-y0;       /* Y轴方向上的增量 */

    if( dx == 0 )     /* X轴上没有增量 画垂直线 */ 
    {
        do
        { 
            LCD_SetPoint(x0, y0, color);   /* 逐点显示 描垂直线 */
            y0++;
        }
        while( y1 >= y0 ); 
		return; 
    }
    if( dy == 0 )     /* Y轴上没有增量 画水平直线 */ 
    {
        do
        {
            LCD_SetPoint(x0, y0, color);   /* 逐点显示 描水平线 */
            x0++;
        }
        while( x1 >= x0 ); 
		return;
    }
	/* 布兰森汉姆(Bresenham)算法画线 */
    if( dx > dy )                         /* 靠近X轴 */
    {
	    temp = 2 * dy - dx;               /* 计算下个点的位置 */         
        while( x0 != x1 )
        {
	        LCD_SetPoint(x0,y0,color);    /* 画起点 */ 
	        x0++;                         /* X轴上加1 */
	        if( temp > 0 )                /* 判断下下个点的位置 */
	        {
	            y0++;                     /* 为右上相邻点，即（x0+1,y0+1） */ 
	            temp += 2 * dy - 2 * dx; 
	 	    }
            else         
            {
			    temp += 2 * dy;           /* 判断下下个点的位置 */  
			}       
        }
        LCD_SetPoint(x0,y0,color);
    }  
    else
    {
	    temp = 2 * dx - dy;                      /* 靠近Y轴 */       
        while( y0 != y1 )
        {
	 	    LCD_SetPoint(x0,y0,color);     
            y0++;                 
            if( temp > 0 )           
            {
                x0++;               
                temp+=2*dy-2*dx; 
            }
            else
			{
                temp += 2 * dy;
			}
        } 
        LCD_SetPoint(x0,y0,color);
	}
} 

/******************************************************************************
* Function Name  : PutChar
* Description    : 将Lcd屏上任意位置显示一个字符
* Input          : - Xpos: 水平坐标 
*                  - Ypos: 垂直坐标  
*				   - ASCI: 显示的字符
*				   - charColor: 字符颜色   
*				   - bkColor: 背景颜色 
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void PutChar( uint16_t Xpos, uint16_t Ypos, uint8_t ASCI, uint16_t charColor, uint16_t bkColor )
{
	uint16_t i, j;
    uint8_t buffer[16], tmp_char;
    GetASCIICode(buffer,ASCI);  /* 取字模数据 */
    for( i=0; i<16; i++ )
    {
        tmp_char = buffer[i];
        for( j=0; j<8; j++ )
        {
            if( ((tmp_char >> (7 - j)) & 0x01) == 0x01 )
            {
                LCD_SetPoint( Xpos + j, Ypos + i, charColor );  /* 字符颜色 */
            }
            else
            {
                LCD_SetPoint( Xpos + j, Ypos + i, bkColor );  /* 背景颜色 */
            }
        }
    }
}

/******************************************************************************
* Function Name  : GUI_Text
* Description    : 在指定座标显示字符串
* Input          : - Xpos: 行座标
*                  - Ypos: 列座标 
*				   - str: 字符串
*				   - charColor: 字符颜色   
*				   - bkColor: 背景颜色 
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void GUI_Text(uint16_t Xpos, uint16_t Ypos, uint8_t *str,uint16_t Color, uint16_t bkColor)
{
    uint8_t TempChar;
    do
    {
        TempChar = *str++;  
        PutChar( Xpos, Ypos, TempChar, Color, bkColor );    
        if( Xpos < MAX_X - 8 )
        {
            Xpos += 8;
        } 
        else if ( Ypos < MAX_Y - 16 )
        {
            Xpos = 0;
            Ypos += 16;
        }   
        else
        {
            Xpos = 0;
            Ypos = 0;
        }    
    }
    while ( *str != 0 );
}

/***************************UTILITY FUNCTIONS***************************************************/

void drawMatrixPacman(uint16_t x, uint16_t y) {
    uint16_t i, j;	
		for(i = 0; i < PACMAN_DIM; i++)
		{
			for(j = 0; j < PACMAN_DIM; j++)
			{
					if(matrix_pacman[i][j]==1)
					{
						LCD_SetPoint(x+i,y+j, Yellow);
					}
					else if(matrix_pacman[i][j]==2){
						LCD_SetPoint(x+i,y+j, Black);
					}
					else if(matrix_pacman[i][j]==3 && mouthOpen){
						LCD_SetPoint(x+i,y+j, Black);
					}
					else if(matrix_pacman[i][j]==3 && !mouthOpen){
						LCD_SetPoint(x+i,y+j, Yellow);
					}
				
				

			} 
		};
		mouthOpen = !mouthOpen;
}
void rotateMatrix(uint8_t matrix[PACMAN_DIM][PACMAN_DIM]) {
    uint8_t temp[PACMAN_DIM][PACMAN_DIM];
    uint16_t i, j;
    for (i = 0; i < PACMAN_DIM; i++) {
        for (j = 0; j < PACMAN_DIM; j++) {
            temp[j][i] = matrix[i][j];
        }
    }
    
    for (i = 0; i < PACMAN_DIM; i++) {
        for (j = 0; j < PACMAN_DIM; j++) {
            matrix[i][j] = temp[i][PACMAN_DIM - 1 - j];
        }
   }
}




uint16_t calculateRotations(enum CardinalPoint start, enum CardinalPoint end) {
    uint16_t rotations = (end - start + 4) % 4;
    return rotations;
}

void rotateToCardinalPoint(enum CardinalPoint start, enum CardinalPoint final, uint8_t matrix[PACMAN_DIM][PACMAN_DIM]){
	uint16_t rotations = calculateRotations(start, final);
	uint16_t i;
	
	for(i=0; i < rotations; i++){
		rotateMatrix(matrix);
	}
	
}		

/******************************************************************************/

/************************* FUNCTIONS CALLED BY MAIN *************************/

/***************************************************************************/


void rotatePacman(enum CardinalPoint cardinalPoint){
	
	rotateToCardinalPoint(FACING, cardinalPoint, matrix_pacman);
	FACING = cardinalPoint;
}

void fillCell(uint8_t matrix[CELL_DIM][CELL_DIM], uint16_t x, uint16_t y, uint16_t color, bool fillEmptySpace) {
    uint8_t i, j;
    for (i = 0; i < CELL_DIM; i++) {
        for (j = 0; j < CELL_DIM; j++) {
            if (matrix[i][j] == 1) {
                LCD_SetPoint(x + i, y + j, color);
            } else if (fillEmptySpace) {
                LCD_SetPoint(x + i, y + j, Black);
            }
        }
    }
}
void setup() {
	uint16_t x, y;
	for(x = 0; x < ROWS; x++){
		for(y = 0; y < COLS; y++){
			switch(matrix_map[x][y]){
				case 6:
					counterPointer++;
					fillCell(pointLeftTopCorner, x*5, y*5, Yellow, false);
					break;
				case 7:
					fillCell(pointRightTopCorner, x*5, y*5, Yellow, false);
					break;
				case 8:
					fillCell(pointLeftDownCorner, x*5, y*5, Yellow, false);
					break;
				case 9:
					fillCell(pointRightDownCorner, x*5, y*5, Yellow, false);
					break;
				
				

				case 1:
					fillCell(wallFull, x*5, y*5, Blue, false);
					break;
				case 2:
					fillCell(wallLeftTopCorner, x*5, y*5, Blue, false);
					break;
				case 3:
					fillCell(wallRightTopCorner, x*5, y*5, Blue, false);
					break;
				case 4:
					fillCell(wallLeftDownCorner, x*5, y*5, Blue, false);
					break;
				case 5:
					fillCell(wallRightDownCorner, x*5, y*5, Blue, true);
					break;
				case 11:
					fillCell(wallFull, x*5, y*5, Red, false);
					break;
				default:
					break;
			}
		}
	}
	
	drawLifes();
}


void mirrorMatrixVertically(uint8_t A[CELL_DIM][CELL_DIM], uint8_t B[CELL_DIM][CELL_DIM]) {
		uint8_t i,j;
    for (i = 0; i < CELL_DIM; i++) {
        for (j = 0; j < CELL_DIM; j++) {
            B[i][j] = A[i][CELL_DIM - j - 1]; 
        }
    }
}

void mirrorMatrixHorizontally(uint8_t A[CELL_DIM][CELL_DIM], uint8_t B[CELL_DIM][CELL_DIM]) {
    uint8_t i, j;
    for (i = 0; i < CELL_DIM; i++) {
        for (j = 0; j < CELL_DIM; j++) {
            B[i][j] = A[CELL_DIM - i - 1][j];
        }
    }
}
void setupGlobalVariables(){
	mirrorMatrixVertically(wallLeftTopCorner, wallRightTopCorner);
	mirrorMatrixHorizontally(wallLeftTopCorner, wallLeftDownCorner);
	mirrorMatrixVertically(wallLeftDownCorner, wallRightDownCorner);
	
	mirrorMatrixVertically(pointLeftTopCorner, pointRightTopCorner);
	mirrorMatrixHorizontally(pointLeftTopCorner, pointLeftDownCorner);
	mirrorMatrixVertically(pointLeftDownCorner, pointRightDownCorner);
	
	mirrorMatrixVertically(superPointLeftTopCorner, superPointRightTopCorner);
	mirrorMatrixHorizontally(superPointLeftTopCorner, superPointLeftDownCorner);
	mirrorMatrixVertically(superPointLeftDownCorner, superPointRightDownCorner);
	
	

}
void fillTimesVector() {
    uint16_t i, j;
    int temp;
    int isUnique;

    srand(seed);

    for (i = 0; i < SUPER_PILL_NUMBER; i++) {
        do {
            isUnique = 1; 
            temp = rand() % (INIT_TIME);

            for (j = 0; j < i; j++) {
                if (timesVector[j] == temp) {
                    isUnique = 0;
                    break;
                }
            }
        } while (!isUnique);

        timesVector[i] = temp; 
    }

    for (i = 0; i < SUPER_PILL_NUMBER - 1; i++) {
        for (j = 0; j < SUPER_PILL_NUMBER - i - 1; j++) {
            if (timesVector[j] > timesVector[j + 1]) {
                temp = timesVector[j];
                timesVector[j] = timesVector[j + 1];
                timesVector[j + 1] = temp;
            }
        }
    }
}
void setupInitalPosition(){
	pacman_position_x[0] = 5;
	pacman_position_y[0] = 30;
	
	pacman_position_x[1] = pacman_position_x[0] + 4;

	pacman_position_y[1] = pacman_position_y[0] + 4;

	
	rotatePacman(EST);
	drawMatrixPacman(pacman_position_x[0]*5, pacman_position_y[0]*5);
}

void cleanPacmanOldPosition(int x, int y){
    uint16_t i, j;	
		for(i = 0; i < PACMAN_DIM; i++){
			for(j = 0; j < PACMAN_DIM; j++){
			if(matrix_pacman[i][j]==1 || matrix_pacman[i][j]==2)
				{
					LCD_SetPoint(x+i,y+j, Black);
				}
			}
		} 
	
}
void changePosition(int x_sum, int y_sum){
	int tmp_pacman_position_x[2]; 
	int tmp_pacman_position_y[2];
	
	tmp_pacman_position_x[0] = pacman_position_x[0] + x_sum;
	tmp_pacman_position_y[0] = pacman_position_y[0] + y_sum;
	
	tmp_pacman_position_x[1] = tmp_pacman_position_x[0] + 4;

	
	tmp_pacman_position_y[1] = tmp_pacman_position_y[0] + 4;

	
	uint8_t x, y; 
	bool hasWall = false; 
	
	for(x = tmp_pacman_position_x[0]; x < tmp_pacman_position_x[1]; x++){
		for(y = tmp_pacman_position_y[0]; y < tmp_pacman_position_y[1]; y++){
			if(matrix_map[x][y] >= 1 && matrix_map[x][y] <= 5 || matrix_map[x][y] == 11){
				hasWall = true; 
				break;
			}
		}
	}
	
	if(!hasWall){
			cleanPacmanOldPosition(pacman_position_x[0]*5, pacman_position_y[0]*5);
			

			for (y = 0; y < 2; y++) {
					pacman_position_x[y] = tmp_pacman_position_x[y];
					pacman_position_y[y] = tmp_pacman_position_y[y];
			}
			if(pacman_position_x[0] == 0 && 
					pacman_position_x[1] == 4 && 
					pacman_position_y[0] == 30 &&
					pacman_position_y[1] == 34){
						pacman_position_x[0] = ROWS - 4;
					pacman_position_x[1] = ROWS;
						
					}
			else if(pacman_position_x[0] == 44 && 
					pacman_position_x[1] == 48 && 
					pacman_position_y[0] == 30 &&
					pacman_position_y[1] == 34){
						pacman_position_x[0] = 0;
					pacman_position_x[1] = 4;
						
					}
		
	}
	

	}

uint8_t updateScore(){
	uint8_t x, y;
	uint8_t point = 0;
	for(x = pacman_position_x[0]; x < pacman_position_x[1]-1; x++){
		for(y = pacman_position_y[0]; y < pacman_position_y[1]-1; y++){
			if(matrix_map[x][y] >=  6 && matrix_map[x][y] <= 9){
				if(matrix_map[x][y] ==  6)
					point = 1;
				matrix_map[x][y] = 0;
			}
			else if(matrix_map[x][y] >=  12 && matrix_map[x][y] <= 15){
				if(matrix_map[x][y] ==  12)
					point = 2;
				matrix_map[x][y] = 0;
			}
		}
	}
	
		return point;
	}
void drawLife(int x, int y){
		uint16_t i, j;	
		for(i = 0; i < LIFE_DIM; i++)
		{
			for(j = 0; j < LIFE_DIM; j++)
			{
				if(life_matrix[i][j]==1)
				{
					LCD_SetPoint(x+i,y+j, Red);
				}

			} 
		}
	
	}
	
void cancelLife(int x, int y){
		uint16_t i, j;	
		for(i = 0; i < LIFE_DIM; i++)
		{
			for(j = 0; j < LIFE_DIM; j++)
			{
				if(life_matrix[i][j]==1)
				{
					LCD_SetPoint(x+i,y+j, Black);
				}

			} 
		}
	
	}
void drawLifes(){

	int tmp_x = life_position[0];
	int tmp_y = life_position[1];
	uint8_t counter;
	
	for(counter = 0; counter <= counterLifes; counter++){
		if(counter != counterLifes){
			drawLife(tmp_x*5, tmp_y*5);
			tmp_x += LIFE_DIM/5;
		}
		else{
			cancelLife(tmp_x*5, tmp_y*5);

		}		
	}
}

void drawStartPause(){
	GUI_Text((START_PAUSE_X_POS)*5, (START_PAUSE_Y_POS)*5, (uint8_t *) "PRESS INT0 TO START", Black, White);
}

void drawPause(){
	GUI_Text((PAUSE_X_POS)*5, (PAUSE_Y_POS)*5, (uint8_t *) "PAUSE", Black, Orange);
}
void drawGameOver(){
	GUI_Text((GAMEOVER_X_POS)*5, (GAMEOVER_Y_POS)*5, (uint8_t *) "GAME OVER!", Black, Red);
}
void drawVictory(){
	GUI_Text((GAMEOVER_X_POS)*5, (GAMEOVER_Y_POS)*5, (uint8_t *) " VICTORY! ", Black, Green);
}
void clearPause(){
	uint8_t x, y;
	for(x = PAUSE_X_POS; x < (PAUSE_X_POS+25); x++){
		for(y = PAUSE_Y_POS; y < (PAUSE_Y_POS+4); y++){
			switch(matrix_map[x][y]){
				case 6:
					fillCell(pointLeftTopCorner, x*5, y*5, Yellow, true);
					break;
				case 7:
					fillCell(pointRightTopCorner, x*5, y*5, Yellow, true);
					break;
				case 8:
					fillCell(pointLeftDownCorner, x*5, y*5, Yellow, true);
					break;
				case 9:
					fillCell(pointRightDownCorner, x*5, y*5, Yellow, true);
					break;
				
				case 0:
					fillCell(wallFull, x*5, y*5, Black, true);
					break;

				case 1:
					fillCell(wallFull, x*5, y*5, Blue, true);
					break;
				case 2:
					fillCell(wallLeftTopCorner, x*5, y*5, Blue, true);
					break;
				case 3:
					fillCell(wallRightTopCorner, x*5, y*5, Blue, true);
					break;
				case 4:
					fillCell(wallLeftDownCorner, x*5, y*5, Blue, true);
					break;
				case 5:
					fillCell(wallRightDownCorner, x*5, y*5, Blue, true);
					break;
				case 11:
					fillCell(wallFull, x*5, y*5, Red, true);
					break;
				default:
					break;
			}
		}
	}
}

void clearStartPause(){
	uint8_t x, y;
	for(x = START_PAUSE_X_POS; x < (START_PAUSE_X_POS+33); x++){
		for(y = START_PAUSE_Y_POS; y < (START_PAUSE_Y_POS+5); y++){
			switch(matrix_map[x][y]){
				case 6:
					fillCell(pointLeftTopCorner, x*5, y*5, Yellow, true);
					break;
				case 7:
					fillCell(pointRightTopCorner, x*5, y*5, Yellow, true);
					break;
				case 8:
					fillCell(pointLeftDownCorner, x*5, y*5, Yellow, true);
					break;
				case 9:
					fillCell(pointRightDownCorner, x*5, y*5, Yellow, true);
					break;
				
				case 0:
					fillCell(wallFull, x*5, y*5, Black, true);
					break;

				case 1:
					fillCell(wallFull, x*5, y*5, Blue, true);
					break;
				case 2:
					fillCell(wallLeftTopCorner, x*5, y*5, Blue, true);
					break;
				case 3:
					fillCell(wallRightTopCorner, x*5, y*5, Blue, true);
					break;
				case 4:
					fillCell(wallLeftDownCorner, x*5, y*5, Blue, true);
					break;
				case 5:
					fillCell(wallRightDownCorner, x*5, y*5, Blue, true);
					break;
				case 11:
					fillCell(wallFull, x*5, y*5, Red, true);
					break;
				default:
					break;
			}
		}
	}
}


void generateRandomSuperPill(){
 srand(AD_current);
 uint16_t position = rand() % (counterPointer + 1);
 uint16_t i, j;
	
	for(i = 0; i < ROWS & position != 0; i++){
		for(j = 0; j < COLS & position != 0; j++){
			if(matrix_map[i][j] == 6){//PILLS
				position--;
			}				
		}
	}
	 i--;
	 j--;
	if(matrix_map[i][j] == 6) {
		matrix_map[i][j] = 12;
		fillCell(superPointLeftTopCorner, i*5, j*5, Yellow, false);
	}
	if(matrix_map[i][j+1] == 7) {
		matrix_map[i][j+1] = 13;
		fillCell(superPointRightTopCorner,  i*5, (j+1)*5, Yellow, false);
	}
	if(matrix_map[i+1][j] == 8) {
		matrix_map[i+1][j] = 14;
		fillCell(superPointLeftDownCorner, (i+1)*5, j*5, Yellow, false);
	}
	if(matrix_map[i+1][j+1] == 9) {
		matrix_map[i+1][j+1] = 15;
		fillCell(superPointRightDownCorner, (i+1)*5, (j+1)*5, Yellow, false);
	}
	

	superPillsCoordinates[superPillsToGenerate][0] = i; 
	superPillsCoordinates[superPillsToGenerate][1] = j; 

	

}
/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
