#include "dynstring.h"

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
  size_t n = strlen(src);

  dynstr * res = malloc(sizeof(dynstr));
  res->data = malloc((n+1)*sizeof(char));
  res->data[n] = 0;
  memcpy(res->data, src, n);
  res->size = n;

  return res;
}


void
dynstr_puts_n  (dynstr     *dst,
                const char *src,
                size_t      n)
{
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
  size_t n = strlen(src);
  dynstr_puts_n(dst, src, n);
}

void    dynstr_cnct    (dynstr     *dst,
                        dynstr     *src)
{
  size_t n = src->size;
  dynstr_puts_n(dst, src->data, n);
}

int
dynstr_cmpr    (dynstr     *a,
                dynstr     *b)
{
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
  return str->size;
}

int
dynstr_scmpr   (dynstr     *a,
                const char *b)
{
  return strcmp(a->data, b);
}

int
dynstr_cmpr_at (dynstr     *a,
                const char *b,
                dyniter     iter)
{
  if (iter > a->size)
    return -1;
  return strcmp(a->data + iter, b);
}

void
dynstr_printf  (dynstr     *dst,
                const char *fmt,
                ...)
{
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


void
dynstr_strip     (dynstr     *dst,
                  char        trg)
{
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
