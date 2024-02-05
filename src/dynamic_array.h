#ifndef DYNAMIC_ARRAY_H_
#define DYNAMIC_ARRAY_H_

#include <assert.h>
#include <stdlib.h>

#define DA_GROWTH_FACTOR 2

#define da_push(da, item)                                                          \
    do {                                                                           \
        size_t __da_push_initial_capacity = (da).capacity;                         \
        while ((da).count >= (da).capacity) {                                      \
            (da).capacity *= DA_GROWTH_FACTOR;                                     \
            if ((da).capacity == 0) {                                              \
                (da).capacity = 1;                                                 \
            }                                                                      \
        }                                                                          \
                                                                                   \
        if (__da_push_initial_capacity != (da).capacity) {                         \
            (da).items = realloc((da).items, (da).capacity * sizeof(*(da).items)); \
            assert((da).items && "Unable to increase capacity of dynamic array."); \
        }                                                                          \
                                                                                   \
        (da).items[(da).count] = item;                                             \
        ++(da).count;                                                              \
    } while (0)

#define da_extend(da, src, n)                                                      \
    do {                                                                           \
        size_t __da_extend_initial_capacity = (da).capacity;                       \
        while ((da).count + (n) > (da).capacity) {                                 \
            (da).capacity *= DA_GROWTH_FACTOR;                                     \
            if ((da).capacity == 0) {                                              \
                (da).capacity = 1;                                                 \
            }                                                                      \
        }                                                                          \
                                                                                   \
        if (__da_extend_initial_capacity != (da.capacity)) {                       \
            (da).items = realloc((da).items, (da).capacity * sizeof(*(da).items)); \
            assert((da).items && "Unable to increase capacity of dynamic array."); \
        }                                                                          \
                                                                                   \
        for (size_t __da_extend_i = 0; __da_extend_i < (n); ++__da_extend_i) {     \
            (da).items[(da).count++] = (src)[__da_extend_i];                       \
        }                                                                          \
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
