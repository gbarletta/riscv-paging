.option norvc
.global entry
.extern stack_top

entry:
    csrr t0, mhartid
	bnez t0, loop

    la t0, smode
    csrw mepc, t0

    # csrr t0, mstatus
    # or t0, t0, 1 << 11
    # or t0, t0, 
    li t0, (1 << 11)
    csrw mstatus, t0
    mret 

    j loop

loop:
	wfi
	j loop

smode:
    li t0, 0xdeadbeef
    j loop
    