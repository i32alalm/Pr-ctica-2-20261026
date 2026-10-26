#include <stdint.h>


// mirad el manual de usuario, PINSEL4 en el LPC2378. Página: 162 de 706
// emplea 2 bits de configuración 00 01 10 11
#define PINSEL4     (*(volatile uint32_t *)0xE002C010u)

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

#define LED10 (1u << 10)

static void delay(void)
{
    for (volatile uint32_t i = 0; i < 5000u; ++i) {
        __asm__ volatile ("nop");
    }
}

/* Required by startup.S */
void irq_handler_c(void)
{
    for (;;) {
    }
}

/* Required by startup.S */
void fiq_handler_c(void)
{
    for (;;) {
    }
}

int main(void)
{
    // configuramos el puerto como GPIO, valor 00. Página: 
    PINSEL4 &= ~(0b1100001111111111111111);
    // 8 bits bajos como salidas
    FIO2DIR0 |= 255u;
    // bit 10 como entrada
    FIO2DIR1 &= ~(1 << 2);

    // vamos a fijar la entrada con resistencia pull-up
    // página  156!
    PINMODE4 &= ~(3u << 20);

    uint8_t counter = 0;
    FIO2CLR0 = ~counter;    
    for (;;) {
        if(!(FIO2PIN & LED10))
        {
            delay();
            FIO2SET0 += counter;
            FIO2CLR0 = ~counter; 
            counter++;
            while(!(FIO2PIN & LED10))
            {}
        }
    }
    return 0;
}