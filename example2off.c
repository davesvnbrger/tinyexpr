#include "tinyexpr.h"
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <math.h>
#include <assert.h>

struct my_data {
  char c;
  double y;
  int b;
  double x;
};

int main(int argc, char *argv[])
{
    (void)argc; (void)argv;
    int ret = 0;
    te_expr *n = NULL;

    if (argc != 5)
      goto print_usage;

    static double x = NAN, y = NAN;
    double expected_res = NAN;
    x = atof(argv[2]);
    y = atof(argv[3]);
    expected_res = atof(argv[4]);
    if (x == NAN || y == NAN || expected_res == NAN)
      goto print_usage;

    const char *expression = argv[1];
    printf("Evaluating:\n\t%s\n", expression);
    printf("\tWith x=%g y=%g\n", x, y);

    /* This shows an example where the variables
     * x and y are bound at eval-time using their offset and a base_addr.
     * Such variables have not to exist at compile time. */
    te_variable vars[] = { TE_OFFSET("x", offsetof(struct my_data, x)),
      TE_OFFSET("y", offsetof(struct my_data, y)) };

    /* This will compile the expression and check for errors. */
    int err;
    n = te_compile(expression, vars, 2, &err);

    if (n) {
      /* The actual data could be allocated after expression compile-time.
       * This allows to call the same compiled expression on different data,
       * possibly by-thread data. */
      struct my_data d1;
      d1.x = x; d1.y = y;
      const double r1 = te_eval(n, &d1);
      printf("Result: %f\n", r1);
      if (r1 != expected_res) {
        printf("Expected result was: %f\n", expected_res);
        goto error;
      }
      struct my_data d2;
      d2.x = d1.x; d2.y =d1. y;
      const double r2 = te_eval(n, &d2);
      assert(r1 == r2); (void) r2;
    } else {
        /* Show the user where the parsing error is. */
        printf("\n\t%s\n", expression);
        printf("\t%*s^\nError near here", err-1, "");
        goto error;
    }

  exit:
    if (n) te_free(n);
    return ret;

  print_usage:
    printf("Usage: %s \"expression\" xvalue yvalue expected_result\n", argv[0]);
  error:
    ret = 1; /* Don't return OK if not! */
    goto exit;
}
