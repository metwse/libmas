#ifndef MAS_ERROR_INTERNAL_H
#define MAS_ERROR_INTERNAL_H

#include "../include/error.h"
#include "../include/detail.h"

#include <stddef.h>
#include <stdint.h>


#define return_ok(...) do { (*out) = (__VA_ARGS__); return NULL; } while (0)
#define return_try(res2) do { \
		mas_err_t *res = res2; \
		if (res != NULL) return res; \
	} while(0)
#define return_err(ty, ...) do { return err_xnew_ ## ty(__VA_ARGS__); } while (0)


_masi mas_err_t *err_xnew_list(const char *msg) _mas_wur;

_masi mas_err_t *err_xnew_inv_value(const char *msg,
				    ptrdiff_t min,
				    ptrdiff_t max,
				    ptrdiff_t opt_step) _mas_wur;

_masi mas_err_t *err_xnew_inv_mnemonic(const char *msg,
				       const char *mnemonic) _mas_wur;

_masi mas_err_t *err_xnew_label_not_found(const char *msg,
					  const char *label) _mas_wur;

_masi mas_err_t *err_xnew_label_too_far(const char *msg,
					const char *label,
					ptrdiff_t offset) _mas_wur;

_masi mas_err_t *err_xnew_syntax(const char *msg) _mas_wur;

/* Push a new error to the linked list, and return the new tail.
 *
 * This operation is similar to array concatenation. */
_masi mas_err_t *err_push(mas_err_t *err, mas_err_t *elem);


#endif
