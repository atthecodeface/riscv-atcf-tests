extern void rv_stop(void *lw_axi_base);
extern void rv_reset(void *lw_axi_base);
extern void rv_start(void *lw_axi_base);
extern void rv_load_sram(void *lw_axi_base, int base, int size, uint32_t *src);

