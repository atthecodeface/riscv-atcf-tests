#include "lib/types.h"
#include "lib/atcf_bitmanip.h"

RV_TEST_CODE

static inline void set_scratch_csr(t_uint32 a)
{
    __asm volatile("csrw 0x340, %0" : : "r"(a));
}

static volatile t_uint32 output;
static int test_shift_rotate_reg(t_uint32 d, int n) {
    int mn = 32-n;
    t_uint32 a, b, c;
    a = atcf_bm_sll(d,n);
    b = atcf_bm_srl(a,n); // top n bits of b should be clear, rest are d
    a = atcf_bm_srl(d,mn);
    c = atcf_bm_sll(a,mn); // top n bits of c should be d, rest are zero
    set_scratch_csr(c^b^d);
    if ((c^b^d)!=0) fail(11);

    a = atcf_bm_sll(d,n);
    b = atcf_bm_srlo(a,n); // top n bits of b should be set, rest are d
    a = atcf_bm_srl(d,mn);
    c = atcf_bm_sllo(a,mn); // top n bits of c should be d, rest are set
    if ((c^b^d)!=~0) fail(12);

    a = atcf_bm_sll(d,n);
    b = atcf_bm_srl(d,mn);
    c = atcf_bm_rol(d,n);
    if ((c^b^a)!=0) fail(3);
    c = atcf_bm_ror(d,mn);
    if ((c^b^a)!=0) fail(13);

    a = atcf_bm_srl(d,n);
    b = atcf_bm_sra(d,n);
    c = atcf_bm_srlo(0,n); // top n bits of c are set, rest 0
    if (d&(1<<31)) b^=c;
    if (b!=a) fail(14);
    return n;
}

static t_uint32 constants[] = { 0x12345678, 0xfeedbeef, 0xcafef00d, 0xaa55aa55, 0x12481248 };
extern int start(void) {
    RV_TEST_INIT;

    for (int j=0; j<5; j++) {
        for (int i=1; i<32; i++) {
            set_scratch_csr(j);
            t_uint32 k = test_shift_rotate_reg(constants[j], i);
            set_scratch_csr(k);
        }
    }

    RV_TEST_PASS(reason);
}
