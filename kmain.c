#include "riscv.h"
#include "uart.h"
#include "types.h"
#include "mm.h"

void kmain() 
{
    uart_init();
    k_mm_init();
    while (1);
}

void funny()
{
    uart_print_str("provaaa\r\n");
}

void kmain_continue()
{
    __asm__ volatile ("li t0, 0xdeadbabe");
    uart_print_u64_hex_nl((uint64_t) *((uint64_t *)0x80000000));
    uart_print_newline();
    uart_print_u64_hex_nl((uint64_t) *((uint64_t *)0x40000000));

    /* while(1) {
        *(uint8_t *)(0x40000000) = 65;
    }*/
    
    for (int i = 0; i < 4; i++) {
        __asm__ volatile ("li a0, 0x80");
        __asm__ volatile ("ecall");
    }
}

void continua_trap()
{
    uart_print_str("lmao\r\n");
    
    while (1);
}
