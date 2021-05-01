/*----------------------------------------------------------------------------
		AULA - TCC 1

AUTOR:  Vinicius Ramon					DATA:	19/02/2016						VERSAO: 1.0

*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
		Includes
 *----------------------------------------------------------------------------*/
 #include <TM4C129.h>

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
int 	TimerUp, cnt, Init;
int		contador;
long 	ticks;
char 	Letra, nibble, tela, S1, S2;
char 	oldtela, oldswitch;
	
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

void SysTick_Handler()
{
			ticks++;
			contador++;
}

void Delay(long tempo)
{
	long start=ticks;
	while((ticks-start)<tempo)
	{
	}
			
}


// FUNÇÃO ENABLE
void Enable()
{
				Delay(5);
				GPIOK->DATA &= ~PK1;
				Delay(5);
				GPIOK->DATA |= PK1;
				Delay(5);
				GPIOK->DATA &= ~PK1;
				Delay(5);
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

		/*********************** CYCLIC ***********************/
			
		//Inicialização do Display
		Delay(500);		//Delay de 500ms
				
		GPIOB_AHB->DATA &= ~PB5; 			// COLOCA RS=0
		GPIOK->DATA &= ~PK0; 					// COLOCA RW=0
		Enable();											// Faz o Enable
		
		//INICIALIZAÇÃO DO DISPLAY E MODO DE UTILIZAÇÃO
		// 4BITS, 2 LINHAS E 5X7 CARACTERES
		LCD_DATA(2);			// Manda o valor 2
		Enable();					// Faz o Enable
		LCD_DATA(2);			// Manda o valor 2
		Enable();					// Faz o Enable
		
		LCD_DATA(8); 			// Manda o valor 8
		Enable();					// Faz o Enable

		//CONTROLE DO DISPLAY
		LCD_DATA(0);			// Manda o valor 0
		Enable();					// Faz o Enable
		
		LCD_DATA(0x0F);		// Manda o valor 15
		Enable();					// Faz o Enable
		
		//LIMPA DISPLAY
		LCD_DATA(0);			// Manda o valor 0
		Enable();					// Faz o Enable
		
		LCD_DATA(1);			// Manda o valor 1
		Enable();					// Faz o Enable
		
		GPIOB_AHB->DATA |= PB5; // COLOCA RS=1
		
		while(1)
		{
			
			S1 = (((GPIOF_AHB->DATA) & PF1))?1:0;
			S2 = (((GPIOJ_AHB->DATA) & PJ0))?0:1;
			
			
			if((S1==1)&&(S2==1))
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
			
				if(contador>=1000)
			{	
				oldtela=99;
				tela=1;
				contador=0;
				cnt++;
				if(cnt==9)
				{
					cnt=0;
				}
			}
			
			if(S1==0)
			{
				tela=0;
				while(1);
			}
			
			
														
			if(tela != oldtela)
			{		
				oldtela = tela;		

			switch(tela)
			{
				case 0: // Tela 1 CHAVE S1 EM OFF
				
					GPIOB_AHB->DATA &= ~PB5; //Coloca RS=0

						//LIMPA DISPLAY
						LCD_DATA(0);			// Manda o valor 0
						Enable();					// Faz o Enable
			
						LCD_DATA(1);			// Manda o valor 1
						Enable();					// Faz o Enable

						// MANDA O CARACTERE PARA A COLUNA SETE DA PRIMEIRA LINHA
						LCD_DATA(8); 			//Manda o valor 8
						Enable();					// Faz o Enable

						LCD_DATA(6); 			//Manda o valor 6
						Enable();					// Faz o Enable
						
						GPIOB_AHB->DATA |= PB5; // COLOCA RS=1
					
						// MANDA O CARACTERE 'T'
						Letra='T';
						nibble=(Letra&0xF0);
						nibble=nibble>>4;
						LCD_DATA(nibble);
						Enable();
		
						Letra='T';
						nibble=(Letra&0x0F);
						LCD_DATA(nibble);
						Enable();
											
						// MANDA O CARACTERE 'C'
						Letra='C';
						nibble=(Letra&0xF0);
						nibble=nibble>>4;
						LCD_DATA(nibble);
						Enable();
		
						Letra='C';
						nibble=(Letra&0x0F);
						LCD_DATA(nibble);
						Enable();
															
						// MANDA O CARACTERE 'C'
						Letra='C';
						nibble=(Letra&0xF0);
						nibble=nibble>>4;
						LCD_DATA(nibble);
						Enable();
		
						Letra='C';
						nibble=(Letra&0x0F);
						LCD_DATA(nibble);
						Enable();
						
						// MANDA O CARACTERE PARA A COLUNA UM DA SEGUNDA LINHA
						
						GPIOB_AHB->DATA &= ~PB5; // COLOCA RS=0
							
						LCD_DATA(0xC); 			//Manda o valor C
						Enable();					// Faz o Enable

						LCD_DATA(0); 			//Manda o valor 0
						Enable();					// Faz o Enable
						
						GPIOB_AHB->DATA |= PB5; // COLOCA RS=1
					
						// MANDA O CARACTERE 'C'
						Letra='C';
						nibble=(Letra&0xF0);
						nibble=nibble>>4;
						LCD_DATA(nibble);
						Enable();
		
						Letra='C';
						nibble=(Letra&0x0F);
						LCD_DATA(nibble);
						Enable();
																	
						// MANDA O CARACTERE 'O'
						Letra='O';
						nibble=(Letra&0xF0);
						nibble=nibble>>4;
						LCD_DATA(nibble);
						Enable();
		
						Letra='O';
						nibble=(Letra&0x0F);
						LCD_DATA(nibble);
						Enable();
																	
						// MANDA O CARACTERE 'N'
						Letra='N';
						nibble=(Letra&0xF0);
						nibble=nibble>>4;
						LCD_DATA(nibble);
						Enable();
		
						Letra='N';
						nibble=(Letra&0x0F);
						LCD_DATA(nibble);
						Enable();
																	
						// MANDA O CARACTERE 'T'
						Letra='T';
						nibble=(Letra&0xF0);
						nibble=nibble>>4;
						LCD_DATA(nibble);
						Enable();
		
						Letra='T';
						nibble=(Letra&0x0F);
						LCD_DATA(nibble);
						Enable();
																	
						// MANDA O CARACTERE 'R'
						Letra='R';
						nibble=(Letra&0xF0);
						nibble=nibble>>4;
						LCD_DATA(nibble);
						Enable();
		
						Letra='R';
						nibble=(Letra&0x0F);
						LCD_DATA(nibble);
						Enable();
											
						// MANDA O CARACTERE 'O'
						Letra='O';
						nibble=(Letra&0xF0);
						nibble=nibble>>4;
						LCD_DATA(nibble);
						Enable();
		
						Letra='O';
						nibble=(Letra&0x0F);
						LCD_DATA(nibble);
						Enable();
																
						// MANDA O CARACTERE 'L'
						Letra='L';
						nibble=(Letra&0xF0);
						nibble=nibble>>4;
						LCD_DATA(nibble);
						Enable();
		
						Letra='L';
						nibble=(Letra&0x0F);
						LCD_DATA(nibble);
						Enable();
						
						// MANDA O CARACTERE 'E'
						Letra='E';
						nibble=(Letra&0xF0);
						nibble=nibble>>4;
						LCD_DATA(nibble);
						Enable();
		
						Letra='E';
						nibble=(Letra&0x0F);
						LCD_DATA(nibble);
						Enable();
						
						// MANDA O CARACTERE PARA A COLUNA DEZ DA SEGUNDA LINHA
						GPIOB_AHB->DATA &= ~PB5; // COLOCA RS=0
						
						LCD_DATA(0xC); 			//Manda o valor C
						Enable();					// Faz o Enable

						LCD_DATA(9); 			//Manda o valor 9
						Enable();					// Faz o Enable
						
						GPIOB_AHB->DATA |= PB5; // COLOCA RS=1
																									
						// MANDA O CARACTERE 'D'
						Letra='D';
						nibble=(Letra&0xF0);
						nibble=nibble>>4;
						LCD_DATA(nibble);
						Enable();
		
						Letra='D';
						nibble=(Letra&0x0F);
						LCD_DATA(nibble);
						Enable();
										
						// MANDA O CARACTERE 'E'
						Letra='E';
						nibble=(Letra&0xF0);
						nibble=nibble>>4;
						LCD_DATA(nibble);
						Enable();
		
						Letra='E';
						nibble=(Letra&0x0F);
						LCD_DATA(nibble);
						Enable();
												
						// MANDA O CARACTERE PARA A COLUNA DOZE DA SEGUNDA LINHA
						GPIOB_AHB->DATA &= ~PB5; // COLOCA RS=0
						
						LCD_DATA(0x0C); 			//Manda o valor C
						Enable();					// Faz o Enable

						LCD_DATA(0X0C); 			//Manda o valor C
						Enable();					// Faz o Enable
						
						GPIOB_AHB->DATA |= PB5; // COLOCA RS=1
					
						// MANDA O CARACTERE 'N'
						Letra='N';
						nibble=(Letra&0xF0);
						nibble=nibble>>4;
						LCD_DATA(nibble);
						Enable();
		
						Letra='N';
						nibble=(Letra&0x0F);
						LCD_DATA(nibble);
						Enable();
																	
						// MANDA O CARACTERE 'V'
						Letra='V';
						nibble=(Letra&0xF0);
						nibble=nibble>>4;
						LCD_DATA(nibble);
						Enable();
		
						Letra='V';
						nibble=(Letra&0x0F);
						LCD_DATA(nibble);
						Enable();
							
				break;
				
				case 1: //TELA 2 CHAVE S1 EM ON
				
						GPIOB_AHB->DATA &= ~PB5; //Coloca RS=0

						//LIMPA DISPLAY
						LCD_DATA(0);			// Manda o valor 0
						Enable();					// Faz o Enable
			
						LCD_DATA(1);			// Manda o valor 1
						Enable();					// Faz o Enable

						// MANDA O CARACTERE PARA A COLUNA SETE DA PRIMEIRA LINHA
						LCD_DATA(8); 			//Manda o valor 8
						Enable();					// Faz o Enable

						LCD_DATA(6); 			//Manda o valor 6
						Enable();					// Faz o Enable
						
						GPIOB_AHB->DATA |= PB5; // COLOCA RS=1
					
						// MANDA O CARACTERE 'P'
						Letra='P';
						nibble=(Letra&0xF0);
						nibble=nibble>>4;
						LCD_DATA(nibble);
						Enable();
		
						Letra='P';
						nibble=(Letra&0x0F);
						LCD_DATA(nibble);
						Enable();
											
						// MANDA O CARACTERE 'I'
						Letra='I';
						nibble=(Letra&0xF0);
						nibble=nibble>>4;
						LCD_DATA(nibble);
						Enable();
		
						Letra='I';
						nibble=(Letra&0x0F);
						LCD_DATA(nibble);
						Enable();
																	
						// MANDA O CARACTERE 'D'
						Letra='D';
						nibble=(Letra&0xF0);
						nibble=nibble>>4;
						LCD_DATA(nibble);
						Enable();
		
						Letra='D';
						nibble=(Letra&0x0F);
						LCD_DATA(nibble);
						Enable();
						
						// MANDA O CARACTERE PARA A COLUNA UM DA SEGUNDA LINHA
						GPIOB_AHB->DATA &= ~PB5; // COLOCA RS=0
						
						LCD_DATA(0x0C); 			//Manda o valor C
						Enable();					// Faz o Enable

						LCD_DATA(0); 			//Manda o valor 0
						Enable();					// Faz o Enable
						
						GPIOB_AHB->DATA |= PB5; // COLOCA RS=1
					
						// MANDA O CARACTERE 'P'
						Letra='P';
						nibble=(Letra&0xF0);
						nibble=nibble>>4;
						LCD_DATA(nibble);
						Enable();
		
						Letra='P';
						nibble=(Letra&0x0F);
						LCD_DATA(nibble);
						Enable();
																	
						// MANDA O CARACTERE '='
						Letra='=';
						nibble=(Letra&0xF0);
						nibble=nibble>>4;
						LCD_DATA(nibble);
						Enable();
		
						Letra='=';
						nibble=(Letra&0x0F);
						LCD_DATA(nibble);
						Enable();
												
						// MANDA O CARACTERE '='
						Letra=cnt+0x30;
						nibble=(Letra&0xF0);
						nibble=nibble>>4;
						LCD_DATA(nibble);
						Enable();
		
						Letra=cnt+0x30;
						nibble=(Letra&0x0F);
						LCD_DATA(nibble);
						Enable();
						
						// MANDA O CARACTERE PARA A COLUNA QUATRO DA SEGUNDA LINHA
						GPIOB_AHB->DATA &= ~PB5; // COLOCA RS=0
			
						LCD_DATA(0x0C); 			//Manda o valor C
						Enable();					// Faz o Enable

						LCD_DATA(3); 			//Manda o valor 3
						Enable();					// Faz o Enable
						
						GPIOB_AHB->DATA |= PB5; // COLOCA RS=1
					
						// MANDA O CARACTERE 'PONTO'
						Letra='.';
						nibble=(Letra&0xF0);
						nibble=nibble>>4;
						LCD_DATA(nibble);
						Enable();
		
						Letra='.';
						nibble=(Letra&0x0F);
						LCD_DATA(nibble);
						Enable();
						
							// MANDA O CARACTERE PARA A COLUNA SEIS DA SEGUNDA LINHA
						GPIOB_AHB->DATA &= ~PB5; // COLOCA RS=0
						
						LCD_DATA(0x0C); 			//Manda o valor C
						Enable();					// Faz o Enable

						LCD_DATA(5); 			//Manda o valor 5
						Enable();					// Faz o Enable
						
						GPIOB_AHB->DATA |= PB5; // COLOCA RS=1
					
						// MANDA O CARACTERE 'I'
						Letra='I';
						nibble=(Letra&0xF0);
						nibble=nibble>>4;
						LCD_DATA(nibble);
						Enable();
		
						Letra='I';
						nibble=(Letra&0x0F);
						LCD_DATA(nibble);
						Enable();
											
						// MANDA O CARACTERE 'P'
						Letra='=';
						nibble=(Letra&0xF0);
						nibble=nibble>>4;
						LCD_DATA(nibble);
						Enable();
		
						Letra='=';
						nibble=(Letra&0x0F);
						LCD_DATA(nibble);
						Enable();
						
							// MANDA O CARACTERE PARA A COLUNA NOVE DA SEGUNDA LINHA
						GPIOB_AHB->DATA &= ~PB5; // COLOCA RS=0
						
						LCD_DATA(0x0C); 			//Manda o valor C
						Enable();					// Faz o Enable

						LCD_DATA(8); 			//Manda o valor 8
						Enable();					// Faz o Enable
						
						GPIOB_AHB->DATA |= PB5; // COLOCA RS=1
					
						// MANDA O CARACTERE 'PONTO'
						Letra='.';
						nibble=(Letra&0xF0);
						nibble=nibble>>4;
						LCD_DATA(nibble);
						Enable();
		
						Letra='.';
						nibble=(Letra&0x0F);
						LCD_DATA(nibble);
						Enable();
						
							// MANDA O CARACTERE PARA A COLUNA ONZE DA SEGUNDA LINHA
						GPIOB_AHB->DATA &= ~PB5; // COLOCA RS=0
						
						LCD_DATA(0x0C); 			//Manda o valor C
						Enable();					// Faz o Enable

						LCD_DATA(0x0A); 			//Manda o valor A
						Enable();					// Faz o Enable
						
						GPIOB_AHB->DATA |= PB5; // COLOCA RS=1
					
						// MANDA O CARACTERE 'D'
						Letra='D';
						nibble=(Letra&0xF0);
						nibble=nibble>>4;
						LCD_DATA(nibble);
						Enable();
		
						Letra='D';
						nibble=(Letra&0x0F);
						LCD_DATA(nibble);
						Enable();
																	
						// MANDA O CARACTERE '='
						Letra='=';
						nibble=(Letra&0xF0);
						nibble=nibble>>4;
						LCD_DATA(nibble);
						Enable();
		
						Letra='=';
						nibble=(Letra&0x0F);
						LCD_DATA(nibble);
						Enable();
						
						// MANDA O CARACTERE PARA A COLUNA QUATORZE DA SEGUNDA LINHA
						GPIOB_AHB->DATA &= ~PB5; // COLOCA RS=0
						
						LCD_DATA(0x0C); 			//Manda o valor C
						Enable();					// Faz o Enable

						LCD_DATA(0x0D); 			//Manda o valor D
						Enable();					// Faz o Enable
						
						GPIOB_AHB->DATA |= PB5; // COLOCA RS=1
					
						// MANDA O CARACTERE 'P'
						Letra='.';
						nibble=(Letra&0xF0);
						nibble=nibble>>4;
						LCD_DATA(nibble);
						Enable();
		
						Letra='.';
						nibble=(Letra&0x0F);
						LCD_DATA(nibble);
						Enable();
						
				break;
					
						case 2: //TELA 3 CONTROLE
				
						GPIOB_AHB->DATA &= ~PB5; //Coloca RS=0

						//LIMPA DISPLAY
						LCD_DATA(0);			// Manda o valor 0
						Enable();					// Faz o Enable
			
						LCD_DATA(1);			// Manda o valor 1
						Enable();					// Faz o Enable

						// MANDA O CARACTERE PARA A COLUNA UM DA PRIMEIRA LINHA
						LCD_DATA(8); 			//Manda o valor 8
						Enable();					// Faz o Enable

						LCD_DATA(0); 			//Manda o valor 0
						Enable();					// Faz o Enable
						
						GPIOB_AHB->DATA |= PB5; // COLOCA RS=1
					
						// MANDA O CARACTERE 'S'
						Letra='S';
						nibble=(Letra&0xF0);
						nibble=nibble>>4;
						LCD_DATA(nibble);
						Enable();
		
						Letra='S';
						nibble=(Letra&0x0F);
						LCD_DATA(nibble);
						Enable();
																	
						// MANDA O CARACTERE 'P'
						Letra='P';
						nibble=(Letra&0xF0);
						nibble=nibble>>4;
						LCD_DATA(nibble);
						Enable();
		
						Letra='P';
						nibble=(Letra&0x0F);
						LCD_DATA(nibble);
						Enable();
											
						// MANDA O CARACTERE '='
						Letra='=';
						nibble=(Letra&0xF0);
						nibble=nibble>>4;
						LCD_DATA(nibble);
						Enable();
		
						Letra='=';
						nibble=(Letra&0x0F);
						LCD_DATA(nibble);
						Enable();
						
						// MANDA O CARACTERE PARA A COLUNA SETE DA PRIMEIRA LINHA
						GPIOB_AHB->DATA &= ~PB5; // COLOCA RS=0
			
						LCD_DATA(8); 			//Manda o valor 8
						Enable();					// Faz o Enable

						LCD_DATA(6); 			//Manda o valor 6
						Enable();					// Faz o Enable
						
						GPIOB_AHB->DATA |= PB5; // COLOCA RS=1
					
						// MANDA O CARACTERE '%'
						Letra='%';
						nibble=(Letra&0xF0);
						nibble=nibble>>4;
						LCD_DATA(nibble);
						Enable();
		
						Letra='%';
						nibble=(Letra&0x0F);
						LCD_DATA(nibble);
						Enable();
						
							// MANDA O CARACTERE PARA A COLUNA DEZ DA PRIMEIRA LINHA
						GPIOB_AHB->DATA &= ~PB5; // COLOCA RS=0
			
						LCD_DATA(8); 			//Manda o valor 8
						Enable();					// Faz o Enable

						LCD_DATA(9); 			//Manda o valor 9
						Enable();					// Faz o Enable
						
						GPIOB_AHB->DATA |= PB5; // COLOCA RS=1
					
						// MANDA O CARACTERE 'P'
						Letra='P';
						nibble=(Letra&0xF0);
						nibble=nibble>>4;
						LCD_DATA(nibble);
						Enable();
		
						Letra='P';
						nibble=(Letra&0x0F);
						LCD_DATA(nibble);
						Enable();
											
						// MANDA O CARACTERE 'V'
						Letra='V';
						nibble=(Letra&0xF0);
						nibble=nibble>>4;
						LCD_DATA(nibble);
						Enable();
		
						Letra='V';
						nibble=(Letra&0x0F);
						LCD_DATA(nibble);
						Enable();
											
						// MANDA O CARACTERE '='
						Letra='=';
						nibble=(Letra&0xF0);
						nibble=nibble>>4;
						LCD_DATA(nibble);
						Enable();
		
						Letra='=';
						nibble=(Letra&0x0F);
						LCD_DATA(nibble);
						Enable();
						
							// MANDA O CARACTERE PARA A COLUNA DEZESSEIS DA PRIMEIRA LINHA
						GPIOB_AHB->DATA &= ~PB5; // COLOCA RS=0
			
						LCD_DATA(8); 			//Manda o valor 8
						Enable();					// Faz o Enable

						LCD_DATA(0x0F); 			//Manda o valor F
						Enable();					// Faz o Enable
						
						GPIOB_AHB->DATA |= PB5; // COLOCA RS=1
					
						// MANDA O CARACTERE '%'
						Letra='%';
						nibble=(Letra&0xF0);
						nibble=nibble>>4;
						LCD_DATA(nibble);
						Enable();
		
						Letra='%';
						nibble=(Letra&0x0F);
						LCD_DATA(nibble);
						Enable();
						
												
						// MANDA O CARACTERE PARA A COLUNA UM DA SEGUNDA LINHA
						GPIOB_AHB->DATA &= ~PB5; // COLOCA RS=0
			
						LCD_DATA(0x0C); 			//Manda o valor C
						Enable();					// Faz o Enable

						LCD_DATA(0); 			//Manda o valor 0
						Enable();					// Faz o Enable
						
						GPIOB_AHB->DATA |= PB5; // COLOCA RS=1
					
						// MANDA O CARACTERE 'E'
						Letra='E';
						nibble=(Letra&0xF0);
						nibble=nibble>>4;
						LCD_DATA(nibble);
						Enable();
		
						Letra='E';
						nibble=(Letra&0x0F);
						LCD_DATA(nibble);
						Enable();
											
						// MANDA O CARACTERE 'R'
						Letra='R';
						nibble=(Letra&0xF0);
						nibble=nibble>>4;
						LCD_DATA(nibble);
						Enable();
		
						Letra='R';
						nibble=(Letra&0x0F);
						LCD_DATA(nibble);
						Enable();
											
						// MANDA O CARACTERE '='
						Letra='=';
						nibble=(Letra&0xF0);
						nibble=nibble>>4;
						LCD_DATA(nibble);
						Enable();
		
						Letra='=';
						nibble=(Letra&0x0F);
						LCD_DATA(nibble);
						Enable();
						
							// MANDA O CARACTERE PARA A COLUNA SETE DA SEGUNDA LINHA
						GPIOB_AHB->DATA &= ~PB5; // COLOCA RS=0
			
						LCD_DATA(0x0C); 			//Manda o valor C
						Enable();					// Faz o Enable

						LCD_DATA(6); 			//Manda o valor 6
						Enable();					// Faz o Enable
						
						GPIOB_AHB->DATA |= PB5; // COLOCA RS=1
					
						// MANDA O CARACTERE '%'
						Letra='%';
						nibble=(Letra&0xF0);
						nibble=nibble>>4;
						LCD_DATA(nibble);
						Enable();
		
						Letra='%';
						nibble=(Letra&0x0F);
						LCD_DATA(nibble);
						Enable();
						
							// MANDA O CARACTERE PARA A COLUNA DEZ DA SEGUNDA LINHA
						GPIOB_AHB->DATA &= ~PB5; // COLOCA RS=0
			
						LCD_DATA(0x0C); 			//Manda o valor C
						Enable();					// Faz o Enable

						LCD_DATA(9); 			//Manda o valor 9
						Enable();					// Faz o Enable
						
						GPIOB_AHB->DATA |= PB5; // COLOCA RS=1
					
						// MANDA O CARACTERE 'C'
						Letra='C';
						nibble=(Letra&0xF0);
						nibble=nibble>>4;
						LCD_DATA(nibble);
						Enable();
		
						Letra='C';
						nibble=(Letra&0x0F);
						LCD_DATA(nibble);
						Enable();
											
						// MANDA O CARACTERE 'V'
						Letra='V';
						nibble=(Letra&0xF0);
						nibble=nibble>>4;
						LCD_DATA(nibble);
						Enable();
		
						Letra='V';
						nibble=(Letra&0x0F);
						LCD_DATA(nibble);
						Enable();
																	
						// MANDA O CARACTERE '='
						Letra='=';
						nibble=(Letra&0xF0);
						nibble=nibble>>4;
						LCD_DATA(nibble);
						Enable();
		
						Letra='=';
						nibble=(Letra&0x0F);
						LCD_DATA(nibble);
						Enable();
						
						// MANDA O CARACTERE PARA A COLUNA DEZESSEIS DA SEGUNDA LINHA
						GPIOB_AHB->DATA &= ~PB5; // COLOCA RS=0
			
						LCD_DATA(0x0C); 			//Manda o valor C
						Enable();					// Faz o Enable

						LCD_DATA(0x0F); 			//Manda o valor F
						Enable();					// Faz o Enable
						
						GPIOB_AHB->DATA |= PB5; // COLOCA RS=1
					
						// MANDA O CARACTERE '%'
						Letra='%';
						nibble=(Letra&0xF0);
						nibble=nibble>>4;
						LCD_DATA(nibble);
						Enable();
		
						Letra='%';
						nibble=(Letra&0x0F);
						LCD_DATA(nibble);
						Enable();		
						
				break;
				
				case 3: // Tela 4 FLUXO
				
					GPIOB_AHB->DATA &= ~PB5; //Coloca RS=0

						//LIMPA DISPLAY
						LCD_DATA(0);			// Manda o valor 0
						Enable();					// Faz o Enable
			
						LCD_DATA(1);			// Manda o valor 1
						Enable();					// Faz o Enable

						// MANDA O CARACTERE PARA A COLUNA SEIS DA PRIMEIRA LINHA
						LCD_DATA(8); 			//Manda o valor 8
						Enable();					// Faz o Enable

						LCD_DATA(5); 			//Manda o valor 5
						Enable();					// Faz o Enable
						
						GPIOB_AHB->DATA |= PB5; // COLOCA RS=1
					
						// MANDA O CARACTERE 'T'
						Letra='F';
						nibble=(Letra&0xF0);
						nibble=nibble>>4;
						LCD_DATA(nibble);
						Enable();
		
						Letra='F';
						nibble=(Letra&0x0F);
						LCD_DATA(nibble);
						Enable();
											
						// MANDA O CARACTERE 'L'
						Letra='L';
						nibble=(Letra&0xF0);
						nibble=nibble>>4;
						LCD_DATA(nibble);
						Enable();
		
						Letra='L';
						nibble=(Letra&0x0F);
						LCD_DATA(nibble);
						Enable();
											
						// MANDA O CARACTERE 'U'
						Letra='U';
						nibble=(Letra&0xF0);
						nibble=nibble>>4;
						LCD_DATA(nibble);
						Enable();
		
						Letra='U';
						nibble=(Letra&0x0F);
						LCD_DATA(nibble);
						Enable();
																
						// MANDA O CARACTERE 'X'
						Letra='X';
						nibble=(Letra&0xF0);
						nibble=nibble>>4;
						LCD_DATA(nibble);
						Enable();
		
						Letra='X';
						nibble=(Letra&0x0F);
						LCD_DATA(nibble);
						Enable();
										
						// MANDA O CARACTERE 'O'
						Letra='O';
						nibble=(Letra&0xF0);
						nibble=nibble>>4;
						LCD_DATA(nibble);
						Enable();
		
						Letra='O';
						nibble=(Letra&0x0F);
						LCD_DATA(nibble);
						Enable();
						
						// MANDA O CARACTERE PARA A COLUNA UM DA SEGUNDA LINHA
						GPIOB_AHB->DATA &= ~PB5; // COLOCA RS=0
			
						LCD_DATA(0x0C); 			//Manda o valor C
						Enable();					// Faz o Enable

						LCD_DATA(0); 			//Manda o valor 0
						Enable();					// Faz o Enable
						
						GPIOB_AHB->DATA |= PB5; // COLOCA RS=1
					
						// MANDA O CARACTERE 'I'
						Letra='I';
						nibble=(Letra&0xF0);
						nibble=nibble>>4;
						LCD_DATA(nibble);
						Enable();
		
						Letra='I';
						nibble=(Letra&0x0F);
						LCD_DATA(nibble);
						Enable();
											
						// MANDA O CARACTERE 'N'
						Letra='N';
						nibble=(Letra&0xF0);
						nibble=nibble>>4;
						LCD_DATA(nibble);
						Enable();
		
						Letra='N';
						nibble=(Letra&0x0F);
						LCD_DATA(nibble);
						Enable();
											
						// MANDA O CARACTERE '='
						Letra='=';
						nibble=(Letra&0xF0);
						nibble=nibble>>4;
						LCD_DATA(nibble);
						Enable();
		
						Letra='=';
						nibble=(Letra&0x0F);
						LCD_DATA(nibble);
						Enable();
												
						// MANDA O CARACTERE PARA A COLUNA SETE DA SEGUNDA LINHA
						GPIOB_AHB->DATA &= ~PB5; // COLOCA RS=0
			
						LCD_DATA(0x0C); 			//Manda o valor C
						Enable();					// Faz o Enable

						LCD_DATA(6); 			//Manda o valor 6
						Enable();					// Faz o Enable
						
						GPIOB_AHB->DATA |= PB5; // COLOCA RS=1
					
						// MANDA O CARACTERE '%'
						Letra='%';
						nibble=(Letra&0xF0);
						nibble=nibble>>4;
						LCD_DATA(nibble);
						Enable();
		
						Letra='%';
						nibble=(Letra&0x0F);
						LCD_DATA(nibble);
						Enable();
												
						// MANDA O CARACTERE PARA A COLUNA NOVE DA SEGUNDA LINHA
						GPIOB_AHB->DATA &= ~PB5; // COLOCA RS=0
			
						LCD_DATA(0x0C); 			//Manda o valor C
						Enable();					// Faz o Enable

						LCD_DATA(8); 			//Manda o valor 8
						Enable();					// Faz o Enable
						
						GPIOB_AHB->DATA |= PB5; // COLOCA RS=1
					
						// MANDA O CARACTERE 'O'
						Letra='O';
						nibble=(Letra&0xF0);
						nibble=nibble>>4;
						LCD_DATA(nibble);
						Enable();
		
						Letra='O';
						nibble=(Letra&0x0F);
						LCD_DATA(nibble);
						Enable();
											
						// MANDA O CARACTERE 'U'
						Letra='U';
						nibble=(Letra&0xF0);
						nibble=nibble>>4;
						LCD_DATA(nibble);
						Enable();
		
						Letra='U';
						nibble=(Letra&0x0F);
						LCD_DATA(nibble);
						Enable();
															
						// MANDA O CARACTERE 'T'
						Letra='T';
						nibble=(Letra&0xF0);
						nibble=nibble>>4;
						LCD_DATA(nibble);
						Enable();
		
						Letra='T';
						nibble=(Letra&0x0F);
						LCD_DATA(nibble);
						Enable();
											
						// MANDA O CARACTERE '='
						Letra='=';
						nibble=(Letra&0xF0);
						nibble=nibble>>4;
						LCD_DATA(nibble);
						Enable();
		
						Letra='=';
						nibble=(Letra&0x0F);
						LCD_DATA(nibble);
						Enable();
						
						// MANDA O CARACTERE PARA A COLUNA DEZESSEIS DA SEGUNDA LINHA
						GPIOB_AHB->DATA &= ~PB5; // COLOCA RS=0
			
						LCD_DATA(0x0C); 			//Manda o valor C
						Enable();					// Faz o Enable

						LCD_DATA(0X0F); 			//Manda o valor F
						Enable();					// Faz o Enable
						
						GPIOB_AHB->DATA |= PB5; // COLOCA RS=1
					
						// MANDA O CARACTERE '%'
						Letra='%';
						nibble=(Letra&0xF0);
						nibble=nibble>>4;
						LCD_DATA(nibble);
						Enable();
		
						Letra='%';
						nibble=(Letra&0x0F);
						LCD_DATA(nibble);
						Enable();
																
				break;
				
						}
	
					}
	} // while(1)
}
