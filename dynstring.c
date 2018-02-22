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
