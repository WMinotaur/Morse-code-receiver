#include "mors.h"
#include "uart0.h"
#include <string.h>

#define MORS_BUFFER_SIZE 8  // Maksymalna d³ugoœæ sygna³u Morse'a

// Tablica kodów Morse'a (alfabet + cyfry)
static const struct {
    const char *morse;
    char letter;
} morse_table[] = {
    {".-", 'A'}, {"-...", 'B'}, {"-.-.", 'C'}, {"-..", 'D'}, {".", 'E'},
    {"..-.", 'F'}, {"--.", 'G'}, {"....", 'H'}, {"..", 'I'}, {".---", 'J'},
    {"-.-", 'K'}, {".-..", 'L'}, {"--", 'M'}, {"-.", 'N'}, {"---", 'O'},
    {".--.", 'P'}, {"--.-", 'Q'}, {".-.", 'R'}, {"...", 'S'}, {"-", 'T'},
    {"..-", 'U'}, {"...-", 'V'}, {".--", 'W'}, {"-..-", 'X'}, {"-.--", 'Y'},
    {"--..", 'Z'}, {"-----", '0'}, {".----", '1'}, {"..---", '2'},
    {"...--", '3'}, {"....-", '4'}, {".....", '5'}, {"-....", '6'},
    {"--...", '7'}, {"---..", '8'}, {"----.", '9'}
};

static char buffer[MORS_BUFFER_SIZE];  // Bufor na sygna³ Morse'a
static uint8_t buffer_index = 0;       // Aktualny indeks bufora

void mors_init(void) {
    buffer_index = 0;
    memset(buffer, 0, MORS_BUFFER_SIZE);
}

void mors_add_signal(char signal) {
    if (buffer_index < MORS_BUFFER_SIZE - 1) {
        buffer[buffer_index++] = signal;  // Dodaj sygna³ '.' lub '-'
        buffer[buffer_index] = '\0';     // Zakoñcz ci¹g
    }
}

void mors_process_gap(uint32_t gap_duration) {
    if (gap_duration > 3000) {  // Du¿a przerwa (np. 3s) oznacza spacjê
        while (!(UART0->S1 & UART0_S1_TDRE_MASK));
        UART0->D = ' ';  // Wyœlij spacjê
    } else if (gap_duration > 1000) {  // Ma³a przerwa oznacza koniec znaku
        // Przet³umacz bufor na literê
        char letter = mors_interpret(buffer);
        if (letter != '\0') {
            while (!(UART0->S1 & UART0_S1_TDRE_MASK));
            UART0->D = letter;  // Wyœlij literê
        }
        mors_reset_buffer();  // Zresetuj bufor
    }
}

void mors_reset_buffer(void) {
    buffer_index = 0;
    memset(buffer, 0, MORS_BUFFER_SIZE);
}

char mors_interpret(const char *buffer) {
    for (int i = 0; i < sizeof(morse_table) / sizeof(morse_table[0]); i++) {
        if (strcmp(buffer, morse_table[i].morse) == 0) {
            return morse_table[i].letter;
        }
    }
    return '\0';  // Znak nie zosta³ znaleziony
}
