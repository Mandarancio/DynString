#include "dynstring.h"

#include <assert.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>

dynstr*
dynstr_new     (void)
{
  dynstr * res = malloc(sizeof(dynstr));
  res->data = malloc(sizeof(char));
  res->data[0] = 0;
  res->size = 0;
  return res;
}

dynstr*
dynstr_from    (const char *src)
{
  assert(src);

  size_t n = strlen(src);

  dynstr * res = malloc(sizeof(dynstr));
  res->data = malloc((n+1)*sizeof(char));
  res->data[n] = 0;
  memcpy(res->data, src, n);
  res->size = n;

  return res;
}

dynstr*
dynstr_copy      (dynstr *src)
{
  assert(src);

  dynstr * res = malloc(sizeof(dynstr));
  res->data = malloc(sizeof(char)*(1+src->size));
  res->data[src->size] = 0;
  memcpy(res->data, src->data, src->size);
  res->size = src->size;
  return res;
}

void
dynstr_free      (dynstr     *str)
{
  assert(str);
  free(str->data);
  free(str);
  str = NULL;
}

void
dynstr_puts_n  (dynstr     *dst,
                const char *src,
                size_t      n)
{
  assert(dst);
  if (n)
  {
    size_t m = dst->size;
    dst->data = realloc(dst->data, m + n + 1);
    dst->data[m+n] = 0;
    memcpy(dst->data+m, src, n);
    dst->size += n;
  }
}

void
dynstr_puts    (dynstr     *dst,
                const char *src)
{
  assert(dst && src);
  size_t n = strlen(src);
  dynstr_puts_n(dst, src, n);
}

void    dynstr_dynstr_concat (dynstr     *dst,
                              dynstr     *src)
{
  assert(dst && src);
  size_t n = src->size;
  dynstr_puts_n(dst, src->data, n);
}

int
dynstr_cmpr    (dynstr     *a,
                dynstr     *b)
{
  assert(a && b);
  if (a->size != b->size)
  {
    return (int)a->size - (int)b->size;
  }
  if (a->size == 0)
    return 0;
  return strcmp(a->data, b->data);
}

size_t
dynstr_len     (dynstr     *str)
{
  assert(str);
  return str->size;
}
char*
dynstr_cstr      (dynstr     *str)
{
  assert(str);
  return str->data;
}

int
dynstr_scmpr   (dynstr     *a,
                const char *b)
{
  assert(a && b);
  return strcmp(a->data, b);
}

int
dynstr_cmpr_at (dynstr     *a,
                const char *b,
                dyniter     iter)
{
  assert(a && b);
  if (iter > a->size)
    return -1;
  return strcmp(a->data + iter, b);
}

void
dynstr_printf  (dynstr     *dst,
                const char *fmt,
                ...)
{
  assert(dst && fmt);

  va_list ap;
  int n;
  char buff[1024];
  va_start(ap, fmt);

  n = vsnprintf(buff, 1024, fmt, ap);

  va_end(ap);

  if (n < 0) {
  	return;
  }
  dynstr_puts(dst, buff);
}

dyniter*
dynstr_match   (dynstr     *dst,
                const char *exp,
                dyniter    *iter)
{
  assert(dst && exp);
  size_t i = 0;
  size_t n = strlen(exp);

  if (iter)
    i = *iter;

  size_t m = dst->size;

  if (m - i > n) {
    for (; i < m - n; i++)
    {
      if (!strncmp(dst->data+i, exp, n))
      {
        dyniter * res = malloc(sizeof(dyniter));
        *res = i;
        return res;
      }
    }
  }
  return NULL;
}

dynstr*
dynstr_substr  (dynstr     *str,
                dyniter     a,
                dyniter     b)
{
  assert(str);
  size_t n = str->size;
  if (a > n || a >= b || n == 0) {
    return NULL;
  }
  b = b > n ? n : b;
  size_t m = b - a;
  dynstr * res = malloc(sizeof(dynstr));
  res->data = malloc((m+1)*sizeof(char));
  res->data[m] = 0;
  strncpy(res->data, str->data+a, m);
  res->size = m;
  return res;
}

dynstr**
dynstr_splits  (dynstr     *dst,
                const char *exp,
                size_t     *n)
{
  assert(dst && exp);
  size_t k = 0;
  size_t m = strlen(exp);
  size_t l = dst->size;
  dynstr** res=NULL;

  if (l < m || l == 0 || m == 0) {
    res = malloc(sizeof(dynstr*));
    res[0] = dst;
    if (n)
      *n = 1;
    return res;
  }


  dyniter str = 0;
  dyniter *end = NULL;
  do {
    end = dynstr_match(dst, exp, &str);
    if (end) {
      k ++;
      res = realloc(res, sizeof(dynstr*)*k);
      res[k-1] = dynstr_substr(dst, str, *end);
      str = *end + m;
      free(end);
    }
  } while(end!=NULL && *end <l);
  if (str < l) {
    k ++;
    res = realloc(res, sizeof(dynstr*)*k);
    res[k-1] = dynstr_substr(dst, str, l);
  }

  if (n)
    *n = k;
  return res;
}


dyniter*
dynstr_match_all (dynstr     *str,
                  const char *exp,
                  size_t     *n)
{
  assert(str && exp);
  size_t k = 0;
  size_t m = strlen(exp);
  size_t l = str->size;
  dyniter* res=NULL;

  if (l < m || l == 0 || m == 0) {
    if (n)
      *n = 0;
    return res;
  }

  dyniter it = 0;
  dyniter *end = NULL;
  do {
    end = dynstr_match(str, exp, &it);
    if (end)
    {
      k++;
      res = realloc(res, sizeof(dyniter)*k);
      res[k-1] = *end;
      it = *end + m;
      free(end);
    }
  } while(end != NULL && *end < l);
  if (n)
    *n = k;
  return res;
}

dynstr**
dynstr_splitc    (dynstr     *dst,
                  char        trg,
                  size_t     *n)
{
  assert(dst);
  dyniter it;
  dyniter last = 0;
  size_t l = dst->size;

  if (!l) {
    if (n)
      *n = 0;
    return NULL;
  }

  dynstr** res = NULL;
  size_t k = 0;
  for (it = 0; it < l; it++) {
    if (dst->data[it] == trg) {
      k ++;
      res = realloc(res, k * sizeof(dynstr*));
      res[k-1] = dynstr_substr(dst, last, it);
      last = it + 1;
    }
  }
  if (last < l) {
    k++;
    res = realloc(res, k * sizeof(dynstr*));
    res[k-1] = dynstr_substr(dst, last, l);
  }
  if (n)
    *n = k;
  return res;
}



void
dynstr_strip     (dynstr     *dst,
                  char        trg)
{
  assert(dst);
  size_t i = 0;
  char *pr = dst->data, *pw = dst->data;
  while (*pr) {
    *pw = *pr++;
    if (*pw == trg){
      i++;
    }
    pw += (*pw != trg);
  }
  *pw = '\0';
  dst->data = realloc(dst->data, dst->size - i + 1);
  dst->size -= i;
}
