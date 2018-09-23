#include "types.h"
#include "riscv.h"
#include "hps_fpga.h"

void rv_stop(void *lw_axi_base) {
    volatile uint32_t *csrs_rv_sram     = APB_CSR(lw_axi_base, APB_RV_SRAM_SEL, 0);
    csrs_rv_sram[SRAM_CSR_CONTROL] = 0;
}

void rv_reset(void *lw_axi_base) {
    volatile uint32_t *csrs_rv_sram     = APB_CSR(lw_axi_base, APB_RV_SRAM_SEL, 0);
    csrs_rv_sram[SRAM_CSR_CONTROL] = 0;
}

void rv_start(void *lw_axi_base) {
    volatile uint32_t *csrs_rv_sram     = APB_CSR(lw_axi_base, APB_RV_SRAM_SEL, 0);
    csrs_rv_sram[SRAM_CSR_CONTROL] = 1;
}

void rv_load_sram(void *lw_axi_base, int base, int size, uint32_t *src) {
    volatile uint32_t *csrs_rv_sram     = APB_CSR(lw_axi_base, APB_RV_SRAM_SEL, 0);
    int i;
    csrs_rv_sram[SRAM_CSR_ADDRESS] = base;
    for (i=0; i<size; i++) {
        csrs_rv_sram[SRAM_CSR_DATA_INC] = src[i];
    }
}
