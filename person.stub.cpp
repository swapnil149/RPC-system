#include <stdio.h>
#include <string>
using namespace std;

#include "person.idl"

#include <cstdlib>

#define INITIAL_CAPACITY (1 << 16)

typedef uint32_t rpcptr_t;
struct rpcmem_t {
  unsigned char *data;
  rpcptr_t hp;
  rpcptr_t sp;

  // internal only
  rpcptr_t capacity;
};

rpcmem_t *rpcmem_new() {
  rpcmem_t *m = (rpcmem_t *)malloc(sizeof(*m));
  m->capacity = INITIAL_CAPACITY;
  m->hp = 0;
  m->sp = m->capacity;
  m->data = (unsigned char *)malloc(m->capacity);
  return m;
}

void rpcmem_free(rpcmem_t **m) {
  free((*m)->data);
  free(*m);
  *m = NULL;
}

void rpcmem_expand(rpcmem_t *m) { (void)m; }

#define MAX_FNAME_LEN 80
#define MAX_PREFIX_LEN (MAX_FNAME_LEN + 40)
#define PREFIX_FMT "%s %08x"

// clang-format off

int rpcmem_tobuf(const char *fname, const rpcmem_t *m, char **outbuf) {
  // calculate prefix length (contains fname and RSP value)
  int prefixlen = snprintf(NULL, 0, PREFIX_FMT, fname, m->hp);
  assert(prefixlen < MAX_PREFIX_LEN && prefixlen);
  // allocate appropriate sized buffer (using calculated prefix len)
  *outbuf = (char *)malloc(prefixlen + m->hp + (m->capacity - m->sp));
  // finally can actually write the prefix
  snprintf(*outbuf, MAX_PREFIX_LEN, PREFIX_FMT, fname, m->hp);
  memcpy(*outbuf + prefixlen,         m->data,         m->hp);
  memcpy(*outbuf + prefixlen + m->hp, m->data + m->sp, m->capacity - m->sp);
  return prefixlen + m->hp + (m->capacity - m->sp);
}

// clang-format on

void rpcmem_frombuf(const char *inbuf, int len, char fname[MAX_PREFIX_LEN],
                    rpcmem_t *m) {
  int stackptr, prefixlen;
  sscanf(inbuf, PREFIX_FMT "%n", fname, &stackptr, &prefixlen);
  m->sp = stackptr;
  m->hp = 0;
  memcpy(m->data, inbuf + prefixlen, len);
}

void pack_int(int x, rpcmem_t *m) {
  unsigned char bytes[4] = {
      (unsigned char)x,
      (unsigned char)(x >> 8),
      (unsigned char)(x >> 16),
      (unsigned char)(x >> 24),
  };
  for (int i = 0; i < 4; i++)
    m->data[m->sp--] = bytes[i];
}
void pack_bool(bool b, rpcmem_t *m) { m->data[m->sp--] = (char)b; }
void pack_rpcptr(rpcptr_t x, rpcmem_t *m) { pack_int(x, m); }
void pack_float(float f, rpcmem_t *m) { pack_int(*(unsigned *)&f, m); }
void pack_string(string s, rpcmem_t m) {
  (void)s;
  (void)m;
}

int unpack_int(rpcmem_t *m) {
  unsigned char bytes[4];
  for (int i = 4 - 1; i >= 0; i--)
    bytes[i] = m->data[++m->sp];
  return bytes[0] | bytes[1] << 8 | bytes[2] << 16 | bytes[3] << 24;
}
bool unpack_bool(rpcmem_t *m) { return m->data[++m->sp]; }
rpcptr_t unpack_rpcptr(rpcmem_t *m) { return unpack_int(m); }
float unpack_float(rpcmem_t *m) {
  int x = unpack_int(m);
  return *(float *)&x;
}
string unpack_string(rpcmem_t *m) {
  string result;
  (void)m;
  return result;
}
