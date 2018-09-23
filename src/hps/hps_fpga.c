#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

// LW fpga mapped to 1MB (?) at 0xff200000 by HPS by default
#define REGION_BASE  0xfc000000
#define REGION_MASK  0x03ffffff
#define LW_FPGA      0x03200000

struct hps_fpga {
	int fd;
    void *virtual_base;
    void *lw_axi_base;
};
static struct hps_fpga hps_fpga;
void *hps_fpga_open(void) {
    hps_fpga.fd = open("/dev/mem", (O_RDWR | O_SYNC));
	if (hps_fpga.fd<0) {
		fprintf( stderr, "Failed to open /dev/mem\n");
        return NULL;
	}

	hps_fpga.virtual_base = mmap(NULL,
                                 REGION_MASK+1, // size
                                 (PROT_READ | PROT_WRITE),
                                 MAP_SHARED,
                                 hps_fpga.fd,
                                 REGION_BASE); // offset from start of memory

	if (hps_fpga.virtual_base == MAP_FAILED) {
		fprintf(stderr, "Failed to mmap\n");
		close(hps_fpga.fd);
        hps_fpga.fd = -1;
		return NULL;
	}

    hps_fpga.lw_axi_base = (void *)(((uint32_t)hps_fpga.virtual_base) + LW_FPGA);
	
    //fprintf(stderr, "virtual_base  %08x\n", (uint32_t)hps_fpga.virtual_base );
    //fprintf(stderr, "lw_axi_base  %08x\n", (uint32_t)hps_fpga.lw_axi_base );
    return hps_fpga.lw_axi_base;
}

void hps_fpga_close(void) {
    if (hps_fpga.fd<=0) return;
	munmap(hps_fpga.virtual_base, REGION_MASK+1);
    close(hps_fpga.fd);
    hps_fpga.fd = -1;
}
