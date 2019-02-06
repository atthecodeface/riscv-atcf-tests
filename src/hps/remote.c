// arm-linux-gnueabi-gcc -I /home/nfp/riscv-atcf-tests/inc/hps remote.c hps_fpga.c -o remote
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // for usleep
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#include "hps_fpga.h"

#include <signal.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define STATUS_BASE (20*40)
#define TIMEOUT_SEC  0
#define TIMEOUT_USEC 100000
#define DATA_MODE_COMMAND  0
#define DATA_MODE_RV_DATA  1

void dprintf4(void *lw_axi_base, int address, uint32_t d0, uint32_t d1, uint32_t d2, uint32_t d3) {
    volatile uint32_t *csrs_dprintf     = APB_CSR(lw_axi_base, APB_DPRINTF_SEL, 0);
    csrs_dprintf[DPRINTF_CSR_DATA+0] = d0;
    csrs_dprintf[DPRINTF_CSR_DATA+1] = d1;
    csrs_dprintf[DPRINTF_CSR_DATA+2] = d2;
    csrs_dprintf[DPRINTF_CSR_DATA+3] = d3;
    csrs_dprintf[DPRINTF_CSR_ADDRESS_COMMIT] = address;
}

void dprintf2(void *lw_axi_base, int address, uint32_t d0, uint32_t d1) {
    volatile uint32_t *csrs_dprintf     = APB_CSR(lw_axi_base, APB_DPRINTF_SEL, 0);
    csrs_dprintf[DPRINTF_CSR_DATA+0] = d0;
    csrs_dprintf[DPRINTF_CSR_DATA+1] = d1;
    csrs_dprintf[DPRINTF_CSR_ADDRESS_COMMIT] = address;
}

struct remote_skt {
    int skt;
    int client_skt;
    int buffer_valid;
    char buffer[8192];
    int data_mode;
    int sram_select;
    int sram_base;
    int sram_size;
    uint32_t sram_csum;
};

static void *lw_axi_base;

static struct remote_skt remote_skt;
static int remote_open(int port) {
    struct sockaddr_in addr;

    remote_skt.skt = socket(AF_INET, SOCK_STREAM, 0);
    if (remote_skt.skt<0) {return -1;}
    int reuse = 1;
    setsockopt(remote_skt.skt, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
    bzero((char *) &addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);
    if (bind(remote_skt.skt, (struct sockaddr *)&addr, sizeof(addr)) < 0) { return -2; }
    if (listen(remote_skt.skt,1)<0) { return -3; }
    return port;
}

static int remote_poll_for_new_client(void) {
    if (remote_skt.client_skt>0) return 0;
    fd_set read, write, err;
    struct timeval timeout;
    timeout.tv_sec  = TIMEOUT_SEC;
    timeout.tv_usec = TIMEOUT_USEC;
    FD_ZERO(&read);
    FD_ZERO(&write);
    FD_ZERO(&err);
    FD_SET(remote_skt.skt, &read);
    FD_SET(remote_skt.skt, &err);

    int i = select(remote_skt.skt+1, &read, &write, &err, &timeout);
    if (i<0) return -1;
    if (i==0) return 0;
    if (FD_ISSET(remote_skt.skt, &err)) return -2;
    if (!FD_ISSET(remote_skt.skt, &read)) return 0;

    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);
    remote_skt.client_skt = accept(remote_skt.skt, (struct sockaddr *)&addr, &addr_len);
    if (remote_skt.client_skt < 0) { return -3; }
    remote_skt.buffer_valid = 0;
    remote_skt.data_mode = DATA_MODE_COMMAND;
    dprintf4(lw_axi_base, STATUS_BASE+8, 0x436f6e6e, 0x20202020, 0x20202020, 0x202020ff);
    return 0;
}

static void remote_close_client(void) {
    if (remote_skt.client_skt>=0) {
        close(remote_skt.client_skt);
        remote_skt.client_skt = -1;
    }
    dprintf4(lw_axi_base, STATUS_BASE+8, 0x446f6e65, 0x20202020, 0x20202020, 0x202020ff);
}

