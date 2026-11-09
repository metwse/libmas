#include "../include/error.h"
#include "common.h"
#include "error.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


union err_data {
	struct err_data_inv_value {
		ptrdiff_t min;
		ptrdiff_t max;
		ptrdiff_t step;
	} inv_value;

	struct err_data_label_too_far {
		ptrdiff_t offset;
	} label_too_far;
};

struct mas_err {
	struct mas_err *next;
	enum mas_err_code code;
	const char *msg;

	union err_data data /* optional data */;

	char str[];
};


#define rdata(ty) struct err_data_ ## ty ty = err->data.ty


void mas_err_free(mas_err_t *err)
{
	if (err == NULL)
		return;

	do {
		mas_err_t *hold_next = err->next;
		free(err);
		err = hold_next;
	} while (err);
}

void mas_err_unwrap(mas_err_t *err)
{
	if (err == NULL || err == (void *) -1)
		return;

	mas_err_message(err, stderr);

	mas_err_free(err);
	fatal("Aborted due to error.");
}

enum mas_err_code mas_err_code(mas_err_t *err)
{
	return err->code;
}

void mas_err_message(mas_err_t *err, FILE *out)
{
	if (err->msg != NULL)
		fprintf(out, "%s ", err->msg);

	switch (err->code) {
	case MAS_ERR_NO_ERROR:
		return;

	case MAS_ERR_LIST:
		fprintf(out, "There are more than one error!\n");
		while ((err = err->next)) {
			fprintf(out, "> ");
			mas_err_message(err, out);
		}
		break;

	case MAS_ERR_INV_VALUE: {
		rdata(inv_value);

		if (!(inv_value.min == 0 && inv_value.max == 0)) {
			fprintf(out,
				"Range error! (range=[%td, %td]",
				inv_value.min,
				inv_value.max);
			if (inv_value.step)
				fprintf(out, ", step=%td)\n", inv_value.step);
			else
				fprintf(out, ")\n");
		} else {
			fprintf(out, "Range error!\n");
		}
		break;
	}

	case MAS_ERR_INV_MNEMONIC:
		fprintf(out, "Mnemonic error! (mnemonic=%s)\n", (char *) err->str);
		break;

	case MAS_ERR_LABEL_NOT_FOUND:
		fprintf(out, "Label not found! (label=%s)\n", (char *) err->str);
		break;

	case MAS_ERR_LABEL_TOO_FAR: {
		rdata(label_too_far);

		fprintf(out,
			"Label too far away! (label=%s, offset=%td)\n",
			err->str,
			label_too_far.offset);
		break;
	}

	case MAS_ERR_SYNTAX:
		fprintf(out, "Syntax error!\n");
		break;
	}
}

mas_err_t *mas_err_next(mas_err_t *err)
{
	return err->next;
}

#define malloc_err(code2, data_size) \
	mas_err_t *err = xmalloc(sizeof(mas_err_t) + data_size); \
	err->next = NULL; \
	err->code = code2; \
	err->msg = msg; \

#define malloc_err2(code) malloc_err(code, 0);

#define malloc_err3(code, string) \
	malloc_err(code, strlen(string) + 1); \
	memcpy(&err->str, string, strlen(string) + 1)

#define wdata(ty) struct err_data_ ## ty *ty = &err->data.ty

mas_err_t *err_xnew_list(const char *msg)
{
	malloc_err2(MAS_ERR_LIST);

	return err;
}

mas_err_t *err_xnew_inv_value(const char *msg,
			      ptrdiff_t min,
			      ptrdiff_t max,
			      ptrdiff_t opt_step)
{
	malloc_err2(MAS_ERR_INV_VALUE);
	wdata(inv_value);

	inv_value->min = min;
	inv_value->max = max;
	inv_value->step = opt_step;

	return err;
}

mas_err_t *err_xnew_inv_mnemonic(const char *msg,
				 const char *mnemonic)
{
	malloc_err3(MAS_ERR_INV_MNEMONIC, mnemonic);

	return err;
}

mas_err_t *err_xnew_label_not_found(const char *msg,
				    const char *label)
{
	malloc_err3(MAS_ERR_LABEL_NOT_FOUND, label);

	return err;
}

mas_err_t *err_xnew_label_too_far(const char *msg,
				  const char *label,
				  ptrdiff_t offset)
{
	malloc_err3(MAS_ERR_LABEL_TOO_FAR, label);
	wdata(label_too_far);

	label_too_far->offset = offset;

	return err;
}

mas_err_t *err_xnew_syntax(const char *msg)
{
	malloc_err2(MAS_ERR_SYNTAX);

	return err;
}

_masi mas_err_t *err_push(mas_err_t *err, mas_err_t *elem)
{
	while (err->next != NULL)
		err = err->next;

	err->next = elem;
	err = elem;

	while (err->next != NULL)
		err = err->next;

	return err;
}
