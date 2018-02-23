#include <check.h>
#include <stdlib.h>

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
  ck_assert_int_eq(a->size, b->size);
  ck_assert_int_eq(a->r_size, b->r_size);
  dynstr_free(a);
  dynstr_free(b);
}
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

START_TEST(test_substr_2)
  dynstr * var = dynstr_from("hello world!");
  dynstr * substr = dynstr_substr_s(var, 6, 11);
  ck_assert_str_eq(substr->data, "world");
  dynstr_free(var);
  dynstr_free(substr);
END_TEST

START_TEST(test_printf)
  dynstr * t = dynstr_new();
  dynstr_printf(t, "The answer is %d.", 42);
  ck_assert_str_eq(t->data, "The answer is 42.");
  dynstr_free(t);
END_TEST

START_TEST(test_printf_2)
  dynstr * t = dynstr_new();
  dynstr_printf(t, "The answer is 41.");
  ck_assert_str_ne(t->data, "The answer is 42.");
  dynstr_free(t);
END_TEST

START_TEST(test_iter)
  dynstr * t = dynstr_from("Lorem ipsum dolor sit amet, consectetur adipiscing elit.");
  dyniter * i = dynstr_iter_at(t, 10);
  ck_assert_int_eq(t->data[10], dyniter_at(*i));
  dynstr_free(t);
  dyniter_free(i);
END_TEST

START_TEST(test_iter_col)
  dynstr * t = dynstr_from("Lorem ipsum dolor sit amet, consectetur adipiscing elit.");
  dyniter * i = dynstr_iter_at(t, 10);
  ck_assert_int_eq(i->column, 10);
  dynstr_free(t);
  dyniter_free(i);
END_TEST

START_TEST(test_iter_line)
  dynstr * t = dynstr_from("Lorem ipsum\n dolor sit amet, consectetur adipiscing elit.");
  dyniter * i = dynstr_iter_at(t, 15);
  ck_assert_int_eq(i->line, 1);
  dynstr_free(t);
  dyniter_free(i);
END_TEST

START_TEST(test_iter_at_line)
  dynstr * t = dynstr_from("Lorem ipsum\n dolor sit amet, consectetur adipiscing elit.");
  dyniter * i = dynstr_iter_line(t, 1);
  ck_assert_int_eq(i->i, 12);
  ck_assert_int_eq(i->line, 1);
  ck_assert_int_eq(i->column, 0);
  dynstr_free(t);
  dyniter_free(i);
END_TEST


START_TEST(test_iter_at_wrong_line)
  dynstr * t = dynstr_from("Lorem ipsum\n dolor sit amet, consectetur adipiscing elit.");
  dyniter * i = dynstr_iter_line(t, 2);
  ck_assert_ptr_null(i);
  dynstr_free(t);
  dyniter_free(i);
END_TEST


START_TEST(test_match)
  dynstr * t = dynstr_from("Lorem ipsum\n dolor sit amet, consectetur adipiscing elit.");
  dyniter * i = dynstr_match(t, "dolor", NULL);
  ck_assert_ptr_nonnull(i);
  ck_assert_int_eq(i->i, 13);
  dyniter_free(i);
  i = dynstr_match(t, "amen", NULL);
  ck_assert_ptr_null(i);
  i = dynstr_match(t, " ", NULL);
  ck_assert_ptr_nonnull(i);
  ck_assert_int_eq(i->i, 5);
  dyniter_next(i);
  dyniter * j = dynstr_match(t, " ", i);
  ck_assert_ptr_nonnull(j);
  ck_assert_int_eq(j->i, 12);
  dyniter_free(j);
  dyniter_free(i);
  dynstr_free(t);
END_TEST

START_TEST(test_match_all)
  dynstr * t = dynstr_from("Lorem ipsum\n dolor sit amet, consectetur adipiscing elit.");
  size_t n;
  dyniter * res = dynstr_match_all(t, " ", &n);
  ck_assert_ptr_nonnull(res);
  ck_assert_int_eq(n , 7);
  ck_assert_int_eq(res[0].i, 5);
  ck_assert_int_eq(res[1].i, 12);
  ck_assert_int_eq(res[2].i, 18);
  free(res);
  res = dynstr_match_all(t, "y", &n);
  ck_assert_ptr_null(res);
  ck_assert_int_eq(n, 0);

  dynstr_free(t);
END_TEST


START_TEST(test_splits)
  dynstr * t = dynstr_from("Lorem ipsum\n dolor sit amet, consectetur adipiscing elit.");
  size_t n;
  dynstr ** res = dynstr_splitc(t, ' ', &n);
  ck_assert_ptr_nonnull(res);
  ck_assert_int_eq(n, 8);
  ck_assert_str_eq(res[0]->data, "Lorem");
  ck_assert_str_eq(res[7]->data, "elit.");
  size_t i;
  for (i = 0; i < n; i++)
  {
    dynstr_free(res[i]);
  }
  free(res);
  res = dynstr_splits(t, " ", &n);
  ck_assert_ptr_nonnull(res);
  ck_assert_int_eq(n, 8);
  ck_assert_str_eq(res[2]->data, "dolor");
  ck_assert_str_eq(res[6]->data, "adipiscing");
  for (i = 0; i < n; i++)
  {
    dynstr_free(res[i]);
  }
  free(res);
  dynstr_free(t);
END_TEST

START_TEST(test_strip)
  dynstr * t = dynstr_from("Lorem ipsum\n dolor sit amet, consectetur adipiscing elit.");
  dynstr_strip(t, ' ');
  ck_assert_ptr_nonnull(t);
  ck_assert_str_eq(t->data, "Loremipsum\ndolorsitamet,consecteturadipiscingelit.");
  dynstr_free(t);
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
  tcase_add_test(tc_core, test_concat);
  tcase_add_test(tc_core, test_substr);
  tcase_add_test(tc_core, test_substr_2);
  tcase_add_test(tc_core, test_printf);
  tcase_add_test(tc_core, test_printf_2);

  tcase_add_test(tc_core, test_iter);
  tcase_add_test(tc_core, test_iter_col);
  tcase_add_test(tc_core, test_iter_line);
  tcase_add_test(tc_core, test_iter_at_line);
  tcase_add_test(tc_core, test_iter_at_wrong_line);

  tcase_add_test(tc_core, test_match);
  tcase_add_test(tc_core, test_match_all);
  tcase_add_test(tc_core, test_splits);
  tcase_add_test(tc_core, test_strip);

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
