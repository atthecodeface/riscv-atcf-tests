#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // for usleep
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#include "hps_fpga.h"

static uint32_t data[] = {
    0, 0x00001137, 1, 0x00010113, 2, 0x0480006f, 3, 0xff010113,
    4, 0x00512023, 5, 0x00612223, 6, 0x00001337, 7, 0x00030313,
    8, 0x341022f3, 9, 0x00532023, 10, 0x00428293, 11, 0x34129073,
    12, 0x342022f3, 13, 0x00532223, 14, 0x304022f3, 15, 0x00532423,
    16, 0x343022f3, 17, 0x00532623, 18, 0x30200073, 19, 0x00000000,
    20, 0xd6067179, 21, 0x1800d422, 22, 0xf0000537, 23, 0xfea42823,
    24, 0xff042503, 25, 0x000205b7, 26, 0x2623952e, 27, 0x2503fea4,
    28, 0x2423fec4, 29, 0x2503fea4, 30, 0x2223fe84, 31, 0x2503fea4,
    32, 0x0513fe84, 33, 0x20230205, 34, 0x2503fea4, 35, 0x0513fe84,
    36, 0x2e230405, 37, 0x2503fca4, 38, 0x0513fe84, 39, 0x2c230605,
    40, 0x4501fca4, 41, 0xfca42823, 42, 0xfca42a23, 43, 0x0040006f,
    44, 0xfd442503, 45, 0xc863459d, 46, 0x006f02a5, 47, 0x25030040,
    48, 0x2583fe04, 49, 0x058afd44, 50, 0x25b7952e, 51, 0x85932020,
    52, 0xc10c0205, 53, 0x0040006f, 54, 0xfd442503, 55, 0x2a230505,
    56, 0xf06ffca4, 57, 0x4501fcff, 58, 0xfca42a23, 59, 0x0040006f,
    60, 0xfd442503, 61, 0x03b00593, 62, 0x02a5c363, 63, 0x0040006f,
    64, 0xfd442503, 65, 0x25830516, 66, 0xc188fdc4, 67, 0x0040006f,
    68, 0xfd442503, 69, 0x2a230505, 70, 0xf06ffca4, 71, 0x4501fd7f,
    72, 0xfca42a23, 73, 0x0040006f, 74, 0xfd442503, 75, 0xc0634581,
    76, 0x006f04a5, 77, 0x25030040, 78, 0x75b7fe04, 79, 0x85934865,
    80, 0xc10cc6c5, 81, 0xfe042503, 82, 0x6f2105b7, 83, 0xf0058593,
    84, 0x2503c14c, 85, 0x0593fdc4, 86, 0xc10c2580, 87, 0x0040006f,
    88, 0xfd442503, 89, 0x2a230505, 90, 0xf06ffca4, 91, 0x0537fbff,
    92, 0x45850000, 93, 0x04b52623, 94, 0x0040006f, 95, 0x0000006f,
    96, 0x0000ffff, 1024, 0x00000000, 1025, 0x00000000,
    1026, 0x00000000, 1027, 0x00000000, 1028, 0x00000000,
    -1, -1
};
int main() {
    void *lw_axi_base;
    lw_axi_base = hps_fpga_open();
    if (lw_axi_base==NULL) {
        exit(4);
    }
    int loop_count;
	volatile uint32_t *timer_base;
	uint32_t *dprintf_address, *dprintf_address_c;
	uint32_t *dprintf_data,  *dprintf_data_c;

	volatile uint32_t *sram_address, *sram_data;
	volatile uint32_t *sram_control, *sram_data_inc;

    // APB sel is 4;16 - if CSR then that must be 2, and CSR sel is 4;12
    timer_base        = (void *)(lw_axi_base + 0x00000);
    dprintf_address   = (void *)(lw_axi_base + 0x20000);
    dprintf_data      = (void *)(lw_axi_base + 0x20020);
    dprintf_address_c = (void *)(lw_axi_base + 0x20040);
    dprintf_data_c    = (void *)(lw_axi_base + 0x20060);

    sram_address   = (volatile uint32_t *)(lw_axi_base + 0x40000);
    sram_data      = (volatile uint32_t *)(lw_axi_base + 0x40004);
    sram_control   = (volatile uint32_t *)(lw_axi_base + 0x40008);
    sram_data_inc  = (volatile uint32_t *)(lw_axi_base + 0x4000c);

    int i, j;
    for (i=0; (data[i]>>31)==0; i+=2) {
        fprintf(stderr,"%d : %08x\n", data[i], data[i+1]);
        sram_address[0] = data[i];
        sram_data[0] = data[i+1];
    }
    sram_address[0] = 0;
    for (j=0; j<32; j++) {
        uint32_t a = sram_data_inc[0];
        uint32_t b = sram_data_inc[0];
        uint32_t c = sram_data_inc[0];
        uint32_t d = sram_data_inc[0];
        fprintf(stderr,"%04x : %08x %08x %08x %08x\n", j*16, a, b, c, d);
    }
    sram_control[0] = 1;

	loop_count = 0;
	while ( loop_count < 5 ) {
		
        if (0) {
		// control led
        uint32_t tim_val;
        tim_val = timer_base[0];
        dprintf_data[0] = 0x4e45573a;
        dprintf_data[1] = 0x20830000 | (loop_count & 0xffff);
        dprintf_data[2] = 0x20000087;
        dprintf_data[3] = tim_val;
        dprintf_data[4] = 0xff000000;
        dprintf_address_c[0] = 40*10;
        }
		// wait 100ms
		usleep( 100*1000 );
		
		// update led mask
        loop_count++;
		
	} // while
    sram_control[0] = 0;
	
    hps_fpga_close();
	return( 0 );
}
