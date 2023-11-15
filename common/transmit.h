#ifndef TRANSMIT_H
#define TRANSMIT_H

#include "c150debug.h"
#include "c150streamsocket.h"

static int recv(char **outbuf) {
  (void)outbuf;
  return -1;
}

static void send(const char inbuf, int len) {
  (void)inbuf;
  (void)len;
}

#endif
