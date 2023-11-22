#ifndef TRANSMIT_H
#define TRANSMIT_H

#include <cstdio>
#include <string>
using namespace std;

#ifndef RPCSOCKET // just a placeholder to remove warnings
#define RPCSOCKET dummysock
static struct {
    void (*write)(const char *, int);
    void (*read)(char *, int);
} *dummysock;
#endif

#include "../debugmacros.h"
#include "basis.h"
#include "mem.h"
#include <stdio.h>
#include <stdlib.h>

static void rpc_send(string _fname, __rpcmem_t *mem) {
    string fname = string(_fname.c_str());
    ERP("STARTING RPC SEND: %s <-- sent\n", fname.c_str());
    DEBUG("%d", mem->hp);
    DEBUG("%ld", fname.length());
    __pack_string(fname, mem);
    DEBUG("%d", mem->hp);
    int stack_size = mem->capacity - mem->sp;
    int mem_size = mem->hp + stack_size;

    DEBUG("%d", mem_size);
    DEBUGMEM(mem);

    // TODO: don't rely on endianness
    RPCSOCKET->write((char *)&mem_size, sizeof(mem_size));
    RPCSOCKET->write((char *)&mem->hp, sizeof(mem->hp));
    char *tmp = (char *)malloc(mem_size);
    memcpy(tmp, mem->data, mem->hp);
    memcpy(tmp + mem->hp, mem->data + mem->sp, stack_size);

    RPCSOCKET->write(tmp, mem_size);
    // RPCSOCKET->write(mem->data + mem->sp, stack_size);

    DEBUGBUF(tmp, 0, mem_size);
    // DEBUGBUF((mem->data + mem->sp), 0, stack_size);

    ERP("FINISHED SEND\n");
    free(tmp);
}

static string rpc_recv(__rpcmem_t *mem) {
    ERP("STARTING RPC RECEIVE\n");
    int mem_size;
    RPCSOCKET->read((char *)&mem_size, sizeof(mem_size));
    RPCSOCKET->read((char *)&mem->sp, sizeof(mem->sp));
    RPCSOCKET->read(mem->data, mem_size);
    mem->hp = 0;

    DEBUG("%d", mem_size);
    DEBUGMEM(mem);
    DEBUGBUF(mem->data, 0, mem_size);

    string fname = __unpack_string(mem);
    ERP("FINISHING RPC RECEIVE, %s <-- received\n", fname.c_str());
    return fname;
}

#endif
