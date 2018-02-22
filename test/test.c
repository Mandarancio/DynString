/* file minunit_example.c */

#include <stdio.h>

#include "dynstring.h"

#include "minunit.h"

int tests_run = 0;

static ds_bool
eq_cstr (const char * a,
         const char * b)
{
  return !strcmp(a, b);
}

static ds_bool
eq      (dynstr * a,
         dynstr * b)
{
  return !dynstr_cmpr(a, b);
}

static ds_bool
eq_dvc  (dynstr     *a,
         const char *b)
{
  return eq_cstr(a->data, b);
}


static char * test_from() {
  const char text[128] = "hello world!";
  dynstr * var = dynstr_from(text);
  mu_assert("error, var != text", eq_dvc(var, text));
  dynstr_free(var);
  return 0;
}

static char * test_from_size() {
  const char text[128] = "hello world!";
  dynstr * var = dynstr_from(text);
  mu_assert("error, var.r_size % BUFFER != 0", (var->r_size % BUFFER) == 0);
  dynstr_free(var);
  return 0;
}

static char * test_eq() {
  const char text[128] = "hello world!";
  dynstr * a = dynstr_from(text);
  dynstr * b = dynstr_from(text);
  mu_assert("error, a != b", eq(a, b));
  dynstr_free(a);
  dynstr_free(b);
  return 0;
}

static char * all_tests() {
    mu_run_test(test_from);
    mu_run_test(test_from_size);
    mu_run_test(test_eq);
    return 0;
}

int main(int argc, char **argv) {
    char *result = all_tests();
    if (result != 0) {
        printf("%s\n", result);
    }
    else {
        printf("ALL TESTS PASSED\n");
    }
    printf("Tests run: %d\n", tests_run);

    return result != 0;
}
