#define RV_TEST_INIT {}
#define RV_TEST_PASS(reason) {tohost = 1;while (1) {};}
#define RV_TEST_FAIL(reason) {fail(reason);}
#define RV_TEST_CODE int tohost __attribute__((section(".start"),aligned(8))); int fromhost __attribute__((section(".start"),aligned(8))); static void fail(int reason) { tohost = reason; while (1) {}}
#define RV_TIMER_BASE 0x10000000
#define RV_TIMER_TIMER_LOWER (RV_TIMER_BASE | 0)
#define RV_TIMER_TIMER_UPPER (RV_TIMER_BASE | 4)
#define RV_TIMER_CMP_LOWER (RV_TIMER_BASE | 8)
#define RV_TIMER_CMP_UPPER (RV_TIMER_BASE | 12)

// For assembler code, single-threaded, use a single location 'tohost' for
// progress, pass/fail, etc.
// When using progress, the user MUST NOT store 0 or 1 (incomplete / pass)
// If a multithreaded implementation of test is used then a per-thread tohost can be generated,
// and a per-thread progress could be used that may be separate from tohost
#define RV_TEST_ASM_CODE \
    .section ".start" ; \
    .p2align        3 ; \
    .globl tohost ; \
tohost: ; \
    .long 0 ; \
    .p2align        3 ; \
    .globl fromhost ; \
fromhost: ; \
    .long 0 ; \
rv_test_pass: addi a0, x0, 1           ;        \
rv_test_fail: lui     a1, %hi(tohost)  ;        \
    addi    a1, a1, %lo(tohost)        ;        \
    sw      a0, 0(a1)                  ;        \
rv_test_loop:                          \
    j rv_test_loop

#define RV_TEST_ASM_INIT
#define RV_TEST_ASM_PROGRESS(r)               \
    lui     r, %hi(tohost)  ;                \
    addi    r, r, %lo(tohost)

#define RV_TEST_ASM_FAIL j rv_test_fail
#define RV_TEST_ASM_PASS j rv_test_pass


