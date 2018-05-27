#!/usr/bin/env python
def mix32(x):
    """ https://eprint.iacr.org/2011/218.pdf gives an sbox of 03586CB7A49EF12D
       this means 4-bit input 0 maps to 0, 1 maps to 3, 2 maps to 5, and so on
       We require 0 to map to non-zero. Hence swap high and low and reverse.

       The C code is:

       t_uint32 sbox_hi = 0x03586CB7;
       t_uint32 sbox_lo = 0xA49EF12D;

       t_uint32 y=0;
       for (int i=0; i<8; i++, x>>=1) {
         //int j = ((x>>0)&1) | ((x>>7)&2) | ((x>>14)&4); // ((x>>21)&8);
        int j = (x&0x010101);
         j = ((j>>14) | (j>>7) | j) & 7;
         y <<= 4;
        y |= (((x&(1<<24)) ? sbox_hi : sbox_lo) >> (j<<2)) & 0xf;
       }
       return y;
       """
    #print("Mix %08x"%x)
    y=0
    sbox = [0, 3, 5, 8, 6, 0xC, 0xB, 7, 0xA, 4, 9, 0xE, 0xF, 1, 2, 0xD]
    sbox.reverse()
    for i in range(8):
        index  = x&1
        index += ((x>>8)&1)<<1
        index += ((x>>16)&1)<<2
        index += ((x>>24)&1)<<3
        x >>= 1
        y = y << 4
        y = y + sbox[index]
        pass
    #print("is %08x"%y)
    return y

def logic0(x):
    #print("Logic0 with x 0x%08x"%x)
    for i in range(4):
        nx = mix32(x)
        temp = 1         # lui temp,0; addi temp, temp, 1
        temp = temp + nx # add temp, temp, nx
        temp = temp & 0xffffffff
        temp = temp ^  x # xor temp, x, temp
        temp = temp & 0xffffffff
        temp = temp +  x # add temp, temp, x
        temp = temp & 0xffffffff
        temp = temp & nx # and temp, temp, nx
        temp = temp & 0xffffffff
        temp = temp |  x # or  temp, temp, x
        temp = temp & 0xffffffff
        x    = temp - nx # sub x, temp, nx
        x = x & 0xffffffff
        print("Iteration %d mixed was %08x output %08x (temp %08x)"%(i, nx, x, temp))
        pass
    return x

for i in range(4):
    print (i, "%08x"%logic0(i))

