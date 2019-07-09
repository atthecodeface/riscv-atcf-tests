    .section ".start"
    .p2align        2
    .type   count_trap,@function
    .globl count_trap
count_trap:
    addi sp, sp, -16
    sw t0,0(sp)
    sw t1,4(sp)
    lui     t1, %hi(trap_data)
    addi    t1, t1, %lo(trap_data)
    csrr t0, 0x341 /* mepc */
    addi t0, t0, 4 /* Skip past 32-bit instruction ecall/ebreak/illegal instr */
    csrw 0x341, t0
    csrr t0, 0x342 /* mcause */
    andi t0, t0, 0xf
    slli t0, t0, 2
    add  t1, t0, t1
    lw t0,0(t1)
    addi t0, t0, 1
    sw t0,0(t1)
    addi sp, sp, 16
    mret

    .section ".trap_data"
    .globl trap_data

    .p2align        4 /* 16-byte alignment */
    .type   trap_data,@function
trap_data:
    .long 0, 0, 0, 0
    .long 0, 0, 0, 0
    .long 0, 0, 0, 0
    .long 0, 0, 0, 0

