/*f poll_keys
 *
 * Return 0 for no PS2 data, 1 for incomplete key, 2 for a complete key with *key_number updated
 *
 */
extern int ps2_poll(void *lw_axi_base);
extern int ps2_key_pressed(int k);
extern int ps2_next_key(void);

