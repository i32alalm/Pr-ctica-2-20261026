#ifndef _LBC2378_H_
#define _LBC2378_H_

// mirad el manual de usuario, PINSEL4 en el LPC2378. Página: 162 de 706
// emplea 2 bits de configuración 00 01 10 11
#define PINSEL4     (*(volatile uint32_t *)0xE002C010u)
#define PINSEL0     (*(volatile uint32_t *)0xE002C000u)
#define PINSEL1     (*(volatile uint32_t *)0xE002C004u)

// mirad el manual de usuario, página: 173 de 706
// Direccionamos las GPIO de forma indirecta
// Se pueden incluso emplear máscaras
#define FIO2DIR     (*(volatile uint32_t *)0x3FFFC040u)
#define FIO2DIR0    (*(volatile uint8_t *)0x3FFFC040u)
#define FIO2DIR1    (*(volatile uint8_t *)0x3FFFC041u)

#define FIO2SET     (*(volatile uint32_t *)0x3FFFC058u)
#define FIO2SET0     (*(volatile uint8_t *)0x3FFFC058u)

#define FIO2CLR     (*(volatile uint32_t *)0x3FFFC05Cu)
#define FIO2CLR0     (*(volatile uint8_t *)0x3FFFC05Cu)

#define FIO2PIN  (*(volatile uint32_t *) 0x3FFFC054)
#define PINMODE4 (*(volatile uint32_t*) 0xE002C050)


#define PCLKSEL0 (*(volatile uint32_t*) 0xE01FC1A8u)


#define U1LCR (*(volatile uint8_t*) 0xE001000Cu)


#define U1DLL  (*(volatile uint8_t*) 0xE0010000u)

// byte alto del divisor (antes U1DLLM)
#define U1DLM  (*(volatile uint8_t*) 0xE0010004u)

#define U1FDR  (*(volatile uint32_t*) 0xE0010028u)


#define U1THR  (*(volatile uint8_t*) 0xE0010000u)

// FIFO Control, bit 0 obligatorio a 1 (pág. 441)
#define U1FCR  (*(volatile uint8_t*) 0xE0010008u)

// Line Status, bit 5 = THRE (pág. 441)
#define U1LSR  (*(volatile uint8_t*) 0xE0010014u)


 
#endif