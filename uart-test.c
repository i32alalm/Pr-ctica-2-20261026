#include <stdint.h>
#include "lbc2378.h"

#define LED10 (1u << 10)

/* espera a que el THR de la UART1 esté vacío (U1LSR bit 5 = THRE) */
#define U1_WAIT_THRE() while (!(U1LSR & (1u << 5))) { }

static void delay(void)
{
    for (volatile uint32_t i = 0; i < 5000u; ++i) {
        __asm__ volatile ("nop");
    }
}

/* envía un carácter por la UART1 */
static void uart1_putc(char c)
{
    U1_WAIT_THRE();
    U1THR = (uint8_t)c;
}

/* envía una cadena terminada en '\0' por la UART1 */
static void uart1_puts(const char *s)
{
    while (*s) {
        uart1_putc(*s++);
    }
}

/* imprime un uint8_t en decimal (0-255) seguido de \r\n.
   Usa restas en vez de / y % porque el ARM7 no tiene división hardware */
static void uart1_print_u8(uint8_t n)
{
    char buf[6];          // 3 dígitos + '\r' + '\n' + '\0'
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

    uart1_puts(buf);
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

    //pines de la uart 
    // P0.15 -> TXD1: PINSEL0[31:30] = 01
    PINSEL0 &= ~(1u << 31);
    PINSEL0 |= (1u << 30);
    // P0.16 -> RXD1: PINSEL1[1:0] = 01
    PINSEL1 &= ~(1u << 1);
    PINSEL1 |= (1u << 0);

    // PCLK_UART1 = CCLK: PCLKSEL0[9:8] = 01
    PCLKSEL0 |= (1u << 8);
    PCLKSEL0 &= ~(1u << 9);

    // 8N1 + DLAB = 1 para acceder al divisor
    U1LCR = 0x83;

    // DL = 26 -> 9600 baudios con PCLK = 4 MHz
    U1DLL = 26u;
    U1DLM = 0u;

    // MulVal = 1, DivAddVal = 0 (sin divisor fraccional)
    U1FDR &= ~(15u);
    U1FDR |= (1u << 4);

    // 8N1, DLAB = 0
    U1LCR = 0x03;

    // habilitar y resetear FIFOs
    U1FCR = 0x07;



    U1_WAIT_THRE();
    U1THR = 'h';
    U1_WAIT_THRE();
    U1THR = 'o';
    U1_WAIT_THRE();
    U1THR = 'l';
    U1_WAIT_THRE();
    U1THR = 'a';
    U1_WAIT_THRE();
    U1THR = '\r';
    U1_WAIT_THRE();
    U1THR = '\n';


        uart1_puts("hola-directo\r\n");


    // vamos a fijar la entrada con resistencia pull-up
    // página  156!
    PINMODE4 &= ~(3u << 20);

    uint8_t counter = 0;
    FIO2CLR0 = ~counter;    
    for (;;) {
        if(!(FIO2PIN & LED10))
        {
            delay();
            counter++;
            FIO2SET0 = counter;
            FIO2CLR0 = ~counter; 
            uart1_print_u8(counter);
            while(!(FIO2PIN & LED10))
            {}
        }
    }
    return 0;
}