#include "lib/mix.h"

RV_TEST_CODE

static int
logic0(t_uint32 x)
{
    t_uint32 nx, temp;
    int i;
    for (i=0; i<4; i++) {
        nx = mix32(x);
        __asm volatile("lui %0, 0" : "=r"(temp) );
        __asm volatile("addi %0, %1, 1" : "=r"(temp) : "r"(temp) );
        __asm volatile("add %0, %1, %2" : "=r"(temp) : "r"(temp), "r"(nx) );
        __asm volatile("xor %0, %1, %2" : "=r"(temp) : "r"(x), "r"(temp)  );
        __asm volatile("add %0, %1, %2" : "=r"(temp) : "r"(temp), "r"(x)  );
        __asm volatile("and %0, %1, %2" : "=r"(temp) : "r"(temp), "r"(nx) );
        __asm volatile("or  %0, %1, %2" : "=r"(temp) : "r"(temp), "r"(x)  );
        __asm volatile("sub %0, %1, %2" : "=r"(x)    : "r"(temp), "r"(nx) );
    }
    return x;
}

static t_uint32 logic0_results[5] = {
    0xffffffff,
    0x3f54015f,
    0xbbfc06e4,
    0x8a53fd4d,
    0xbad00bad}; // end marker as LLVM CS 04538fb309782ee0225a41ee58e1774fcaaaab20 does not output the last 16-bits

extern int start(void) {
    int i;
    RV_TEST_INIT;
    for (i=0; i<4; i++) {
        t_uint32 x=logic0(i);
        if (x != logic0_results[i]) {
            RV_TEST_FAIL(10);
        }
    }
    RV_TEST_PASS(1);
}
