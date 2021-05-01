/*----------------------------------------------------------------------------
		AULA - TCC 1

AUTOR:  Vinicius Ramon												VERSAO: 1.0

*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
		Includes
 *----------------------------------------------------------------------------*/
 #include <TM4C129.h>
 #include <string.h>

/*----------------------------------------------------------------------------
		Defines
 *----------------------------------------------------------------------------*/
#define PA4			(1UL << 4) // Habilita a porta PA4 = D6
#define PA5			(1UL << 5) // Habilita a porta PA5 = D7

#define PB5			(1UL << 5) // Habilita a porta PB5 = RS

#define	PF1			(1UL << 1) // Habilita a Porta PF1 = Chave On/Off
#define	PJ0			(1UL << 0) // Habilita a Porta PJ0 = Muda Tela/Habilita SP

#define PK0			(1UL << 0) // Habilita a porta PK0 = R/W	
#define PK1			(1UL << 1) // Habilita a porta PK1 = ENABLE
#define PK2			(1UL << 2) // Habilita a porta PK2 = D4
#define PK3			(1UL << 3) // Habilita a porta PK3 = D5

#define GrupoA (1UL << 0) // Habilita o grupo das portas A
#define GrupoB (1UL << 1) // Habilita o grupo das portas B
#define	GrupoF (1UL << 5) // Habilita o grupo das portas F
#define	GrupoJ (1UL << 8) // Habilita o grupo das portas J
#define GrupoK (1UL << 9) // Habilita o grupo das portas K

#define PORTA		GPIOA_AHB->DATA // Define que serão entradas de dados
#define PORTB		GPIOB_AHB->DATA // Define que serão entradas de dados
#define PORTK		GPIOK->DATA 		// Define que serão entradas de dados

/*----------------------------------------------------------------------------
		Global Variables
 *----------------------------------------------------------------------------*/
char	Tecla;	
char refresh;
int 	TimerUp;
int		cnt;
int		Init;
int 	S1;
int		S2;
long 	ticks;
long  contador;
char  Letra;
char	nibble;
char	tela;
char  oldtela;
	
/*----------------------------------------------------------------------------
    Functions
 *----------------------------------------------------------------------------*/

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

// Função Contador
void SysTick_Handler()
{
			ticks++;
			contador++;
}

//	Função Delay
void Delay(long tempo)
{
	long start=ticks;
	while((ticks-start)<tempo)
	{
	}
			
}

// Função Habilita/Desabilita RS
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

// Função Habilita/Desabilita RW
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

// FUNÇÃO ENABLE
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

void Enable()
{
		Delay(5);
		LCD_E(1);
		Delay(5);
		LCD_E(0);
		Delay(5);
}

void LCD_NIBBLE(char data)
{
		char nibble;
		nibble = data & 0xF0;
		nibble = nibble >> 4;
		LCD_DATA(nibble);
		Enable();
		nibble = data & 0x0F;
		LCD_DATA(nibble);
		Enable();		
}
void LCD_Cursor(char type)
{
		LCD_RS(0);
		if(type)
		{
				// configura display - cursor piscante 
				LCD_NIBBLE(0x0F);
		}
		else
		{
				// configura display - cursor piscante 
				LCD_NIBBLE(0x0C);
		}
		LCD_RS(1);
}


char LCD_CLEAR()								//Limpa o display
{
			LCD_RS(0); 								// COLOCA RS=0
			LCD_NIBBLE(0x01);
			LCD_RS(1);
}



// 	Função de Inicialização do Display
void	LCD_Init()
{
		/* modo de operacao 4 bits */
		LCD_RS(0);
		LCD_RW(0);	
		Enable();
		LCD_DATA(0x02);
		Enable();
	
		// configura display - 2 linhas 
		LCD_NIBBLE(0x28);

		// cursor piscante	
		LCD_Cursor(1);
	
		LCD_CLEAR();
	
		LCD_RS(1);
	}

	void SetPos(char line, char loc)
{
		LCD_RS(0);
		if(line==1)
		{
				LCD_NIBBLE(0x80 + loc);	
		}
		else
		{
				LCD_NIBBLE(0xC0 + loc);	
		}
		LCD_RS(1);
}
	
//Escreve um texto qualquer no LCD.
char LCD_Out(int line, int column, char text[]) //Line - Em qual linha será inserido o texto. colunm - Em qual coluna será inserido o texto. text - Texto qualquer.
{
		int i;
		int size = strlen(text);
		
		SetPos(line, column);
		
		for(i=0;i<size;i++)
		{
				LCD_NIBBLE(text[i]);
		}
		return 0;	
}

