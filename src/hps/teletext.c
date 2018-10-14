#include "types.h"
#include "teletext.h"
#include "hps_fpga.h"

#if 1
#define DISP_WIDTH  50
#define DISP_HEIGHT 48
#else
#define DISP_WIDTH  85
#define DISP_HEIGHT 76
#endif
#define FB_SRAM_SIZE 16384
#define DISP_BOTTOM_LINE_OFS  ((DISP_HEIGHT-1) * DISP_WIDTH)

// For 50x48 on VGA (i.e. 600 x 480 pixels)
// each character is 12 pixels wide and 10 pixels high
// This is 40=0x28 fewer pixels horizontally
// DISPLAY SIZE 600 x 400 (0x1400 = 0x01df0257) (originally 0x01df027f)
// Horizontal             (0x1401 = 0x0037008f) (originally 0x000f008f)
// Vertical               (0x1402 = 0x00090022) (originally same)

// For 85x76 on XGA (i.e. 1020 x 760 pixels)
// each character is 12 pixels wide and 10 pixels high
// This is 4 fewer pixels horizontally
//  XGA is 1024+24+136+160=1344  768+ 3+6+29 = 806
//  using  1020+24+140+160=1344  760+ 7+6+33 = 806
// DISPLAY SIZE 1020 x 760 (0x1400 = 0x02f703fb)
// Horizontal              (0x1401 = 0x0017012b)
// Vertical                (0x1402 = 0x00060026)
struct tt_display {
    int screen_base;
    int vpos;
    int hpos;
};

static struct tt_display tt_display;
void tt_init(void *lw_axi_base) {
    volatile uint32_t *csrs_vga_display  = CSR_CSR(lw_axi_base, CSR_VGA_TELETEXT_SEL, 0);
    volatile uint32_t *csrs_vga_timing   = CSR_CSR(lw_axi_base, CSR_VGA_TIMING_SEL, 0);
#if 1
    csrs_vga_timing[FB_TIMING_CSR_SIZE]  = ((480-1)<<16) | ((600-1)<<0);
    csrs_vga_timing[FB_TIMING_CSR_HORIZ] = ( (56-1)<<16) | ((144-1)<<0);
    csrs_vga_timing[FB_TIMING_CSR_VERT]  = ( (10-1)<<16) | ( (35-1)<<0);
#else
    csrs_vga_timing[FB_TIMING_CSR_SIZE]  = ((760-1)<<16) | ((1020-1)<<0);
    csrs_vga_timing[FB_TIMING_CSR_HORIZ] = ( (24-1)<<16) | (( 300-1)<<0);
    csrs_vga_timing[FB_TIMING_CSR_VERT]  = ( ( 7-1)<<16) | ( ( 39-1)<<0);
#endif
    csrs_vga_display[TELETEXT_CSR_BASE] = 0;
    csrs_vga_display[TELETEXT_CSR_WORDS_PER_LINE] = DISP_WIDTH;
    tt_cls(lw_axi_base);
}

static void blank_line(volatile uint32_t *csrs_fb_vga) {
    int j;
    for (j=0; j<DISP_WIDTH; j++) {
        csrs_fb_vga[SRAM_CSR_DATA_INC] = (j==0)?'>':((j==DISP_WIDTH-1)?'<':32);
    }
}

void tt_cls(void *lw_axi_base) {
    volatile uint32_t *csrs_fb_vga       = APB_CSR(lw_axi_base, APB_FB_VGA_SEL, 0);
    int i;
    csrs_fb_vga[SRAM_CSR_ADDRESS] = 0;
    for (i=0; i<DISP_HEIGHT; i++) {
        blank_line(csrs_fb_vga);
    }

    tt_display.screen_base = 0;
    tt_display.vpos = 0;
    tt_display.hpos = 0;

}

void tt_scroll_up(void *lw_axi_base) {
    volatile uint32_t *csrs_fb_vga       = APB_CSR(lw_axi_base, APB_FB_VGA_SEL, 0);
    volatile uint32_t *csrs_vga_display  = CSR_CSR(lw_axi_base, CSR_VGA_TELETEXT_SEL, 0);

    tt_display.screen_base = (tt_display.screen_base + DISP_WIDTH) % FB_SRAM_SIZE;
    csrs_vga_display[TELETEXT_CSR_BASE] = tt_display.screen_base;
    csrs_fb_vga[SRAM_CSR_ADDRESS] = tt_display.screen_base + DISP_BOTTOM_LINE_OFS;
    blank_line(csrs_fb_vga);
}

void tt_scroll_down(void *lw_axi_base) {
    volatile uint32_t *csrs_fb_vga       = APB_CSR(lw_axi_base, APB_FB_VGA_SEL, 0);
    volatile uint32_t *csrs_vga_display  = CSR_CSR(lw_axi_base, CSR_VGA_TELETEXT_SEL, 0);

    tt_display.screen_base = (tt_display.screen_base + FB_SRAM_SIZE - DISP_WIDTH) % FB_SRAM_SIZE;
    csrs_vga_display[TELETEXT_CSR_BASE] = tt_display.screen_base;
    csrs_fb_vga[SRAM_CSR_ADDRESS] = tt_display.screen_base;
    blank_line(csrs_fb_vga);
}

void tt_up(void *lw_axi_base) {
    if (tt_display.vpos == 0) {
        tt_scroll_down(lw_axi_base);
        tt_display.vpos = 0;
    } else {
        tt_display.vpos -= 1;
    }
}

void tt_set_pos(void *lw_axi_base, int hpos, int vpos) {
    tt_display.hpos = hpos;
    tt_display.vpos = vpos;
}

void tt_down(void *lw_axi_base) {
    if (tt_display.vpos >= DISP_HEIGHT-1) {
        tt_scroll_up(lw_axi_base);
        tt_display.vpos = DISP_HEIGHT-1;
    } else {
        tt_display.vpos += 1;
    }
}

void tt_next_line(void *lw_axi_base) {
    tt_down(lw_axi_base);
    tt_display.hpos = 0;
}

void tt_fwd(void *lw_axi_base) {
    if (tt_display.hpos>=DISP_WIDTH-1) {
        tt_next_line(lw_axi_base);
    } else {
        tt_display.hpos += 1;
    }
}

void tt_write_char(void *lw_axi_base, int ch) {
    volatile uint32_t *csrs_fb_vga       = APB_CSR(lw_axi_base, APB_FB_VGA_SEL, 0);
    csrs_fb_vga[SRAM_CSR_ADDRESS] = tt_display.screen_base + (tt_display.vpos*DISP_WIDTH) + tt_display.hpos;
    csrs_fb_vga[SRAM_CSR_DATA_INC] = ch;
    tt_fwd(lw_axi_base);
}
