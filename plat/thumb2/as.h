/* The ARMv6 Thumb-2 assembler public methods. */

#ifndef MAS_THUMB2_AS_H
#define MAS_THUMB2_AS_H

#include "../../include/as-common.h"
#include "../../include/error.h"
#include "../../include/detail.h"

#include <stdbool.h>
#include <stdarg.h>
#include <stddef.h>

/* An ARMv6 Thumb-2 assembler instance. */
struct mas_thumb2_as;


/* Create a new assembler instance. */
struct mas_thumb2_as *mas_thumb2_as_xnew(enum mas_as_hint hints);

/* Feed an instruction or assembler directive to the assembler. */
mas_err_t *mas_thumb2_as_xfeed(struct mas_thumb2_as *as,
			       const char *inst, ...) _mas_wur;

/* Similar to mas_thumb2_as_xfeed, but raises an error in case of failure. */
void mas_thumb2_as_yxfeed(struct mas_thumb2_as *as, const char *inst, ...);

/* Append the content of another assembler instance.
 *
 * The assembler instance whose contents are appended will be freed. If
 * isolate_scope set, labels of appended instance will not be visible to the
 * extended (parent) assembler. */
mas_err_t *mas_thumb2_as_xappend(struct mas_thumb2_as *as,
				 struct mas_thumb2_as *extend_with,
				 bool isolate_scope) _mas_wur;

/* Similar to mas_thumb2_as_xappend, but raises an error in case of failure. */
static inline void mas_thumb2_as_xyappend(struct mas_thumb2_as *as,
					  struct mas_thumb2_as *extend_with,
					  bool isolate_scope)
{ mas_err_unwrap(mas_thumb2_as_xappend(as, extend_with, isolate_scope)); }

/* Calculate the size of the generated machine code. */
size_t mas_thumb2_as_output_size(const struct mas_thumb2_as *as);

/* Finalize the assembler and emit the generated macihe code.
 *
 * The assembler instance is freed. */
mas_err_t *mas_thumb2_as_finalize(struct mas_thumb2_as *as,
				  void *out) _mas_wur;

/* Similar to mas_thumb2_as_finalize, but raises an error in case of failure. */
static inline void mas_thumb2_as_yfinalize(struct mas_thumb2_as *as, void *out)
{ mas_err_unwrap(mas_thumb2_as_finalize(as, out)); }

/* Free the assembler without finalizing it.
 *
 * This is used if finalizing was failed. */
void mas_thumb2_as_destroy(struct mas_thumb2_as *as);


#endif
