#ifndef __OOO_H__
#define __OOO_H__

#define OOO_EXIT_SUCCESS 0
#define OOO_EXIT_FAILURE 1

#define REG_MAX 16

typedef enum {
    false, true
} bool;

typedef enum {
    IntAlu, MemRead, MemWrite
} inst;


struct fetch_node {
    inst type;
    int  dest;
    int  src1;
    int  src2;
	// addr is omitted

	int  pc;
	bool alloc;
};

struct rsv_stn {
	int  rob_idx;
    int  eu_idx;
    inst type;
    // v1, v2 are omitted
    int  q1;
    int  q2;

	int pc;
	bool alloc;
};

struct reg_alias_tbl {
    int  rob_idx;
    bool rf_valid;
};

struct execution_unit {
    int time_left;
	int rs_idx;
};

struct rob_node {
    // op type, value are omited
    int  dest;
    bool state;

	bool alloc;
};

int ooo_create(int dump, int width, int rob_sz, int rs_sz);
int ooo_destroy();
int ooo_do_cycle();

#endif
