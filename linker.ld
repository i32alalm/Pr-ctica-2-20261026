ENTRY(_start)

/* ============================================================
 * LPC2378 memory map
 * ============================================================ */

MEMORY
{
    FLASH (rx)  : ORIGIN = 0x00000000, LENGTH = 512K
    RAM   (rwx) : ORIGIN = 0x40000000, LENGTH = 32K
}


/* ============================================================
 * Stack sizes
 *
 * Initial values. They can be tuned later.
 * ============================================================ */

STACK_SYS_SIZE = 0x800;    /* 2048 B */
STACK_SVC_SIZE = 0x100;    /*  256 B */
STACK_IRQ_SIZE = 0x200;    /*  512 B */
STACK_FIQ_SIZE = 0x100;    /*  256 B */
STACK_ABT_SIZE = 0x080;    /*  128 B */
STACK_UND_SIZE = 0x080;    /*  128 B */


/* ============================================================
 * Sections
 * ============================================================ */

SECTIONS
{
    /* --------------------------------------------------------
     * Exception vector table
     *
     * Must be the first thing in FLASH, at 0x00000000.
     * -------------------------------------------------------- */

    .vectors ORIGIN(FLASH) :
    {
        KEEP(*(.vectors))
    } > FLASH


    /* --------------------------------------------------------
     * Code and constant data
     * -------------------------------------------------------- */

    .text :
    {
        . = ALIGN(4);

        *(.text*)
        *(.rodata*)

        /*
         * ARM/Thumb interworking veneers, if GCC/linker
         * generates them.
         */
        *(.glue_7)
        *(.glue_7t)

        . = ALIGN(4);
    } > FLASH


    /* --------------------------------------------------------
     * ARM exception unwind metadata
     *
     * GCC may generate this even in bare-metal projects.
     * -------------------------------------------------------- */

    .ARM.extab :
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    } > FLASH

    .ARM.exidx :
    {
        __exidx_start = .;
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
        __exidx_end = .;
    } > FLASH


    /* --------------------------------------------------------
     * Initialized writable data
     *
     * Runtime address: RAM
     * Initial contents: FLASH
     * -------------------------------------------------------- */

    .data :
    {
        . = ALIGN(4);

        _sdata = .;

        *(.data*)

        . = ALIGN(4);
        _edata = .;

    } > RAM AT > FLASH

    /*
     * FLASH address containing the initial image of .data.
     *
     * startup.S:
     *
     *   ldr r0, =_sidata
     */
    _sidata = LOADADDR(.data);


    /* --------------------------------------------------------
     * Zero-initialized variables
     * -------------------------------------------------------- */

    .bss (NOLOAD) :
    {
        . = ALIGN(4);

        _sbss = .;

        *(.bss*)
        *(COMMON)

        . = ALIGN(4);
        _ebss = .;

    } > RAM


    /* --------------------------------------------------------
     * End of static RAM allocations.
     *
     * Useful later for Newlib _sbrk().
     * -------------------------------------------------------- */

    . = ALIGN(8);

    _end = .;
    end = .;


    /* ========================================================
     * Stacks
     *
     * ARM stacks grow toward lower addresses.
     *
     * We allocate them from the top of the 32 KiB SRAM
     * downward.
     * ======================================================== */

    _ram_top = ORIGIN(RAM) + LENGTH(RAM);

    _stack_sys_top    = _ram_top;
    _stack_sys_bottom = _stack_sys_top - STACK_SYS_SIZE;

    _stack_svc_top    = _stack_sys_bottom;
    _stack_svc_bottom = _stack_svc_top - STACK_SVC_SIZE;

    _stack_irq_top    = _stack_svc_bottom;
    _stack_irq_bottom = _stack_irq_top - STACK_IRQ_SIZE;

    _stack_fiq_top    = _stack_irq_bottom;
    _stack_fiq_bottom = _stack_fiq_top - STACK_FIQ_SIZE;

    _stack_abt_top    = _stack_fiq_bottom;
    _stack_abt_bottom = _stack_abt_top - STACK_ABT_SIZE;

    _stack_und_top    = _stack_abt_bottom;
    _stack_und_bottom = _stack_und_top - STACK_UND_SIZE;

    /*
     * Lowest address occupied by any stack.
     */
    _stack_limit = _stack_und_bottom;

    _heap_start = _end;
    _heap_end   = _stack_limit;


    /* --------------------------------------------------------
     * Safety checks
     * -------------------------------------------------------- */

    ASSERT(_end <= _stack_limit,
           "RAM overflow: .data/.bss collide with stacks")

    ASSERT((_stack_sys_top & 7) == 0,
           "System stack is not 8-byte aligned")

    ASSERT((_stack_irq_top & 7) == 0,
           "IRQ stack is not 8-byte aligned")

    ASSERT((_stack_fiq_top & 7) == 0,
           "FIQ stack is not 8-byte aligned")


    /* --------------------------------------------------------
     * Non-runtime sections
     * -------------------------------------------------------- */

    /DISCARD/ :
    {
        *(.comment)
        *(.note*)
    }
}