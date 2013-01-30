#ifndef __ISOLE_H
#define __ISOLE_H

#include <stdlib.h>
#include <limits.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#ifndef FLEX_ARRAY
/*
 * See if our compiler is known to support flexible array members.
 */
#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L) && (!defined(__SUNPRO_C) || (__SUNPRO_C > 0x580))
# define FLEX_ARRAY /* empty */
#elif defined(__GNUC__)
# if (__GNUC__ >= 3)
#  define FLEX_ARRAY /* empty */
# else
#  define FLEX_ARRAY 0 /* older GNU extension */
# endif
#endif

/*
 * Otherwise, default to safer but a bit wasteful traditional style
 */
#ifndef FLEX_ARRAY
# define FLEX_ARRAY 1
#endif
#endif


#ifndef UINTMAX_MAX
#define UINTMAX_MAX _UI64_MAX
#endif

#define ARRAY_SIZE(x) (sizeof(x)/sizeof(x[0]))
#define bitsizeof(x)  (CHAR_BIT * sizeof(x))

#define maximum_signed_value_of_type(a) \
      (INTMAX_MAX >> (bitsizeof(intmax_t) - bitsizeof(a)))

#define maximum_unsigned_value_of_type(a) \
      (UINTMAX_MAX >> (bitsizeof(uintmax_t) - bitsizeof(a)))

/*
 * Signed integer overflow is undefined in C, so here's a helper macro
 * to detect if the sum of two integers will overflow.
 *
 * Requires: a >= 0, typeof(a) equals typeof(b)
 */
#define signed_add_overflows(a, b) \
      ((b) > maximum_signed_value_of_type(a) - (a))

#define unsigned_add_overflows(a, b) \
      ((b) > maximum_unsigned_value_of_type(a) - (a))


// TODO ----------------------------------------------------------
void *xmallocz(size_t size);
int error(const char *err, ...);

#endif // __ISOLE_H

