#!/bin/sh
riscv64-unknown-elf-gdb kernel.elf --args target remote localhost:9000
