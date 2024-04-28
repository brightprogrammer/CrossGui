#include <Anvie/Common.h>
#include <Anvie/CrossGui/Utils/Vector.h>
#include <Anvie/Types.h>

/* libc headers */
#include <memory.h>

/**
 * @b Allocate memory for dynamic array (aka vector).
 *
 * @param entry_size Size of each entry in the vector.
 * @param entry_count Total number of entries to allocate space for.
 * @param capacity Pointer to @c Size variable where vector capacity will be stored.
 *
 * @return Pointer to new vector data on success.
 * @return @c Null otherwise.
 * */
void *vector_create (Size entry_size, Size entry_count, Size *capacity) {
    RETURN_VALUE_IF (!entry_size || !capacity, Null, ERR_INVALID_ARGUMENTS);

    /* default cap to 4 */
    Size cap = entry_count ? entry_count : 4;

    void *vec = calloc (entry_size, cap);
    RETURN_VALUE_IF (!vec, Null, ERR_OUT_OF_MEMORY);

    /* store cap */
    *capacity = cap;

    return vec;
}

/**
 * @b Destroy given vector memory region.
 *
 * @param vec Vector to be freed.
 * */
void vector_destroy (void *vec) {
    RETURN_IF (!vec, ERR_INVALID_ARGUMENTS);

    FREE (vec);
}

/**
 * @b Resize given vector memory region.
 *
 * Old count and new count is required to memset the new allocated
 * region to 0. This memset operation can be skipped by setting
 * @c old_count = @c new_count.
 *
 * @param vec Pointer to vector memory region to be resized.
 * @param entry_size Size of each entry in vector.
 * @param old_count Previous vector capacity.
 * @param new_count New vector capacity to resize to.
 * @param old_capacity Old capacity of given vector.
 * @param new_capacity Pointer to @c Size variable, where new capacity of vector will be stored.
 *
 * @return Pointer to new vector memory region if reallocated,
 *         otherwise @c vec will be returned.
 * @return @c Null otherwise. This includes resize failure.
 * */
void *vector_resize (
    void *vec,
    Size  entry_size,
    Size  old_count,
    Size  new_count,
    Size  old_capacity,
    Size *new_capacity
) {
    RETURN_VALUE_IF (!entry_size || !new_count || !new_capacity, Null, ERR_INVALID_ARGUMENTS);

    if (new_count == old_count) {
        return vec;
    }

    /* by default, new capacty will be 4 */
    Size new_cap = old_capacity ? old_capacity : 4;
    while (new_cap < new_count) {
        new_cap *= 2;
    }

    vec = realloc (vec, entry_size * new_cap);
    RETURN_VALUE_IF (!vec, Null, ERR_OUT_OF_MEMORY);

    /* zero-out new memory region */
    memset (vec + entry_size * old_count, 0, entry_size * (new_cap - old_count));

    /* store new capacity */
    *new_capacity = new_cap;

    return vec;
}
