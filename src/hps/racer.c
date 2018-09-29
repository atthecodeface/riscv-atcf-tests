#include "types.h"
#include "ps2.h"
#include "teletext.h"
#include "hps_fpga.h"
#include "dprintf.h"
#include "timer.h"

struct obstacle {
    int valid;
    int lx;
    int rx;
    int ty;
    int by;
};
#define MAX_OBSTACLE 8
#define MAN_Y 5
static struct obstacle obstacles[MAX_OBSTACLE];
void racer(void *lw_axi_base) {
    ps2_set_divider(lw_axi_base, 150); // for 50MHz system
    tt_init(lw_axi_base);
    int x = 20;
    int i;
    int alive=1;
    int num_obstacles=0;
    for (i=0; i<MAX_OBSTACLE; i++) {
        obstacles[i].valid = 0;
    }
    while (1) {
        int o_to_add;
        uint32_t et, t;
        et = timer_get(lw_axi_base) + 0x200000; // 20ns * 1024 * 1024 * 2
        do {
            ps2_poll(lw_axi_base);
            t = timer_get(lw_axi_base);
        } while ((t-et) & 0x80000000);
        tt_scroll_up(lw_axi_base);
        tt_set_pos(lw_axi_base,x,MAN_Y);
        tt_write_char(lw_axi_base, alive?64:45);
        if (ps2_key_pressed(65)) x--;
        if (ps2_key_pressed(66)) x++;
        if (x<1) x=1;
        if (x>40) x=40;
        o_to_add = -1;
        for (i=0; i<MAX_OBSTACLE; i++) {
            if (obstacles[i].valid) {
                obstacles[i].ty--;
                obstacles[i].by--;
                if ((obstacles[i].ty<=MAN_Y) && (obstacles[i].by>=MAN_Y)) {
                    if ((obstacles[i].lx<=x) && (obstacles[i].rx>=x)) {
                        alive=0;
                    }
                }
                if (obstacles[i].by<0) {
                    obstacles[i].valid = 0;
                }
            } else {
                o_to_add = i;
            }
        }
        if ((et & 0x3f)!=0) { o_to_add=-1; }
        if (o_to_add>=0) {
            int j;
            struct obstacle *ptr = obstacles+o_to_add;
            j = (et>>7) & 0x1f;
            ptr->valid = 1;
            ptr->lx = j;
            ptr->rx = j+5;
            ptr->by = 48;
            ptr->ty = 44;
            for (i=0; i<5; i++) {
                tt_set_pos(lw_axi_base,ptr->lx,ptr->ty+i);
                for (j=0; j<6; j++) {
                    tt_write_char(lw_axi_base,35);
                }
            }
        }
    }
}

int main(int argc, char **argv) {
    void *lw_axi_base;
    lw_axi_base = hps_fpga_open();
    if (lw_axi_base==NULL) {
        return 4;
    }

    racer(lw_axi_base);
	
    hps_fpga_close();
	return 0;
}
