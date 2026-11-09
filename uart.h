#ifndef _UART_H_
#define _UART_H_

#include <stdint.h>

/* Frecuencia de PCLK de las UART. uart_init() selecciona PCLK = CCLK,
   y sin PLL el LPC2378 funciona con el IRC de 4 MHz.
   Si se configura el PLL, redefinir este valor (p.ej. -DUART_PCLK_HZ=...). */
#ifndef UART_PCLK_HZ
#define UART_PCLK_HZ 4000000u
#endif

typedef enum {
    UART0 = 0,   /* TXD0 = P0.2,  RXD0 = P0.3  */
    UART1 = 1,   /* TXD1 = P0.15, RXD1 = P0.16 */
    UART2 = 2,   /* TXD2 = P0.10, RXD2 = P0.11 */
    UART3 = 3    /* TXD3 = P0.0,  RXD3 = P0.1  */
} uart_id_t;

/* Etapa 1 - inicialización: alimenta la UART (PCONP), selecciona
   PCLK = CCLK y asigna sus pines TX/RX (PINSEL). */
void uart_init(uart_id_t id);

/* Etapa 2 - configuración: trama 8N1, divisor para los baudios dados
   y FIFOs habilitadas. Llamar después de uart_init(). */
void uart_config(uart_id_t id, uint32_t baud);

/* Envía un carácter (bloqueante). */
void uart_putc(uart_id_t id, char c);

/* Envía una cadena terminada en '\0'. */
void uart_puts(uart_id_t id, const char *s);

/* Imprime un uint8_t en decimal (0-255) seguido de \r\n. */
void uart_print_u8(uart_id_t id, uint8_t n);

/* Devuelve 1 si hay un carácter recibido pendiente de leer. */
int uart_rx_ready(uart_id_t id);

/* Recibe un carácter (bloqueante). */
char uart_getc(uart_id_t id);

#endif
