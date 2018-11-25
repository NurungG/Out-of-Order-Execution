#ifndef __OOO_H__
#define __OOO_H__

#define OOO_EXIT_SUCCESS 0
#define OOO_EXIT_FAILURE 1

typedef enum {
    false, true
} bool;

typedef enum {
    IntAlu, MemRead, MemWrite
} inst;


struct fetch_queue {
    inst type;
    int  dest;
    int  src1;
    int  src2;
    int  addr;
};

struct reservation_station {
    int  eu_idx;
    bool is_busy;
    inst type;
    // v1, v2 is ommitted
    int  q1;
    int  q2;
};

struct reg_alias_tbl {
    int  rob_idx;
    bool is_valid;
};

struct execution_unit {
    int time_left;
};

struct reorder_buffer {
    inst type;
    // value is ommited
    int  dest;
    bool state;
};

int ooo_create(int dump, int width, int rob_sz, int rs_sz);
int ooo_destroy();
int ooo_do_cycle();

#endif
