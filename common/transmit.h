#ifndef TRANSMIT_H
#define TRANSMIT_H

#include <cstdio>
#include <string>
using namespace std;

#ifndef RPCSOCKET // just a placeholder to remove warnings
#define RPCSOCKET dummysock
static struct {
    void (*write)(const char *, int);
    ssize_t (*read)(char *, int);
} *dummysock;
#endif

#include "../debugmacros.h"
#include "basis.h"
#include "mem.h"
#include <stdio.h>
#include <stdlib.h>

static inline void force_read(char *buf, int len) {
    for (int n_read = 0; n_read < len;)
        n_read += RPCSOCKET->read(buf + n_read, len - n_read);
}

static void rpc_send(string fname, __rpcmem_t *mem) {
    ERP("STARTING RPC SEND: %s <-- sent\n", fname.c_str());
    DEBUG("%d", mem->hp);
    DEBUG("%ld", fname.length());
    __pack_string(fname, mem);
    DEBUG("%d", mem->hp);
    int stack_size = mem->capacity - mem->sp;
    int mem_size = mem->hp + stack_size;

    DEBUG("%d", mem_size);
    DEBUGMEM(mem);

    RPCSOCKET->write((char *)&mem_size, sizeof(mem_size));
    RPCSOCKET->write((char *)&mem->hp, sizeof(mem->hp));
    RPCSOCKET->write(mem->data, mem->hp);
    RPCSOCKET->write(mem->data + mem->sp, stack_size);

    DEBUGBUF(mem->data, 0, mem->hp);
    DEBUGBUF((mem->data + mem->sp), 0, stack_size);

    ERP("FINISHED SEND\n");
}

static string rpc_recv(__rpcmem_t *mem) {
    ERP("STARTING RPC RECEIVE\n");
    int mem_size;
    force_read((char *)&mem_size, sizeof(mem_size));
    force_read((char *)&mem->hp, sizeof(mem->hp));
    force_read(mem->data, mem->hp);
    force_read(mem->data + mem->hp, mem_size - mem->hp);
    mem->sp = mem->hp;

    DEBUG("%d", mem_size);
    DEBUGMEM(mem);
    DEBUGBUF(mem->data, 0, mem_size);

    string fname = __unpack_string(mem);
    ERP("FINISHING RPC RECEIVE, %s <-- received\n", fname.c_str());
    return fname;
}

#endif
