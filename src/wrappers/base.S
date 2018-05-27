    .section ".start"
        .p2align        2
        .type   _reset,@function
_reset:
    lui     x2, %hi(_stack)
    addi    x2, x2, %lo(_stack)
    j       start

    .p2align        2
   /* .type   tohost,@function*/
    .globl tohost
tohost:
    .long 0
    
    .section ".stack_end"
    .globl _stack
_stack: 
    
/*.Lfunc_end0:
        .size   blob, .Lfunc_end0-blob
*/

/*        .ident  "clang version 7.0.0 (trunk 331359) (llvm/trunk 331357)"
        .section        ".note.GNU-stack","",@progbits
    */