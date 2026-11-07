/* libmas error types. */

#ifndef MAS_ERR_H
#define MAS_ERR_H

#include <stddef.h>
#include <stdio.h>


/* Error type. Underlying implementation is a linked list of error messages. */
typedef struct mas_err mas_err_t;

enum mas_err_code {
	MAS_ERR_NO_ERROR  /* This value indicates success. */,
	MAS_ERR_LIST  /* The returned error contains more than one error. */,
	MAS_ERR_INV_VALUE  /* Provided value is out of range. */,
	MAS_ERR_INV_MNEMONIC  /* Invalid mnemonic. */,
	MAS_ERR_LABEL_NOT_FOUND  /* Missing a label. */,
	MAS_ERR_LABEL_TOO_FAR  /* Label not in required range. */,
	MAS_ERR_SYNTAX  /* Generic synax error. */,
};


/* Frees the error type.
 *
 * Freeing only head is enough for freeing up a error list. */
void mas_err_free(mas_err_t *);

/* Raises the error and halts the program if it is not NULL. */
void mas_err_unwrap(mas_err_t *);

/* Returns the error code. */
enum mas_err_code mas_err_code(mas_err_t *);

/* Prints out error message. */
void mas_err_message(mas_err_t *, FILE *);

/* Returns the next error in the list. */
mas_err_t *mas_err_next(mas_err_t *);


#endif
