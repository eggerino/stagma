#ifndef DYNAMIC_ARRAY_H_
#define DYNAMIC_ARRAY_H_

#include <assert.h>
#include <stdlib.h>

#define DA_GROWTH_FACTOR 2

#define da_push(da, item)                                                          \
    do {                                                                           \
        while ((da).count >= (da).capacity) {                                      \
            (da).capacity *= DA_GROWTH_FACTOR;                                     \
            if ((da).capacity == 0) {                                              \
                (da).capacity = 1;                                                 \
            }                                                                      \
            (da).items = realloc((da).items, (da).capacity * sizeof(*(da).items)); \
            assert((da).items && "Unable to increase capacity of dynamic array."); \
        }                                                                          \
        (da).items[(da).count++] = item;                                           \
    } while (0)

#define da_pop(da)                                                                 \
    do {                                                                           \
        --(da).count;                                                              \
        if ((da).count < (da).capacity / DA_GROWTH_FACTOR) {                       \
            (da).capacity /= DA_GROWTH_FACTOR;                                     \
            (da).items = realloc((da).items, (da).capacity * sizeof(*(da).items)); \
            assert((da).items && "Unable to decrease capacity of dynamic array."); \
        }                                                                          \
    } while (0)

#endif
