// iniziato a Berlino :)
// 25 dicembre 2021: funzionante

#include "mm.h"
#include "types.h"
#include "uart.h"
#include "mem.h"
#include "riscv.h"

uint8_t kernel_pages[KERNEL_NUM_PAGES];
uint64_t *kernel_page_table;
uint16_t alloced_pages;

extern void kmain_continue();
extern void go_to_supervisor();

void k_mm_init()
{
    alloced_pages = 0;
    memset(kernel_pages, 0, KERNEL_NUM_PAGES);
    kernel_page_table = (uint64_t *) k_page_zalloc(1);

    k_map_memory(kernel_page_table, 0x10000000, 0x10000000, 1, 0, 0);
    k_map_memory(kernel_page_table, 0x40000000, 0x10000000, 1, 1, 0);

    for (uint64_t addr = 0x80000000; addr < 0x80100000; addr += 0x1000) { // 0x80100000
        // uart_print_u64_hex_nl(addr);
        k_map_memory(kernel_page_table, addr, addr, 1, 1, 0);
    }

    riscv_csr_set_satp((uint64_t)(0x8L << 60 | ((uint64_t) kernel_page_table) >> 12));
    go_to_supervisor();
    // __asm__ volatile ("csrrw x0, mstatus, %0" :: "r"((1 << 11) | (1 << 7))); 
    // __asm__ volatile ("csrrw x0, mepc, %0" :: "r"((uint64_t) 0xC0000000L));
    // __asm__ volatile ("mret");
}

void *k_page_zalloc(uint16_t pages)
{
    void *ptr = k_page_alloc(pages);
    memset((uint8_t *) ptr, 0, pages * KERNEL_PAGE_SIZE);
    return ptr;
}

void *k_page_alloc(uint16_t pages)
{
    int found;

    for (uint16_t i = 0; i < KERNEL_NUM_PAGES; i++) {
        if (kernel_pages[i] == KERNEL_PAGE_USED) {
            continue;
        }


        if (kernel_pages[i] == KERNEL_PAGE_UNUSED) {
            found = 1;
            for (uint64_t j = 0; j < pages; j++) {
                if (kernel_pages[i + j] == KERNEL_PAGE_USED || kernel_pages[i + j] == KERNEL_PAGE_LAST) {
                    found = 0;
                }
            }


            if (found) {
                for (uint64_t j = 0; j < pages; j++) {
                    kernel_pages[i + j] = KERNEL_PAGE_USED;
                }
                kernel_pages[i + (pages - 1)] = KERNEL_PAGE_LAST;
                alloced_pages++;
                
                return (void *) ((uint64_t) kernel_heap_start + (i * KERNEL_PAGE_SIZE));
            }
        }
    }

    return (void *) 0;
}

int k_page_free(void *addr)
{
    uint16_t index = ((uint64_t)addr - (uint64_t)kernel_heap_start) / KERNEL_PAGE_SIZE;

    while (index < KERNEL_NUM_PAGES) {
        if (kernel_pages[index] != KERNEL_PAGE_UNUSED) {
            kernel_pages[index] = KERNEL_PAGE_UNUSED;
        }

        if (kernel_pages[index] == KERNEL_PAGE_LAST) {
            alloced_pages--;
            return 1;
        }

        index++;
    }

    return 0;
}

// very ugly code lmao

uint64_t k_map_memory(uint64_t *page_table, uint64_t virt, uint64_t phys, int write, int exec, int user)
{
    uint64_t vpn[3], ppn[3];
    uint64_t *pte;
    uint64_t *alloced_page;

    vpn[2] = VPN2(virt);
    vpn[1] = VPN1(virt);
    vpn[0] = VPN0(virt);

    pte = page_table;

    if (!PTE_IS_VALID(pte[vpn[2]])) {
        uart_print_str("creato livello 2: ");
        uart_print_u64_hex((uint64_t) vpn[2]);
        uart_print_char(' ');
        uart_print_u64_hex((uint64_t) &pte[vpn[2]]);
        uart_print_newline();
        alloced_page = k_page_zalloc(1);
        uart_print_u64_hex_nl((uint64_t) alloced_page);
        pte[vpn[2]] = (((uint64_t) alloced_page) >> 2) | KERNEL_PTE_VALID;
        uart_print_u64_hex_nl((uint64_t) pte[vpn[2]]);
    }

    pte = alloced_page;

    if (!PTE_IS_VALID(pte[vpn[1]])) {
        uart_print_str("creato livello 1: ");
        uart_print_u64_hex((uint64_t) vpn[1]);
        uart_print_char(' ');
        uart_print_u64_hex((uint64_t) &pte[vpn[1]]);
        uart_print_newline();
        alloced_page = k_page_zalloc(1);
        uart_print_u64_hex_nl((uint64_t) alloced_page);
        pte[vpn[1]] = (((uint64_t) alloced_page) >> 2) | KERNEL_PTE_VALID;
        uart_print_u64_hex_nl((uint64_t) pte[vpn[1]]);
    }

    pte = alloced_page;

    if (!PTE_IS_VALID(pte[vpn[0]])) {
        pte[vpn[0]] = (phys >> 2) | KERNEL_PTE_READ | KERNEL_PTE_VALID;

        if (write) pte[vpn[0]] |= KERNEL_PTE_WRITE;
        if (exec)  pte[vpn[0]] |= KERNEL_PTE_EXEC;
        if (user)  pte[vpn[0]] |= KERNEL_PTE_USER;

        uart_print_str("creato livello 0: ");
        uart_print_u64_hex((uint64_t) vpn[0]);
        uart_print_char(' ');
        uart_print_u64_hex((uint64_t) &pte[vpn[0]]);
        uart_print_newline();
        uart_print_u64_hex_nl((uint64_t) pte[vpn[0]]);

        return pte[vpn[0]];
    }

    return 0;
}

/*
void *k_map_memory(uint64_t *page_table, void *virt, void *phys, int write, int exec, int user)
{
    uint64_t *new_page;

    uint16_t vpn2 = (virt >> 12) & 0x1ff;
    uint16_t vpn1 = (virt >> 21) & 0x1ff;
    uint16_t vpn0 = (virt >> 30) & 0x1ff;

    uint16_t ppn2 = (phys >> 12) & 0x1ff;
    uint16_t ppn1 = (phys >> 21) & 0x1ff;
    uint32_t ppn0 = (phys >> 30) & 0x3ffffff;

    for (int i = 0; i < KERNEL_PAGE_SIZE; i++) {
        if (!PTE_IS_VALID(page_table[i])) {
            new_page = k_page_alloc(1);
            page_table[i] = (new_page << 10) | 0x1;
            memset(new_page, 0, KERNEL_PAGE_SIZE);
        }

        page_table = PPN(page_table[i]);

        for (int j = 0; j < KERNEL_PAGE_SIZE; j++) {
            if (!PTE_IS_VALID(page_table[j])) {
                new_page = k_page_alloc(1);
                page_table[j] = (new_page << 10) | 0x1;
                memset(new_page, 0, KERNEL_PAGE_SIZE);
            }

            page_table = PPN(page_table[j]);

            for (int k = 0; k < KERNEL_PAGE_SIZE; k++) {
                if (!PTE_IS_VALID(page_table[k])) {
                    new_page = k_page_alloc(1);
                    page_table[j] = (new_page << 10) | 0x1;
                    memset(new_page, 0, KERNEL_PAGE_SIZE);
                }
            }
        }
    }
}*/
