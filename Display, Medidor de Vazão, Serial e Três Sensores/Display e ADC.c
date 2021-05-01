/************************	INCLUDES **************************/
#include <TM4C129.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "ADC.h"

/************************	DEFINES ***************************/
#define PA4				(1UL << 4)	// CB7
#define PA5				(1UL << 5)	// CB8
#define PA6				(1UL << 6)	

#define PB5				(1UL << 5)	// RS

#define PE0				(1UL << 0)
#define PE1				(1UL << 1)

#define PF0				(1UL << 0)

#define PK0				(1UL << 0)	// R/W
#define PK1				(1UL << 1)	// E
#define PK2				(1UL << 2)	// CB5
#define PK3				(1UL << 3)	// CB6

#define PN0				(1UL << 0)						// Led 2
#define PN1				(1UL << 1)						// Led 1

#define GrupoA (1UL << 0) // Habilita o grupo das portas A
#define GrupoB (1UL << 1) // Habilita o grupo das portas B
#define	GrupoE (1UL << 4) // Habilita o grupo das portas E
#define	GrupoF (1UL << 5) // Habilita o grupo das portas F
#define	GrupoJ (1UL << 8) // Habilita o grupo das portas J
#define GrupoK (1UL << 9) // Habilita o grupo das portas K
#define GrupoN (1UL << 12) // Habilita o grupo das portas K

#define	PORTA			 GPIOA_AHB->DATA
#define	PORTB			 GPIOB_AHB->DATA
#define	PORTK			 GPIOK->DATA

/* LCD ERROR  */
#define LCD_ERROR_SIZE	1

/***********************	  OUTROS  *************************/
#define PJ0				(1ul << 0)
#define PJ1				(1ul << 1)

#define	InPJ0			(GPIOJ_AHB->DATA) & (1ul << 0)
#define	PortN			GPION->DATA
#define	PortF			GPIOF_AHB->DATA

#define	MAX_NR_TIMERS		2
#define	MAX_NR_TEXTS		3
#define	MAX_NR_FIELDS		2

#define	F_VOLTAGE	  		1
#define	F_PART_CNT			2

#define FXTAL 25000000  // fixed, this crystal is soldered to the Connected Launchpad
#define Q            0
#define N            4  // chosen for reference frequency within 4 to 30 MHz
#define MINT        96  // 480,000,000 = (25,000,000/(0 + 1)/(4 + 1))*(96 + (0/1,024))
#define MFRAC        0  // zero to reduce jitter
#define PSYSDIV 		 29
#define SYSCLK (FXTAL/(Q+1)/(N+1))*(MINT+MFRAC/1024)/(PSYSDIV+1)

/************************	ESTRUTURAS ************************/


/********************	 VARIAVEIS GLOBAIS ********************/
int									Valor;
int 								msTicks;			                 // systick counter      
long								TimerDownCnt[MAX_NR_TIMERS];
char								init;
char								NovoValor;
long								Voltage;
int									AVoltage;
long								Counter;
char								refresh;
char								SerialData;
int  								referencia;
int									tela;

/********************	 VARIAVEIS GLOBAIS MEDIDOR DE FLUXO ********************/
int								 flow_frequency; 						// measures flow meter pulse
int 							 l_hour;									 // calculated litres/hour	
int								 vazao;
int								 S1;
int								 S2;
int								 S3;
int								 S4;
int								 flow_time;
long							 currentTime;

/********************	 Predefined functions ********************/
void MainTask(void);

/************************	  FUNCOES   ***********************/

void flow()
{
	flow_frequency++;

}

/*----------------------------------------------------------------------------
    SysTick_Handler
 *----------------------------------------------------------------------------*/
void UART0_Handler(void){
		
		SerialData = (UART0->DR & 0xFF);

		UART0->ICR |= (1ul << 4);
		UART0->ICR |= (1ul << 4);
}
/*----------------------------------------------------------------------------
  Initialize UART pins, Baudrate
      PA0 U0_RX, PA1 U0_TX, 115200 @ 16MHz, 8 N 1
 *----------------------------------------------------------------------------*/
