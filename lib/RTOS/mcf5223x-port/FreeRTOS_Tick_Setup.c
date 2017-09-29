/*
    FreeRTOS V7.6.0 - Copyright (C) 2013 Real Time Engineers Ltd. 
    All rights reserved

    VISIT http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS provides completely free yet professionally developed,    *
     *    robust, strictly quality controlled, supported, and cross          *
     *    platform software that has become a de facto standard.             *
     *                                                                       *
     *    Help yourself get started quickly and support the FreeRTOS         *
     *    project by purchasing a FreeRTOS tutorial book, reference          *
     *    manual, or both from: http://www.FreeRTOS.org/Documentation        *
     *                                                                       *
     *    Thank you!                                                         *
     *                                                                       *
    ***************************************************************************

    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation >>!AND MODIFIED BY!<< the FreeRTOS exception.

    >>! NOTE: The modification to the GPL is included to allow you to distribute
    >>! a combined work that includes FreeRTOS without being obliged to provide
    >>! the source code for proprietary components outside of the FreeRTOS
    >>! kernel.

    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  Full license text is available from the following
    link: http://www.freertos.org/a00114.html

    1 tab == 4 spaces!

    ***************************************************************************
     *                                                                       *
     *    Having a problem?  Start by reading the FAQ "My application does   *
     *    not run, what could be wrong?"                                     *
     *                                                                       *
     *    http://www.FreeRTOS.org/FAQHelp.html                               *
     *                                                                       *
    ***************************************************************************

    http://www.FreeRTOS.org - Documentation, books, training, latest versions,
    license and Real Time Engineers Ltd. contact details.

    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool, a DOS
    compatible FAT file system, and our tiny thread aware UDP/IP stack.

    http://www.OpenRTOS.com - Real Time Engineers ltd license FreeRTOS to High
    Integrity Systems to sell under the OpenRTOS brand.  Low cost OpenRTOS
    licenses offer ticketed support, indemnification and middleware.

    http://www.SafeRTOS.com - High Integrity Systems also provide a safety
    engineered and independently SIL3 certified version for use in safety and
    mission critical applications that require provable dependability.

    1 tab == 4 spaces!
*/

#include "FreeRTOS.h"
#include "task.h"
#include "uart0.h"
#include <stdio.h>
#include "cpu_config.h"
#include "FreeRTOSConfig.h"

/*
 * FreeRTOS.org requires two interrupts - a tick interrupt generated from a
 * timer source, and a spare interrupt vector used for context switching.
 * The configuration below uses PIT0 for the former, and vector 16 for the
 * latter.  **IF YOUR APPLICATION HAS BOTH OF THESE INTERRUPTS FREE THEN YOU DO
 * NOT NEED TO CHANGE ANY OF THIS CODE** - otherwise instructions are provided
 * here for using alternative interrupt sources.
 *
 * To change the tick interrupt source:
 *
 *	1) Modify vApplicationSetupInterrupts() below to be correct for whichever
 *	peripheral is to be used to generate the tick interrupt.
 *
 *	2) Change the name of the function __cs3_isr_interrupt_119() defined within
 *	this file to be correct for the interrupt vector used by the timer peripheral.
 *	The name of the function should contain the vector number, so by default vector
 *	number 119 is being used.
 *
 *	3) Make sure the tick interrupt is cleared within the interrupt handler function.
 *  Currently __cs3_isr_interrupt_119() clears the PIT0 interrupt.
 *
 * To change the spare interrupt source:
 *
 *  1) Modify vApplicationSetupInterrupts() below to be correct for whichever
 *  interrupt vector is to be used.  Make sure you use a spare interrupt on interrupt
 *  controller 0, otherwise the register used to request context switches will also
 *  require modification.  By default vector 16 is used which is free on most MCF52xxx
 *  devices.
 *
 *  2) Change the definition of configYIELD_INTERRUPT_VECTOR within FreeRTOSConfig.h
 *  to be correct for your chosen interrupt vector.
 *
 *  3) Change the name of the function __cs3_isr_interrupt_80() within portasm.S
 *  to be correct for whichever vector number is being used.  By default interrupt
 *  controller 0 vector number 16 is used, which corresponds to vector number 80.
 */


 void vApplicationSetupInterrupts( void )
    {
#ifdef  __freeRTOS_INCLUDED__
    	//MCF_PIT0_PCSR  = (uint16)(MCF_PIT_PCSR_PRE(0));	/* Divide system clock/2 by 2^PCSR	*/
    	MCF_PIT0_PCSR  = (uint16)(MCF_PIT_PCSR_PRE(portPRESCALER));	/* Divide system clock/2 by 2^PCSR	*/
    	//MCF_INTC0_ICR55 = MCF_INTC_ICR_IL(TIMER_NETWORK_LEVEL);
        MCF_INTC0_ICR55 = ( 1 | ( configKERNEL_INTERRUPT_PRIORITY << 3 ) );
        MCF_INTC0_IMRH &= ~( MCF_INTC_IMRH_INT_MASK55 );
    	
    	 MCF_INTC0_ICR16 = ( 0 | ( configKERNEL_INTERRUPT_PRIORITY << 3 ) );
    	 MCF_INTC0_IMRH &= ~( MCF_INTC_IPRL_INT16 );
    	    
    	//MCF_PIT0_PMR = PMR;						/* modulo count	*/
    	MCF_PIT0_PCSR |= MCF_PIT_PCSR_OVW | MCF_PIT_PCSR_PIE |
    	                 MCF_PIT_PCSR_PIF | MCF_PIT_PCSR_RLD |
    	                 MCF_PIT_PCSR_EN;
    	//MCF_PIT0_PMR = 15;
    	MCF_PIT0_PMR = RELOAD_TIME;
    	//uart0_put_str("PIT CONFIGD");
    	asm(move.w #0x2000,SR);
#endif
    }


/*-----------------------------------------------------------*/



__declspec(interrupt)
	void __cs3_isr_interrupt_119()
	{
	
	unsigned portLONG ulSavedInterruptMask;
	//uart0_put_str("PIT ISR");
		/* Clear the PIT0 interrupt. */
		MCF_PIT0_PCSR |= MCF_PIT_PCSR_PIF;

		/* Increment the RTOS tick. */
		ulSavedInterruptMask = portSET_INTERRUPT_MASK_FROM_ISR();
			if( xTaskIncrementTick() != pdFALSE )
			{
				taskYIELD();
			}
		portCLEAR_INTERRUPT_MASK_FROM_ISR( ulSavedInterruptMask );
	}
