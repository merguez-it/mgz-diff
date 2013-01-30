/*
 * This code was greatly inspired by parts of Git by Linus Torvalds
 * https://github.com/git/git
 *
 * Rewritten for mgz-diff by Grégoire Lejeune <gregoire.lejeune@free.fr>, (C) 2013
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include "isolate.h"

static void do_nothing(size_t size)
{
}

static void (*try_to_free_routine)(size_t size) = do_nothing;

static void memory_limit_check(size_t size)
{
  static int limit = -1;
  if (limit == -1) {
    const char *env = getenv("GIT_ALLOC_LIMIT");
    limit = env ? atoi(env) * 1024 : 0;
  }
  if (limit && size > limit) {
    // TODO
    error("attempting to allocate %d over limit %d",
        (intmax_t)size, limit);
    exit(-1);
  }
}

void *xmalloc(size_t size)
{
  void *ret;

  memory_limit_check(size);
  ret = malloc(size);
  if (!ret && !size)
    ret = malloc(1);
  if (!ret) {
    try_to_free_routine(size);
    ret = malloc(size);
    if (!ret && !size)
      ret = malloc(1);
    if (!ret) {
      // TODO
      error("Out of memory, malloc failed (tried to allocate %lu bytes)",
          (unsigned long)size);
      exit(-1);
    }
  }
#ifdef XMALLOC_POISON
  memset(ret, 0xA5, size);
#endif
  return ret;
}

void *xmallocz(size_t size)
{
  void *ret;
  if (unsigned_add_overflows(size, 1)) {
    // TODO
    error("Data too large to fit into virtual memory space.");
    exit(-1);
  }
  ret = xmalloc(size + 1);
  ((char*)ret)[size] = 0;
  return ret;
}

#ifndef error
#include <stdarg.h>
#include <stdio.h>
void vreportf(const char *prefix, const char *err, va_list params)
{
  char msg[4096];
  vsnprintf(msg, sizeof(msg), err, params);
  fprintf(stderr, "%s%s\n", prefix, msg);
}
static void error_builtin(const char *err, va_list params)
{
    vreportf("error: ", err, params);
}
static void (*error_routine)(const char *err, va_list params) = error_builtin;
int error(const char *err, ...)
{
  va_list params;

  va_start(params, err);
  error_routine(err, params);
  va_end(params);
  return -1; 
}
#endif