static int remote_poll_client(void) {
    if (remote_skt.client_skt<=0) return 0;
    fd_set read, write, err;
    struct timeval timeout;
    timeout.tv_sec  = TIMEOUT_SEC;
    timeout.tv_usec = TIMEOUT_USEC;
    FD_ZERO(&read);
    FD_ZERO(&write);
    FD_ZERO(&err);
    FD_SET(remote_skt.client_skt, &read);
    FD_SET(remote_skt.client_skt, &err);

    int i = select(remote_skt.client_skt+1, &read, &write, &err, &timeout);
    if (i<0) return -1;
    if (i==0) return 0;
    if (FD_ISSET(remote_skt.client_skt, &err)) {
        close(remote_skt.client_skt);
        remote_skt.client_skt = -1;
        return 0;
    }
    if (!FD_ISSET(remote_skt.client_skt, &read)) return 0;

    int n = recv(remote_skt.client_skt, remote_skt.buffer+remote_skt.buffer_valid, sizeof(remote_skt.buffer)-remote_skt.buffer_valid, 0);
    if (n==0) {
        remote_close_client();
        return 0;
    }
    remote_skt.buffer_valid += n;
    return remote_skt.buffer_valid;
}

static int remote_add_to_send(const char *buffer, int len) {
    if (send(remote_skt.client_skt, buffer, len, 0)<0) return -1;
    return 0;
}

static int remote_handle_buffer_command(void) {
    int i;
    char buffer[64];
    if (remote_skt.buffer_valid<4) return 0;
    for (i=0; (i<remote_skt.buffer_valid) && (remote_skt.buffer[i]!='\n'); i++);
    if (i<remote_skt.buffer_valid) {
        remote_skt.buffer[i] = 0;
        if (!strncmp(remote_skt.buffer,"APBW",4)) {
            int select, reg;
	    unsigned long long int data;
            if (sscanf(remote_skt.buffer, "APBW %i %i %lli", &select, &reg, &data)==3) {
                volatile uint32_t *csr = APB_CSR(lw_axi_base, select, reg);
                fprintf(stderr, "APBW %x %x %08llx\n", select, reg, data);
                *csr = data;
                sprintf(buffer, "APBW OK\n");
                remote_add_to_send(buffer,strlen(buffer));
            }
        } else if (!strncmp(remote_skt.buffer,"APBR",4)) {
            int select, reg;
            if (sscanf(remote_skt.buffer, "APBR %i %i", &select, &reg)==2) {
                volatile uint32_t *csr = APB_CSR(lw_axi_base, select, reg);
                fprintf(stderr, "APBR %x %x\n", select, reg);
                uint32_t data;
                data = csr[0];
                fprintf(stderr, "Read %08x\n", data);
                sprintf(buffer, "APBR %08x\n",data);
                remote_add_to_send(buffer,strlen(buffer));
            }
        } else if (!strncmp(remote_skt.buffer,"SRAM",4)) {
	    int select, base, size;
	    unsigned long long int csum;
            if (sscanf(remote_skt.buffer, "SRAM %i %i %i %lli", &select, &base, &size, &csum)==4) {
                fprintf(stderr, "SRAM %d 0x%x 0x%x %08llx\n", select, base, size, csum);
                sprintf(buffer, "SRAM %d %x %x %08llx\n",select, base, size, csum);
                remote_add_to_send(buffer,strlen(buffer));
                remote_skt.data_mode = DATA_MODE_RV_DATA;
                remote_skt.sram_select = select;
                remote_skt.sram_base = base;
                remote_skt.sram_size = size;
                remote_skt.sram_csum = csum;
            }
        }
        i += 1;
        memmove(remote_skt.buffer, remote_skt.buffer+i, remote_skt.buffer_valid-i);
        remote_skt.buffer_valid -= i;
    }
    return 1;
}

