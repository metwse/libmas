#include "detail/grammar.h"
#include "detail/methods.h"

#include "../../include/error.h"
#include "../../include/grammar.h"
#include "../../src/common.h"
#include "../../src/error.h"
#include "detail/opcodes.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


static mas_err_t *validate_reg(const struct inst_arg_lexeme *arg_lexeme,
			       enum inst_param param_id)
{
	uint32_t reg = arg_lexeme->seminfo.reg;

	switch (param_id) {
	case PARAM_LOW_REG:
		if (reg > 7)
			return_err(inv_value,
				   "Expected low register (r0-r7).",
				   0, 7, 0);
		break;

	case PARAM_HIGH_NOT_SP_REG:
		if (reg < 8 || reg == 13)
			return_err(inv_value,
				   "Expected high register excluding sp "
				   "(r8-r12,r14-r15).",
				   8, 15, 0);
		break;

	case PARAM_ANY_REG:
		break;

	case PARAM_NOT_SP_REG:
		if (reg == 13)
			return_err(inv_value,
				   "Expected register other than sp "
				   "(r0-r12,r14-r15).",
				   0, 15, 0);
		break;

	case PARAM_NOT_PC_REG:
		if (reg == 15)
			return_err(inv_value,
				   "Expected register other than sp (r0-r14).",
				   0, 14, 0);
		break;

	case PARAM_SP:
		if (reg != 13)
			return_err(inv_value,
				   "Expected register sp.",
				   13, 13, 0);
		break;

	case PARAM_PC:
		if (reg != 15)
			return_err(inv_value,
				   "Expected register pc.",
				   15, 15, 0);
		break;

	default: _unreachable;
	}

	return NULL;
}

static mas_err_t *validate_reg_ls(const struct inst_arg_lexeme *arg_lexeme,
				  enum inst_param param_id)
{
	struct inst_arg_data_reg_ls reg_ls = arg_lexeme->seminfo.reg_ls;

	switch (param_id) {
	case PARAM_LOW_REG_LS:
		if (reg_ls.lr || reg_ls.pc)
			return_err(inv_value,
				   "Cannot have lr or pc here.",
				   0, 0, 0);
		break;

	case PARAM_POP_REG_LS:
		if (reg_ls.lr)
			return_err(inv_value,
				   "Cannot have lr here.",
				   0, 0, 0);
		break;

	case PARAM_PUSH_REG_LS:
		if (reg_ls.pc)
			return_err(inv_value,
				   "Cannot have pc here.",
				   0, 0, 0);
		break;

	default: _unreachable;
	}

	return NULL;
}

static mas_err_t *validate_imm(const struct inst_arg_lexeme *arg_lexeme,
			       enum inst_param param_id)
{
	static const bool is_signed[] = {
		[PARAM_SIMM8_0_LABEL] = true,
		[PARAM_SIMM11_0_LABEL] = true,
		[PARAM_SIMM24_0_LABEL] = true,
	};

	static const uint32_t ranges[] = {
		[PARAM_IMM0] = 0,
		[PARAM_IMM3] = 3,
		[PARAM_IMM5] = 5,
		[PARAM_IMM5_00] = 5,
		[PARAM_IMM7_00] = 7,
		[PARAM_IMM8] = 8,
		[PARAM_IMM8_0] = 8,
		[PARAM_SIMM8_0_LABEL] = 8,
		[PARAM_IMM8_00] = 8,
		[PARAM_IMM8_00_LABEL] = 8,
		[PARAM_SIMM11_0_LABEL] = 11,
		[PARAM_SIMM24_0_LABEL] = 24,
	};

	static const uint32_t pad[] = {
		[PARAM_IMM0] = 0,
		[PARAM_IMM5_00] = 2,
		[PARAM_IMM7_00] = 2,
		[PARAM_IMM8_0] = 1,
		[PARAM_SIMM8_0_LABEL] = 1,
		[PARAM_IMM8_00] = 2,
		[PARAM_IMM8_00_LABEL] = 2,
		[PARAM_SIMM11_0_LABEL] = 1,
		[PARAM_SIMM24_0_LABEL] = 1,
	};

	ptrdiff_t max = 1 << (ranges[param_id] + pad[param_id]);
	ptrdiff_t min = 0;

	/* signed min/max range */
	if (is_signed[param_id]) {
		min -= max / 2;
		max = max / 2 - 2;
	} else {
		max -= 1 << pad[param_id];
	}

	int32_t simm = arg_lexeme->seminfo.simm;
	int32_t mask = -1 << pad[param_id];

	if ((mask & simm) != simm || simm < min || max < simm)
		return_err(inv_value,
			   "IMM is not in valid range.",
			   min,
			   max,
			   1 << pad[param_id]  /* step */);

	return NULL;
}

mas_err_t *thumb2_validate_arg(
		const void *arg_lexeme_,
		mas_grammar_param_id_t param_id,
		void *out_arg_,
		_unused struct mas_extension_get_data *ge)
{
	const struct inst_arg_lexeme *arg_lexeme = arg_lexeme_;
	union inst_arg *out_arg = out_arg_;

	switch (arg_lexeme->ty) {
	case ARG_LEX_REG:
		if (!is_reg(param_id))
			return_err(syntax, "Expected register.");

		return_try(validate_reg(arg_lexeme, param_id));

		out_arg->reg = arg_lexeme->seminfo.reg;
		return NULL;

	case ARG_LEX_REG_LS:
		if (!is_reg_ls(param_id))
			return_err(syntax, "Expected register list.");

		return_try(validate_reg_ls(arg_lexeme, param_id));

		out_arg->reg_ls = arg_lexeme->seminfo.reg_ls;
		return NULL;

	case ARG_LEX_IMM:
		if (!is_imm(param_id))
			return_err(syntax, "Expected IMM.");

		return_try(validate_imm(arg_lexeme, param_id));

		out_arg->imm = arg_lexeme->seminfo.imm;
		return NULL;

	case ARG_LEX_COND:
		if (param_id != PARAM_COND)
			return_err(syntax, "Expected condition suffix.");

		out_arg->cond_suffix = arg_lexeme->seminfo.cond_suffix;
		return NULL;

	case ARG_LEX_REG_VA_STR:
	case ARG_LEX_REG_VA_NUM:
	case ARG_LEX_REG_LS_VA:
	case ARG_LEX_IMM_VA:
		fatal("Unexpected vaarg reference in parser.");
		return NULL;

	case ARG_LEX_EXCL:
		if (param_id != PARAM_EXCL)
			return_err(syntax, "Expected excl mark (!).");
		return (void *) -1;
	case ARG_LEX_BRACE:
		if (param_id != PARAM_BRACE)
			return_err(syntax, "Expected brace mark ([).");
		return (void *) -1;

	case ARG_LEX_END:
		return_err(syntax, "Leftover tokens at the end of instruction.");
	}
}
