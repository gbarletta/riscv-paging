.option norvc
.global entry
.global go_to_supervisor
.extern kmain
.extern kmain_continue
.extern continua_trap
.extern stack_top
.extern funny

entry:
    csrr t0, mhartid
	bnez t0, loop
    la sp, stack_top
    jal kmain

trap_vector:
    li t2, 0xdeadc0de
    j trap_vector
    # j trap_vector
    jal funny
    la t0, continua_trap
    csrw mepc, t0
    mret

go_to_supervisor:
    la t0, trap_vector
    csrw mtvec, t0
    li t0, (1 << 11) | (1 << 5)
    csrw mstatus, t0
    la t1, siu
    csrw mepc, t1
    mret

loop:
	wfi
	j loop

siu:
    li t1, 0xbabecafe
    jal kmain_continue

return_from_trap:
    move t1, a0
    j loop
    