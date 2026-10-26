// Código de startup de nuestro LPC2378
// cada vez que se resetee la placa, se ejecuta este código.
// licencia Apache2 Copyright 2026 Joaquín Cuéllar <p22cupaj@uco.es>

.syntax unified
.cpu arm7tdmi
.arm

.extern main
.extern irq_handler_c
.extern fiq_handler_c


/* ============================================================
 * ARM modes
 * ============================================================ */

.equ MODE_USR, 0x10
.equ MODE_FIQ, 0x11
.equ MODE_IRQ, 0x12
.equ MODE_SVC, 0x13
.equ MODE_ABT, 0x17
.equ MODE_UND, 0x1B
.equ MODE_SYS, 0x1F

.equ I_BIT, 0x80
.equ F_BIT, 0x40


/* ============================================================
 * Exception vector table
 * ============================================================ */

.section .vectors, "ax", %progbits
.align 2

.global _start

_start:

    b Reset_Handler              /* 0x00 */
    b Undefined_Handler          /* 0x04 */
    b SWI_Handler                /* 0x08 */
    b PrefetchAbort_Handler      /* 0x0C */
    b DataAbort_Handler          /* 0x10 */

    // ésto es sólo para openOCD, que hace el checksum para asegurarse que todo va bien
    .word 0                      /* 0x14 */

    b IRQ_Handler                /* 0x18 */
    b FIQ_Handler                /* 0x1C */


/* ============================================================
 * Reset handler
 * ============================================================ */

.section .text.Reset_Handler, "ax", %progbits
.align 2

.global Reset_Handler
.type Reset_Handler, %function

Reset_Handler:

    /*
     * IRQ and FIQ disabled while stacks/runtime are initialized.
     */

    /* Undefined mode stack */
    msr CPSR_c, #(MODE_UND | I_BIT | F_BIT)
    ldr sp, =_stack_und_top

    /* Abort mode stack */
    msr CPSR_c, #(MODE_ABT | I_BIT | F_BIT)
    ldr sp, =_stack_abt_top

    /* FIQ mode stack */
    msr CPSR_c, #(MODE_FIQ | I_BIT | F_BIT)
    ldr sp, =_stack_fiq_top

    /* IRQ mode stack */
    msr CPSR_c, #(MODE_IRQ | I_BIT | F_BIT)
    ldr sp, =_stack_irq_top

    /* Supervisor stack */
    msr CPSR_c, #(MODE_SVC | I_BIT | F_BIT)
    ldr sp, =_stack_svc_top


    /* --------------------------------------------------------
     * Copy initialized .data FLASH -> RAM
     * -------------------------------------------------------- */

    ldr r0, =_sidata
    ldr r1, =_sdata
    ldr r2, =_edata

.L_copy_data:

    cmp r1, r2
    bhs .L_zero_bss

    ldr r3, [r0], #4
    str r3, [r1], #4

    b .L_copy_data


    /* --------------------------------------------------------
     * Clear .bss
     * -------------------------------------------------------- */

.L_zero_bss:

    ldr r1, =_sbss
    ldr r2, =_ebss
    mov r3, #0

.L_zero_bss_loop:

    cmp r1, r2
    bhs .L_enter_system

    str r3, [r1], #4

    b .L_zero_bss_loop


    /*
     * Run application in System mode.
     *
     * System mode uses the User register bank but remains
     * privileged, which is convenient for bare-metal firmware.
    */
    // IRQ y FIQ deshabilitados de inicio


.L_enter_system:

    msr CPSR_c, #(MODE_SYS | I_BIT | F_BIT)
    ldr sp, =_stack_sys_top

    bl main


.L_main_returned:
    b .L_main_returned



/* ============================================================
 * Undefined Instruction
 * ============================================================ */

Undefined_Handler:

    b Undefined_Handler



/* ============================================================
 * Software Interrupt
 * ============================================================ */

SWI_Handler:

    b SWI_Handler



/* ============================================================
 * Prefetch Abort
 * ============================================================ */

PrefetchAbort_Handler:

    b PrefetchAbort_Handler



/* ============================================================
 * Data Abort
 * ============================================================ */

DataAbort_Handler:

    b DataAbort_Handler



/* ============================================================
 * IRQ handler
 *
 * On ARM7TDMI, IRQ entry gives:
 *
 *   LR_irq = return address + 4
 *
 * We must preserve the interrupted context and return using
 *
 *   subs pc, lr, #4
 *
 * which restores CPSR from SPSR_irq.
 * ============================================================ */

IRQ_Handler:

    /*
     * Adjust LR immediately to the real return address.
     */
    sub lr, lr, #4

    /*
     * Save caller registers and adjusted LR on IRQ stack.
     */
    stmdb sp!, {r0-r3, r12, lr}

    /*
     * Save SPSR_irq.
     */
    mrs r0, SPSR
    stmdb sp!, {r0, r1}

    /*
     * Call generic C IRQ dispatcher.
     *
     * irq_handler_c() can inspect VICVectAddr and invoke
     * the registered peripheral handler.
     */
    bl irq_handler_c

    /*
     * Restore SPSR_irq.
     */
    ldmia sp!, {r0, r1}
    msr SPSR_cxsf, r0

    /*
     * Restore registers and LR.
     */
    ldmia sp!, {r0-r3, r12, lr}

    /*
     * Return from IRQ and restore original CPSR.
     */
    subs pc, lr, #0



/* ============================================================
 * FIQ handler
 *
 * FIQ has banked r8-r14, therefore its wrapper can be lighter.
 * ============================================================ */

FIQ_Handler:

    sub lr, lr, #4

    stmdb sp!, {r0-r7, lr}

    mrs r0, SPSR
    stmdb sp!, {r0}

    bl fiq_handler_c

    ldmia sp!, {r0}
    msr SPSR_cxsf, r0

    ldmia sp!, {r0-r7, lr}

    subs pc, lr, #0


.size Reset_Handler, . - Reset_Handler