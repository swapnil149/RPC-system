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

static void rpc_send(string fname, __rpcmem_t *mem) {
    ERP("SENDING RPC\n");
    __pack_string(fname, mem);
    int mem_size = mem->hp + (mem->capacity - mem->sp);

    DEBUG("%s", fname.c_str());
    DEBUG("%d", mem->hp);
    DEBUG("%d", mem->sp);
    DEBUG("%d", mem_size);

    MADEIT;

    // TODO: don't rely on endianness
    RPCSOCKET->write((char *)&mem_size, sizeof(mem_size));

    MADEIT;

    RPCSOCKET->write((char *)&mem->hp, sizeof(mem->hp));

    MADEIT;

    RPCSOCKET->write(mem->data, mem->hp);

    MADEIT;

    RPCSOCKET->write(mem->data + mem->sp, mem->capacity - mem->sp);

    MADEIT;
}

static string rpc_recv(__rpcmem_t *mem) {
    ERP("STARTING RPC RECEIVE\n");

    int mem_size;
    MADEIT;
    RPCSOCKET->read((char *)&mem_size, sizeof(mem_size));
    MADEIT;
    DEBUG("%d", mem_size);
    RPCSOCKET->read((char *)&mem->sp, sizeof(mem->sp));
    MADEIT;

    DEBUG("%d", mem->sp);
    RPCSOCKET->read(mem->data, mem_size);
    MADEIT;
    string fname = __unpack_string(mem);
    ERP("RECEIVED RPC\n");
    DEBUG("%s", fname.c_str());
    DEBUG("%d", mem->hp);
    DEBUG("%d", mem->sp);
    return fname;
}

#endif
