#include <check.h>
#include "dynstring.h"

START_TEST (test_from)
{
  /* unit test code */
  const char text[128] = "hello world!";
  dynstr * var = dynstr_from(text);
  ck_assert_str_eq(var->data, text);
  dynstr_free(var);
}
END_TEST

START_TEST (test_buff_size)
{
  /* unit test code */
  dynstr * var = dynstr_from("hello world!");
  ck_assert(var->r_size % BUFFER == 0);
  dynstr_free(var);
}
END_TEST

START_TEST (test_puts_eq)
{
  const char text[128] = "hello world!";
  dynstr * a = dynstr_from(text);
  dynstr * b = dynstr_new();
  dynstr_puts(b, text);
  ck_assert_str_eq(a->data, b->data);
  dynstr_free(a);
  dynstr_free(b);
}
END_TEST

START_TEST(test_put_size)
  const char text[128] = "hello world!";
  dynstr * a = dynstr_from(text);
  dynstr * b = dynstr_new();
  dynstr_puts(b, text);
  ck_assert_int_eq(a->size, b->size);
  dynstr_free(a);
  dynstr_free(b);
END_TEST

START_TEST(test_put_buff)
  const char text[128] = "hello world!";
  dynstr * a = dynstr_from(text);
  dynstr * b = dynstr_new();
  dynstr_puts(b, text);
  ck_assert_int_eq(a->r_size, b->r_size);
  dynstr_free(a);
  dynstr_free(b);
END_TEST

START_TEST(test_concat)
  dynstr * a = dynstr_from("Hello ");
  dynstr * b = dynstr_from("world!");
  dynstr_concat(a, b);
  dynstr_free(b);
  ck_assert_str_eq(a->data, "Hello world!");
  dynstr_free(b);
END_TEST

START_TEST(test_substr)
  dynstr * var = dynstr_from("hello world!");
  dynstr * substr = dynstr_substr_s(var, 0, 5);
  ck_assert_str_eq(substr->data, "hello");
  dynstr_free(var);
  dynstr_free(substr);
END_TEST

Suite * basic_tests_suite(void)
{
  Suite *s;
  TCase *tc_core;

  s = suite_create("Dynamic String: Basic Tests");

  /* Core test case */
  tc_core = tcase_create("Core");

  tcase_add_test(tc_core, test_from);
  tcase_add_test(tc_core, test_buff_size);
  tcase_add_test(tc_core, test_puts_eq);
  tcase_add_test(tc_core, test_put_size);
  tcase_add_test(tc_core, test_put_buff);
  tcase_add_test(tc_core, test_concat);
  tcase_add_test(tc_core, test_substr);

  suite_add_tcase(s, tc_core);

  return s;
}


int main(void)
{
  int number_failed;
  Suite *s;
  SRunner *sr;

  s = basic_tests_suite();
  sr = srunner_create(s);

  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return (number_failed == 0) ? 0 : -1;
}
