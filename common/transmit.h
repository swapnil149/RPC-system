#ifndef TRANSMIT_H
#define TRANSMIT_H

#include <cstdio>

#ifndef SOCKET
#define SOCKET dummysock
static struct {
  void (*write)(const void *, int);
  void (*read)(void *, int);
} *dummysock;
#endif

// #include "c150debug.h"
// #include "c150streamsocket.h"
#include "mem.h"
#include <stdio.h>

#define PREFIX_FMT "%s %08x"

static void rpc_send(const char *fname, const __rpcmem_t *mem) {
  int prefix_len = snprintf(NULL, 0, PREFIX_FMT, fname, mem->hp);
  char *prefix = (char *)malloc(prefix_len);
  snprintf(prefix, prefix_len, PREFIX_FMT, fname, mem->hp);

  int msg_len = prefix_len + mem->hp + (mem->capacity - mem->sp);
  SOCKET->write(&msg_len, sizeof(msg_len));
  SOCKET->write(prefix, prefix_len);
  SOCKET->write(mem->data, mem->hp);
  SOCKET->write(mem->data + mem->sp, mem->capacity - mem->sp);

  free(prefix);
}

static void rpc_recv(char *fname, __rpcmem_t *mem) {
  int msg_len, prefixlen;

  SOCKET->read(&msg_len, sizeof(msg_len));

  char *buf = (char *)malloc(msg_len);
  SOCKET->read(buf, msg_len);

  sscanf(buf, PREFIX_FMT "%n", fname, &mem->sp, &prefixlen);
  memcpy(mem->data, buf + prefixlen, msg_len - prefixlen);
  free(buf);
}

#endif
