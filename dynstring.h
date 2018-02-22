#ifndef __DYNSTRING_H__
#define __DYNSTRING_H__
#include <string.h>

struct dynstr {
  size_t  size;
  char   *data;
} typedef dynstr;

typedef size_t dyniter;

dynstr*  dynstr_new       (void);
dynstr*  dynstr_from      (const char *src);
void     dynstr_puts_n    (dynstr     *dst,
                           const char *src,
                           size_t      n);
void     dynstr_puts      (dynstr     *dst,
                           const char *src);
void     dynstr_cnct      (dynstr     *dst,
                           dynstr     *src);
int      dynstr_cmpr      (dynstr     *a,
                           dynstr     *b);
size_t   dynstr_len       (dynstr     *str);
int      dynstr_scmpr     (dynstr     *a,
                           const char *b);
int      dynstr_cmpr_at   (dynstr     *a,
                           const char *b,
                           dyniter     iter);
void     dynstr_printf    (dynstr     *dst,
                           const char *fmt,
                           ...) __attribute__ ((format (printf, 2, 3)));

dyniter* dynstr_match     (dynstr     *dst,
                           const char *exp,
                           dyniter    *iter);
dyniter* dynstr_match_all (dynstr     *str,
                           const char *exp,
                           size_t     *n);

dynstr*  dynstr_substr    (dynstr     *str,
                           dyniter     a,
                           dyniter     b);
dynstr** dynstr_splits    (dynstr     *dst,
                           const char *exp,
                           size_t     *n);

void     dynstr_strip     (dynstr     *dst,
                           char        trg);
#endif
