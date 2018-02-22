#ifndef __DYNSTRING_H__
#define __DYNSTRING_H__
#include <string.h>

struct dynstr {
  size_t  size;
  char   *data;
} typedef dynstr;

struct dyniter {
  size_t  i;
  size_t  line;
  size_t  column;
  dynstr *__src__;
} typedef dyniter;

enum ds_bool {
  FALSE = 0,
  TRUE  = 1
} typedef ds_bool;

/** constructors functions **/
dynstr*  dynstr_new        (void);
dynstr*  dynstr_from       (const char *src);
dynstr*  dynstr_copy       (dynstr *src);

/** destructor function **/
void     dynstr_free       (dynstr     *str);


/** data access functions **/
size_t   dynstr_len        (dynstr     *str);
char*    dynstr_cstr       (dynstr     *str);

/** insert content functions **/
void     dynstr_puts_n     (dynstr     *dst,
                            const char *src,
                            size_t      n);
void     dynstr_puts       (dynstr     *dst,
                            const char *src);
void     dynstr_concat     (dynstr     *dst,
                            dynstr     *src);
void     dynstr_printf     (dynstr     *dst,
                            const char *fmt,
                            ...) __attribute__ ((format (printf, 2, 3)));

/** Compare functions **/
int      dynstr_cmpr       (dynstr     *a,
                            dynstr     *b);
int      dynstr_scmpr      (dynstr     *a,
                            const char *b);
int      dynstr_cmpr_at    (dynstr     *a,
                            const char *b,
                            dyniter     iter);

/** Match functions **/
dyniter*  dynstr_match     (dynstr     *dst,
                            const char *exp,
                            dyniter    *iter);
dyniter*  dynstr_match_all (dynstr     *str,
                            const char *exp,
                            size_t     *n);

/** Extraction functions **/
dynstr*  dynstr_substr     (dynstr     *str,
                            dyniter     a,
                            dyniter     b);
dynstr*  dynstr_substr_s   (dynstr     *str,
                            size_t      a,
                            size_t      b);
dynstr** dynstr_splits     (dynstr     *dst,
                            const char *exp,
                            size_t     *n);
dynstr** dynstr_splitc     (dynstr     *dst,
                            char        exp,
                            size_t     *n);

/** String manipulation functions **/
void     dynstr_strip     (dynstr     *dst,
                           char        trg);

/** Iter function **/
dyniter* dyniter_new      (void);
dyniter* dynstr_iter      (dynstr     *src);

void     dyniter_free     (dyniter    *iter);

char     dyniter_at       (dyniter     it);

ds_bool  dyniter_next     (dyniter    *it);
ds_bool  dyniter_at_end   (dyniter    *it);
size_t   dyniter_skip     (dyniter    *it,
                           size_t      n);
char*    dyniter_print    (dyniter     it);

#endif

