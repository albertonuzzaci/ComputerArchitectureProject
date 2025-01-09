/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_timer.c
** Last modified Date:  2014-09-25
** Last Version:        V1.00
** Descriptions:        functions to manage T0 and T1 interrupts
** Correlated files:    timer.h
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include "LPC17xx.h"
#include "timer.h"
#include "../led/led.h"
#include <stdio.h>
#include "GLCD/GLCD.h" 
#include <string.h>  // Include la libreria per memset

/******************************************************************************
** Function name:		Timer0_IRQHandler
**
** Descriptions:		Timer/Counter 0 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/

char tmp_time_buffer[10];
char tmp_score_buffer[10];
void TIMER0_IRQHandler (void)
{
	if(LPC_TIM0->IR & 1) // MR0
	{ 
				
		

		if(counterTime != 0){
			counterTime--;
			if(superPillsToGenerate != 0){
						if(counterTime == timesVector[superPillsToGenerate-1]){
							superPillsToGenerate--;
							generateRandomSuperPill(); 
				}
			}
		}
		else{
			counterTime = INIT_TIME; 
			counterLifes--;
			drawLifes();
			if(counterLifes == 0){
				disable_RIT(); 
				disable_timer(0);
				disable_timer(1);
				drawGameOver();
			}
		}

		
		sprintf(tmp_time_buffer, "%2u", counterTime);
		
		GUI_Text(113, 6, (uint8_t *)tmp_time_buffer, White, Black);
		
		LPC_TIM0->IR = 1;			//clear interrupt flag
	}
	else if(LPC_TIM0->IR & 2){ // MR1
		// your code	
		LPC_TIM0->IR = 2;			// clear interrupt flag 
	}
	else if(LPC_TIM0->IR & 4){ // MR2
		// your code	
		LPC_TIM0->IR = 4;			// clear interrupt flag 
	}
	else if(LPC_TIM0->IR & 8){ // MR3
		// your code	
		LPC_TIM0->IR = 8;			// clear interrupt flag 
	}
  return;
}

/******************************************************************************
** Function name:		Timer1_IRQHandler
**
** Descriptions:		Timer/Counter 1 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
void TIMER1_IRQHandler (void)
{
	if(LPC_TIM1->IR & 1) // MR0
	{ 
		if(FACING == SOUTH){
				changePosition(0,1);
		}
		if(FACING == OVEST){
			changePosition(-1,0);
		}
		if(FACING == EST){
			changePosition(1,0);
		}
		if(FACING == NORTH){
			changePosition(0,-1);
		}
		drawMatrixPacman(pacman_position_x[0]*5, pacman_position_y[0]*5);


		uint8_t score = updateScore();
		if(score > 0){
			counterPointer--;

			uint16_t previousScore = counterScore;
			if(score == 1)
				counterScore += PILL_INC;
			else
				counterScore += SUPER_PILL_INC; 
			
			

			if(counterPointer == 0 || counterScore == (SUPER_PILL_NUMBER*SUPER_PILL_INC + (240-SUPER_PILL_NUMBER)*PILL_INC)){
				disable_RIT(); 
				disable_timer(0);
				disable_timer(1);
				drawVictory();
			}
			else if(counterScore / LIFE_GAIN_POINTS > previousScore / LIFE_GAIN_POINTS){
				counterLifes += 1; 
				drawLifes();
			}
		}
		sprintf(tmp_score_buffer, "%4d", counterScore);
		GUI_Text(190, 6, (uint8_t *)tmp_score_buffer, White, Black);
			uint8_t i; 
		for(i = superPillsToGenerate; i < SUPER_PILL_NUMBER; i++){
					if(matrix_map[superPillsCoordinates[i][0]][superPillsCoordinates[i][1]] == 12){
						fillCell(superPointLeftTopCorner, superPillsCoordinates[i][0]*5, superPillsCoordinates[i][1]*5, Yellow, false);
						fillCell(superPointRightTopCorner,  superPillsCoordinates[i][0]*5, (superPillsCoordinates[i][1]+1)*5, Yellow, false);
						fillCell(superPointLeftDownCorner,  (superPillsCoordinates[i][0]+1)*5, superPillsCoordinates[i][1]*5, Yellow, false);
						fillCell(superPointRightDownCorner,  (superPillsCoordinates[i][0]+1)*5, (superPillsCoordinates[i][1]+1)*5, Yellow, false);
					}				
			}
		LPC_TIM1->IR = 1;			//clear interrupt flag
	}
	else if(LPC_TIM1->IR & 2){ // MR1
		LPC_TIM1->IR = 2;			// clear interrupt flag 
	}
	else if(LPC_TIM1->IR & 4){ // MR2
		// your code	
		LPC_TIM1->IR = 4;			// clear interrupt flag 
	}
	else if(LPC_TIM1->IR & 8){ // MR3
		// your code	
		LPC_TIM1->IR = 8;			// clear interrupt flag 
	} 

	return;
}

/******************************************************************************
** Function name:		Timer2_IRQHandler
**
** Descriptions:		Timer/Counter 1 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/


/******************************************************************************
**                            End Of File
******************************************************************************/

/*
void TIMER0_IRQHandler (void)
{
	if(LPC_TIM0->IR & 1) // MR0
	{ 
		// your code
		LPC_TIM0->IR = 1;			//clear interrupt flag
	}
	else if(LPC_TIM0->IR & 2){ // MR1
		// your code	
		LPC_TIM0->IR = 2;			// clear interrupt flag 
	}
	else if(LPC_TIM0->IR & 4){ // MR2
		// your code	
		LPC_TIM0->IR = 4;			// clear interrupt flag 
	}
	else if(LPC_TIM0->IR & 8){ // MR3
		// your code	
		LPC_TIM0->IR = 8;			// clear interrupt flag 
	}
  return;
}*/