#include "types.h"
#include "hps_fpga.h"

void dprintf4(void *lw_axi_base, int address, uint32_t d0, uint32_t d1, uint32_t d2, uint32_t d3) {
    volatile uint32_t *csrs_dprintf     = APB_CSR(lw_axi_base, APB_DPRINTF_SEL, 0);
    csrs_dprintf[DPRINTF_CSR_DATA+0] = d0;
    csrs_dprintf[DPRINTF_CSR_DATA+1] = d1;
    csrs_dprintf[DPRINTF_CSR_DATA+2] = d2;
    csrs_dprintf[DPRINTF_CSR_DATA+3] = d3;
    csrs_dprintf[DPRINTF_CSR_ADDRESS_COMMIT] = address;
}
