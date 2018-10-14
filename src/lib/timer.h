#include "types.h"

__attribute__((always_inline)) inline t_uint32 rv_timer_low(void)
{
    volatile t_uint32 *csr = (t_uint32 *)(void *)(RV_TIMER_TIMER_LOWER);
    return *csr;
}
__attribute__((always_inline)) inline t_uint32 rv_timer_high(void)
{
    volatile t_uint32 *csr = (t_uint32 *)(void *)(RV_TIMER_TIMER_UPPER);
    return *csr;
}
__attribute__((always_inline)) inline t_uint32 rv_timer_cmp_low(void)
{
    volatile t_uint32 *csr = (t_uint32 *)(void *)(RV_TIMER_CMP_LOWER);
    return *csr;
}
__attribute__((always_inline)) inline t_uint32 rv_timer_cmp_high(void)
{
    volatile t_uint32 *csr = (t_uint32 *)(void *)(RV_TIMER_CMP_UPPER);
    return *csr;
}
__attribute__((always_inline)) inline void rv_timer_cmp_write(t_uint32 *cmp)
{
    volatile t_uint32 *csr_l = (t_uint32 *)(void *)(RV_TIMER_CMP_LOWER);
    volatile t_uint32 *csr_h = (t_uint32 *)(void *)(RV_TIMER_CMP_UPPER);
    *csr_h = 0xffffffff;
    *csr_l = cmp[0];
    *csr_h = cmp[1];
}

extern void rv_timer_read_timer(t_uint32 *timer);
extern void rv_timer_set_comparator_delay(t_uint32 d);
