    .globl start
    .globl tohost

    /* Tested:

    addi      (sign extended, -32 to +31) ; = addi rd, rd, imm
    li        (sign extended, -32 to +31) ; = addi rd, x0, imm
    lw        (zero-extended 0 to 124 in steps of 4) ;  lw rs2', imm(rs1') 
    sw        (zero-extended 0 to 124 in steps of 4) ;  sw rs2', imm(rs1') 
    beqz rs, label
    bnez rs, label
    sub rd', rd', rs2'
    xor rd', rd', rs2'
    add rd, rd, rs2 (note: all registers)
    mv  rd, rs2 (note: all registers)
    or  rd', rd', rs2'
    and rd', rd', rs2'
    slli rd', rd', 1-31

    Still to test:

    nop
    lui       (sign extended, -128k to +127k in steps of 1k) ; = lui rd, imm (rd!=2)
    srli
    srai
    andi
    ebreak

    registers:
    0-7:    zero, ra, sp, gp, tp, t0, t1, t2
    8-15:   s0, s1, a0-a5
    16-23:  a6, a7, s2-7
    24-31:  s8-s11, t3-t6
    */
    .p2align        4 /* 16-byte alignment */
    .type   start,@function
start:
    addi    sp,sp,-32 /* Give us a bit of stack space for testing the sp ops */
    
    lui     a0, %hi(tohost)
    addi    a0, a0, %lo(tohost)

    addi    a1, x0, 16 // become li a1, 16 but the assembler cannot cope with that yet
    sw      a1, 0(a0) // [tohost] = 16

    addi    a2, x0, 1
    slli    a2, a2, 4 // a2 = 16

    bne     a1, a2, fail

    addi    a1, a1, 1 // a1 = 17
    sub     a2, a2, a1    // a2 = -1
    beqz    a2, fail

    addi    a2, a2, 1 // a2 = 0
    addi    a1, a1, 1 // a1 = 18
    bnez    a2, fail

    addi    a1, a1, 1 // a1 = 19
    lw      a3, 0(a0) // a3 = 16, read from tohost
    xor     a3, a3, a1 // a3 = 3
    addi    a3, a3, -3 // a3 = 0
    bnez    a3, fail

    addi    a1, a1, 1 // a1 = 20
    addi    t0, x0, 0x7fc // uncompressed
    add     a3, a3, t0 // a3 = 0x7fc (compressed)
    mv      a4, a3     // a4 = 0x7fc
    addi    a3, a3, 4  // a3 = 0x800
    or      a3, a3, a4 // a3 = 0xffc
    addi    a3, a3, 4  // a3 = 0x1000
    addi    a4, x0, 1  // a4 = 1
    slli    a4, a4, 12 // a4 = 0x1000
    addi    a5, x0, 0  // a5 = 0
    addi    a5, a5, -1 // a5 = -1
    sub     a5, a5, a4 // a5 = 0xffffefff
    mv      a2, a3     // a2 = a3 (test value)
    and     a2, a2, a5 // a2 = 0
    bnez    a2, fail

    addi    a1, a1, 1 // a1 = 21
    or      a3, a3, a5 // a3 = 0xffffffff
    addi    a3, a3, 1
    bnez    a3, fail

    addi    a1, a1, 1 // a1 = 22
    addi    a2, x0, 1
    addi    a3, x0, -1
    add     a2, a2, a3
    bnez    a2, fail
    
    addi    a1, a1, 1 // a1 = 23
    addi    a2, x0, 1
    addi    a3, x0, 2
    mv      a2, a3
    bne     a2, a3, fail
    
pass:   
    addi    a1, x0, 1
fail:
    sw      a1, 0(a0)
loop:
    j loop