void SER_Initialize (void) {

  SYSCTL->RCGCGPIO |=   ( 1ul << 0);             /* enable clock for GPIOs    */
  GPIOA_AHB->DEN   |=  (( 1ul << 0) | ( 1ul << 1));
  GPIOA_AHB->AFSEL |=  (( 1ul << 0) | ( 1ul << 1));
  GPIOA_AHB->PCTL  &= ~((15ul << 0) | (15ul << 4));
  GPIOA_AHB->PCTL  |=  (( 1ul << 0) | ( 1ul << 4));

  SYSCTL->RCGCUART |=  (1ul << 0);               /* enable clock for UART0    */
  SYSCTL->SRUART   |=  (1ul << 0);               /* reset UART0 */
  SYSCTL->SRUART   &= ~(1ul << 0);               /* clear reset UART0 */
  while ((SYSCTL->PRUART & (1ul << 0)) == 0);    /* wait until UART0 is ready */

  UART0->CTL  =   0;                             /* disable UART              */
  UART0->IM   =   0x10;					                 /* interrupts  enabled       */
  UART0->IBRD =   8;
  UART0->FBRD =   44;
  UART0->LCRH =   (3ul << 5);                     /* 8 bits                   */
  UART0->CC   =   0;                              /* use system clock         */
  UART0->CTL |=  ((1ul << 9) | (1ul << 8));       /* enable RX, TX            */
  UART0->CTL |=   (1ul << 0);                     /* enable UART              */
	/* Enable interrupt */
	NVIC_EnableIRQ(UART0_IRQn);	
}

/*----------------------------------------------------------------------------
  Write character to Serial Port
 *----------------------------------------------------------------------------*/
int SER_PutChar (int c) {

  while ((UART0->FR & (1ul << 7)) == 0x00);
  UART0->DR = (c & 0xFF);

  return (c);
}


/*----------------------------------------------------------------------------
  Read character from Serial Port   (blocking read)
 *----------------------------------------------------------------------------*/
int SER_GetChar (void) {
  return (UART0->DR & 0xFF);
}


/*----------------------------------------------------------------------------
    SysTick_Handler - 1ms
 *----------------------------------------------------------------------------*/
void SysTick_Handler(void) {
		int i;
		msTicks++;
		for(i=0;i<MAX_NR_TIMERS;i++)
		{
				if(TimerDownCnt[i]>0)
				{
						TimerDownCnt[i]--;
				}
		}

		MainTask();
}

void ADC0SS3_Handler(void){
		ADC0->ISC = 0x00000008;             // acknowledge ADC sequence 3 completion
																				// execute user task
		Voltage = ADC0->SSFIFO3 & 0x00000FFF;
		
		Counter++;
}

int String2Int(char Str[])
{
		int i;
		char Svalue[4];
		strcpy(Svalue, Str);
	
		for(i=2;i>0;i--)
		{
				Svalue[i] = Svalue[i-1];
		}										
		Svalue[0] = ' ';										

		return atoi(Svalue);
}

/*----------------------------------------------------------------------------
    delays number of tick Systicks (happens every 1 ms)
 *----------------------------------------------------------------------------*/
void Delay (long dlyTicks) {
		int curTicks;

		curTicks = msTicks;
		while ((msTicks - curTicks) < dlyTicks)
		{
			__NOP();
		}
}

void LCD_RS(char value)
{
	  if(value)
	  {
				PORTB |= PB5;
		}
	  else 
		{
				PORTB &= ~PB5;			
		}	
}

void LCD_RW(char value)
{
	  if(value)
	  {
				PORTK |= PK0;
		}
	  else 
		{
				PORTK &= ~PK0;			
		}	
}

void LCD_E(char value)
{
	  if(value)
	  {
				PORTK |= PK1;
		}
	  else 
		{
				PORTK &= ~PK1;			
		}	
}

