#include "dynamic_array.h"

#include <assert.h>

typedef struct {
    size_t capacity;
    size_t count;
    int* items;
} DynamicArray;

int main(void) {
    DynamicArray array = {0};
    DynamicArray array2 = {0};
    DynamicArray* array_ptr = &array;

    da_push(array, 1);
    assert(array.count == 1);
    assert(array.capacity == 1);

    da_push(*array_ptr, 2);
    assert(array.count == 2);
    assert(array.capacity == 2);

    da_push(array, 3);
    assert(array.count == 3);
    assert(array.capacity == 4);

    assert(array.items[0] == 1);
    assert(array.items[1] == 2);
    assert(array.items[2] == 3);

    da_extend(array2, &array.items[1], 2);
    assert(array2.count == 2);
    assert(array2.capacity == 2);

    assert(array2.items[0] == 2);
    assert(array2.items[1] == 3);

    da_pop(array);
    assert(array.count == 2);
    assert(array.capacity == 4);

    da_pop(*array_ptr);
    assert(array.count == 1);
    assert(array.capacity == 2);

    da_pop(array);
    assert(array.count == 0);
    assert(array.capacity == 1);

    return 0;
}
