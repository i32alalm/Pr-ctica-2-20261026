#include <stdint.h>
#include "lbc2378.h"
#include "uart.h"
#include <stdio.h>

#define LED10 (1u << 10)

/* UART usada por el programa y sus baudios */
#define UART_ID   UART1
#define UART_BAUD 115200u

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
    /* ===== 1. Inicialización ===== */

    // GPIO: P2.0-P2.7 (LEDs) y P2.10 (pulsador) como GPIO, valor 00
    PINSEL4 &= ~(0b1100001111111111111111);
    // UART: alimentación, PCLK y pines TX/RX
    uart_init(UART_ID);

    /* ===== 2. Configuración ===== */

    // GPIO: 8 bits bajos como salidas, bit 10 como entrada
    FIO2DIR0 |= 255u;
    FIO2DIR1 &= ~(1 << 2);
    // pulsador con resistencia pull-up (página 156)
    PINMODE4 &= ~(3u << 20);
    // UART: 8N1, baudios y FIFOs
    uart_config(UART_ID, UART_BAUD);

    /* ===== 3. Imprimir ===== */

    uart_puts(UART_ID, "hola\r\n");

    uint8_t counter = 0;
    FIO2CLR0 = ~counter;
    for (;;) {
        if (!(FIO2PIN & LED10)) {
            delay();
            counter++;
            FIO2SET0 = counter;
            FIO2CLR0 = ~counter;
            uart_print_u8(UART_ID, counter);
            while (!(FIO2PIN & LED10)) { }
        }
    }
    return 0;
}
