    .globl start
RV_TEST_ASM_CODE

    /* Tested in c_arith:

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

    registers:
    0-7:    zero, ra, sp, gp, tp, t0, t1, t2
    8-15:   s0, s1, a0-a5
    16-23:  a6, a7, s2-7
    24-31:  s8-s11, t3-t6


    Tested here (briefly):
    
    addi4spn  (zero-extended, 4 to 1020 in steps of 4) ; addi rd', x2, imm
    addi16sp  (sign extended, -512 to +496 in steps of 16)   ; = addi x2, x2, imm
    lwsp      (zero-extended, 0 to 252 in steps of 4) ; lw rd', imm(x2)
    swsp      (zero-extended, 0 to 252 in steps of 4) ; sw rs2', imm(x2)
    */
    .p2align        4 /* 16-byte alignment */
    .type   start,@function
start:
RV_TEST_ASM_INIT

    addi    sp,sp,-32 /* Give us a bit of stack space for testing the sp ops */
    
    RV_TEST_ASM_PROGRESS(a0)

    addi    a1, x0, 16 // become li a1, 16 but the assembler cannot cope with that yet
    sw      a1, 0(a0) // [progress] = 16

    mv      t0, sp
    addi    a3, x0, 0x7ca // not compressed
    sw      a3, 16(sp) // compressed we hope
    lw      t6, 16(t0) // not compressed
    bne     a3, t6, fail

    addi a1, a1, 1

    addi    a3, a3, 0x13 // a3 = 0x7dd
    sw      a3, 12(t0) // not compressed
    lw      a2, 12(sp) // compressed
    bne     a3, a2, fail

    addi a1, a1, 1

    addi    sp, sp, -16
    addi    a3, a3, 0x13 // a3 = 0x7f0
    sw      a3, 12(t0) // not compressed
    lw      a2, 28(sp) // compressed
    bne     a3, a2, fail

    addi a1, a1, 1

    addi    a4, sp, 28 // fec if sp started at 0x1000
    addi    a3, a3, 0x13 // a3 = 0x803
    sw      a3, 12(t0) // not compressed
    lw      a2, 0(a4) // compressed
    bne     a3, a2, fail

    j pass
    
    j fail
    
pass:   
    RV_TEST_ASM_PASS
fail:
    add a0, x0, a1
    RV_TEST_ASM_FAIL
