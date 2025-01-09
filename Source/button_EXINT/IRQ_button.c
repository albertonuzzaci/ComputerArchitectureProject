#include "button.h"
#include "LPC17xx.h"
#include "../RIT/RIT.h"		  			/* you now need RIT library 			 */

extern int down_0;


void EINT0_IRQHandler (void)	  	/* INT0														 */
{		
	down_0 = 1;
	NVIC_DisableIRQ(EINT0_IRQn);		/* disable Button interrupts			 */
	LPC_PINCON->PINSEL4    &= ~(1 << 20);     /* GPIO pin selection */
	LPC_SC->EXTINT &= (1 << 0);     /* clear pending interrupt         */
}