void LCD_DATA(char value)
{
		if(value & 0x01)
		{
				PORTK |= PK2;
		}
		else
		{
				PORTK &= ~PK2;
		}

		if(value & 0x02)
		{
				PORTK |= PK3;
		}
		else
		{
				PORTK &= ~PK3;
		}

		if(value & 0x04)
		{
				PORTA |= PA4;
		}
		else
		{
				PORTA &= ~PA4;
		}

		if(value & 0x08)
		{
				PORTA |= PA5;
		}
		else
		{
				PORTA &= ~PA5;
		}		
}

void LCD_EX()
{
		Delay(1);
		LCD_E(1);
		Delay(3);
		LCD_E(0);
		Delay(1);
}

void LCD_COut(char data)
{
		char nibble;
		nibble = data & 0xF0;
		nibble = nibble >> 4;
		LCD_DATA(nibble);
		LCD_EX();
		nibble = data & 0x0F;
		LCD_DATA(nibble);
		LCD_EX();		
}

void SetLoc(char line, char loc)
{
		LCD_RS(0);
		if(line==1)
		{
				LCD_COut(0x80 + loc);	
		}
		else
		{
				LCD_COut(0xC0 + loc);	
		}
		LCD_RS(1);
}

char LCD_Out(char text[], char line, char loc)
{
		int i;
		int size = strlen(text);
		
		if(size>16)
		{
				return LCD_ERROR_SIZE;
		}
		
		SetLoc(line, loc);
		
		for(i=0;i<size;i++)
		{
				LCD_COut(text[i]);
		}
		return 0;
}

void LCD_Limpa()
{
		LCD_RS(0);
		LCD_COut(0x01);
		LCD_RS(1);
}

void LCD_Cursor(char type)
{
		LCD_RS(0);
		if(type)
		{
				// configura display - cursor piscante 
				LCD_COut(0x0F);
		}
		else
		{
				// configura display - cursor piscante 
				LCD_COut(0x0C);
		}
		LCD_RS(1);
}

void LCDConfig()
{
		/* modo de operacao 4 bits */
		LCD_RS(0);
		LCD_RW(0);	
		LCD_EX();
		LCD_DATA(0x02);
		LCD_EX();
	
		// configura display - 2 linhas 
		LCD_COut(0x28);

		// cursor piscante	
		LCD_Cursor(1);
	
		LCD_Limpa();
	
		LCD_RS(1);

}

