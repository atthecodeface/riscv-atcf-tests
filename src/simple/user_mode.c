#include "lib/types.h"
#include "lib/rv.h"

RV_TEST_CODE
extern volatile t_uint32 trap_data[];


// Bugs...
//  accesses of machine mode CSRs do not fault
//  ret in user mode faults (should it? have we got user mode interrupts enabled?)
//  exception handler does not start in machine mode (from user mode)
//  Two fixes that Netronome have suggested?
// interrupts break the debugger
extern void count_trap(void);
extern int start(void) {
    RV_TEST_INIT;

    RV_INSTALL_TRAP_COUNT_HANDLER;
    RV_ENTER_USER_MODE;

    t_uint32 temp;
    __asm volatile("rdcycle %0" : "=r"(temp));
    __asm volatile("csrr %0, mstatus" : "=r"(temp));
    __asm volatile("rdcycle %0" : "=r"(temp));
    //RV_ENTER_MACHINE_MODE;
    {
        int i;
        for (i=0; i<16; i++) {
            t_uint32 temp;
            temp = trap_data[i];
            if (temp!=0) {
                RV_TEST_FAIL(256+i);
            }
        }
    }
    RV_TEST_PASS(reason);
}
