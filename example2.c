#include "tinyexpr.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

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
     * x and y are bound at eval-time. */
    te_variable vars[]= { TE_DEF_VARIABLE("x", x), TE_DEF_VARIABLE("y", y) };

    /* This will compile the expression and check for errors. */
    int err;
    n = te_compile(expression, vars, 2, &err);

    if (n) {
      /* Eval can be called as many times as you like.
       * This is fairly efficient because the parsing has already been done. */
      const double r = te_eval(n, NULL);
      printf("Result: %f\n", r);
      if (r != expected_res) {
        printf("Expected result was: %f\n", expected_res);
        goto error;
      }
    } else {
      /* Show the user where the parsing error is. */
      printf("\n\t%s\n", expression);
      printf("\t%*s^\nError near here", err - 1, "");
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
