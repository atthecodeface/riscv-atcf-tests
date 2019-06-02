#include "types.h"

// O4=51 is op
// O4=19 is op_imm
// f3=1 for sll class, 5 for srl class
// f7[4;3] = 0 for sll / srl
// f7[4;3] = 4 for sll / sra
// f7[4;3] = 8 for sllo / srlo
// f7[4;3] = 9 for rol / ro
// f7[4;3] = 10 for bit/byte/half swap / count
// f7[4;3] = 12 for insert byte (byte in f7[3;0])
#define ATCF_BM_R(F3,F7,a,b,c) __asm volatile(".insn r 51," #F3 "," #F7 ",%0,%1,%2": "=r"(a):"r"(b), "r"(c) );
#define ATCF_BM_SLL(a,b,c)  ATCF_BM_R(1,0,a,b,c)
#define ATCF_BM_SRL(a,b,c)  ATCF_BM_R(5,0,a,b,c)
#define ATCF_BM_SRA(a,b,c)  ATCF_BM_R(5,32,a,b,c)
#define ATCF_BM_SLLO(a,b,c) ATCF_BM_R(1,64,a,b,c)
#define ATCF_BM_SRLO(a,b,c) ATCF_BM_R(5,64,a,b,c)
#define ATCF_BM_ROL(a,b,c)  ATCF_BM_R(1,72,a,b,c)
#define ATCF_BM_ROR(a,b,c)  ATCF_BM_R(5,72,a,b,c)

// works:
// #define ATCF_BM_I(F3,F2,a,b,C) __asm volatile(".insn i 19," #F3 ",%0,%1," #C: "=r"(a):"r"(b) );
// The c+F7 only works with gcc if -O1 is specified; this is because constant folding MUST be performed
#define ATCF_BM_I(F3,F7,a,b,c) __asm volatile(".insn i 19," #F3 ",%0,%1,%2": "=r"(a):"r"(b),"i"(c | ((F7)<<5)) );
// There is no way to do this without -O1 AFAIK
//#define ATCF_BM_I(F3,F7,a,b,c) __asm volatile(".insn i 19," #F3 ",%0,%1,(%2|" #F7 ")": "=r"(a):"r"(b),"i"(c) );
#define ATCF_BM_I_SLL(a,b,c)  ATCF_BM_I(1,0,a,b,c)
#define ATCF_BM_I_SRL(a,b,c)  ATCF_BM_I(5,0,a,b,c)
#define ATCF_BM_I_SRA(a,b,c)  ATCF_BM_I(5,32,a,b,c)
#define ATCF_BM_I_SLLO(a,b,c) ATCF_BM_I(1,64-128,a,b,c)
#define ATCF_BM_I_SRLO(a,b,c) ATCF_BM_I(5,64-128,a,b,c)
#define ATCF_BM_I_ROL(a,b,c)  ATCF_BM_I(1,72-128,a,b,c)
#define ATCF_BM_I_ROR(a,b,c)  ATCF_BM_I(5,72-128,a,b,c)

// Bit/byte/half-word swap - sll with funct7 of 7b0100000 (shift[4]/[3]/[0] for half/byte/bit swap)
#define ATCF_BM_SWAP(a,b,c)                ATCF_BM_R(1,80,a,b,c)
#define ATCF_BM_SWAP_HALVES(a,b)           ATCF_BM_I(1,80-128,a,b,16)
#define ATCF_BM_SWAP_HALF_BYTES(a,b)       ATCF_BM_I(1,80-128,a,b,8)
#define ATCF_BM_REVERSE_BYTES(a,b)         ATCF_BM_I(1,80-128,a,b,24)
#define ATCF_BM_REVERSE_BITS(a,b)          ATCF_BM_I(1,80-128,a,b,25)
#define ATCF_BM_REVERSE_BITS_IN_BYTES(a,b) ATCF_BM_I(1,80-128,a,b,1)
// Replace byte - sll with funct7 of 7b11000bb
#define ATCF_BM_BYTE_REPLACE(a,b,c,byte)   ATCF_BM_R(1,96+byte,a,b,c)
#define ATCF_BM_COUNT_LEADING(a,b,c)       ATCF_BM_R(5,80,a,b,c)

