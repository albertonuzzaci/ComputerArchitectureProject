/****************************************Copyright (c)****************************************************
**                                      
**                                 http://www.powermcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               main.c
** Descriptions:            The GLCD application function
**
**--------------------------------------------------------------------------------------------------------
** Created by:              AVRman
** Created date:            2010-11-7
** Version:                 v1.0
** Descriptions:            The original version
**
**--------------------------------------------------------------------------------------------------------
** Modified by:             Paolo Bernardi
** Modified date:           03/01/2020
** Version:                 v2.0
** Descriptions:            basic program for LCD and Touch Panel teaching
**
*********************************************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "LPC17xx.h"
#include "GLCD/GLCD.h" 
#include "TouchPanel/TouchPanel.h"
#include "timer/timer.h"
#include "joystick/joystick.h"
#include "led/led.h"
#include "button_EXINT/button.h"
#include "RIT/RIT.h"
#include "adc/adc.h"


#ifdef SIMULATOR
extern uint8_t ScaleFlag; // <- ScaleFlag needs to visible in order for the emulator to find the symbol (can be placed also inside system_LPC17xx.h but since it is RO, it needs more work)
#endif


int main(void)
{
	
  SystemInit();  												/* System Initialization (i.e., PLL)  */
	/********** ADC **********/
	ADC_init();
	
	/********** Button **********/
	LED_init();
	
	/********** Button **********/
	BUTTON_init();

	
	/********** JOYSTICK **********/
	joystick_init();
	
	/********** RIT **********/
	init_RIT(0x004C4B40);									/* RIT Initialization 50 msec       	*/
	enable_RIT();	

	LPC_SC -> PCONP |= (1 << 22);  // TURN ON TIMER 2
	LPC_SC -> PCONP |= (1 << 23);  // TURN ON TIMER 3	

	
	/********** GLCD **********/
	LCD_Initialization();
  TP_Init();
	LCD_Clear(Black);
	
	char buffer[10];
	sprintf(buffer, "%2u", counterTime);
	GUI_Text(10, 6, (uint8_t *)"Game over in", White, Black);
	GUI_Text(113, 6, (uint8_t *)buffer, White, Black);
	
	GUI_Text(145, 6, (uint8_t *)"Score ", White, Black);
	sprintf(buffer, "%3u", counterScore);
	GUI_Text(190, 6, (uint8_t *)buffer, White, Black);
	setupGlobalVariables();
	setup();
	setupInitalPosition();
	
	
	/********** TIMER **********/
	init_timer(0, 0, 0, 3, 0x17D7840); //MR0 // 
	init_timer(1, 0, 0, 3, 0x2DC6C0); 
	drawStartPause(); 

	/********** POWER DOWN MODE **********/
	LPC_SC->PCON |= 0x1;									/* power-down	mode										*/
	LPC_SC->PCON &= ~(0x2);						
		
  while (1)	
  {
		__ASM("wfi");
  }
}

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
