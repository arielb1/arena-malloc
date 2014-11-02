#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <valgrind/memcheck.h>
#include <assert.h>
#include <string.h>

#ifndef ARENA_SIZE
#define ARENA_SIZE 0x70000000
#endif

static char arena[ARENA_SIZE];
static size_t pos;
static int marker;

static void *real_malloc(size_t s) {
  char *result = arena+pos;
  assert(s<ARENA_SIZE-pos);
  pos += s;
  return result;
}

static void trace_init()
{
  marker=1;
  VALGRIND_CREATE_MEMPOOL(&marker, 0, 1);
}

void *malloc(size_t s) {
  if(!marker) trace_init();
  if(s>0x7fffffff) return 0;
  size_t *p = real_malloc(s+sizeof(s));
  if(!p) return p;
  *p++ = s;
  void *result = p;
  VALGRIND_MEMPOOL_ALLOC(&marker, result, s);
  return result;
}

void *calloc(size_t a, size_t b) {
  __uint128_t s = a*b;
  if(s > 0x7fffffff) return 0;
  return malloc((size_t)s);
}

void *realloc(void* a, size_t news) {
  if(!marker) trace_init();
  if(!a) return malloc(news);
  size_t *x = a;
  --x;
  size_t olds = *x;
  void *result = malloc(news);
  if(!result) return result;
  memcpy(result, a, olds);
  free(a);
  return result;
}

void free(void* a) {
  if(a) VALGRIND_MEMPOOL_FREE(&marker, a);
}
