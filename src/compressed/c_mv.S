    .globl start
RV_TEST_ASM_CODE

    .p2align        4 /* 16-byte alignment */
    .type   start,@function
start:
RV_TEST_ASM_INIT

    j over

test_me:    
    lw	x12,0(x10)
    ret // jr ra

over:   
    lui	x10,0x1
    lui	x11,0x1234
    sw	x11,0(x10)
    lui	x11,0x1111
    jal	test_me
    nop
    nop
    nop
    mv	x11,x12

    // in pipeline we get x11==x12 == 0x01234000
    // in pipeline3 we get x11!=x12
    j check_values

    nop
    
check_values:   
    bne x11, x12, fail

pass:   
    RV_TEST_ASM_PASS
fail:
    add a0, x0, a1
    RV_TEST_ASM_FAIL
