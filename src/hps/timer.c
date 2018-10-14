#include "types.h"
#include "timer.h"
#include "hps_fpga.h"

uint32_t timer_get(void *lw_axi_base)
{
    volatile uint32_t *csrs_timer     = APB_CSR(lw_axi_base, APB_TIMER_SEL, 0);
    uint32_t timer;
    timer = (csrs_timer[TIMER_CSR_TIMER] << 1);
    return timer;
}