static int remote_handle_buffer_sram_data(void) {
    char buffer[64];
    if (remote_skt.sram_size==0) {
        remote_skt.data_mode = DATA_MODE_COMMAND;
        if (remote_skt.sram_csum==0) {
            sprintf(buffer, "SRAM OK %d %x %x\n", remote_skt.sram_select, remote_skt.sram_base, remote_skt.sram_size);
            remote_add_to_send(buffer,strlen(buffer));
            return 1;
        } else {
            sprintf(buffer, "SRAM CSUM ERR %d %x %x %08x\n", remote_skt.sram_select, remote_skt.sram_base, remote_skt.sram_size, remote_skt.sram_csum);
            remote_add_to_send(buffer,strlen(buffer));
            return -1;
        }
    }
    if (remote_skt.buffer_valid<4) return 0;
    int i=0;
    volatile uint32_t *sram_csr   = APB_CSR(lw_axi_base, remote_skt.sram_select, 0);
    sram_csr[SRAM_CSR_ADDRESS] = remote_skt.sram_base;
    while ((remote_skt.sram_size>0) && (remote_skt.buffer_valid>=i+4)) {
        uint32_t data;
        data = remote_skt.buffer[i];
        data = (data<<8) | remote_skt.buffer[i+1];
        data = (data<<8) | remote_skt.buffer[i+2];
        data = (data<<8) | remote_skt.buffer[i+3];
        fprintf(stderr,"Write %d %08x\n", remote_skt.sram_base+i/4, data);
        sram_csr[SRAM_CSR_DATA_INC] = data;
        remote_skt.sram_csum -= data;
        remote_skt.sram_size -= 1;
        i += 4;
    }
    remote_skt.sram_base += i/4;
    memmove(remote_skt.buffer, remote_skt.buffer+i, remote_skt.buffer_valid-i);
    remote_skt.buffer_valid -= i;
    return 1;
}

// return -1 for error, 0 for could do nothing, 1 for did something so might do more
static int remote_handle_buffer(void) {
    if (remote_skt.data_mode==0) return remote_handle_buffer_command();
    else return remote_handle_buffer_sram_data();
}

static int remote_poll(void) {
    int i = remote_poll_for_new_client();
    if (i<0) return -2;
    do {
        i = remote_handle_buffer();
        if (i<0) {
            remote_close_client();
        }
    } while (i>0);
    i = remote_poll_client();
    if (i<0) return -1;
    return 0;
}

static void close_down(void) {
    dprintf4(lw_axi_base, STATUS_BASE, 0x20202020, 0x20202020, 0x20202020, 0x202020ff);
    dprintf4(lw_axi_base, STATUS_BASE+15, 0x20202020, 0x20202020, 0x20202020, 0x202020ff);
    hps_fpga_close();
    fprintf(stderr, "Remote completed\n");
}

static void sig_handler(int s) {
    exit(4);
}

int main() {
    lw_axi_base = hps_fpga_open();
    if (lw_axi_base==NULL) {
        exit(4);
    }
    signal(SIGINT, sig_handler);
    signal(SIGHUP, sig_handler);
    signal(SIGTERM, sig_handler);
    atexit(close_down);
    dprintf4(lw_axi_base, STATUS_BASE, 0x52656d6f, 0x74653a02, 0xffffffff, 0xffffffff);
    int i = remote_open(1234);
    if (i<0) {
        dprintf4(lw_axi_base, STATUS_BASE+7, 0x01455252, 0x4f5220c0, (((-i)&0xff)<<24) | 0xff0000, 0xffffffff);
        usleep(1000*1000);
        exit(4);
    }
    dprintf4(lw_axi_base, STATUS_BASE+8, 0x4f70656e, 0x20c10000 | ((i&0xffff)<<0), 0xffffffff, 0xffffffff);
    while (remote_poll()>=0);
    // close client
    // retry if server socket is still open
    exit (0);
}
