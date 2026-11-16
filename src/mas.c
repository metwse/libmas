#include "common.h"
#include "mas.h"
#include "error.h"
#include "../include/grammar.h"
#include "../include/mas.h"

#include <stdbool.h>
#include <stddef.h>


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
	size_t grammar_idx = 0;
	const struct mas_grammar *g = fstack_grammars_at(&h->mas->grammars,
							 grammar_idx);

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
				g->sizeof_arg_lexeme);

		mas_err_t *res = g->consume_arg(&inst_string,
						opcode,
						NULL /* TODO: opcode seminfo */,
						arg_lexeme,
						argp,
						NULL /* TODO: extensions */);
		if (res == NULL)
			lexeme_count++;
		if (res == (void *) -1 || inst_string[0] == '\0')
			break;
		return_try(res);
	}


#define symbol (*(struct mas_grammar_symbol (*) \
	[g->inst_count][g->max_alt_count + 1][g->max_alt_size + 1]) g->grammar)\
	[opcode]
#define current_symbol (symbol)[alt_idx][symbol_idx]

	mas_grammar_alt_idx_t alt_idx;
	size_t symbol_idx;

	for (alt_idx = 0;
	     symbol[alt_idx][0].ty != MAS_GRAMMAR_SYMBOL_ALTS_END;
	     alt_idx++) {
		char *arg;
#define new_arg arg = fstack_arg_buf_xmultipush(&h->args, NULL, g->sizeof_arg);

		fstack_arg_buf_clear(&h->args);
		new_arg;

		bool break_for = false;
		size_t valid_lexeme_count = 0;

		for (symbol_idx = 0; valid_lexeme_count < lexeme_count; symbol_idx++) {
			switch (current_symbol.ty) {
			case MAS_GRAMMAR_SYMBOL_PARAMS_END:
				break_for = true;
				break;

			case MAS_GRAMMAR_SYMBOL_PARAM_ID: {
				const char *arg_lexeme = fstack_arg_lexeme_buf_at(
						&h->arg_lexemes,
						g->sizeof_arg_lexeme * valid_lexeme_count);
				void *res = g->validate_arg(arg_lexeme,
							    current_symbol.data.param_id,
							    arg,
							    NULL /* TODO: extensions */);

				if (res == (void *) -1) {
					/* no argument is produced by the lexeme */
					valid_lexeme_count++;
				} else if (res) {
					mas_err_free(res);  /* TODO: error reporting. */
					break_for = true;
				} else {
					/* continue with a new argument slot */
					new_arg;
					valid_lexeme_count++;
				}

				break;
			}

			case MAS_GRAMMAR_SYMBOL_NOPARAM:
				break_for = true;
				valid_lexeme_count = 0;
				break;

			case MAS_GRAMMAR_SYMBOL_QUALIFICATION:
				/* TODO: regex qualifications */
				break;

			case MAS_GRAMMAR_SYMBOL_ALTS_END:
				_unreachable; break;
			}

			if (break_for)
				break;
		}

		/* The next symbol should be end of params sentinel. */
		if ((current_symbol.ty == MAS_GRAMMAR_SYMBOL_PARAMS_END ||
		     current_symbol.ty == MAS_GRAMMAR_SYMBOL_NOPARAM) &&
		    lexeme_count == valid_lexeme_count) {
			size_t args_len = fstack_arg_buf_len(&h->args);
			struct asm_item_inst inst = {
				.grammar_idx = grammar_idx,
				.args_len = args_len,
				.opcode = opcode,
				.alt_idx = alt_idx
			};
			const char *args = fstack_arg_buf_at(&h->args, 0);

			fstack_asm_items_xmultipush(&h->asm,
						    (void *) &inst,
						    sizeof(inst));
			fstack_asm_items_xmultipush(&h->asm,
						    args,
						    args_len);

			h->output_size += g->get_inst_size(opcode,
							   args,
							   alt_idx,
							   NULL /* TODO: extensions */);

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
					   char *out)
{
	for (size_t i = 0; i < fstack_asm_items_len(&h->asm); ){
		const struct asm_item_inst *inst =
			(void *) fstack_asm_items_at(&h->asm, i);

		const struct mas_grammar *g =
			fstack_grammars_at(&h->mas->grammars,
					   inst->grammar_idx);

		g->emit(inst->opcode,
			inst->args,
			inst->alt_idx,
			out,
			NULL  /* TODO: extensions */);

		out += g->get_inst_size(inst->opcode,
					inst->args,
					inst->alt_idx,
					NULL /* TODO: extensions */);

		i += inst->args_len + sizeof(struct asm_item_inst);
	}

	mas_handle_destroy(h);
	return NULL;
}

size_t mas_handle_get_output_size(const struct mas_handle *h)
{
	return h->output_size;
}
