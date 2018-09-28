#include "types.h"
#include "ps2.h"
#include "teletext.h"
#include "hps_fpga.h"
#include "dprintf.h"

static const char *hex="0123456789abcdef";
void terminal(void *lw_axi_base) {
    ps2_set_divider(lw_axi_base, 150); // for 50MHz system
    tt_init(lw_axi_base);
    while (1) {
        ps2_poll(lw_axi_base);
        int key;
        key = ps2_next_key();
        if (key!=0) {
            if ((key>=32) && (key<=127)) {
                tt_write_char(lw_axi_base,key);
            }
        }
    }
}

int main(int argc, char **argv) {
    void *lw_axi_base;
    lw_axi_base = hps_fpga_open();
    if (lw_axi_base==NULL) {
        return 4;
    }

    terminal(lw_axi_base);
	
    hps_fpga_close();
	return 0;
}
