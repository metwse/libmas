/* The Thumb-2 grammar. */

#ifndef THUMB2_GRAMMAR_H
#define THUMB2_GRAMMAR_H

#include "opcodes.h"

#include "../../../include/detail.h"
#include "../../../include/error.h"

#include <stdbool.h>
#include <stdint.h>


/* Argument token to be parsed. */
struct inst_arg_tk {
	enum inst_arg_tk_ty {
		ARG_TK_REG,  /* argument is register */
		ARG_TK_REG_VA_STR,  /* read register string from vaargs */
		ARG_TK_REG_VA_NUM,  /* read register number from vaargs */
		ARG_TK_REG_LS,  /* the argument is register list */
		ARG_TK_REG_LS_VA,  /* read register list from vaargs */
		ARG_TK_IMM,  /* the argument is imm */
		ARG_TK_IMM_VA,  /* read imm from vaargs */
		ARG_TK_COND,  /* the argument is execution cond flag */
		ARG_TK_EXCL,  /* used for parsing */
		ARG_TK_BRACE,  /* used for parsing */
		ARG_TK_END,
	} ty;

	union inst_arg_tk_seminfo {
		struct {
			const char *argstr;
			size_t argstr_len;
		} argstr;

		union inst_arg_data parsed;
	} seminfo;
};


_masi const char *const thumb2_mnemonics[INST_COUNT];
_masi const char *const thumb2_mnemonic_cond_suffix[COND_SUFFIX_COUNT];

/* Human-readable parameter names (for debug/grammar dump). */
_masi const char *const thumb2_param_names[PARAM_KIND_COUNT];

/* Instruction-parameter table, for assembly syntax. */
_masi const enum inst_param
thumb2_inst_params[INST_COUNT][MAX_ALTERNATIVE_COUNT + 1][MAX_PARAM_COUNT + 1];


/* Prints assembler grammar, for development debug. */
_masi void thumb2_dump_grammar(void);

/* Extract opcode from the string, consuming its mnemonic. Returns -1 if
 * the mnemonic is not valid.*/
_masi mas_err_t *thumb2_consume_mnemonic(const char **inst,
					 enum inst_opcode *out) _mas_wur;

/* Extract one argument from instruction string.
 * Checks condition suffix if first==true. */
_masi mas_err_t *thumb2_consume_arg_tk(const char **inst,
				       bool first,
				       struct inst_arg_tk *out) _mas_wur;

/* Extract one register argument from instruction string. */
_masi mas_err_t *thumb2_consume_arg_reg_tk(const char **inst,
					   uint32_t *out) _mas_wur;

/* Parse and validate the token list. */
_masi mas_err_t *thumb2_validate(enum inst_opcode,
				 const struct inst_arg_tk *,
				 struct inst *out) _mas_wur;


#endif
