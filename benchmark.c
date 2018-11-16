/*
 * TINYEXPR - Tiny recursive descent parser and evaluation engine in C
 *
 * Copyright (c) 2015, 2016 Lewis Van Winkle
 *
 * http://CodePlea.com
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 * claim that you wrote the original software. If you use this software
 * in a product, an acknowledgement in the product documentation would be
 * appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 * misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */

#include <stdio.h>
#include <time.h>
#include <math.h>
#include <stdlib.h>
#include "tinyexpr.h"



#define default_loops 100000000


typedef double (*function1)(double);

static void bench(const char *expr, function1 func, int count) {
    int i;
    volatile double d;
    static double tmp;
    clock_t start;

    te_variable lk = TE_DEF_VARIABLE("a", tmp);

    printf("Expression: %s\n", expr);

    printf("native: ");
    start = clock();
    d = 0;
    for (i = 0; i < count; ++i) {
        tmp = i;
        d += func(tmp);
    }
    const long int nelapsed = (clock() - start) * 1000 / CLOCKS_PER_SEC;

    /*Million floats per second input.*/
    printf("%.8g", d);
    if (nelapsed)
        printf("\t%5ldms\t%5ldmfps\n", nelapsed, count / nelapsed / 1000);
    else
        printf("\tinf\n");

    printf("interp: ");
    te_expr *n = te_compile(expr, &lk, 1, 0);
    start = clock();
    d = 0;
    for (i = 0; i < count; ++i) {
        tmp = i;
        d += te_eval(n, NULL);
    }
    const long int eelapsed = (clock() - start) * 1000 / CLOCKS_PER_SEC;
    te_free(n);

    /*Million floats per second input.*/
    printf(" %.8g", d);
    if (eelapsed)
        printf("\t%5ldms\t%5ldmfps\n", eelapsed, count / eelapsed / 1000);
    else
        printf("\tinf\n");

    printf("%.2f%% longer\n\n", (((double)eelapsed / (double)nelapsed) - 1.0) * 100.0);
}


static double a5(double a) {
    return a+5;
}

static double a52(double a) {
    return (a+5)*2;
}

static double a10(double a) {
    return a+(5*2);
}

static double as(double a) {
    return sqrt(pow(a, 1.5) + pow(a, 2.5));
}

static double al(double a) {
    return (1/(a+1)+2/(a+2)+3/(a+3));
}

static double ac(double a) {
    return (a > 100 ? 1 : 2);
}

int main(int argc, char *argv[])
{
    int count = default_loops;
    if (argc == 2)
      count = atoi(argv[1]);
    if (count <= 0) {
      printf("Usage: %s [count]\n", argv[0]);
      return 1;
    }
    bench("sqrt(a^1.5+a^2.5)", as, count);
    bench("a+5", a5, count);
    bench("a+(5*2)", a10, count);
    bench("(a+5)*2", a52, count);
    bench("(1/(a+1)+2/(a+2)+3/(a+3))", al, count);
    bench("if(a>100, 1, 2)", ac, count);

    return 0;
}
