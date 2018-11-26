#include <stdio.h>

#include "ooo.h"

int main(int argc, char *argv[]) {
    FILE *fp;
    int in_dump, in_width, in_rob_sz, in_rs_sz;
    char tr_inst[16];
    int tr_dest, tr_src1, tr_src2, tr_addr;

    // argv[1]: config
    // argv[2]: trace

    fp = fopen(argv[1], "r");
    fscanf(fp, "%d %d %d %d", &in_dump, &in_width, &in_rob_sz, &in_rs_sz);
    fclose(fp);

    ooo_create(in_dump, in_width, in_rob_sz, in_rs_sz);

    fp = fopen(argv[2], "r");
	while (1) {
		int is_exit = ooo_do_cycle(fp);
		if (is_exit) break;
	}
	fclose(fp);

    ooo_destroy();

    return 0;
}
