    .globl start
RV_TEST_ASM_CODE

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
RV_TEST_ASM_INIT
    addi    sp,sp,-32 /* Give us a bit of stack space for testing the sp ops */
    
    RV_TEST_ASM_PROGRESS(a0)

    addi    a1, x0, 16 // become li a1, 16 but the assembler cannot cope with that yet
    sw      a1, 0(a0) // [progress] = 16

    beqz a1, fail
    bnez a1, conditions_working
    j fail

conditions_working: 
    addi a1, a1, 1

    j test_b_bits
    
    j fail
    
pass:   
    RV_TEST_ASM_PASS
fail:
    add a0, x0, a1
    RV_TEST_ASM_FAIL


test_b_bits_complete:
    addi a1, a1, 1
    bne a3, x0, fail
    addi a2, a2, -8 // currently tests 8 bits of b immediate!
    bne a2, x0, fail
    j pass
    
test_b_bits: // attempt to test jump immediate encode
    addi a2, x0, 0
    addi a3, x0, 0
    beqz a3, bit_2 // +00000000010x (i.e. +4)
    addi a3, a3, 1 // MUST not execute this
bit_2:  // should be +4 on
    addi a2, a2, 1
    beqz a3, bits_12 // +00000000011x (i.e. +6)
    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this
bits_12:  // should be +6 on
    addi a2, a2, 1
    beqz a3, bit_3 // +00000000100x + 8
    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this
bit_3:
    addi a2, a2, 1
    beqz a3, bit_4 // +00000001000x + 16
    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this
bit_4:
    addi a2, a2, 1
    beqz a3, bit_5 // +00000010000x + 32
    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this
bit_5:
    addi a2, a2, 1
    beqz a3, bit_6 // +00000100000x + 64
    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this

    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this
bit_6:
    addi a2, a2, 1
    beqz a3, bit_7 // +00001000000x + 128
    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this

    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this

    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this

    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this

bit_7:
    addi a2, a2, 2
    j test_b_bits_complete // tests bit 8 set (backward) - all others have been tested one at a time and are independent

    
