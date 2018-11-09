#include "tinyexpr.h"
#include <stdio.h>


/* An example of calling a C function. */
static double my_sum(double a, double b) {
    printf("Called C function with %f and %f.\n", a, b);
    return a + b;
}


int main(int argc, char *argv[])
{
    (void)argc; (void)argv;
    int ret = 0;
    te_variable vars[] = { TE_FUNCTION("mysum", my_sum, 2) };

    const char *expression = "mysum(5, 6)";
    printf("Evaluating:\n\t%s\n", expression);

    int err;
    te_expr *n = te_compile(expression, vars, 1, &err);

    if (n) {
      const double r = te_eval(n, NULL);
      const double expected_res = 11;
        printf("Result:\n\t%f\n", r);
        if (r != expected_res) {
          printf("Expected result was: %f\n", expected_res);
          goto error;
        }
    } else {
        /* Show the user where the parsing error is. */
        printf("\t%*s^\nError near here", err-1, "");
        goto error;
    }

  exit:
    if (n) te_free(n);
    return ret;

  error:
    ret = 1; /* Don't return OK if not! */
    goto exit;
}