#define force_inline __attribute__((always_inline)) inline
static force_inline t_uint32 atcf_bm_sll(t_uint32 val, int by) { t_uint32 r;ATCF_BM_SLL(r,val,by); return r; }
static force_inline t_uint32 atcf_bm_srl(t_uint32 val, int by) { t_uint32 r;ATCF_BM_SRL(r,val,by); return r; }
static force_inline t_uint32 atcf_bm_sra(t_uint32 val, int by) { t_uint32 r;ATCF_BM_SRA(r,val,by); return r; }
static force_inline t_uint32 atcf_bm_rol(t_uint32 val, int by) { t_uint32 r;ATCF_BM_ROL(r,val,by); return r; }
static force_inline t_uint32 atcf_bm_ror(t_uint32 val, int by) { t_uint32 r;ATCF_BM_ROR(r,val,by); return r; }
static force_inline t_uint32 atcf_bm_sllo(t_uint32 val, int by) { t_uint32 r;ATCF_BM_SLLO(r,val,by); return r; }
static force_inline t_uint32 atcf_bm_srlo(t_uint32 val, int by) { t_uint32 r;ATCF_BM_SRLO(r,val,by); return r; }

static force_inline t_uint32 atcf_bm_swap(t_uint32 val, int by)    { t_uint32 r;ATCF_BM_SWAP(r,val,by); return r; }
static force_inline t_uint32 atcf_bm_swap_halves(t_uint32 val)     { t_uint32 r;ATCF_BM_SWAP_HALVES(r,val); return r; }
static force_inline t_uint32 atcf_bm_reverse_bytes(t_uint32 val)   { t_uint32 r;ATCF_BM_REVERSE_BYTES(r,val); return r; }
static force_inline t_uint32 atcf_bm_swap_half_bytes(t_uint32 val) { t_uint32 r;ATCF_BM_SWAP_HALF_BYTES(r,val); return r; }
static force_inline t_uint32 atcf_bm_reverse_bits(t_uint32 val)    { t_uint32 r;ATCF_BM_REVERSE_BITS(r,val); return r; }
static force_inline t_uint32 atcf_bm_reverse_bits_in_bytes(t_uint32 val)    { t_uint32 r;ATCF_BM_REVERSE_BITS_IN_BYTES(r,val); return r; }
//static force_inline t_uint32 atcf_bm_byte_replace(t_uint32 val, int by) { t_uint32 r;ATCF_BM_BYTE_REPLACE(r,val,by); return r; }

static force_inline t_uint32 atcf_bm_slli(t_uint32 val, int by) { t_uint32 r;ATCF_BM_I_SLL(r,val,by); return r; }
static force_inline t_uint32 atcf_bm_srli(t_uint32 val, int by) { t_uint32 r;ATCF_BM_I_SRL(r,val,by); return r; }
static force_inline t_uint32 atcf_bm_srai(t_uint32 val, int by) { t_uint32 r;ATCF_BM_I_SRA(r,val,by); return r; }
static force_inline t_uint32 atcf_bm_roli(t_uint32 val, int by) { t_uint32 r;ATCF_BM_I_ROL(r,val,by); return r; }
static force_inline t_uint32 atcf_bm_rori(t_uint32 val, int by) { t_uint32 r;ATCF_BM_I_ROR(r,val,by); return r; }
static force_inline t_uint32 atcf_bm_slloi(t_uint32 val, int by) { t_uint32 r;ATCF_BM_I_SLLO(r,val,by); return r; }
static force_inline t_uint32 atcf_bm_srloi(t_uint32 val, int by) { t_uint32 r;ATCF_BM_I_SRLO(r,val,by); return r; }
