#include "lib/types.h"
RV_TEST_CODE

extern int start(void) {
    t_uint32 csr_base   = (0xf0000000);
    t_uint32 timer_base = csr_base + ((0<<12)<<2);
    t_uint32 sram_base  = csr_base + ((1<<12)<<2);
    volatile t_uint32 *sram_csr_base    = (t_uint32 *)sram_base;
    volatile t_uint32 *sram_csr_address = sram_csr_base + 0;
    volatile t_uint32 *sram_csr_data    = sram_csr_base + 1;
    volatile t_uint32 *sram_csr_control = sram_csr_base + 2;
    volatile t_uint32 *sram_csr_data_inc= sram_csr_base + 3;
    volatile t_uint32 *sram_csr_data_window = sram_csr_base + 128;
    int i;
    int j=0;
    RV_TEST_INIT;
    for (i=0; i<4; i++) {
        sram_csr_address[0] = 0;
        t_uint32 x;
        x = sram_csr_data[0];
        x = sram_csr_data[0];
        x = sram_csr_data[0];
        x = sram_csr_data_inc[0];
        x = sram_csr_data_inc[0];
        x = sram_csr_data_window[0];
        x = sram_csr_data_window[33];
        sram_csr_data_window[0] = j;
        j += 0x12345678;
    }
    RV_TEST_PASS(1);
}
