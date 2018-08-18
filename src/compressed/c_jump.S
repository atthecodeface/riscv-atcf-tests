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
    jal fwd0 // will not use this link; fwd0 -> bck0 -> fwd1 -> bck1 [calls bck2] -> done_bounce
    j fail
    
bck2:
    addi a2, a2, 2
    jr ra
    
bck0: // bck0 -> fwd1
    addi a2, a2, 4
    j fwd1
    j fail
    
fwd0: // fwd0 -> bck0
    addi a2, a2, 8
    j bck0
    j fail
    
bck1: // bck1 -> bck2 -> returns
    addi a2, a2, 16
    jal bck2
    j done_bounce
    
fwd1: // fwd2 -> bck1
    addi a2, a2, 7
    j bck1
    j fail

done_bounce: // check it bounced around properly
    addi a1, a1, 1
    addi a3, x0, 0x26 // check we went through all the bits once
    bne a2, a3, fail

    jal fwd10 // -> sub fwd10, which jumps to link+4 -> after long -> fwd10 call + 2
bck11: // must follow jal
    .long -1 // illegal instruction, 4 bytes long
    mv a4, ra
    addi a4, a4, 2
    jr a4
    j fail
    
fwd10: // -> callee + 4
    mv a3, ra
    addi a3, a3, 4
    jalr a3
    j fail // MUST be compressed as this is skipped; bck11 returns to ra+2

    j test_j_bits
    
    j fail
    
pass:   
    addi    a1, x0, 1
fail:
    sw      a1, 0(a0)
loop:
    j loop

test_j_bits_complete:
    addi a1, a1, 1
    bne a3, x0, fail
    addi a2, a2, -11 // currently tests 11 bits of j immediate!
    bne a2, x0, fail
    j pass
    
test_j_bits: // attempt to test jump immediate encode
    addi a2, x0, 0
    addi a3, x0, 0
    j bit_2 // +00000000010x (i.e. +4)
    addi a3, a3, 1 // MUST not execute this
bit_2:  // should be +4 on
    addi a2, a2, 1
    j bits_12 // +00000000011x (i.e. +6)
    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this
bits_12:  // should be +6 on
    addi a2, a2, 1
    j bit_3 // +00000000100x + 8
    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this
bit_3:
    addi a2, a2, 1
    j bit_4 // +00000001000x + 16
    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this
bit_4:
    addi a2, a2, 1
    j bit_5 // +00000010000x + 32
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
    j bit_6 // +00000100000x + 64
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
    j bit_7 // +00001000000x + 128
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
    addi a2, a2, 1
    j bit_8 // +00010000000x + 256
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
    addi a3, a3, 1 // MUST not execute this

bit_8:
    addi a2, a2, 1
    j bit_9 // +00100000000x + 512
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
    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this
    addi a3, a3, 1 // MUST not execute this

bit_9:
    addi a2, a2, 3
    j test_j_bits_complete // tests bit 10 as bit 11 is set (backward) and bit 10 clear and all others are independent
        