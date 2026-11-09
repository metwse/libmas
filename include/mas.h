/**
 * @file mas.h
 * @brief libmas main header.
 */

#ifndef MAS_H
#define MAS_H

#include "error.h"

#include <stddef.h>

struct mas_extension  /* defined in extension.h */;
struct mas_grammar  /* defined in grammar.h */;


/** @brief The assembler. */
struct mas;

/**
 * @brief An assebler instance
 *
 * This struct uses grammars and extensions defined by `mas`, and produce
 * machine code from assembly code.
 *
 * More than one `mas_handle`s can use the same `mas` configuration and produce
 * assembly in parallel. */
struct mas_handle;


/* --- mas methods --- */

/** @brief Initializes the library. */
void mas_xinit_lib(void);

/** @brief Frees the resources owned by libmas. */
void mas_deinit_lib(void);

/** @brief Creates a new assebler. */
struct mas *mas_xnew(void);

/** @brief Frees the resources owned by the assembler. */
struct mas *mas_destroy(void);

/** @brief Add an extension to the assembler. */
void mas_xpush_extension(const struct mas_extension *);

/** @brief Add a grammar to the assembler. */
void mas_xpush_grammar(const struct mas_grammar *);


/* --- mas_handle methods --- */

/** @brief Creates a new assebler. */
struct mas_handle *mas_xstart(const struct mas *mas);

/** @brief Frees the space required by mas. */
void mas_handle_destroy(struct mas_handle *mas_handle);

/** @brief Feed the assembler with an instruction. */
mas_err_t *mas_handle_xfeed(const struct mas_handle *mas_handle,
			    const char *inst_string,
			    ...);

/** @brief Produce the machine code output. */
mas_err_t *mas_handle_finalize_and_destroy(struct mas_handle *mas_handle,
					   char *out);

/** @brief Query required size for the machine code. */
size_t mas_handle_get_output_size(const struct mas_handle *mas_handle);


#endif
