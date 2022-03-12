#ifndef __RISCV_H__
#define __RISCV_H__

#include "types.h"

__attribute__((always_inline)) inline uint64_t riscv_csr_get_mhartid()
{
    uint64_t value;
    __asm__ volatile ("csrr %0, mhartid" : "=r"(value));
    return value;
}

__attribute__((always_inline)) inline uint64_t riscv_csr_get_misa()
{
    uint64_t value;
    __asm__ volatile ("csrr %0, misa" : "=r"(value)); 
    return value;
}

__attribute__((always_inline)) inline uint64_t riscv_csr_get_time()
{
    uint64_t value;
    __asm__ volatile ("csrr %0, time" : "=r"(value));
    return value;
}

__attribute__((always_inline)) inline uint64_t riscv_csr_get_satp()
{
    uint64_t value;
    __asm__ volatile ("csrr %0, satp" : "=r"(value));
    return value;
}

__attribute__((always_inline)) inline uint64_t riscv_csr_get_mtvec()
{
    uint64_t value;
    __asm__ volatile ("csrr %0, mtvec" : "=r"(value));
    return value;
}

__attribute__((always_inline)) inline void riscv_csr_set_mtvec(uint64_t value)
{
    __asm__ volatile ("csrrw x0, mtvec, %0" : "=r"(value));
}

__attribute__((always_inline)) inline void riscv_csr_set_satp(uint64_t value)
{
    __asm__ volatile ("csrrw x0, satp, %0" : "=r"(value));
}

#endif