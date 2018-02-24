#include "dynstring.h"

#include <assert.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>

#define PREVIEW 15

dynstr*
dynstr_new     (void)
{
  dynstr * res = malloc(sizeof *res);
  assert(res);
  res->data = malloc(BUFFER*sizeof(char));
  assert(res->data);
  res->data[0] = 0;
  res->size = 0;
  res->r_size = BUFFER;
  return res;
}

dynstr*
dynstr_new_size (size_t n)
{
  dynstr * res = malloc(sizeof *res);
  assert(res);
  size_t t = n + (BUFFER - (n % BUFFER));

  res->data = malloc(t*sizeof(char));
  assert(res->data);

  res->data[0] = 0;
  res->size = 0;
  res->r_size = t;

  return res;
}

dynstr*
dynstr_from    (const char *src)
{
  assert(src);

  size_t n = strlen(src) + 1;

  dynstr * res =  dynstr_new_size(n);
  res->data[n-1] = 0;
  memcpy(res->data, src, n);
  res->size = n-1;

  return res;
}

dynstr*
dynstr_copy      (dynstr *src)
{
  assert(src);

  dynstr * res = malloc(sizeof(dynstr));
  assert(res);

  res->data = malloc(src->r_size * sizeof(char));
  assert(res->data);

  res->data[src->size] = 0;
  memcpy(res->data, src->data, src->size);
  res->size = src->size;
  res->r_size = res->r_size;
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
dynstr_grow    (dynstr     *str,
                size_t      n)
{
  assert(str);
  str->data = realloc(str->data, (str->r_size + n) * sizeof(char));
  str->r_size += n;
  assert(str->data);
}

void
dynstr_dgrow      (dynstr     *str,
                   size_t      n)
{
  assert(str);
  size_t m = str->r_size - str->size;
  if (m <= n + 1) {
    size_t t = str->size + n - m + 1;
    size_t mem = t + (BUFFER - (t % BUFFER));
    dynstr_grow(str, mem - str->r_size);
  }
}

void
dynstr_puts_n  (dynstr     *dst,
                const char *src,
                size_t      n)
{
  assert(dst);
  size_t l = strlen(src);
  n = n < l ? n : l;
  if (n)
  {
    size_t m = dst->size;
    dynstr_dgrow(dst, n);
    memcpy(dst->data+m, src, n);
    dst->data[m+n] = 0;
    dst->size += n;
  } else {
    fprintf(stderr, "PUTS: nothing to puts (n == 0 || l == 0)\n");
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

void
dynstr_concat (dynstr     *dst,
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
dynstr_substr  (dyniter     a,
                dyniter     b)
{
  return dynstr_substr_s(a.__src__, a.i, b.i);
}

dynstr*
dynstr_substr_r   (dynrange    range)
{
  if (range.end.i >= range.start.i)
    return dynstr_substr(range.start, range.end);
  return dynstr_substr(range.end, range.start);
}
dynstr*
dynstr_substr_s  (dynstr     *str,
                  size_t      a,
                  size_t      b)
{
  assert(str);

  size_t n = str->size;
  if (a > n || a >= b+1 || n == 0) {
    fprintf(stderr, "SUBSTR: syntax wrong (a > n || a >= b || n == 0)\n");
    return NULL;
  }

  b = b+1 > n ? n : b+1;
  size_t m = b - a;

  dynstr  * r = dynstr_new_size(m);
  memcpy(r->data, str->data + a , m);
  r->size = m;
  r->data[m] = 0;

  return  r;
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
    assert(res);

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
      assert(res);

      res[k-1] = dynstr_substr_s(dst, str->i, end->i-1);
      free(str);
      str = end;
      dyniter_skip(str, m);
    }
  } while(end!=NULL && end->i < l);
  if (str->i < l) {
    k ++;
    res = realloc(res, sizeof(dynstr*)*k);
    assert(res);

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
      assert(res);

      res[k-1] = *end;
      free(it);
      it = end;
      dyniter_skip(it, m);
    }
  } while(end != NULL && end->i < l);
  if(end && end != it)
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
    fprintf(stderr, "SPLIT: source string is empty\n");
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
      assert(res);

      res[k-1] = dynstr_substr_s(dst, last->i, it->i-1);
      *last = *it;
      dyniter_next(last);
    }
  } while (dyniter_next(it));
  if (last->i < l) {
    k++;
    res = realloc(res, k * sizeof(dynstr*));
    assert(res);

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
  size_t j;
  for (j = 0; j < dst->size; j ++) {
    *pw = *pr++;
    if (*pw == trg){
      i++;
    }
    pw += (*pw != trg);
  }
  *pw = '\0';
  dst->data = realloc(dst->data, sizeof(char) * (dst->r_size - i + 1));
  assert(dst->data);
  dst->size -= i;
  dst->r_size -= i;
}
void
dynstr_replace_c (dynstr     *dst,
                  char        trg,
                  char        src)
{
  assert(dst && dst->data);
  size_t i;
  for (i = 0; i < dst->size; i ++) {
    if (dst->data[i] == trg){
      dst->data[i] = src;
    }
  }
}

