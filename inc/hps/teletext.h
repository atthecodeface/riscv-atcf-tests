/*f tt_*
 *
 *
 */
extern void tt_init(void *lw_axi_base);
extern void tt_cls(void *lw_axi_base);
extern void tt_set_pos(void *lw_axi_base, int hpos, int vpos);
extern void tt_scroll_up(void *lw_axi_base);
extern void tt_scroll_down(void *lw_axi_base);
extern void tt_up(void *lw_axi_base);
extern void tt_down(void *lw_axi_base);
extern void tt_next_line(void *lw_axi_base);
extern void tt_fwd(void *lw_axi_base);
extern void tt_write_char(void *lw_axi_base, int ch);


