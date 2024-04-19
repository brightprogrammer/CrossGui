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
 *
 * @return Pointer to new vector data on success.
 * @return @c Null otherwise.
 * */
void *vector_create (Size entry_size, Size entry_count) {
    RETURN_VALUE_IF (!entry_size || !entry_count, Null, ERR_INVALID_ARGUMENTS);

    void *vec = calloc (entry_size, entry_count);
    RETURN_VALUE_IF (!vec, Null, ERR_OUT_OF_MEMORY);

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
 *
 * @return Pointer to new vector memory region if reallocated,
 *         otherwise @c vec will be returned.
 * @return @c Null otherwise. This includes resize failure.
 * */
void *vector_resize (void *vec, Size entry_size, Size old_count, Size new_count) {
    RETURN_VALUE_IF (!entry_size || !new_count, Null, ERR_INVALID_ARGUMENTS);

    vec = realloc (vec, entry_size * new_count);
    RETURN_VALUE_IF (!vec, Null, ERR_OUT_OF_MEMORY);

    if (old_count != new_count) {
        memset (vec + entry_size * old_count, 0, entry_size * (new_count - old_count));
    }

    return vec;
}
