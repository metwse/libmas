#include "common.h"
#include "mas.h"
#include "error.h"
#include "../include/grammar.h"
#include "../include/mas.h"

#include <stdbool.h>
#include <stddef.h>


#define aligned_sizeof(ty) \
	((sizeof(ty) + sizeof(size_t) - 1) / sizeof(size_t) * sizeof(size_t))

/* --- mas methods --- */

struct mas *mas_xnew(void)
{
	struct mas *mas = xmalloc(sizeof(struct mas));

	fstack_grammars_xinit(&mas->grammars);

	return mas;
}

void mas_destroy(struct mas *mas)
{
	fstack_grammars_destroy(&mas->grammars);
	free(mas);
}

void mas_xpush_grammar(struct mas *mas, const struct mas_grammar *grammar)
{
	fstack_grammars_xpush(&mas->grammars, grammar);
}


/* --- mas_handle methods --- */

struct mas_handle *mas_xstart(const struct mas *mas)
{
	struct mas_handle *mas_handle = xmalloc(sizeof(struct mas_handle));

	mas_handle->mas = mas;
	fstack_asm_items_xinit(&mas_handle->asm);
	fstack_arg_lexeme_buf_xinit(&mas_handle->arg_lexemes);
	fstack_arg_buf_xinit(&mas_handle->args);
	mas_handle->output_size = 0;

	return mas_handle;
}

void mas_handle_destroy(struct mas_handle *h)
{
	fstack_asm_items_destroy(&h->asm);
	fstack_arg_lexeme_buf_destroy(&h->arg_lexemes);
	fstack_arg_buf_destroy(&h->args);
	free(h);
}

static mas_err_t *mas_handle_xfeed_inner(struct mas_handle *h,
			    const char *inst_string,
			    va_list argp)
{
	/* TODO: handle multiple grammars */
	const struct mas_grammar *g = fstack_grammars_at(&h->mas->grammars, 0);

	mas_grammar_opcode_t opcode;
	mas_err_t *res = g->consume_opcode(&inst_string,
					   &opcode,
					   NULL /* TODO: opcode seminfo */,
					   NULL /* TODO: extensions */);
	return_try(res);
	if (res == (void *) -1) {
		/* TODO: try the next grammar */
	}

	fstack_arg_lexeme_buf_clear(&h->arg_lexemes);
	size_t lexeme_count = 0;
	while (true) {
		void *arg_lexeme = fstack_arg_lexeme_buf_xmultipush(
				&h->arg_lexemes,
				NULL,
				aligned_sizeof(g->sizeof_arg_lexeme));

		mas_err_t *res = g->consume_arg(&inst_string,
						opcode,
						NULL /* TODO: opcode seminfo */,
						arg_lexeme,
						argp,
						NULL /* TODO: extensions */);
		return_try(res);
		if (res == (void *) -1 || inst_string[0] == '\0')
			break;

		lexeme_count++;
	}

	void *arg;
#define new_arg arg = fstack_arg_buf_xmultipush( \
				&h->args, \
				NULL, \
				aligned_sizeof(g->sizeof_arg));
#define clear_args fstack_arg_buf_clear(&h->args);

#define symbol (*(struct mas_grammar_symbol (*) \
	[g->inst_count][g->max_alt_count + 1][g->max_alt_size + 1]) g->grammar)\
	[opcode]

#define current_symbol (*(struct mas_grammar_symbol (*) \
	[g->inst_count][g->max_alt_count + 1][g->max_alt_size + 1]) g->grammar)\
	[opcode][alt_idx][arg_idx]

	mas_grammar_alt_idx_t alt_idx, arg_idx;
	for (alt_idx = 0;
	     symbol[alt_idx][0].ty != MAS_GRAMMAR_SYMBOL_ALTS_END;
	     alt_idx++) {
		clear_args;
		new_arg;

		for (arg_idx = 0;
		     current_symbol.ty != MAS_GRAMMAR_SYMBOL_PARAMS_END &&
			arg_idx < lexeme_count;
		     arg_idx++) {
			/* TODO: regex qualifications */
			const void *arg_lexeme = fstack_arg_lexeme_buf_at(
					&h->arg_lexemes,
					aligned_sizeof(g->sizeof_arg_lexeme));
			void *res = g->validate_arg(
				arg_lexeme,
				current_symbol.data.param_id,
				&arg,
				NULL /* TODO: extensions */);

			if (res == (void *) -1) {
				continue;
			} else if (res) {
				mas_err_free(res); /* TODO: error reporting. */
			} else {
				new_arg;
			}
		}

		/* The next symbol should be end of params sentinel. */
		arg_idx++;
		if (current_symbol.ty == MAS_GRAMMAR_SYMBOL_PARAMS_END &&
		    arg_idx - 1 == lexeme_count  /* we exceeded argument count
						    by exactly one */) {
			/* TODO: push instruction to asm items. */
			return NULL;
		}
	}
	return err_xnew_syntax("No match for the instruction.");
}

 mas_err_t *mas_handle_xfeed(struct mas_handle *h,
			     const char *inst_string,
			     ...)
{
	va_list argp;
	va_start(argp, inst_string);
	mas_err_t *res = mas_handle_xfeed_inner(h, inst_string, argp);
	va_end(argp);
	return res;
}

void mas_handle_yxfeed(struct mas_handle *h,
		       const char *inst_string,
		       ...)
{
	va_list argp;
	va_start(argp, inst_string);
	mas_err_unwrap(mas_handle_xfeed_inner(h, inst_string, argp));
	va_end(argp);
}

mas_err_t *mas_handle_finalize_and_destroy(struct mas_handle *h,
					   char *out);

size_t mas_handle_get_output_size(const struct mas_handle *h);
