#include "ooo.h"

int dump;
int width;
int rob_sz;
int rs_sz;

bool stop_flag;

struct fetch_queue {
	int max;
	int idx;
	int size;
	struct fetch_node *arr;
} fq;

struct rsv_stn *rs;
struct reg_alias_tbl *rat;
struct execution_unit *eu;

struct rsv_stn_queue {
	int max;
	int idx;
	int size;
	struct rsv_stn **arr;
} rs_free, rs_pending, rs_executing;

struct rob_queue {
	int max;
	int idx;
	int size;
	struct rob_node *arr;
} rob;

int *ordered_arr;

int ooo_create(int _dump, int _width, int _rob_sz, int _rs_sz) {

	/** Variable init */
	dump   = _dump;
	width  = _width;
	rob_sz = _rob_sz;
	rs_sz  = _rs_sz;

	/** Module init */

	// Fetch queue
	fq.max  = 2 * width;
	fq.idx  = 0;
	fq.size = 0;
	fq.arr  = (struct fetch_node *)calloc(fq.max, sizeof(struct fetch_node));
	for (int i = 0; i < fq.max; i++) fq.arr[i] = NULL;

	// Reservation statation
	rs = (struct rsv_stn *)calloc(rs_sz, sizeof(struct rsv_stn));
	rs_free.max  = rs_sz;
	rs_free.idx  = 0;
	rs_free.size = rs_sz;
	rs_free.arr  = (struct rsv_stn **)calloc(rs_sz, sizeof(struct srv_stn *));
	for (int i = 0; i < rs_free.max; i++) rs_free.arr[i] = &rs[i];
	rs_pending.max  = rs_sz;
	rs_pending.idx  = 0;
	rs_pending.size = 0;
	rs_pending.arr  = (struct rsv_stn **)calloc(rs_sz, sizeof(struct srv_stn *));
	rs_executing.max  = rs_sz;
	rs_executing.idx  = 0;
	rs_executing.size = 0;
	rs_executing.arr  = (struct rsv_stn **)calloc(rs_sz, sizeof(struct srv_stn *));


	// Register alias table
	rat = (struct reg_alias_tbl *)calloc(NUM_REG + 1, sizeof(struct reg_alias_tbl));

	// Execution unit
	eu = (struct execution_unit *)calloc(width, sizeof(struct execution_unit));
	eu_size = 0;

	// Reorder buffer
	rob.max  = rob_sz;
	rob.idx  = 0;
	rob.size = 0;
	rob.arr  = (struct rob_node *)calloc(rob_sz, sizeof(struct rob_node));

	ordered_arr = (int *)calloc(, sizeof(int));
	for (int i = 0; i < width; i++) ordered_arr[i] = 0;

	
    return OOO_EXIT_SUCCESS;
}

int ooo_destory() {

	/** Mudule destroy */
	free(fq.arr);
	free(rs_free.arr);
	free(rs);
	free(rat);
	free(eu);
	free(rob);
	
    return OOO_EXIT_SUCCESS;
}

static int enable_dealloc() {
	return OOO_EXIT_SUCCESS;
}

static int do_fetch(FILE *fp) {
	char tr_inst[16];
	int  tr_dest, tr_src1, tr_src2;
	static int pc = 0;

	for (int i = 0; i < width; i++) {
		int idx;

		if (fq.size == fq.max) break;

		rc = fscanf(fp, "%s %d %d %d %d\n", tr_inst, &tr_dest, &tr_src1, &tr_src2);
		if (rc == EOF) {
			stop_flag = true;
			break;
		}

		idx = (fq.idx + fq.size) % fq.max;
		switch (tr_inst[3]) {
		case 'A':
			fq.arr[idx].type = IntAlu;
			break;
		case 'R':
			fq.arr[idx].type = MemRead;
			break;
		case 'W':
			fq.arr[idx].type = MemWrite;
		}
		fq.arr[idx].dest = tr_dest;
		fq.arr[idx].src1 = tr_src1;
		fq.arr[idx].src2 = tr_src2;

		fq.arr[idx].pc = pc++;
		fq.arr[idx].alloc = true;

		fq.size++;
	}
	
    return OOO_EXIT_SUCCESS;
}

static int do_decode() {
	for (int i = 0; i < width; i++) {
		struct rsv_stn *tgt;
		inst type;
		int dest, src1, src2, pc;
		int idx;

		if (rob.size == rob.max) break;
		if (rs_free.size == 0) break;
		if (fq.size == 0) break;

		// Dequeue from fetch queue
		type = fq.arr[fq.idx].type;
		dest = fq.arr[fq.idx].dest;
		src1 = fq.arr[fq.idx].src1;
		src2 = fq.arr[fq.idx].src2;
		pc = fq.arr[fq.idx].pc;
		fq.arr[fq.idx].alloc = false;

		// Register rob
		idx = (rob.idx + rob.size++) % rob.max;
		rob.arr[idx].dest  = dest;
		rob.arr[idx].state = false;
		rob.arr[idx].alloc = true;

		// Register rs
		tgt = rs_free.arr[rs_free.idx++]; rs_free.size--;
		tgt->rob_idx = idx;
		tgt->eu_idx  = -1;
		tgt->type    = type;
		if (src1 != 0 && !rat[src1].rf.valid) q1 = rat[src1].rob_idx;
		if (src2 != 0 && !rat[src2].rf.valid) q2 = rat[src2].rob_idx;
		tgt->pc = pc;
		tgt->alloc = true;

		// Register rat
		rat[dest].rob_idx  = idx;
		rat[dest].rf_valid = false;

		rs_pending.arr[rs_pending.idx+rs_pending.size++] = tgt;

		rs_free.idx %= rs_free.max;
		++fq.idx %= fq.max;
	}
    return OOO_EXIT_SUCCESS;
}

static int do_issue() {
	for (int i = 0, j; i < width; i++) {
		struct rsv_stn *tgt;

		if (eu_size == width) break;

		for (j = rs_pending.idx; j < rs_pending.idx + rs_pending.size; j++) {
			if (rs_pending.arr[j].q1 && rs_pending.arr[j].q2) {
				tgt = rs_pending.arr[rs_pending.idx];
				break;
			}
		}
		if (j == rs_pending.idx+rs_pending.size) break;

		++rs_pending.idx %= rs_pending.max;
		rs_pending.size--;

		tgt->eu_idx = 
	}
    return OOO_EXIT_SUCCESS;
}

static int do_execution() {
    return OOO_EXIT_SUCCESS;
}

static int do_commit() {
    return OOO_EXIT_SUCCESS;
}

int ooo_do_cycle(FILE *fp) {
	enable_dealloc();

    do_commit();
    do_execution();
    do_issue();
    do_decode();
    if (!stop_flag) do_fetch(fp);

    return OOO_EXIT_SUCCESS;
}
