/**
 * @file Vector.h
 * @time 18/04/2024 1:25:00
 * @author Siddharth Mishra (admin@brightprogrammer.in)
 * @copyright Copyright (c) 2024 Siddharth Mishra
 * @copyright Copyright (c) 2024 Anvie Labs
 */

#ifndef ANVIE_CROSSGUI_UTILS_VECTOR_H
#define ANVIE_CROSSGUI_UTILS_VECTOR_H

#include <Anvie/Types.h>

/* This can be used for memory management of vector data types.
 * Any type is of vector type if it requires dynamic allocation
 * for storage of multiple objects of same type in a contiguous
 * memory region.
 */

void *vector_create (Size entry_size, Size entry_count);
void  vector_destroy (void *vec);
void *vector_resize (void *vec, Size entry_size, Size old_count, Size new_count);

/**
 * @b Use this to define interface/wrapper methods to interact with
 * type-specific vectors.
 * */
#define NEW_VECTOR_TYPE(tname, prefix)                                                             \
    static inline tname *prefix##_vector_create (Size entry_count) {                               \
        return vector_create (sizeof (tname), entry_count);                                        \
    }                                                                                              \
                                                                                                   \
    static inline void prefix##_vector_destroy (tname *vec) {                                      \
        return vector_destroy ((void *)vec);                                                       \
    }                                                                                              \
                                                                                                   \
    static inline tname *prefix##_vector_resize (tname *vec, Size from_count, Size to_count) {     \
        return vector_resize (vec, sizeof (tname), from_count, to_count);                          \
    }

#endif // CROSSGUI_UTILS_VECTOR_H