/*----------------------------------------------------------------------------
    MAIN
 *----------------------------------------------------------------------------*/
int main()
{
		oldtela = 99;
		cnt = 0;
	
		/***********************  INIT  ***********************/
		/* Enable clock and init GPIO outputs */
		SYSCTL->RCGCGPIO |= GrupoA | GrupoB | GrupoF | GrupoJ | GrupoK;  /* enable clock for GPIOs  */
		
			/* SWITCH */
		/* INPUTS */
		GPIOF_AHB->DIR	 &= ~PF1;  /* direcao: 0 = entrada ou 1 = saida */ 	
		GPIOF_AHB->PDR 	 |= PF1;  /* habilita o pino como Pull Down  */
		GPIOF_AHB->DEN	 |= PF1;  /* habilita o pino  */
	
		GPIOJ_AHB->PUR	 |= PJ0;		// Habilita pino como Pull Up
		GPIOJ_AHB->DIR	 &= ~PJ0;  /* direcao: 0 = entrada ou 1 = saida */ 	
		GPIOJ_AHB->DEN	 |= PJ0;  /* habilita o pino */
			
	
		//Display LCD
		/* OUTPUTS */
		GPIOA_AHB->DIR	 |= PA4 | PA5;  /* direcao: 0 = entrada ou 1 = saida */ 	
		GPIOA_AHB->DEN	 |= PA4 | PA5;  /* habilita o pino */ 	
	
		GPIOB_AHB->DIR	 |= PB5;  /* direcao: 0 = entrada ou 1 = saida */ 	
		GPIOB_AHB->DEN	 |= PB5;  /* habilita o pino */ 	
	
		GPIOK->DIR	 |= PK0 | PK1 | PK2 | PK3;  /* direcao: 0 = entrada ou 1 = saida */ 	
		GPIOK->DEN	 |= PK0 | PK1 | PK2 | PK3;  /* habilita o pino */ 	
	
		SystemCoreClockUpdate();
		SysTick_Config(SystemCoreClock/1000); // base de tempo 
		
		
		LCD_Init();
		Delay(500);
		/*********************** CYCLIC ***********************/
while(1)
		{
			
			S1 = (((GPIOF_AHB->DATA) & PF1))?1:0;
			S2 = (((GPIOJ_AHB->DATA) & PJ0))?0:1;
			
			if(contador>=1000)
			{
				if(tela==1)
				{
						oldtela=99;
						refresh=1;
				}
				
				contador=0;
				cnt++;
				
				if(cnt==10)
				{
					cnt=0;
				}
			}
				
			if(S1==0)
			{
				tela=0;
			}
					
			else if((S1==1)&&(S2==1))
			{				
				if(tela==4)
				{
					tela=1;
				}
				
				else
				{
					tela++;
				}
								
			}
													
			if(tela != oldtela)
			{		
				oldtela = tela;		

			switch(tela)
			{
				case 0: // Tela 1 CHAVE S1 EM OFF
					LCD_CLEAR();
					LCD_Out(1,0,"      TCC       ");
					LCD_Out(2,0,"Controle de Nv.");
											
				break;
				
				case 1: //TELA 2 CHAVE S1 EM ON			

					if(!refresh)
					{
							LCD_CLEAR();
							LCD_Out(1,0,"      PID       ");
							LCD_Out(2,0,"P=");
							LCD_Out(2,5,"I=");
							LCD_Out(2,10,"D=");
					}
					
					else
					{
						SetPos(2,2);
						LCD_NIBBLE(cnt+0x30);
						refresh=0;
					}

							
				break;
					
				case 2: //TELA 3 CONTROLE
					LCD_CLEAR();
					LCD_Out(1,0,"SP=");
					LCD_Out(1,6,"%");
					LCD_Out(2,0,"ER=");
					LCD_Out(2,6,"%");
					LCD_Out(1,9,"PV=");
				  LCD_Out(1,15,"%");
					LCD_Out(2,9,"CV=");
					LCD_Out(2,15,"%");
												
				break;
				
				case 3: // Tela 4 FLUXO	
					LCD_CLEAR();
					LCD_Out(1,0,"      FLUXO     ");
					LCD_Out(2,0,"IN=");
					LCD_Out(2,6,"%");
					LCD_Out(2,8,"OUT=");
					LCD_Out(2,15,"%");
																
				break;
				
						}
	
					}
	} // while(1)
}
