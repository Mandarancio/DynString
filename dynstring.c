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
  if (iter.i > a->size)
    return -1;
  return strcmp(a->data + iter.i, b);
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
  size_t n = strlen(exp);
  dyniter * res = dynstr_iter(dst);
  if (iter){
    *res = *iter;
  }
  size_t m = dst->size;

  if (m - res->i > n) {

    for (; res->i < m - n; dyniter_next(res))
    {
      if (!strncmp(dst->data+res->i, exp, n))
      {
        return res;
      }
    }
  }
  free(res);
  return NULL;
}

dynstr*
dynstr_substr  (dynstr     *str,
                dyniter     a,
                dyniter     b)
{
  return dynstr_substr_s(str, a.i, b.i);
}

dynstr*
dynstr_substr_s  (dynstr     *str,
                  size_t      a,
                  size_t      b)
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
  strncpy(res->data, str->data + a, m);
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


  dyniter *str = dynstr_iter(dst);
  dyniter *end = NULL;
  do {
    end = dynstr_match(dst, exp, str);
    if (end) {
      k ++;
      res = realloc(res, sizeof(dynstr*)*k);
      res[k-1] = dynstr_substr(dst, *str, *end);
      free(str);
      str = end;
      dyniter_skip(str, m);
    }
  } while(end!=NULL && end->i < l);
  if (str->i < l) {
    k ++;
    res = realloc(res, sizeof(dynstr*)*k);
    res[k-1] = dynstr_substr_s(dst, str->i, l);
  }
  free(str);
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

  dyniter *it = dynstr_iter(str);
  dyniter *end = NULL;
  do {
    end = dynstr_match(str, exp, it);
    if (end)
    {
      k++;
      res = realloc(res, sizeof(dyniter)*k);
      res[k-1] = *end;
      free(it);
      it = end;
      dyniter_skip(it, m);
    }
  } while(end != NULL && end->i < l);
  if(end)
    free(end);
  free(it);
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

  size_t l = dst->size;

  if (!l) {
    if (n)
      *n = 0;
    return NULL;
  }

  dyniter *it = dynstr_iter(dst);
  dyniter *last = dynstr_iter(dst);
  dynstr** res = NULL;
  size_t k = 0;
  do {
    if (dyniter_at(*it) == trg) {
      k ++;
      res = realloc(res, k * sizeof(dynstr*));
      res[k-1] = dynstr_substr(dst, *last, *it);
      *last = *it;
      dyniter_next(last);
    }
  } while (dyniter_next(it));
  if (last->i < l) {
    k++;
    res = realloc(res, k * sizeof(dynstr*));
    res[k-1] = dynstr_substr_s(dst, last->i, l);
  }
  free(it);
  free(last);
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


/** Iter function **/

dyniter*
dyniter_new      (void)
{
  dyniter * it = malloc(sizeof(dyniter));
  it->i = 0;
  it->column = 0;
  it->line = 0;
  it->__src__ = NULL;
  return it;
}
dyniter*
dynstr_iter      (dynstr     *src)
{
  dyniter * it = dyniter_new();
  it->__src__ = src;
  return it;
}

void
dyniter_free     (dyniter    *iter)
{
  iter->__src__  = NULL;
  free(iter);
}

ds_bool
dyniter_next     (dyniter    *it)
{
  assert(it && it->__src__);
  dynstr * str = it->__src__;
  if (it->i < str->size - 1)
  {
    if (str->data[it->i] == '\n') {
      it->line ++;
      it->column = 0;
    } else {
      it->column ++;
    }
    it->i ++;
    return TRUE;
  }
  return FALSE;
}

ds_bool
dyniter_at_end   (dyniter    *it)
{
  assert(it && it->__src__);
  return (it->i == it->__src__->size - 1);
}

size_t
dyniter_skip     (dyniter    *it,
                  size_t      n)
{
  assert(it);
  size_t i;
  for (i = 0; i < n; i++) {
    if (!dyniter_next(it)) {
      break;
    }
  }
  return i;
}

char*
dyniter_print    (dyniter    it)
{
  char * buff = malloc(256 * sizeof(char));
  sprintf(buff, "pos %lu, line %lu, col %lu", it.i, it.line, it.column);
  return buff;
}
char
dyniter_at       (dyniter     it)
{
  assert(it.__src__);
  return it.__src__->data[it.i];
}
