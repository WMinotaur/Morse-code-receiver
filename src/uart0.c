#include "uart0.h"

void UART0_Init(void)
{
	
	SIM->SCGC4 |= SIM_SCGC4_UART0_MASK;							//UART0 dołączony do zegara
	SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;							//Port B dołączony do zegara
	SIM->SOPT2 |= SIM_SOPT2_UART0SRC(MCGFLLCLK);		//Zegar MCGFLLCLK=41943040Hz (CLOCK_SETUP=0)
	PORTB->PCR[1] = PORT_PCR_MUX(2);								//PTB1=TX_D
	PORTB->PCR[2] = PORT_PCR_MUX(2);								//PTB2=RX_D
	
	UART0->C2 &= ~(UART0_C2_TE_MASK | UART0_C2_RE_MASK );		//Blokada nadajnika i o dbiornika
	UART0->BDH = 0;			//Dla CLOCK_SETUP=0 BR=28800	BDH=0 BR=9600	BDH=1
	UART0->BDL =91;			//Dla CLOCK_SETUP=0 BR=28800	BDL=91 BR=9600	BDL=17	:	CLOCK_SETUP=1	BR=230400		BDL=13
	UART0->C4 = UART0_C4_OSR(15);	//Dla CLOCK_SETUP=0 BR=28800	OSR=15	:	CLOCK_SETUP=1	BR=230400		OSR=15
	UART0->C5 |= UART0_C5_BOTHEDGE_MASK;	//Próbkowanie odbiornika na obydwu zboczach zegara
	UART0->C2 |= UART0_C2_TE_MASK;		//Włącz nadajnik
}

void send_string(const char *str)
{
	while (*str) {
        while (!(UART0->S1 & UART0_S1_TDRE_MASK));  // Czekaj na dostępność UART
        UART0->D = *str++;  // Wyślij znak
    }
}

