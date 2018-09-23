
void *hps_fpga_open(void) {
    return (void *)0xf0000000;
}

void hps_fpga_close(void) {
}

extern int main(int argc, char **argv);
void start(void) {
    main(0,(void *)0);
    do {} while (1);
}
