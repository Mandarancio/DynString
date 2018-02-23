#ifndef __DYNSTRING_H__
#define __DYNSTRING_H__
#include <string.h>

#define BUFFER 256

struct dynstr {
  size_t  size;
  size_t  r_size;
  char   *data;
} typedef dynstr;

struct dyniter {
  size_t  i;
  size_t  line;
  size_t  column;
  dynstr *__src__;
} typedef dyniter;

struct dynrange {
  dyniter start;
  dyniter end;
} typedef dynrange;

enum ds_bool {
  FALSE = 0,
  TRUE  = 1
} typedef ds_bool;

/** constructors functions **/
dynstr*  dynstr_new        (void);
dynstr*  dynstr_new_size   (size_t      size);
dynstr*  dynstr_from       (const char *src);
dynstr*  dynstr_copy       (dynstr     *src);

/** destructor function **/
void     dynstr_free       (dynstr     *str);

/** mem utility **/
void     dynstr_grow       (dynstr     *str,
                            size_t      n);
void     dynstr_dgrow      (dynstr     *str,
                            size_t      n);


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
dynstr*  dynstr_substr     (dyniter     a,
                            dyniter     b);
dynstr*  dynstr_substr_s   (dynstr     *str,
                            size_t      a,
                            size_t      b);
dynstr*  dynstr_substr_r   (dynrange    range);
dynstr** dynstr_splits     (dynstr     *dst,
                            const char *exp,
                            size_t     *n);
dynstr** dynstr_splitc     (dynstr     *dst,
                            char        exp,
                            size_t     *n);

/** String manipulation functions **/
void     dynstr_strip     (dynstr     *dst,
                           char        trg);
void     dynstr_replace_c (dynstr     *dst,
                           char        trg,
                           char        src);

/** Iter function **/
dyniter* dyniter_new      (void);
dyniter* dyniter_copy     (dyniter    *iter);
dyniter* dynstr_iter      (dynstr     *src);
dyniter* dynstr_iter_at   (dynstr     *src,
                           size_t      pos);
dyniter* dynstr_iter_line (dynstr     *src,
                           size_t      line);
dyniter* dynstr_iter_pos  (dynstr     *src,
                           size_t      line,
                           size_t      col);

void     dyniter_free     (dyniter    *iter);

char     dyniter_at       (dyniter     it);

ds_bool  dyniter_goto     (dyniter    *it,
                           size_t      n);
ds_bool  dyniter_go_line  (dyniter    *it,
                           size_t      line);
ds_bool  dyniter_go_pos   (dyniter    *it,
                           size_t      line,
                           size_t      col);
void     dyniter_end_line (dyniter    *it);

ds_bool  dyniter_next     (dyniter    *it);
ds_bool  dyniter_prev     (dyniter    *it);
ds_bool  dyniter_at_end   (dyniter    *it);
size_t   dyniter_skip     (dyniter    *it,
                           size_t      n);

/** PRINT FUNCTIONS **/
char*    dyniter_print    (dyniter     it);
char*    dynstr_print     (dynstr     *str);
#endif

