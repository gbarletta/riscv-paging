.option norvc
.global entry
.extern kmain
.extern stack_top

entry:
    csrr	t0, mhartid
	bnez	t0, loop
    la  sp, stack_top
    jal kmain

loop:
	wfi
	j loop
    