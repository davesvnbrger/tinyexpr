/*
 * TINYEXPR - Tiny recursive descent parser and evaluation engine in C
 *
 * Copyright (c) 2015-2018 Lewis Van Winkle
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

#ifndef __TINYEXPR_H__
#define __TINYEXPR_H__

#include <stddef.h>

/* Helper macro to be used when defining a te_variable */
#define TE_DEF_VARIABLE(Name, Var) {(Name), {.var=&(Var)}, TE_VARIABLE, NULL}
#define TE_DEF_OFFSET(Name, Offset) {(Name), {.offset=(Offset)}, TE_OFFSET, NULL}
#define TE_DEF_CONSTANT(Name, Value) {(Name), {.value=(Value)}, TE_CONSTANT, NULL}
#define TE_DEF_FUNCTION(Name, Fun, Arity) {(Name), {.f##Arity=(Fun)}, TE_FUNCTION##Arity, NULL}
#ifndef TE_WITHOUT_CLOSURES
#define TE_DEF_CLOSURE(Name, Closure, Arity, Ctx) {(Name), {.cl##Arity=(Closure)}, TE_CLOSURE##Arity, (Ctx)}
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef TE_MAX_FUNCTION_ARITY
  static_assert(TE_MAX_FUNCTION_ARITY >= 3 && TE_MAX_FUNCTION_ARITY <= 7, "valid range for TE_MAX_FUNCTION_ARITY is [3 7]");
#else 
#define TE_MAX_FUNCTION_ARITY 7
#endif


#ifndef TE_WITHOUT_FUNCTION_0
#define FUN_TYPE_0 \
  double(*f0)(void);
#else 
#define FUN_TYPE_0
#endif
#define FUN_TYPES_1_3 \
  void *any;\
  double(*f1)(double);\
  double(*f2)(double, double);\
  double(*f3)(double, double, double);
#if TE_MAX_FUNCTION_ARITY >= 4
#define FUN_TYPE_4 \
  double(*f4)(double, double, double, double);
#else 
#define FUN_TYPE_4
#endif
#if TE_MAX_FUNCTION_ARITY >= 5
#define FUN_TYPE_5 \
  double(*f5)(double, double, double, double, double);
#else 
#define FUN_TYPE_5
#endif
#if TE_MAX_FUNCTION_ARITY >= 6
#define FUN_TYPE_6 \
  double(*f6)(double, double, double, double, double, double);
#else 
#define FUN_TYPE_6
#endif
#if TE_MAX_FUNCTION_ARITY == 7
#define FUN_TYPE_7 \
  double(*f7)(double, double, double, double, double, double, double);
#else 
#define FUN_TYPE_7
#endif

#ifndef TE_WITHOUT_CLOSURES
#define CLOSURE_TYPES \
  double(*cl0)(void*);\
  double(*cl1)(void*, double);\
  double(*cl2)(void*, double, double);\
  double(*cl3)(void*, double, double, double);\
  double(*cl4)(void*, double, double, double, double);\
  double(*cl5)(void*, double, double, double, double, double);\
  double(*cl6)(void*, double, double, double, double, double, double);\
  double(*cl7)(void*, double, double, double, double, double, double, double);
#else 
#define CLOSURE_TYPES
#endif

union fun {
  FUN_TYPE_0
  FUN_TYPES_1_3
  FUN_TYPE_4
  FUN_TYPE_5
  FUN_TYPE_6
  FUN_TYPE_7
  CLOSURE_TYPES
};
  
union value {
  FUN_TYPE_0
  FUN_TYPES_1_3
  FUN_TYPE_4
  FUN_TYPE_5
  FUN_TYPE_6
  FUN_TYPE_7
  CLOSURE_TYPES
  double value;
  const double *var;
  size_t offset;
  const double *bound;
};

typedef struct te_expr {
    int type;
    union value v;
    void *parameters[1];
} te_expr;


enum {
    TE_VARIABLE = 0,
    TE_OFFSET = 1,
    TE_CONSTANT = 2,

#ifndef TE_WITHOUT_FUNCTION_0
    TE_FUNCTION0 = 8,
#endif
    TE_FUNCTION1 = 9, TE_FUNCTION2, TE_FUNCTION3,
#if TE_MAX_FUNCTION_ARITY >= 4
    TE_FUNCTION4,
#endif
#if TE_MAX_FUNCTION_ARITY >= 5
    TE_FUNCTION5,
#endif
#if TE_MAX_FUNCTION_ARITY >= 6
    TE_FUNCTION6,
#endif
#if TE_MAX_FUNCTION_ARITY == 7
    TE_FUNCTION7,
#endif
#ifndef TE_WITHOUT_CLOSURES
    TE_CLOSURE0 = 16, TE_CLOSURE1, TE_CLOSURE2, TE_CLOSURE3,
    TE_CLOSURE4, TE_CLOSURE5, TE_CLOSURE6, TE_CLOSURE7,
#endif
    TE_FLAG_PURE = 64
};

typedef struct te_variable {
    const char *name;
    const union value v;
    int type;
    /* context could be member only if TE_WITHOUT_CLOSURES is undefined
     * we don't take the opportunity as it would add too much complexity */
    void *context;
} te_variable;



/* Parses the input expression, evaluates it, and frees it. */
/* Returns NaN on error. */
double te_interp(const char *expression, int *error);

/* Parses the input expression and binds variables. */
/* Returns NULL on error. */
te_expr *te_compile(const char *expression, const te_variable *variables, int var_count, int *error);

/* Evaluates the expression. */
double te_eval(const te_expr *n, const void* base_addr);

/* Prints debugging information on the syntax tree. */
void te_print(const te_expr *n);

/* Frees the expression. */
/* This is safe to call on NULL pointers. */
void te_free(te_expr *n);


#ifdef __cplusplus
}
#endif

#endif /*__TINYEXPR_H__*/
