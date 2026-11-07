#include "detail/grammar.h"
#include "detail/opcodes.h"

#include "../../src/common.h"
#include "../../src/error.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>


#define cur_param thumb2_inst_params[res.opcode][res.alt_idx][i]
#define set_err(new_err) do { \
		if (*err) mas_err_free(*err); \
		*err = new_err; \
	} while (0)


static bool validate_reg(const struct inst_arg_tk *arg_in,
			 enum inst_param param,
			 mas_err_t **err)
{
	uint32_t reg = arg_in->seminfo.parsed.reg;

	switch (param) {
	case PARAM_LOW_REG:
		if (reg > 7) {
			*err = err_xnew_inv_value("Expected low register (r0-r7).",
						  0, 7, 0);
			return false;
		}
		break;

	case PARAM_HIGH_NOT_SP_REG:
		if (reg < 8 || reg == 13) {
			*err = err_xnew_inv_value("Expected high register "
						  "excluding sp (r8-r12,r14-r15).",
						  8, 15, 0);
			return false;
		}
		break;

	case PARAM_ANY_REG:
		break;

	case PARAM_NOT_SP_REG:
		if (reg == 13) {
			*err = err_xnew_inv_value("Expected register other "
						  "than sp (r0-r12,r14-r15).",
						  0, 15, 0);
			return false;
		}
		break;

	case PARAM_NOT_PC_REG:
		if (reg == 15) {
			*err = err_xnew_inv_value("Expected register other "
						  "than sp (r0-r14).",
						  0, 14, 0);
			return false;
		}
		break;

	case PARAM_SP:
		if (reg != 13) {
			*err = err_xnew_inv_value("Expected register sp.",
						  13, 13, 0);
			return false;
		}
		break;

	case PARAM_PC:
		if (reg != 15) {
			*err = err_xnew_inv_value("Expected register pc.",
						  15, 15, 0);
			return false;
		}
		break;

	default: _unreachable;
	}

	return true;
}

static bool validate_reg_ls(const struct inst_arg_tk *arg_in,
			    enum inst_param param,
			    mas_err_t **err)
{
	struct inst_arg_data_reg_ls reg_ls = arg_in->seminfo.parsed.reg_ls;

	switch (param) {
	case PARAM_LOW_REG_LS:
		if (reg_ls.lr || reg_ls.pc) {
			*err = err_xnew_inv_value("Cannot have lr or pc here.",
						  0, 0, 0);
			return false;
		}
		break;

	case PARAM_POP_REG_LS:
		if (reg_ls.lr) {
			*err = err_xnew_inv_value("Cannot have lr here.",
						  0, 0, 0);
			return false;
		}
		break;

	case PARAM_PUSH_REG_LS:
		if (reg_ls.pc) {
			*err = err_xnew_inv_value("Cannot have pc here.",
						  0, 0, 0);
			return false;
		}
		break;

	default: _unreachable;
	}

	return true;
}

static bool validate_imm(const struct inst_arg_tk *arg_in,
			 enum inst_param param,
			 mas_err_t **err)
{
	uint32_t imm = arg_in->seminfo.parsed.imm;

	static const uint32_t ranges[] = {
		[PARAM_IMM0] = 0,
		[PARAM_IMM3] = 3,
		[PARAM_IMM5] = 5,
		[PARAM_IMM5_00] = 5,
		[PARAM_IMM7_00] = 7,
		[PARAM_IMM8] = 8,
		[PARAM_IMM8_0] = 8,
		[PARAM_IMM8_0_LABEL] = 8,
		[PARAM_IMM8_00] = 8,
		[PARAM_IMM8_00_LABEL] = 8,
		[PARAM_IMM11_0_LABEL] = 11,
		[PARAM_IMM24_0_LABEL] = 24,
	};

	static const uint32_t pad[] = {
		[PARAM_IMM0] = 0,
		[PARAM_IMM5_00] = 2,
		[PARAM_IMM7_00] = 2,
		[PARAM_IMM8_0] = 1,
		[PARAM_IMM8_0_LABEL] = 1,
		[PARAM_IMM8_00] = 2,
		[PARAM_IMM8_00_LABEL] = 2,
		[PARAM_IMM11_0_LABEL] = 1,
		[PARAM_IMM24_0_LABEL] = 1,
	};

	uint32_t mask = ((1 << ranges[param]) - 1) << (pad[param]);

	if ((mask & imm) != imm) {
		*err = err_xnew_inv_value("IMM is not in valid range.",
					  0,
					  1 << (ranges[param] + pad[param]),
					  1 << pad[param]);
		return false;
	}

	return true;
}

