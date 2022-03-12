/* Made in Berlin :) */

#ifndef __MM_H__
#define __MM_H__

#include "types.h"

#define KERNEL_PAGE_UNUSED 0
#define KERNEL_PAGE_USED   1
#define KERNEL_PAGE_LAST   2
#define KERNEL_PAGE_SIZE   4096
#define KERNEL_NUM_PAGES   1024
#define KERNEL_PTE_SIZE    8
#define KERNEL_PTE_ENTRIES KERNEL_PAGE_SIZE / KERNEL_PTE_SIZE
#define KERNEL_PTE_VALID   1
#define KERNEL_PTE_READ    (1 << 1)
#define KERNEL_PTE_WRITE   (1 << 2)
#define KERNEL_PTE_EXEC    (1 << 3)
#define KERNEL_PTE_USER    (1 << 4)

#define PTE_IS_VALID(x) (x & 0x1)
#define PPN(x)          (((x >> 28) & 0x3ffffff) | ((x >> 19) & 0x1ff) | ((x >> 10) & 0x1ff)) >> 12
#define VPN0(x)         (x >> 12) & 0x1ff
#define VPN1(x)         (x >> 21) & 0x1ff
#define VPN2(x)         (x >> 30) & 0x1ff

extern uint64_t kernel_heap_start[];

void  k_mm_init();
void *k_page_alloc(uint16_t pages);
void *k_page_zalloc(uint16_t pages);
int   k_page_free(void *addr);
uint64_t k_map_memory(uint64_t *page_table, uint64_t virt, uint64_t phys, int write, int exec, int user);

#endif