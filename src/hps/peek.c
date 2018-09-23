#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // for usleep
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#include "hps_fpga.h"

int main(int argc, char **argv) {
    if (argc<3) {
        fprintf(stderr, "Usage: %s <sel> <reg>\n", argv[0]);
        exit(4);
    }
    uint32_t sel = strtol(argv[1],NULL,0);
    uint32_t reg = strtol(argv[2],NULL,0);
    printf("Sel 0x%x reg 0x%x\n", sel, reg);

    void *lw_axi_base;
    lw_axi_base = hps_fpga_open();
    if (lw_axi_base==NULL) {
        exit(4);
    }

    // APB sel is 4;16 - if CSR then that must be 2, and CSR sel is 4;12
    uint32_t *csr = (uint32_t *)(lw_axi_base + (sel<<16) + (reg<<2));
    printf("Value: 0x%08x\n", csr[0]);
	
    hps_fpga_close();
	return( 0 );
}
