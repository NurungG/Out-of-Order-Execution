#include "ooo.h"

int ooo_create() {
    return OOO_EXIT_SUCCESS;
}

int ooo_destory() {
    return OOO_EXIT_SUCCESS;
}

static int do_fetch() {
    return OOO_EXIT_SUCCESS;
}

static int do_decode() {
    return OOO_EXIT_SUCCESS;
}

static int do_issue() {
    return OOO_EXIT_SUCCESS;
}

static int do_execution() {
    return OOO_EXIT_SUCCESS;
}

static int do_commit() {
    return OOO_EXIT_SUCCESS;
}

int ooo_do_cycle() {
    do_fetch();
    do_decode();
    do_issue();
    do_execution();
    do_commit();

    return OOO_EXIT_SUCCESS;
}
