#include <iostream>
#include <fstream>
#include <queue>
#include <list>
#include <vector>

#define NUM_REG 16

using namespace std;

struct inst {
    int type;
    int dest;
    int src1;
    int src2;
};

struct rs_node {
    int my_idx;
    int rob_idx;
    int type;
    int q1, q2;
};

struct rat_node {
    int rob_idx;
    bool rf_valid;
};

struct eu_node {
    int time_left;
    int rs_idx;
};

struct rob_node {
    int dest;
    bool vstate;
    bool rstate;
    queue<int> pending;
};

ifstream in;
int dump, width, rob_sz, rs_sz;
int fq_max;

int fq_dealloc, rs_dealloc, eu_dealloc, rob_dealloc;

int cycles, total_insts, num_intalu, num_memread, num_memwrite;
float ipc;

queue<inst> fq;
vector<rs_node> rs;
vector<rat_node> rat;
vector<eu_node> eu;
vector<rob_node> rob;

queue<rs_node *> rs_free;
deque<rs_node *> rs_pend;
queue<eu_node *> eu_free;
int rob_idx, rob_size;

void ooo_create();
void ooo_destroy();
int do_cycle();
void do_fetch();
void do_decode();
void do_issue();
void do_execution();
void do_commit();
void do_dump();
void enable_dealloc();
int check_exit();

int main (int argc, char *argv[]) {
    // Config
    in.open(argv[1]);
    in >> dump >> width >> rob_sz >> rs_sz;
    in.close();

    // Init
    ooo_create();

    // Proceed
    in.open(argv[2]); // trace
    while (1) {
        cycles++;
        int is_exit = do_cycle();
        if (is_exit) break;
    }

    // Free
    in.close();
    ooo_destroy();
    return 0;
}

void ooo_create() {
    fq_max = 2 * width;
    rs.resize(rs_sz);
    for (int i = 0; i < rs_sz; i++) {
        rs[i].my_idx = i;
        rs[i].rob_idx = -1;
        rs_free.push(&rs[i]);
    }
    rat.resize(NUM_REG+1);
    for (int i = 1; i <= NUM_REG; i++) rat[i].rf_valid = true;
    eu.resize(width);
    for (int i = 0; i < width; i++) eu_free.push(&eu[i]);
    rob.resize(rob_sz);
    for (int i = 0; i < rob_sz; i++) {
        rob[i].dest = -1;
    }
}

void ooo_destroy() {
    // empty
    printf("Cycles      %d\n", cycles);
    printf("IPC         %.2f\n", (float)total_insts/cycles);
    printf("Total Insts %d\n", total_insts);
    printf("IntAlu      %d\n", num_intalu);
    printf("MemRead     %d\n", num_memread);
    printf("MemWrite    %d\n", num_memwrite);
}

int do_cycle() {

    do_commit();
    do_execution();
    do_issue();
    do_decode();
    do_fetch();

    enable_dealloc();
    do_dump();

    if (check_exit()) return 1;

    return 0;
}

void do_fetch() {
    char buf[16];
    unsigned int addr;

    for (int i = 0; i < width; i++) {
        if (fq.size() + fq_dealloc == fq_max) break;
        inst a;
        if (in >> buf >> dec >> a.dest >> a.src1 >> a.src2 >> hex >> addr) {
            total_insts++;

            if (buf[3] == 'A') {  a.type = 0; num_intalu++; }
            else if (buf[3] == 'W') { a.type = 1; num_memwrite++; }
            else if (buf[3] == 'R') { a.type = 2; num_memread++; }

            fq.push(a);
        }
    }
}