/** Iter function **/

dyniter*
dyniter_new      (void)
{
  dyniter * it = malloc(sizeof *it);
  assert(it);
  it->i = 0;
  it->column = 0;
  it->line = 0;
  it->__src__ = NULL;
  return it;
}

dyniter*
dyniter_copy     (dyniter    *iter)
{
  dyniter * res = dyniter_new();
  *res = *iter;
  return res;
}

dyniter*
dynstr_iter      (dynstr     *src)
{
  dyniter * it = dyniter_new();
  it->__src__ = src;
  return it;
}

dyniter*
dynstr_iter_at   (dynstr     *src,
                  size_t      pos)
{
  assert(src && src->size > pos);
  dyniter * r = dynstr_iter(src);
  dyniter_skip(r, pos);
  return r;
}

dyniter*
dynstr_iter_line (dynstr     *src,
                  size_t      line)
{
  assert(src);
  dyniter * it = dynstr_iter(src);
  if (line)
  {
    do {
      if (it->line == line)
        break;
    } while(dyniter_next(it));
    if (it->line != line)
    {
      fprintf(stderr, "ITER_LINE: document have only %lu lines, line %lu not found.\n", it->line, line);
      dyniter_free(it);
      return NULL;
    }
  }
  return it;
}

dyniter*
dynstr_iter_pos  (dynstr     *src,
                  size_t      line,
                  size_t      col)
{
  assert(src);
  dyniter * it = dynstr_iter(src);
  if (line)
  {
    do {
      if (it->line == line && it->column == col)
        break;
      if (it->line > line)
        break;
    } while(dyniter_next(it));
    if (it->line != line || it->column != col)
    {
      dyniter_free(it);
      fprintf(stderr, "ITER_POS: line (%lu) or columns (%lu) not found", it->line, col);
      return NULL;
    }
  }
  return it;
}

void
dyniter_free     (dyniter    *iter)
{
  if (!iter)
    return;
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
dyniter_prev (dyniter    *it)
{
  assert(it && it->__src__);
  if (it->i > 0){
    dynstr * str = it->__src__;
    it->i --;
    if (str->data[it->i] == '\n') {
      it->line --;
      if (it->i) {
        size_t i;
        size_t n = 1;
        for (i = it->i-1; i >0; i--) {
          if (str->data[i] == '\n')
            break;
          n++;
        }
        it->column = i;
      } else {
        it->column = 0;
      }
    }
    else {
      it->column --;
    }
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

char
dyniter_at       (dyniter     it)
{
  assert(it.__src__);
  return it.__src__->data[it.i];
}

ds_bool
dyniter_goto     (dyniter    *it,
                  size_t      n)
{
  assert(it && it->__src__);
  if (n < it->__src__->size)
  {
    if (n < it->i){
      dyniter * b = dynstr_iter_at(it->__src__, n);
      *it = *b;
      free(b);
    } else {
      dyniter_skip(it, n - it->i);
    }
    return TRUE;
  }
  return FALSE;
}

ds_bool
dyniter_go_line  (dyniter    *it,
                  size_t      line)
{
  assert(it && it->__src__);
  dyniter * b = dynstr_iter_line(it->__src__, line);
  if (b)
  {
    *it = *b;
    free(b);
    return TRUE;
  }
  return FALSE;
}

ds_bool
dyniter_go_pos   (dyniter    *it,
                  size_t      line,
                  size_t      col)
{
  assert(it && it->__src__);
  dyniter * b = dynstr_iter_pos(it->__src__, line, col);
  if (b)
  {
    *it = *b;
    free(b);
    return TRUE;
  }
  return FALSE;
}

ds_bool
dyniter_end_line (dyniter    *it)
{
  assert(it);
  do {
    if (dyniter_at(*it) == '\n')
      return TRUE;
  } while (dyniter_next(it));
  return FALSE;
}

char*
dyniter_print    (dyniter    it)
{
  char * buff = malloc(256*sizeof(char));
  assert(buff);
  sprintf(buff, "pos %lu, line %lu, col %lu", it.i, it.line, it.column);
  return buff;
}

char*
dynstr_print     (dynstr     *str)
{
  assert(str);
  size_t n = str->size;
  dynstr * s;
  if (n > PREVIEW) {
    n = PREVIEW;
    s = dynstr_substr_s(str, 0, PREVIEW-3);
    dynstr_puts(s, "...");
  } else {
    s = dynstr_copy(str);
  }
  dynstr_replace_c(s, '\n', '/');
  dynstr_replace_c(s, '\t', ' ');

  char * buff = malloc((256+n)*sizeof(char));
  assert(buff);
  sprintf(buff, "{\"%s\", %lu}", s->data, str->size);
  dynstr_free(s);
  return buff;
}
