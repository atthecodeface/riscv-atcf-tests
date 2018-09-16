#include "lib/types.h"
RV_TEST_CODE

extern int start(void) {
    t_uint32 csr_base   = (0xf0000000);
    t_uint32 dprintf_base  = csr_base + (2<<16);
    volatile t_uint32 *dprintf_csr_base    = (t_uint32 *)dprintf_base;
    volatile t_uint32 *dprintf_csr_address   = dprintf_csr_base + 0;
    volatile t_uint32 *dprintf_csr_data      = dprintf_csr_base + 8;
    volatile t_uint32 *dprintf_csr_address_c = dprintf_csr_base + 16;
    volatile t_uint32 *dprintf_csr_data_c    = dprintf_csr_base + 24;
    int i;
    int j=0;
    RV_TEST_INIT;
    for (i=0; i<8; i++) {
        dprintf_csr_data[i] = 0x20202020;
    }
    for (i=0; i<60; i++) {
        dprintf_csr_address_c[0] = i*32;
    }
    for (i=0; i<1; i++) {
        dprintf_csr_data[0] = 0x48656c6c;
        dprintf_csr_data[1] = 0x6f20ff00;
        dprintf_csr_address_c[0] = 600;
    }
    RV_TEST_PASS(1);
}