static bool validate_arg(const struct inst_arg_tk *arg_in,  /* argument to parse */
			 union inst_arg_data *arg_out,  /* parsed output */
			 enum inst_param param,  /* parameter of the argument */
			 mas_err_t **err)  /* optional error output*/
{
	switch (arg_in->ty) {
	case ARG_TK_REG:
		if (!is_reg(param)) {
			*err = err_xnew_syntax("Expected register.");
			return false;
		}

		if (!validate_reg(arg_in, param, err))
			return false;

		arg_out->reg = arg_in->seminfo.parsed.reg;
		break;

	case ARG_TK_REG_LS:
		if (!is_reg_ls(param)) {
			*err = err_xnew_syntax("Expected register list.");
			return false;
		}

		if (!validate_reg_ls(arg_in, param, err))
			return false;

		arg_out->reg_ls = arg_in->seminfo.parsed.reg_ls;
		break;

	case ARG_TK_IMM:
		if (!is_imm(param)) {
			*err = err_xnew_syntax("Expected IMM.");
			return false;
		}

		if (!validate_imm(arg_in, param, err))
			return false;

		arg_out->imm = arg_in->seminfo.parsed.imm;
		break;

	case ARG_TK_COND:
		if (param != PARAM_COND) {
			*err = err_xnew_syntax("Expected condition suffix.");
			return false;
		}

		arg_out->cond_suffix = arg_in->seminfo.parsed.cond_suffix;
		break;

	case ARG_TK_REG_VA_STR:
	case ARG_TK_REG_VA_NUM:
	case ARG_TK_REG_LS_VA:
	case ARG_TK_IMM_VA:
		fatal("unexpected vaarg reference in parser");
		break;

	case ARG_TK_EXCL:
		if (param != PARAM_EXCL) {
			*err = err_xnew_syntax("Expected excl mark (!).");
			return false;
		}
		break;
	case ARG_TK_BRACE:
		if (param != PARAM_BRACE) {
			*err = err_xnew_syntax("Expected brace mark ([).");
			return false;
		}
		break;

	case ARG_TK_END:
		return param == PARAME;
		break;
	}

	return true;
}

mas_err_t *thumb2_validate(enum inst_opcode op,
			   const struct inst_arg_tk *args,
			   struct inst *out)
{
	mas_err_t *errs[MAX_ALTERNATIVE_COUNT] = { 0 };
	size_t max_match_i = 0;

	struct inst res = { .opcode = op, .alt_idx = 0, };

	/* iterate through all of the alternative encodings */
	for (; res.alt_idx < MAX_ALTERNATIVE_COUNT; res.alt_idx++) {
		if (thumb2_inst_params[res.opcode][res.alt_idx][0] == PARAME)
			break;

		/* match every parameter in the alternative */
		for (size_t i = 0; i < MAX_PARAM_COUNT + 1; i++) {
			if ((cur_param == PARAME && args[i].ty == ARG_TK_END) ||
			    (cur_param == PARAM_NOPARAM && i == 0)) {
				for (size_t i = 0; i < MAX_ALTERNATIVE_COUNT; i++)
					if (errs[i])
						mas_err_free(errs[i]);
				return_ok(res);
			}

			if (!validate_arg(&args[i],
					  &res.args[i],
					  cur_param,
					  &errs[res.alt_idx])) {
				if (i > max_match_i) {
					max_match_i = i;
					for (size_t j = 0;
					     j < MAX_ALTERNATIVE_COUNT;
					     j++) {
						if (i != j && errs[j]) {
							mas_err_free(errs[j]);
							errs[j] = NULL;
						}
					}
				}

				break;
			}
		}
	}

	mas_err_t *err = err_xnew_list("Invalid instruction.");
	for (size_t i = 0; i < MAX_ALTERNATIVE_COUNT; i++)
		if (errs[i])
			err_push(err, errs[i]);

	return err;
}
