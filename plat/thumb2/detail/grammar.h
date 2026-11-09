/* The Thumb-2 grammar. */

#ifndef THUMB2_GRAMMAR_H
#define THUMB2_GRAMMAR_H

#include "opcodes.h"

#include "../../../include/detail.h"
#include "../../../include/grammar.h"

#include <stdbool.h>
#include <stdint.h>


/* Argument token to be parsed. */
struct inst_arg_lexeme {
	/* lexemes containing _VA got replaced by the lexer */
	enum inst_arg_lexeme_ty {
		ARG_LEX_END,
		ARG_LEX_REG,  /* argument is register */
		ARG_LEX_REG_VA_STR,  /* read register string from vaargs */
		ARG_LEX_REG_VA_NUM,  /* read register number from vaargs */
		ARG_LEX_REG_LS,  /* the argument is register list */
		ARG_LEX_REG_LS_VA,  /* read register list from vaargs */
		ARG_LEX_IMM,  /* the argument is imm */
		ARG_LEX_IMM_VA,  /* read imm from vaargs */
		ARG_LEX_COND,  /* the argument is execution cond flag */
		ARG_LEX_EXCL,  /* used for parsing */
		ARG_LEX_BRACE,  /* used for parsing */
	} ty;

	union inst_arg seminfo;
};


_masi const char *const thumb2_mnemonics[INST_COUNT];
_masi const char *const thumb2_mnemonic_cond_suffix[COND_SUFFIX_COUNT];

/* Instruction-parameter table, for assembly syntax. */
_masi const struct mas_grammar_symbol
thumb2_grammar[INST_COUNT][MAX_ALT_COUNT + 1][MAX_ALT_SIZE + 1];


#endif
