#include "types.h"

extern
t_uint32 mix32(t_uint32 x)
{
    // https://eprint.iacr.org/2011/218.pdf gives an sbox of 03586CB7A49EF12D
    // this means 4-bit input 0 maps to 0, 1 maps to 3, 2 maps to 5, and so on
    // We require 0 to map to non-zero. Hence swap high and low and reverse.
    //t_uint32 sbox_hi = 0x03586CB7;
    //t_uint32 sbox_lo = 0xA49EF12D;
#define sbox_hi 0x03586CB7
#define sbox_lo 0xA49EF12D

    t_uint32 y=0;
    for (int i=0; i<8; i++, x>>=1) {
        //int j = ((x>>0)&1) | ((x>>7)&2) | ((x>>14)&4); // ((x>>21)&8);
        int j = (x&0x010101);
        j = ((j>>14) | (j>>7) | j) & 7;
        y <<= 4;
        y |= (((x&(1<<24)) ? sbox_hi : sbox_lo) >> (j<<2)) & 0xf;
    }
    return y;
/*    .text
    .file    "fred.c"
    .globl    blob                    # -- Begin function blob
    .p2align    2
    .type         blob,@function

# 22 * 7 + 27 = 181
mix32:
          li    a2,8
        lui    a3,0x1000 # a3 = 1<<24
          lui       a6,0x10
        addi    a6,a6,257 # a6 = 0x10101
          li    a1,0 # y (result)
loop:
        and        a4,a0,a3  # a4 = x & (1<<24)
        lui        a7,0xa49ef
        addi    a7,a7,301 # 0xa49ef12d
        beqz    a4, lower_half
        lui        a7,0x3587
        addi    a7,a7,-841 # 0x3586cb7
lower_half:
        and    a4,a0,a6       # j = x & 0x010101
        srli    a5,a4,0x7  # a5 = (x&010100)>>7
        or    a5,a5,a4
        srli    a4,a4,0xe
        or    a4,a5,a4
          slli    a4,a4,0x2
        andi    a4,a4,28 # a4 = shift for nybble
        srl    a7,a7,a4       # a7 = nybble a4
          slli    a1,a1,0x4
        andi    a7,a7,15
        or    a1,a7,a1        # update result
          srai    a0,a0,0x1   # x >>= 1
          addi    a2,a2,-1
          bnez    a2,loop
          mv    a0,a1
          ret

blob:                                   # @blob
# %bb.0:                                # %entry
    addi    sp, sp, -16
    sw    ra, 12(sp)
    sw    s0, 8(sp)
    addi    s0, sp, 16
    lui    a0, %hi(fred)
    addi    a0, a0, %lo(fred)
    sw    a0, -12(s0)
    lw    a0, -12(s0)
    call    deref
    lw    s0, 8(sp)
    lw    ra, 12(sp)
    addi    sp, sp, 16
          ret
*/

}
