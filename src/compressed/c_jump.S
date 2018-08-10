    .globl start
    .globl tohost

    /*
    registers:
    0-7:    zero, ra, sp, gp, tp, t0, t1, t2
    8-15:   s0, s1, a0-a5
    16-23:  a6, a7, s2-7
    24-31:  s8-s11, t3-t6


    Tested here (briefly):
    j
    jal
    jr
    jalr
    
    */
    .p2align        4 /* 16-byte alignment */
    .type   start,@function
start:
    addi    sp,sp,-32 /* Give us a bit of stack space for testing the sp ops */
    
    lui     a0, %hi(tohost)
    addi    a0, a0, %lo(tohost)

    addi    a1, x0, 16 // become li a1, 16 but the assembler cannot cope with that yet
    sw      a1, 0(a0) // [tohost] = 16

    addi a2, x0, 1
    jal fwd0 // will not use this link
    j fail
    
bck2:
    addi a2, a2, 2
    jr ra
    
bck0:
    addi a2, a2, 4
    j fwd1
    j fail
    
fwd0:
    addi a2, a2, 8
    j bck0
    j fail
    
bck1:   
    addi a2, a2, 16
    jal bck2
    j done_bounce
    
fwd1:
    addi a2, a2, 7
    j bck1
    j fail

done_bounce:
    addi a1, a1, 1
    addi a3, x0, 0x26 // check we went through all the bits once
    bne a2, a3, fail

    jal fwd10
bck11: // must follow jal
    .long -1 // illegal instruction
    mv a4, ra
    addi a4, a4, 2
    jr a4
    j fail
    
fwd10:
    mv a3, ra
    addi a3, a3, 4
    jalr a3
    j fail // MUST be compressed
    
    j pass
    
    j fail
    
pass:   
    addi    a1, x0, 1
fail:
    sw      a1, 0(a0)
loop:
    j loop
