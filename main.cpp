#include <iostream>
#include <fstream>
#include <queue>
#include <list>
#include <vector>

using namespace std;

struct inst {
    int type;
    int dest;
    int src1;
    int src2;
    int addr;
}

struct rs_node {
    int rob_idx;
    int eu_idx;
    int type;
    int q1, q2;
}

struct rat_node {
    int rob_idx;
    bool rf_valid;
}

struct eu_node {
    int time_left;
    int rs_idx;
}

struct rob_node {
    int dest;
    bool state;
}

ifstream in;
int dump, width, rob_sz, rs_sz;
int fq_max;

queue<inst> fq;
vector<rs_node> rs;
queue<rs_node *> rs_free, rs_pend, rs_exe;
vector<rat_node> rat;
vector<eu_node> eu;
queue<eu_node *> eu_free, eu_exe1, ex_exe2;
queue<rob_node> rob;


void ooo_create();
void ooo_destroy();
int do_cydle();

int main (int argc, char *argv[]) {
    in.open(argv[1]);
    in >> dump >> width >> rob_sz >> rs_sz;
    in.close();

    ooo_create();

    in.open(argv[2]);
    while (1) {
        int is_exit = do_cycle();
        if (is_exit) break;
    }
    in.close();

    ooo_destroy();
    return 0;
}

void ooo_create() {
    fq_max = 2 * width;
    rs.resize(rs_sz);
    for (int i = 0; i < rs_sz; i++) rs_free.push(&rs[i]);
    rat.resize(NUM_REG+1);
    eu.resize(width);
    for (int i = 0; i < width; i++) eu_free.push(&eu[i]);
}

void ooo_destroy() {
    // empty
}

int do_cycle() {
    enable_dealloc();

    do_commit();
    do_execution();
    do_issue();
    do_decode();
    do_fetch();

    return 0;
}

void do_fetch() {
    char buf[16]

    for (int i = 0; i < width; i++) {
        if (fq.size() == fq_max) break;
        if (in.eof()) break;

        inst a;
        in >> buf >> a.dest >> a.src1 >> a.src2 >> a.addr;
        if (buf[3] == 'A') a.type = 0;
        else if (buf[3] == 'R') a.type = 1;
        else if (buf[3] == 'W') a.type = 2;
        else cout << "wtf" << endl;

        fq.push(a);
    }
}

void do_decode() {
    for (int i = 0; i < width; i++) {
        if (rob.size() == rob_sz) break;
        if (rs_free.empty()) break;
        if (fq.empty()) break;

        inst a = fq.pop();
        rob_node b;
        b.dest = a.dest;
        b.state = false;

        rs_node *
    }
}
