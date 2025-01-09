/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_RIT.c
** Last modified Date:  2014-09-25
** Last Version:        V1.00
** Descriptions:        functions to manage T0 and T1 interrupts
** Correlated files:    RIT.h
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include "LPC17xx.h"
#include "RIT.h"
#include "../adc/adc.h"
#include "../GLCD/GLCD.h"

/******************************************************************************
** Function name:		RIT_IRQHandler
**
** Descriptions:		REPETITIVE INTERRUPT TIMER handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/

volatile int down_0 = 0;

void RIT_IRQHandler (void)
{					
	static int up=0;
	static int right=0;
	static int left=0;
	static int down=0;
	ADC_start_conversion();
	seed = LPC_TIM0->TC ^ LPC_TIM1->TC ^ AD_current;
	if(empty && seed != 0){
		fillTimesVector();
		empty = false;
	}

	if(down_0 !=0){
		down_0++;
		if((LPC_GPIO2->FIOPIN & (1<<10)) == 0){
			switch(down_0){
				case 2:
					if(!start){
						enable_timer(0);
						enable_timer(1);
						clearStartPause(); 
						start = true; 
					}
					else if(pause){
						enable_timer(0);
						enable_timer(1);
						clearPause();
						pause = false;
					}
					else{
						disable_timer(0);
						disable_timer(1);
						drawPause();
						pause=true;
					}
					break;
				default:
					break;
			}
		}
		else {	/* button released */
			down_0=0;			
			NVIC_EnableIRQ(EINT0_IRQn);							 /* disable Button interrupts			*/
			LPC_PINCON->PINSEL4    |= (1 << 20);     /* External interrupt 0 pin selection */
			
		}
	} // end INT0
	reset_RIT();
	if((LPC_GPIO1->FIOPIN & (1<<29)) == 0){	
		/* Joytick UP pressed */
		up++;
		switch(up){
			case 1:
				rotatePacman(NORTH);
			default:
				break;
		}
	}
	else{
			up=0;
	}
	
	if((LPC_GPIO1->FIOPIN & (1<<28)) == 0){	
		/* Joytick RIGHT pressed */
		right++;
		switch(right){
			case 1:
				rotatePacman(EST);
				
			default:
				break;
		}
	}
	else{
			right=0;
	}
	
	
	if((LPC_GPIO1->FIOPIN & (1<<27)) == 0){	
		/* Joytick LEFT pressed */
		left++;
		switch(left){
			case 1:
				rotatePacman(OVEST);
			default:
				break;
		}
	}
	else{
			left=0;
	}
	
	if((LPC_GPIO1->FIOPIN & (1<<26)) == 0){	
		/* Joytick DOWN pressed */
		down++;
		switch(down){
			case 1:
				rotatePacman(SOUTH);
			default:
				break;
		}
	}
	else{
			down=0;
	}
	

	reset_RIT();
  LPC_RIT->RICTRL |= 0x1;	/* clear interrupt flag */
	
  return;
}

/******************************************************************************
**                            End Of File
******************************************************************************/
