#ifndef MORS_H
#define MORS_H

#include <stdint.h>

// Inicjalizacja systemu Morse'a
void mors_init(void);

// Dodanie kropki ('.') lub kreski ('-') do bufora Morse'a
void mors_add_signal(char signal);

// Obs�uga przerwy mi�dzy sygna�ami i t�umaczenie bufora na liter�
void mors_process_gap(uint32_t gap_duration);

// Resetowanie bufora Morse'a
void mors_reset_buffer(void);

// T�umaczenie bufora Morse'a na znak ASCII
char mors_interpret(const char *buffer);

#endif // MORS_H
