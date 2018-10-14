    .section ".start"
    .p2align        2
    .type   record_trap,@function
    .globl record_trap
record_trap:
    addi sp, sp, -16
    sw t0,0(sp)
    sw t1,4(sp)
    lui     t1, %hi(trap_data)
    addi    t1, t1, %lo(trap_data)
    csrr t0, 0x341 /* mepc */
    sw t0,0(t1)
    addi t0, t0, 4 /* Skip past 32-bit instruction ecall/ebreak/illegal instr */
    csrw 0x341, t0
    csrr t0, 0x342 /* mcause */
    sw t0,4(t1)
    csrr t0, 0x304 /* mstatus */
    sw t0,8(t1)
    csrr t0, 0x343 /* mtval */
    sw t0,12(t1)
    addi sp, sp, 16
    mret

    .type   int_callback,@function
    .globl int_callback
int_callback:
    addi sp, sp, -48
    sw a0,0(sp)
    sw a1,4(sp)
    sw a2,8(sp)
    sw a3,12(sp)
    sw ra,16(sp)
    sw t0,20(sp)
    sw t1,24(sp)
    sw t2,28(sp)
    sw t3,32(sp)
    sw t4,36(sp)
    sw t5,40(sp)
    sw t6,44(sp)

    csrr a0, 0x341 /* mepc */
    csrr a1, 0x342 /* mcause */
    csrr a2, 0x304 /* mstatus */
    csrr a3, 0x343 /* mtval */
    lui     t1, %hi(trap_data)
    addi    t1, t1, %lo(trap_data)
    lw      ra,0(t1)
    jalr    ra, ra, 0
    lw t6,44(sp)
    lw t5,40(sp)
    lw t4,36(sp)
    lw t3,32(sp)
    lw t2,28(sp)
    lw t1,24(sp)
    lw t0,20(sp)
    lw ra,16(sp)
    lw a3,12(sp)
    lw a2,8(sp)
    lw a1,4(sp)
    lw a0,0(sp)
    addi sp, sp, 48
    mret

    

    .section ".trap_data"
    .globl trap_data
    
    .p2align        4 /* 16-byte alignment */
    .type   trap_data,@function
trap_data:  
    .long 0 // mepc
    .long 0 // mcause
    .long 0 // mstatus
    .long 0 // mtval

