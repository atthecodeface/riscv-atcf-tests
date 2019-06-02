#include "lib/types.h"
#include "lib/atcf_bitmanip.h"

RV_TEST_CODE

static inline void set_scratch_csr(t_uint32 a)
{
    __asm volatile("csrw 0x340, %0" : : "r"(a));
}

// Tested
// swap_halves           : ABCD -> CDAB
// swap_half_bytes       : ABCD -> BADC
// reverse_bytes         : ABCD -> DCBA
// reverse_bits          : ABCD -> dcba
// reverse_bits_in_bytes : ABCD -> abcd
// swap based on reg

static volatile t_uint32 output;
static const unsigned char rev4[16]  = {0,8,4,12,2,10,6,14,1,9,5,13,3,11,7,15};
static const unsigned char rev4h[16] = {0,0x80,0x40,0xc0,0x20,0xa0,0x60,0xe0,0x10,0x90,0x50,0xd0,0x30,0xb0,0x70,0xf0};
static t_uint32 reverse8(t_uint32 a) {
    return rev4h[a&0xf] | rev4[a>>4];
}
static int test_swap_reg(t_uint32 d) {
    t_uint32 d0 = (d>> 0) & 0xff;
    t_uint32 d1 = (d>> 8) & 0xff;
    t_uint32 d2 = (d>>16) & 0xff;
    t_uint32 d3 = (d>>24) & 0xff;

    t_uint32 a, b, c;
    a = atcf_bm_rori(d,16);
    b = atcf_bm_swap_halves(d);
    if ((b^a)!=0) fail(11);

    a = atcf_bm_reverse_bytes(d);
    a ^= d3;
    a = atcf_bm_rori(a,8);
    a ^= d2;
    a = atcf_bm_rori(a,8);
    a ^= d1;
    a = atcf_bm_rori(a,8);
    a ^= d0;
    if (a!=0) fail(12);

    a = atcf_bm_rori(d,16);
    b = atcf_bm_swap_half_bytes(d);
    c = atcf_bm_reverse_bytes(b);
    if ((c^a)!=0) fail(13);

    t_uint32 rd0 = reverse8(d0);
    t_uint32 rd1 = reverse8(d1);
    t_uint32 rd2 = reverse8(d2);
    t_uint32 rd3 = reverse8(d3);
    a = atcf_bm_reverse_bits(d);
    a ^= rd3;
    a = atcf_bm_rori(a,8);
    a ^= rd2;
    a = atcf_bm_rori(a,8);
    a ^= rd1;
    a = atcf_bm_rori(a,8);
    a ^= rd0;
    if (a!=0) fail(14);

    a = atcf_bm_reverse_bits_in_bytes(d);
    a ^= rd0;
    a = atcf_bm_rori(a,8);
    a ^= rd1;
    a = atcf_bm_rori(a,8);
    a ^= rd2;
    a = atcf_bm_rori(a,8);
    a ^= rd3;
    if (a!=0) fail(15);

    for (int i=0; i<32; i++) {
        a = atcf_bm_swap(d,i);
        if (i&1) {a=atcf_bm_reverse_bits_in_bytes(a);}
        if (i&8) {a=atcf_bm_swap_half_bytes(a);}
        if (i&16) {a=atcf_bm_swap_halves(a);}
        if (a!=d) fail(100+i);
    }
    return d;
}

static t_uint32 constants[] = { 0x12345678, 0xfeedbeef, 0xcafef00d, 0xaa55aa55, 0x12481248 };
extern int start(void) {
    RV_TEST_INIT;

    for (int j=0; j<5; j++) {
        set_scratch_csr(j);
        t_uint32 k = test_swap_reg(constants[j]);
        set_scratch_csr(k);
    }

    RV_TEST_PASS(reason);
}
