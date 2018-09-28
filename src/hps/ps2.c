#include "types.h"
#include "ps2.h"
#include "hps_fpga.h"
#include "dprintf.h"

#define PS2_KBD_MAP_SIZE 94
const char ps2_kbd_map[PS2_KBD_MAP_SIZE] = "\n\n\n\n\n\n\n\n\n\n\n\n\n\n`\n\n\n\n\n\nQ1\n\n\nZSAW2\n\nCXDE43\n\n VFTR5\n\nNBHGY6\n\n\nMJU78\n\n,KIO09\n\n./L;P-\n\n\n'\n[=\n\n\n\n\n]\n\\";
//0x111,"Alt (right)",
//0x114,"Ctrl (right)",
//0x11F,"Windows (left)",
//0x127,"Windows (right)",
//0x12F,"Menus",
//0x14A,"KP/",
//0x15A,"KPEnter",
//0x169,"End",
//0x16B,"Left Arrow",
//0x16C,"Home",
//0x170,"Insert",
//0x171,"Delete",
//0x172,"Down Arrow",
//0x174,"Right Arrow",
//0x175,"Up Arrow",
//0x17A,"Page Down",
//0x17D,"Page Up",

//0x01,"F9",
//0x03,"F5",
//0x04,"F3",
//0x05,"F1",
//0x06,"F2",
//0x07,"F12",
//0x09,"F10",
//0x0A,"F8",
//0x0B,"F6",
//0x0C,"F4",
//0x0D,"Tab",
//0x11,"Alt (left)",
//0x12,"Shift",
//0x14,"Ctrl",

//0x58,"Caps Lock",
//0x59,"RtShift",
//0x5A,"Enter",
//0x66,"Backspace",
//0x69,"KP1",
//0x6B,"KP4",
//0x6C,"KP7",
//0x70,"KP0",
//0x71,"KP.",
//0x72,"KP2",
//0x73,"KP5",
//0x74,"KP6",
//0x75,"KP8",
//0x76,"ESC",
//0x77,"Num Lock",
//0x78,"F11",
//0x79,"KP+",
//0x7A,"KP3",
//0x7B,"KP-",
//0x7C,"KP*",
//0x7D,"KP9",
//0x7E,"Scroll Lock",
//0x83,"F7",


static int key_prefixes;
static int ps2_poll_ps2_host(void *lw_axi_base, int *key_number) {
   volatile uint32_t *csrs_ps2_host     = APB_CSR(lw_axi_base, APB_PS2_HOST_SEL, 0);
   int status;
   status = csrs_ps2_host[PS2_HOST_CSR_STATE];
   if ((status & 0x10)==0) {
       int data = csrs_ps2_host[PS2_HOST_CSR_FIFO];
       if (data==0xf0) {
           key_prefixes |= 2;
       } else if (data==0xe0) {
           key_prefixes |= 1;
       } else {
           data = (data & 0xff) | (key_prefixes<<8);
           if ((data&0x1ff) < PS2_KBD_MAP_SIZE) {
               int key = ps2_kbd_map[data&0x1ff];
               data = (data&0xfe00) | key;
           } else {
               data = (data&0xfe00) | 0x0d;
           }
           *key_number = data;
           key_prefixes = 0;
           return 2;
       }
       return 1;
   }
   return 0;
}

#define KBD_BUFFER_SIZE 8
struct keys {
    uint32_t down[8]; // 256-bits
    int      wptr;
    int      rptr;
    int      nkeys;
    char     buffer[KBD_BUFFER_SIZE];
};

static struct keys keys;

static void key_buffer(int key) {
    if (keys.nkeys<KBD_BUFFER_SIZE) {
        keys.buffer[keys.wptr] = key;
        keys.wptr += 1;
        keys.nkeys += 1;
        if (keys.wptr==KBD_BUFFER_SIZE) keys.wptr=0;
    }
}

static int key_read(void) {
    if (keys.nkeys==0) { return 0; }
    int key = keys.buffer[keys.rptr];
    keys.rptr += 1;
    keys.nkeys -= 1;
    if (keys.rptr==KBD_BUFFER_SIZE) keys.rptr=0;
    return key;
}

int ps2_poll(void *lw_axi_base) {
    int key;
    int reason = ps2_poll_ps2_host(lw_axi_base, &key);
    if (reason==0) return 0;
    int key_index = (key>>5)&7;
    int key_bit   = 1<<(key&0x1f);
    if (reason==2) {
        if (key & 0x200) {
            keys.down[key_index] &= ~key_bit;
        } else {
            keys.down[key_index] |= key_bit;
            key_buffer(key);
        }
    }
    return 1;
}

int ps2_key_pressed(int k) {
    return (keys.down[(k>>5)&7]>>(k&0x1f))!=0;
}

int ps2_next_key(void) {
    return key_read();
}

void ps2_set_divider(void *lw_axi_base, int n) {
   volatile uint32_t *csrs_ps2_host     = APB_CSR(lw_axi_base, APB_PS2_HOST_SEL, 0);
   csrs_ps2_host[PS2_HOST_CSR_STATE] = n<<16;
}