int main()
{
		//*** variaveis locais
		char SValue[16];
		long timeout;
		int  frequency;
		int  flow_hour;
		int	 set_max;
		int	 set_min;
		int  ref_max;
		int  ref_min;
		int  oldtela;
		
	
		//*** inicializoes
		init = 1;
	
	/* Enable clock and init GPIO outputs */
		SYSCTL->RCGCGPIO |= GrupoA | GrupoB | GrupoE | GrupoF | GrupoJ	| GrupoK | GrupoN;  /* enable clock for GPIOs  */

		/* LCD */
		/* OUTPUTS */
		GPIOB_AHB->DIR       |= PB5;   								/* PB5 is output        */
		GPIOB_AHB->DEN       |= PB5;   								/* PB5 enable 				   */

		GPIOA_AHB->DIR       |= PA4 | PA5;   					/* PA4, PA5 is output        */
		GPIOA_AHB->DEN       |= PA4 | PA5;   					/* PA4, PA5 enable 				   */

		GPIOK->DIR       		 |= PK0 | PK1 | PK2 | PK3;   /* PK0, PK1, PK2, PK3 is output        */
		GPIOK->DEN       		 |= PK0 | PK1 | PK2 | PK3;   /* PK0, PK1, PK2, PK3 enable 				   */

		/* MEDIDOR DE VAZÃO */
		
		GPIOE_AHB->DIR					&= ~PE0;												/* PE0 is input        */
		GPIOE_AHB->DEN					|=  PE0;												/* PE0 enable 				   */
		
		/* ACIONAMENTO VALVULA */
		
		GPIOE_AHB->DIR					|= PE1;												/* PE1 is output        */
		GPIOE_AHB->DEN					|= PE1;												/* PE1 enable 				   */
		
		/* LED's */
		
		GPION->DIR      		 |= PN0 | PN1;   								/* PN1, PN0 is output        */
		GPION->DEN       		 |= PN0 | PN1;   								/* PN1, PN0 enable 				   */
	
  	/* SWITCH */
		
		GPIOA_AHB->DIR					&= ~PA6;												/* PA6 is input        */
		GPIOA_AHB->DEN					|=  PA6;												/* PA6 enable 				   */

		GPIOJ_AHB->PUR   		 |=  PJ0 | PJ1;  								/* PJ0, PJ1 pull-up          */
		GPIOJ_AHB->DIR   		 &= ~(PJ0 | PJ1);								/* PJ0, PJ1 is intput        */
		GPIOJ_AHB->DEN   		 |=  (PJ0 | PJ1);								/* PJ0, PJ1 is digital func. */

	  /*****************    PLL        *****************/

		// Habilita o MOSC resetando o bit NOXTAL e PWRDN bit. O datasheet diz para realizar esta operacao de uma unica vez
		SYSCTL->MOSCCTL  &= ~(0x00000004 | 0x00000008);
		while((SYSCTL->RIS & 0x00000100)==0){};
			
		// USEPLL = 1: liga PLL; OSCSRC = 3:MOSC is the oscillator source ; PLLSRC = 3: MOSC is the PLL input clock
		SYSCTL->RSCLKCFG = 0x13300000 | PSYSDIV;
		SYSCTL->PLLFREQ0 |= ((MFRAC << 10) | (MINT << 0));
		SYSCTL->PLLFREQ1 |= ((Q << 8) |(N << 0));
		SYSCTL->PLLFREQ0 |= 0x00800000;       // liga PLL
		SYSCTL->RSCLKCFG |= 0x40000000;       // update PLL clock
		timeout = 0;
		while(((SYSCTL->PLLSTAT & 0x00000001) == 0) && (timeout < 0xFFFF)){
			timeout = timeout + 1;
		}
		if(timeout == 0xFFFF){
		}
	  /*****************   PLL (FIM)  *****************/

		SysTick_Config(SYSCLK / 1000);     							// Setup SysTick for 1 msec   

	  // ADC 
		ADC0_InitTimer0ATrigger();

		/* preset */
		memset(TimerDownCnt, 0, sizeof(TimerDownCnt));
		
		LCDConfig();
		
		// initialization page
		Delay(500);
		LCD_Limpa();

		refresh = 0;

		//LCD_Out("V:",2,0);
		//LCD_Out("F:",2,8);
		//LCD_Out("L/H",2,13);
		//LCD_Out("E:",1,11);

SER_Initialize();	
	
SerialData = '?';

frequency =0;
vazao = 0;
flow_time = 0;
flow_hour = 0;
set_max = 360;
set_min = 240;
ref_max = 0;
ref_min = 0;
referencia = 0;


		//*** ciclica
		while(1)
		{
				S1 = (((GPIOE_AHB->DATA) & PE0))?1:0;
			  S2 = (((GPIOJ_AHB->DATA) & PJ0))?0:1;
				S3 = (((GPIOJ_AHB->DATA) & PJ1))?0:1;
				S4 = (((GPIOA_AHB->DATA) & PA6))?1:0;
				init = 0;
			
			if(tela==1)
				{
						oldtela=99;
						refresh=1;
				}
			
		/* SERIAL */

			printf("%d\n\r",(int)(GPIOJ_AHB->DATA & PJ0));
			
				if(SerialData == '0')
				{
					  GPION->DATA |= PN0;
				}
				else
				{
						GPION->DATA &= ~PN0;
				}
				
								
			/* SETPOINT */
			switch(referencia)
			{
				case 0:
					//LCD_Limpa();
					LCD_Out("SETPOINT MAX:",1,0);
					sprintf(SValue,"%3d",set_max);
					LCD_Out(SValue,2,0);
				
				if(S2 == 1)
			{			
				set_max++;
			}
			
				else if(S3 == 1)
			{
				set_max--;
			}						
			
			else if (S4 == 1)
			{
				ref_max = set_max;
				referencia = 1;
		  	LCD_Limpa();
			}
			
			break;
			
				case 1:
					
					LCD_Out("SETPOINT MIN:",1,0);
					sprintf(SValue,"%3d",set_min);
					LCD_Out(SValue,2,0);
			
			if(S2 == 1)
			{			
				set_min++;
			}
			
				else if(S3 == 1)
			{
				set_min--;
			}						
			
			else if (S4 == 1)
			{
				ref_min = set_min;
				refresh =1;
				tela = 0;
				//LCD_Limpa();
			}
			
			break;
		}
			
			/* ATUALIZAÇÃO DOS VALORES */
		/*if(tela != oldtela)
			{		
				oldtela = tela;	
				//LCD_Limpa();*/
		/*	switch(tela)
			{
				case 0: // Tela 1 CHAVE S1 EM OFF
					//LCD_Limpa();
					LCD_Out("V:",2,0);
					LCD_Out("F:",2,8);
					LCD_Out("L/H",2,13);
					LCD_Out("E:",1,11);
											
				break;
				
				case 1:
						if(!refresh)
							{
								LCD_Limpa();
								LCD_Out("V:",2,0);
								LCD_Out("F:",2,8);
								LCD_Out("L/H",2,13);
								LCD_Out("E:",1,11);
							}
							
						else
						{
						sprintf(SValue,"%.2f",(float)AVoltage/100);			
						LCD_Out(SValue,2,2);
						sprintf(SValue,"%3d",flow_hour);	
						LCD_Out(SValue,2,10);
						refresh = 0;
						}

			}
	//	}
				
			/* CONDIÇÕES DO SOLO */

		/*	if ((AVoltage) >= 371)
				{
					LCD_Out("Seco      ",1,0);
				//	LCD_Out("On ",1,13);
				//	PortN |= PN1; // seta bit 1 da porta PORTA N

				}
				
				else if (((AVoltage) <= 370) && ((AVoltage) >= 241))
				{
					LCD_Out("Ideal     ",1,0);
				//	LCD_Out("Off",1,13);
				//	PortN &= ~PN1; // reseta o bit 1 da porta N 	

				}
				
				else if ((AVoltage) <= 240)
				{
					LCD_Out("Encharcado",1,0);
				//	LCD_Out("Off",1,13);
				//	PortN &= ~PN1; // reseta o bit 1 da porta N 	

				}
					
			/* TRANSIENTE */
				
				if ((AVoltage) >= ref_max)
				{
					LCD_Out("On ",1,13);
					PortN |= PN1; // seta bit 1 da porta N
					GPIOE_AHB->DATA |= PE1; // seta bit 1 da porta E
				}
				
				else if ((AVoltage) <= ref_min)
				{
					LCD_Out("Off",1,13);
					PortN &= ~PN1; // reseta o bit 1 da porta N 
					GPIOE_AHB->DATA &= ~PE1; // reseta o bit 1 da porta E
				}


				/* MEDIDOR DE VAZÃO */
				
				switch(vazao)
			{
								
				case 0: 
					
					if (S1==0)
					{
						vazao=1;
										
					}
					
																
				break;
					
				case 1:
					
				if (S1==1)
				{
					
					flow_frequency++;
					vazao=0;
				}
										
			}
			
			switch(flow_time)
			{
								
				case 0: 
					
						TimerDownCnt[0] = 1000;
						flow_time = 1;
																
				break;
					
				case 1:
					
				if (TimerDownCnt[0]==0)
				{
					frequency = flow_frequency;
					flow_hour = ((frequency *60)/ 7.5);
					flow_frequency=0;
					flow_time=0;
				}
						break;				
			}
				
				
		}
}

void MainTask()
{
		if(!init)
		{
					/* voltage update*/
					if(TimerDownCnt[1]==0)
					{
							TimerDownCnt[1] = 1000;
							AVoltage = 100 * ((float)Voltage * (float)5)/4096; // alimetacao 5V : 12 bits (4096)
							refresh = 1;
					}						
		}
}
