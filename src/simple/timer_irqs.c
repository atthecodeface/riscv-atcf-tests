#include "lib/types.h"
#include "lib/timer.h"

RV_TEST_CODE

struct trap_data {
    t_uint32 mepc;
    t_uint32 mcause;
    t_uint32 mstatus;
    t_uint32 mtval;
};

extern void int_callback(void);
extern struct trap_data trap_data;

static volatile int interrupt_count=0;
static void
check_int(t_uint32 mepc, t_uint32 mstatus, t_uint32 mcause, t_uint32 mtval)
{
    rv_timer_set_comparator_delay(100);
    interrupt_count ++;
}

extern int start(void) {
    t_uint32 temp, temp2;

    RV_TEST_INIT;
    
    trap_data.mepc = (t_uint32) (void *)check_int;
    // Set mtvec to point to ecall
    temp = (t_uint32) int_callback;
    __asm volatile("csrw 0x305, %0" : : "r"(temp) );

    rv_timer_set_comparator_delay(100);

    // Enable machine mode timer interrupt in MIE
    temp = 128;
    __asm volatile("csrrw x0, 0x304, %0" : : "r"(temp));

    // Enable machine mode interrupts in MSTATUS
    __asm volatile("csrrwi x0, 0x300, 8" : : );
    volatile int *interrupt_count_ptr = &interrupt_count;
    do {
        temp = interrupt_count;
        __asm volatile("csrw 0x340, %0" : : "r"(temp));
    } while (*interrupt_count_ptr<10);

    // Disable machine mode interrupts in MSTATUS
    __asm volatile("csrrwi x0, 0x300, 0" : : );

    RV_TEST_PASS(reason);
}
