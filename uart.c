#include <stdint.h>
#include "lbc2378.h"
#include "uart.h"

/* Direcciones base de las UART (manual de usuario LPC23xx, cap. UART) */
static const uint32_t uart_base[4] = {
    0xE000C000u,   /* UART0 */
    0xE0010000u,   /* UART1 */
    0xE0078000u,   /* UART2 */
    0xE007C000u    /* UART3 */
};

/* Offsets de los registros respecto a la base (iguales en las 4 UART) */
#define UREG8(id, off)  (*(volatile uint8_t  *)(uart_base[(id)] + (off)))
#define UREG32(id, off) (*(volatile uint32_t *)(uart_base[(id)] + (off)))

#define RBR(id) UREG8(id, 0x00)   /* DLAB = 0, lectura   */
#define THR(id) UREG8(id, 0x00)   /* DLAB = 0, escritura */
#define DLL(id) UREG8(id, 0x00)   /* DLAB = 1 */
#define DLM(id) UREG8(id, 0x04)   /* DLAB = 1 */
#define FCR(id) UREG8(id, 0x08)
#define LCR(id) UREG8(id, 0x0C)
#define LSR(id) UREG8(id, 0x14)
#define FDR(id) UREG32(id, 0x28)

#define LSR_RDR  (1u << 0)   /* dato recibido disponible */
#define LSR_THRE (1u << 5)   /* THR vacío */

/* División entera sin signo por restas desplazadas: el ARM7TDMI no tiene
   división hardware y enlazamos con -nostdlib (sin __aeabi_uidiv). */
static uint32_t udiv(uint32_t n, uint32_t d)
{
    uint32_t q = 0;
    for (int i = 31; i >= 0; --i) {
        if ((n >> i) >= d) {
            n -= d << i;
            q |= 1u << i;
        }
    }
    return q;
}

/* Escribe un campo de 2 bits (valor v) en la posición pos de un registro */
static void set2(volatile uint32_t *reg, unsigned pos, uint32_t v)
{
    *reg = (*reg & ~(3u << pos)) | ((v & 3u) << pos);
}

void uart_init(uart_id_t id)
{
    switch (id) {
    case UART0:
        PCONP |= (1u << 3);
        set2(&PCLKSEL0, 6, 1);     /* PCLK_UART0 = CCLK */
        set2(&PINSEL0, 4, 1);      /* P0.2 -> TXD0 */
        set2(&PINSEL0, 6, 1);      /* P0.3 -> RXD0 */
        break;
    case UART1:
        PCONP |= (1u << 4);
        set2(&PCLKSEL0, 8, 1);     /* PCLK_UART1 = CCLK */
        set2(&PINSEL0, 30, 1);     /* P0.15 -> TXD1 */
        set2(&PINSEL1, 0, 1);      /* P0.16 -> RXD1 */
        break;
    case UART2:
        PCONP |= (1u << 24);
        set2(&PCLKSEL1, 16, 1);    /* PCLK_UART2 = CCLK */
        set2(&PINSEL0, 20, 1);     /* P0.10 -> TXD2 */
        set2(&PINSEL0, 22, 1);     /* P0.11 -> RXD2 */
        break;
    case UART3:
        PCONP |= (1u << 25);
        set2(&PCLKSEL1, 18, 1);    /* PCLK_UART3 = CCLK */
        set2(&PINSEL0, 0, 2);      /* P0.0 -> TXD3 */
        set2(&PINSEL0, 2, 2);      /* P0.1 -> RXD3 */
        break;
    default:
        break;
    }
}

void uart_config(uart_id_t id, uint32_t baud)
{
    if ((unsigned)id > UART3 || baud == 0u) {
        return;
    }

    /* DL = PCLK / (16 * baud), redondeado al entero más cercano.
       Ej.: 4 MHz, 9600 baudios -> DL = 26 (9615 baudios, +0.16 %) */
    uint32_t div = 16u * baud;
    uint32_t dl  = udiv(UART_PCLK_HZ + div / 2u, div);
    if (dl == 0u) {
        dl = 1u;
    }

    /* 8N1 + DLAB = 1 para acceder al divisor */
    LCR(id) = 0x83;
    DLL(id) = (uint8_t)(dl & 0xFFu);
    DLM(id) = (uint8_t)((dl >> 8) & 0xFFu);

    /* MulVal = 1, DivAddVal = 0 (sin divisor fraccional) */
    FDR(id) = (1u << 4);

    /* 8N1, DLAB = 0 */
    LCR(id) = 0x03;

    /* habilitar y resetear FIFOs */
    FCR(id) = 0x07;
}

void uart_putc(uart_id_t id, char c)
{
    while (!(LSR(id) & LSR_THRE)) { }
    THR(id) = (uint8_t)c;
}

void uart_puts(uart_id_t id, const char *s)
{
    while (*s) {
        uart_putc(id, *s++);
    }
}

/* Usa restas en vez de / y % porque el ARM7 no tiene división hardware */
void uart_print_u8(uart_id_t id, uint8_t n)
{
    char buf[6];          /* 3 dígitos + '\r' + '\n' + '\0' */
    uint8_t i = 0;
    uint8_t c = 0, d = 0;

    while (n >= 100u) { n -= 100u; c++; }
    while (n >= 10u)  { n -= 10u;  d++; }

    if (c)       buf[i++] = '0' + c;
    if (c || d)  buf[i++] = '0' + d;
    buf[i++] = '0' + n;
    buf[i++] = '\r';
    buf[i++] = '\n';
    buf[i]   = '\0';

    uart_puts(id, buf);
}

int uart_rx_ready(uart_id_t id)
{
    return (LSR(id) & LSR_RDR) != 0;
}

char uart_getc(uart_id_t id)
{
    while (!uart_rx_ready(id)) { }
    return (char)RBR(id);
}
