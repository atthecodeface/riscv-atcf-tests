    .section ".start"
        .p2align        2
        .type   _reset,@function
_reset:
    lui     x2, %hi(_stack)
    addi    x2, x2, %lo(_stack)
    j       start

    .section ".stack"

    .section ".stack_end"
    .globl _stack
_stack: 
    
