#include "MKL05Z4.h"
#include "frdm_bsp.h"
#include "ADC.h"
#include "uart0.h"
#include <stdio.h>
#include <string.h>
#include "klaw.h"

// SysTick
uint32_t light = 0;         // Licznik trwania sygnału "jasnego"
uint32_t pause = 0;         // Licznik czasu "ciemnego"
uint8_t sekunda = 0;          // Licznik pauseń SysTick
uint8_t sekunda_OK = 0;       // Flaga dla 100 ms
char kod[4] = "";      // tablica z kodem
char message[32] = "";      // Bufor wiadomości
int letter = 0;
int index = 0;                // Indeks bufora wiadomości

float adc_volt_coeff = 2.91 / 4096.0;  // Przeliczenie ADC na napięcie
uint8_t wynik_ok = 0;
uint16_t temp = 0;
float wynik = 0;

void SysTick_Handler(void) {
    sekunda++;
    if (sekunda == 10) {  // 100 ms
        sekunda = 0;
        sekunda_OK = 1;  // Ustaw flagę co 100 ms
    }
}

void ADC0_IRQHandler() {
    temp = ADC0->R[0];  // Odczyt danej i skasowanie flagi COCO
    if (!wynik_ok) {
        wynik = temp * adc_volt_coeff;  // Skalowanie na napięcie
        wynik_ok = 1;  // Flaga gotowego wyniku
    }
}
const char* morseCode[] = {
    ".-",   //A
    "-...", //B
    "-.-.", //C
    "-..",  //D
    ".",    //E
    "..-.", //F
    "--.",  //G
    "....", //H
    "..",   //I
    ".---", //J
    "-.-",  //K
    ".-..", //L
    "--",   //M
    "-.",   //N
    "---",  //O
    ".--.", //P
    "--.-", //Q
    ".-.",  //R
    "...",  //S
    "-",    //T
    "..-",  //U
    "...-", //V
    ".--",  //W
    "-..-", //X
    "-.--", //Y
    "--.."  //Z
};

char decode_from_morse(const char *morse) {
    for (int i = 0; i < 26; i++) {
        if (strcmp(morse, morseCode[i]) == 0) {
            return 'A' + i;
        }
    }
    return '?';  // Zwraca "?" jeśli nie znaleziono
}


volatile uint8_t S2_press=0;	// "1" - klawisz został wciśnięty "0" - klawisz "skonsumowany"
volatile uint8_t S3_press=0;

void PORTA_IRQHandler(void) {
    uint32_t buf;
    buf = PORTA->ISFR & (S2_MASK | S3_MASK);  // Pobierz flagi pauseń

    if (buf & S2_MASK) {  // Czy pausenie od S12
        DELAY(100);  
        if (!(PTA->PDIR & S2_MASK)) {  // Sprawdzenie stanu pinu
            DELAY(100);
            if (!(PTA->PDIR & S2_MASK)) {
                S2_press = 1;  // Ustaw flagę wciśnięcia
            }
        }
    }
		if(buf & S3_MASK){
									if(!(PTA->PDIR&S3_MASK))		// Minimalizacja drgań zestyków
									{
										DELAY(100)
										if(!(PTA->PDIR&S3_MASK))	// Minimalizacja drgań zestyków (c.d.)
										{
											S3_press =1;
										}
									}
									
								}
    PORTA->ISFR = buf;  // Skasuj flagi pauseń (ważne!)
    NVIC_ClearPendingIRQ(PORTA_IRQn);
}




int main(void) {
    UART0_Init();                          // Inicjalizacja UART
    ADC_Init();                            // Inicjalizacja przetwornika ADC
    SysTick_Config(SystemCoreClock / 10);  // pausenie co 100 ms (10 razy na sekundę)
		char decoded_message = ' ';
		Klaw_Init();
		Klaw_S2_4_Int(); 

    ADC0->SC1[0] = ADC_SC1_AIEN_MASK | ADC_SC1_ADCH(8);  // Wyzwolenie ADC na kanale 8 z pauseniami

    while (1) {
        if (sekunda_OK) {  // Przetwarzaj co 100 ms
            sekunda_OK = 0;  // Zresetuj flagę

            if (wynik_ok) {
                wynik_ok = 0;  // Zresetuj flagę wyniku

                if (wynik > 1.65) {  // Sygnał powyżej progu (światło)
                    light++;
                    pause = 0;
                } 
								else {  // Sygnał poniżej progu (pause)
                    if (light > 0) {  // Koniec sygnału
                        if (light <= 1) {  // Krótki sygnał - kropka
                            kod[index] = '.';
                            send_string(".");
														index++;
                        } if(light >1 && light <5) {  // Długi sygnał - kreska
                            kod[index] = '-';
                            send_string("-");
														index++;
                        }
													if(light > 5){
                            send_string(" ");
														kod[index] = ' ';
														index++;
														message[letter] = ' ' ;
														letter++;
													}
                        light = 0;  // Zresetuj licznik światła
                    }

                    pause++;  // Zliczaj czas ciemnego sygnału
                    if (pause >= 5) {  // Długa pause (np. 500 ms)
                        decoded_message = decode_from_morse(kod);  // dostajemy litere
												if( decoded_message != '?'){
													message[letter] = decoded_message; //tablica z literami
													letter++;									
																									
												}
												memset(kod, 0, sizeof(kod));
                        index = 0;
                        pause = 0;  // Zresetuj licznik przerwy
												
                    }
                }

              
            }
						if (S2_press) {
								send_string("\033[2J");  // Kod ANSI do czyszczenia ekranu
								send_string("\033[H");  // Kod ANSI do ustawienia kursora w lewym górnym rogu
								send_string(message);
								send_string("\n");	
                S2_press = 0;  // Zresetuj flagę wciśnięcia
            }
						if(S3_press){
							send_string("\033[2J");  // Kod ANSI do czyszczenia ekranu
							send_string("\033[H");  // Kod ANSI do ustawienia kursora w lewym górnym rogu
							for(int j=0; j<32;j++){
							message[j] = '\0';
							}
							S3_press = 0;
							letter = 0;
						}
		}

        }
    }