void do_decode() {
    for (int i = 0; i < width; i++) {
        if (rob_size + rob_dealloc == rob_sz) break;
        if (rs_free.size() - rs_dealloc == 0) break;
        if (fq.empty()) break;

        // Fetch & register rob
        inst a = fq.front(); fq.pop(); fq_dealloc++;
        int idx = (rob_idx+rob_size++) % rob_sz;
        rob[idx].dest = a.dest;
        rob[idx].vstate = false;
        rob[idx].rstate = false;

        // Register rs
        rs_node *c = rs_free.front(); rs_free.pop();
        c->rob_idx = idx;
        c->type = a.type;
        if (a.src1 && !rat[a.src1].rf_valid) {
            c->q1 = rat[a.src1].rob_idx;
            rob[c->q1].pending.push(c->my_idx);
        }
        else c->q1 = -1;
        if (a.src2 && !rat[a.src2].rf_valid) {
            c->q2 = rat[a.src2].rob_idx;
            if (a.src1 != a.src2) rob[c->q2].pending.push(c->my_idx);
        }
        else c->q2 = -1;
        rs_pend.push_back(c);

        // Register rat
        rat[a.dest].rob_idx = idx;
        rat[a.dest].rf_valid = false;
    }
}

void do_issue() {
    for (int i = 0; i < width; i++) {
        if (eu_free.size() == 0) break;
        if (rs_pend.empty()) break;

        for (auto it = rs_pend.begin(); it != rs_pend.end(); it++) {
            if ((*it)->q1 == -1 && (*it)->q2 == -1) {
                eu_node *e = eu_free.front(); eu_free.pop();
                e->time_left = ((*it)->type < 2) ? 1 : 3;
                e->rs_idx = (*it)->my_idx;
                rs_pend.erase(it);
                break;
            }
        }
    }
}

void do_execution() {
    for (int i = 0; i < width; i++) {
        if (eu[i].time_left) {
            eu[i].time_left--;
            if (eu[i].time_left == 0) {
                eu_free.push(&eu[i]);
                rs_free.push(&rs[eu[i].rs_idx]); rs_dealloc++;
                rob[rs[eu[i].rs_idx].rob_idx].vstate = true;
                rs[eu[i].rs_idx].rob_idx = -1;
            }
        }
    }
}

void do_commit() {
    int i;
    if (!rob_size) return;
    for (i = rob_idx; i < rob_idx + width; i++) {
        if (rob[i % rob_sz].rstate) {
            rob[i%rob_sz].dest = -1;
            rob[i%rob_sz].vstate = false;
            rob[i%rob_sz].rstate = false;
            rob_dealloc++;
        }
        else break;
        if (i == rob_idx + rob_size) break;
    }
    rob_size -= i - rob_idx;
    rob_idx = i % rob_sz;
}

void enable_dealloc() {
    fq_dealloc = 0;
    rs_dealloc = 0;
    rob_dealloc = 0;
    for (int i = rob_idx; i < rob_idx + rob_size; i++) {
        if (rob[i % rob_sz].vstate == true) {
            rob[i % rob_sz].rstate = true;
            rat[rob[i%rob_sz].dest].rob_idx = -1;
            rat[rob[i%rob_sz].dest].rf_valid = true;
            while (!rob[i % rob_sz].pending.empty()) {
                int idx = rob[i % rob_sz].pending.front(); rob[i % rob_sz].pending.pop();
                if (rs[idx].q1 == i % rob_sz) rs[idx].q1 = -1;
                if (rs[idx].q2 == i % rob_sz) rs[idx].q2 = -1;
            }
        }
    }
}

void do_dump() {
    if (dump > 0) {
        printf("= Cycle %d\n", cycles);
        if (dump > 1) {
            for (int i = 0; i < rs_sz; i++) {
                if (rs[i].rob_idx == -1) { 
                    //printf("RS%d\n", i+1);
                }
                else {
                    printf("RS%d\t: ROB%d", i+1, rs[i].rob_idx+1);
                    if (rs[i].q1 == -1) printf(" V");
                    else printf(" %d", rs[i].q1+1);
                    if (rs[i].q2 == -1) printf(" V\n");
                    else printf(" %d\n", rs[i].q2+1);
                }
            }
            puts("");
        }
        for (int i = 0; i < rob_sz; i++) {
            if (rob[i].dest == -1) {
                //printf("ROB%d\n", i+1);
            }
            else printf("ROB%d\t: %c\n", i+1, rob[i].vstate ? 'C' : 'P');
        }
        puts("");
    }
}

int check_exit() {
    if (fq.size() == 0 && rs_free.size() == rs_sz && eu_free.size() == width && rob_size == 0) {
        return 1;
    }
    return 0;
}
