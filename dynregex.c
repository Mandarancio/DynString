#include "dynstring.h"


regex mono (char c) {
  exrange *range = malloc(sizeof *range);
  range[0] = (exrange){c, c, POSITIVE};
  regex rule = {1, ONE, range};

  return rule;
}

regex*
parse_regex       (const char *reg,
                   size_t     *size)
{
  size_t l = strlen(reg);
  regex* arr = NULL;
  size_t n = 0;
  size_t i;
  for (i = 0; i < l; i++) {
    if (reg[i] =='(') {
      if (i > 0 && reg[i-1] == '\\') {
        regex r = mono('(');
        n++;
        arr = realloc(arr, n * sizeof *arr);
        arr[n-1] = r;
      } else {
        /** MATCH RULE **/
        char prev = reg[i];
        i++;
        exarity arity = ONE;
        if (reg[i] == '#') {
          arity = MANY;
          i++;
          prev = reg[i];
        }
        char start = 0;
        char end = 0;

        if (prev != ')') {

          exrange * ranges = NULL;
          size_t rn = 0;
          exsign sign = POSITIVE;
          accmode mode = OR;
          do {
            if (reg[i] == ')' && prev != '\\') {
              if (start) {
                exrange r;
                if (end != 0)
                  r = (exrange){start, end, sign, mode};
                else
                  r = (exrange){start, start, sign, mode};
                rn++;
                ranges = realloc(ranges, rn * sizeof *ranges);
                ranges[rn-1] = r;
              }
              break;
            }
            if (reg[i] == ',') {
              if (reg[i-1] == '\\') {
                if (start == 0)
                  start = ',';
                else
                  end = ',';
              } else {
                if (start) {
                  exrange r;
                  if (end != 0)
                    r = (exrange){start, end, sign, mode};
                  else
                    r = (exrange){start, start, sign, mode};
                  rn++;
                  ranges = realloc(ranges, rn * sizeof *ranges);
                  ranges[rn-1] = r;
                }
                sign = POSITIVE;
                mode = OR;
                start = 0;
                end = 0;
              }
            } else if (reg[i] == '&') {
              if (start) {
                exrange r;
                if (end != 0)
                  r = (exrange){start, end, sign, mode};
                else
                  r = (exrange){start, start, sign, mode};
                rn++;
                ranges = realloc(ranges, rn * sizeof *ranges);
                ranges[rn-1] = r;
              }
              sign = POSITIVE;
              mode = AND;
              start = 0;
              end = 0;
            } else if (prev == '\\') {
              if (start == 0)
                start = reg[i];
              else
                end = reg[i];
            } else if (reg[i] == '!') {
              sign = NEGATIVE;
            } else if (reg[i] == '*') {
              start = -128;
              end = 127;
            }else if (reg[i] != ':' && reg[i] != '\\') {
              if (start == 0)
                start = reg[i];
              else
                end = reg[i];
            }
            prev = reg[i];
            i++;
          } while (i < l);
          if (rn > 0) {
            regex r = {rn, arity, ranges};
            n++;
            arr = realloc(arr, n * sizeof *arr);
            arr[n-1] = r;
          }
        }
      }
    } else if (reg[i] == '\\') {
      if (!(i < l -1 && reg[i+1] == '(')) {
        regex r = mono(reg[i]);
        n++;
        arr = realloc(arr, n * sizeof *arr);
        arr[n-1] = r;
      }
    } else {
      regex r = mono(reg[i]);
      n++;
      arr = realloc(arr, n * sizeof *arr);
      arr[n-1] = r;
    }
  }

  if (size)
    *size = n;
  return arr;
}

ds_bool
in_range (char    c,
         exrange range)
{
  if (c >= range.start && c <= range.end)
    return range.sign == POSITIVE;
  return range.sign == NEGATIVE;
}

ds_bool
match (char  c,
       regex rule)
{
  size_t i;
  ds_bool res = FALSE;
  for (i = 0; i < rule.n; i++) {
    if (rule.ranges[i].mode == OR) {
      res = res || in_range(c, rule.ranges[i]);
    } else {
      res = res && in_range(c, rule.ranges[i]);
    }
  }
  return res;
}

void
print_range(exrange r)
{
  if (r.mode == AND)
    printf("&& ");
  if (r.sign == NEGATIVE)
    printf("!");

  if (r.start == r.end)
    printf("%c\n", r.start);
  else
    printf("(%c-%c)\n", r.start, r.end);
}

void
print_rule(regex r)
{
  if (r.n == 0)
    printf("rule: ERROR\n");
  else if (r.n == 1) {
    printf("rule(%c): ", (r.arity == ONE ? '1' : '#'));
    print_range(r.ranges[0]);
  } else {
    printf("rule(%c):\n", (r.arity == ONE ? '1' : '#'));
    size_t i;
    for (i = 0; i < r.n; i++) {
      printf(" - ");
      print_range(r.ranges[i]);
    }
  }
}

ds_bool
dynstr_exp        (dyniter     start,
                   const char *exp,
                   dyniter    *end)
{
  size_t n;
  regex * rules = parse_regex(exp, &n);
  return dynstr_regex(start, rules, n, end);
}

ds_bool
dynstr_regex      (dyniter     start,
                   regex      *rules,
                   size_t      n,
                   dyniter    *end)
{
  if (n == 0)
    return FALSE;

  dyniter e = start;
  size_t i = 0;

  do {
    if (!match(dyniter_at(e), rules[i])) {
      if (rules[i].arity == ONE)
        return FALSE;
      else {
        i++;
        dyniter_prev(&e);
      }
    } else if (rules[i].arity == ONE)
      i++;
  } while (i < n && dyniter_next(&e));
  if (i < n - 1)
    return FALSE;
  if (end)
    *end = e;
  return TRUE;
}
