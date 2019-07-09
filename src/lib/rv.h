#define RV_ENTER_USER_MODE {int temp = 0x3<<11; __asm volatile("csrc mstatus, %0" : : "r"(temp) ); __asm volatile("auipc %0, 0; addi %0, %0, 16; csrw mepc, %0 ; mret" : "=r"(temp)); }

// This following MUST NOT WORK - it is provided for negative testing
// The way in to machine mode from user mode is through an ecall... :-)
#define RV_ENTER_MACHINE_MODE {int temp = 0x0<<11; __asm volatile("csrc mstatus, %0" : : "r"(temp) ); __asm volatile("auipc %0, 0; addi %0, %0, 16; csrw mepc, %0 ; ret" : "=r"(temp)); }

#define RV_INSTALL_TRAP_RECORD_HANDLER { t_uint32 temp = (t_uint32) record_trap; __asm volatile("csrw mtvec, %0" : : "r"(temp) ); }
#define RV_INSTALL_TRAP_COUNT_HANDLER  { t_uint32 temp = (t_uint32) count_trap;  __asm volatile("csrw mtvec, %0" : : "r"(temp) ); }


