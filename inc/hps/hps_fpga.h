#define APB_TIMER_SEL     0
#define APB_GPIO_SEL      1
#define APB_DPRINTF_SEL   2
#define APB_CSR_SEL       3
#define APB_RV_SRAM_SEL   4
#define APB_LED_CHAIN_SEL 5
#define APB_INPUTS_SEL    6
#define APB_FB_VGA_SEL    7
#define APB_PS2_HOST_SEL  8

#define CSR_LCD_TELETEXT_SEL  2
#define CSR_LCD_TIMING_SEL    3
#define CSR_VGA_TELETEXT_SEL  4
#define CSR_VGA_TIMING_SEL    5

#define TELETEXT_CSR_BASE            0
#define TELETEXT_CSR_WORDS_PER_LINE  1

#define FB_TIMING_CSR_SIZE        0
#define FB_TIMING_CSR_HORIZ       1
#define FB_TIMING_CSR_VERT        2

#define DPRINTF_CSR_ADDRESS        0
#define DPRINTF_CSR_DATA           8
#define DPRINTF_CSR_ADDRESS_COMMIT 16
#define DPRINTF_CSR_DATA_COMMIT    24

#define PS2_HOST_CSR_STATE 0
#define PS2_HOST_CSR_FIFO  1

#define LED_CHAIN_CSR_CONFIG 0
#define LED_CHAIN_CSR_LEDS   16

#define DE1_CL_INPUTS_CSR_STATE 0
#define DE1_CL_INPUTS_CSR_DIALS 1

#define SRAM_CSR_ADDRESS     0
#define SRAM_CSR_DATA        1
#define SRAM_CSR_CONTROL     2
#define SRAM_CSR_DATA_INC    3
#define SRAM_CSR_DATA_WINDOW 128

#define APB_CSR(lw_axi_base,sel,reg) ( (volatile uint32_t *)((lw_axi_base) + ((sel)<<16) + ((reg)<<2)) )
#define CSR_CSR(lw_axi_base,sel,reg) ( (volatile uint32_t *)((lw_axi_base) + ((APB_CSR_SEL)<<16) + ((sel)<<12) + ((reg)<<2)) )

extern void *hps_fpga_open(void);
extern void hps_fpga_close(void);
