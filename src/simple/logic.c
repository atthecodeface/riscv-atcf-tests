#include "lib/mix.h"

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

static t_uint32 logic0_results[4] = {
    0xffffffff,
    0x3f54015f,
    0xbbfc06e4,
    0x8a53fd4d};

extern int start(void) {
    int i;
    for (i=0; i<4; i++) {
        t_uint32 x=logic0(i);
        if (x != logic0_results[i]) {
            while (1) {} // FAIL
        }
    }
    while (1) {} // PASS
}
