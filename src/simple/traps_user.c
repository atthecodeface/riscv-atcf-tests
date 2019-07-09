#include "lib/types.h"

RV_TEST_CODE

struct trap_data {
    t_uint32 mepc;
    t_uint32 mcause;
    t_uint32 mstatus;
    t_uint32 mtval;
};

extern void record_trap(void);
extern struct trap_data trap_data;

static void
check_trap(t_uint32 mstatus, t_uint32 mcause, t_uint32 mtval)
{
    t_uint32 temp;
    __asm volatile("csrr %0, 0x340" : "=r"(temp) );
    if ((trap_data.mepc-temp)!=8) {
        RV_TEST_FAIL(temp-trap_data.mepc+256);
    }
    // In a machine-mode only RV, mprv, mxr, sum, tvm, tw, tsr, fs, xs, sd, spp, spie, sie, upie, uie are all 0
    // only mie, mpie and mpp may be set - and mpp will be 3 since it came from user mode
    if (trap_data.mstatus!=mstatus) { // might be 128 if interrupts used to be enabled
        RV_TEST_FAIL(2);
    }
    if (trap_data.mcause!=mcause) { // currently seems to be 0
        RV_TEST_FAIL(3);
    }
    if (trap_data.mtval!=mtval) { // must be 0 (by spec) for ecall
        RV_TEST_FAIL(4);
    }
}

extern int start(void) {
    t_uint32 temp, temp2;

    RV_TEST_INIT;
    temp=0;
    __asm volatile("csrc mstatus, %1; auipc %0, 0; addi %0, %0, 16; csrw mepc, %0; mret" : "=r"(temp2):"r"(3<<11) );

    temp = (t_uint32) record_trap;
    // Set mtvec to point to ecall
    __asm volatile("csrw mtvec, %0" : : "r"(temp) );

    // ecall : Makes mcause=8,9,11 (env call U, S, M). Sets mepc to address of break. Branches to mtvec. Sets mpp to current mode (user), sets mpie to mie, sets mie to 0
    __asm volatile("auipc %0, 0; csrw mscratch, %0; ecall" : "=r"(temp2) );

    check_trap(3<<11, 8, 0);

    // ebreak : note c.ebreak i32c version too. Makes mcause=3 (breakpoint). Sets mepc to address of break. Branches to mtvec
    __asm volatile("auipc %0, 0; csrw 0x340, %0; ebreak" : "=r"(temp2) );
    check_trap(3<<11, 3, temp2+8);

    // Illegal instruction. Makes mcause=2. Sets mepc to address of break. Branches to mtvec. Sets mtval to the instruction.
    __asm volatile("auipc %0, 0; csrw 0x340, %0; .long 0x0" : "=r"(temp2) );
    check_trap(3<<11, 2, 0);

    // mret is a branch to mepc; sets mie to mpie; sets mode to mpp; mpie is set to 1; mpp is set to u (or m if u not supported)
    RV_TEST_PASS(reason);
}
