#include "types.h"
#include "timer.h"
void rv_timer_read_timer(t_uint32 *timer)
{
    t_uint32 high;
    high = rv_timer_high();
    do {
        timer[1] = high;
        timer[0] = rv_timer_low();
        high = rv_timer_high();
    } while (high!=timer[1]);
}

void rv_timer_set_comparator_delay(t_uint32 d) {
    t_uint32 timer[2];
    rv_timer_read_timer(timer);
    if (d+timer[0] < d) {
        timer[1] += 1;
    }
    timer[0] += d;
    rv_timer_cmp_write(timer);
}
