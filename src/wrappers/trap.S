    .section ".start"
    .p2align        2
    .type   record_trap,@function
record_trap:
    addi sp, sp, -16
    sw t0,0(sp)
    sw t1,4(sp)
    lui     t1, %hi(trap_data)
    addi    t1, t1, %lo(trap_data)
    csrr t0, 0x341 /* mepc */
    sw t0,0(t1)
    addi t0, t0, 4
    csrw 0x341, t0
    csrr t0, 0x342 /* mcause */
    sw t0,4(t1)
    csrr t0, 0x304 /* mstatus */
    sw t0,8(t1)
    csrr t0, 0x304 /* mtval */
    sw t0,12(t1)
    mret

    
    .globl record_trap
    .globl trap_data
    
    .p2align        4 /* 16-byte alignment */
    .type   trap_data,@function
trap_data:  
    .long 0 // mepc
    .long 0 // mcause
    .long 0 // mstatus
    .long 0 // mtval

