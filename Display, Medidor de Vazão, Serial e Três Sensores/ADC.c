/************************	INCLUDES **************************/

#include <stdint.h>
#include "ADC.h"
#include "tm4c1294ncpdt.h"	

/************************	DEFINES ***************************/
#define PE2				(1UL << 2)
#define PE3				(1UL << 3)
#define PE4				(1UL << 4)

void ADC0_InitTimer0ATrigger(){
		volatile uint32_t delay;
		// **** GPIO pin initialization ****
		SYSCTL->RCGCGPIO |= (1UL << 4);                                      
		while((SYSCTL->PRGPIO & (1UL << 4)) == 0){};	//     allow time for clock to stabilize

		GPIOE_AHB->AFSEL |= PE2 | PE3 | PE4;     // enable alternate function on PE3
		GPIOE_AHB->DEN &= ~(PE2 | PE3 | PE4);      // disable digital I/O on PE3
		GPIOE_AHB->AMSEL |= PE2 | PE3 | PE4;     // enable analog functionality on PE3

		// **** general initialization ****
    SYSCTL->RCGCTIMER |= 0x01;     									// activate TIMER0
		while((SYSCTL->PRTIMER & 0x01) == 0){};					// allow time for clock to stabilize

		TIMER0->CTL		= 0x00000000;      // disable TIMER0A during setup
		TIMER0->CTL  |= 0x00000020;      // enable timer0A trigger to ADC
		TIMER0->ADCEV |= 0x00000001;		 // timer0A time-out event ADC trigger enabled
		TIMER0->CFG   = 0x00000000;      // configure for 32-bit mode
		TIMER0->TAMR  = 0x00000002;      // configure for periodic mode, default down-count settings
		TIMER0->TAILR = (16000000/10)-1; // reload value ( 100 ms )
		TIMER0->TAPR  = 0;               // bus clock resolution
		TIMER0->ICR   = 0x00000001;      // clear TIMER0A timeout flag
		TIMER0->IMR   = 0x00000000;      // disable arm timeout interrupt
		TIMER0->CTL |= 0x00000001;    	 // enable TIMER0A
                                    
		// **** ADC initialization **** //                                      
		SYSCTL->RCGCADC |= 0x00000001;	// activate clock for ADC0
		while((SYSCTL->PRADC & 0x00000001) == 0){}; //     allow time for clock to stabilize

		ADC0->SSPRI = 0x3210;        				 				// sequencer 3 is lowest priority (default)
		ADC0->ACTSS = 0x00000000;   								// disable sample sequencer 3
		ADC0->SAC = (ADC0->SAC &~ 0x00000007); 			// configure for no hardware oversampling (default)
		ADC0->CTL = (ADC0->CTL &~ 0x00000001); 			// configure for internal reference (default)

		ADC0->SSOP3 &= ~0x00000001;  																// configure for ADC result saved to FIFO (default)
		ADC0->SSTSH3 = (ADC0->SSTSH3 &~ 0x0000000F)+0; 			  			// configure for 4 ADC clock period S&H (default)
		ADC0->SSMUX3 = (ADC0->SSMUX3 &~ 0x000F)+(0x00 & 0xF); 			// set channel
		ADC0->SSEMUX3 = (ADC0->SSEMUX3 &~ 0x01)+((0x00 & 0x10)>>4); // set extended channel
		ADC0->SSCTL3 = 0x0006;           // no TS0 D0, yes IE0 END0
		ADC0->IM |= 0x00000008;          // enable SS3 interrupts
		ADC0->CC = ((ADC0->CC &~ 0x000003F0)+(14<<4)) |
               ((ADC0->CC &~ 0x0000000F)+0);

		ADC0->ACTSS = 0x00000008;       										// enable sample sequencer 3
		ADC0->EMUX = (ADC0->EMUX &~ 0x0000F000)+0x00005000;	// configure seq3 for timer trigger
			
		/* Enable interrupt */
		NVIC_EnableIRQ(ADC0SS3_IRQn);	
}

