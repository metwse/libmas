/**
 * @file osal.h
 * @brief Operating system abstraction layer, OSAL.
 */

#ifndef OSAL_H
#define OSAL_H

#include "../include/detail.h"

#include <assert.h>
#include <stddef.h>


/** @brief Executable memory protections. */
enum mas_execmem_mode {
	MAS_EXECMEM_W  /** write */,
	MAS_EXECMEM_RE  /** read-execute */,
};

/** @brief Executable memory. */
struct mas_execmem {
	/** Should only be changed via mas_execmem_set_mode() */
	enum mas_execmem_mode mode;

	union {
		void *mem  /** Writable memory. */;
		void (*execmem)(void)  /** Readable and executable memory. */;
	};

	size_t size  /** Size of the memory allocation. */;
};


/**
 * @brief Allocates at least size bytes page-aligned execmem.
 *
 * Returned memory is not executable. Returns non-zero in case of a failure.
 */
int mas_execmem_alloc(struct mas_execmem *out, size_t size) _mas_wur;

/** @brief Free the executable memory. */
void mas_execmem_free(struct mas_execmem *e);

/** @brief Set W/RE mode of execmem. */
void mas_execmem_set_mode(struct mas_execmem *e, enum mas_execmem_mode mode);

/** @brief Similar to mas_execmem_alloc, but raises an erorr if memory
 * allocation fails. */
static inline void mas_execmem_xalloc(struct mas_execmem *out, size_t size)
{
	assert(mas_execmem_alloc(out, size) == 0 && "failed to allocate execmem");
}


#endif
