#include "../include/mas.h"
#include "../plat/thumb2/detail/grammar.h"
#include "../plat/thumb2/detail/opcodes.h"
#include "../plat/thumb2/detail/methods.h"

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>


static void test_valid_lex(struct inst_arg_lexeme *arg_lexeme,
			   enum inst_param param,
			   bool has_output)
{
	union inst_arg arg;

	mas_err_t *res = thumb2_validate_arg(arg_lexeme, param, &arg, NULL);
	mas_err_unwrap(res);

	assert(has_output == (res == NULL));
}

static void test_invalid_lex(struct inst_arg_lexeme *arg_lexeme,
			     enum inst_param param,
			     enum mas_err_code err_code)
{
	union inst_arg arg;

	mas_err_t *err = thumb2_validate_arg(arg_lexeme, param, &arg, NULL);

	assert(mas_err_code(err) == err_code);

	mas_err_free(err);
}

int main(void)
{
	mas_xinit_lib();

	struct inst_arg_lexeme valid_lexemes[] = {
		{ .ty = ARG_LEX_IMM, .seminfo.imm = 1 },
		{ .ty = ARG_LEX_REG, .seminfo.reg = 7 },
		{ .ty = ARG_LEX_REG, .seminfo.reg = 12 },
		{ .ty = ARG_LEX_REG_LS, .seminfo.reg_ls.r1 = 1 },
		{ .ty = ARG_LEX_REG, .seminfo.reg = 13 },
		{ .ty = ARG_LEX_COND, .seminfo.cond_suffix = COND_AL },
		{ .ty = ARG_LEX_EXCL },
		{ .ty = ARG_LEX_BRACE },
	};

	enum inst_param valid_lexeme_params[] = {
		PARAM_IMM8,
		PARAM_LOW_REG,
		PARAM_HIGH_NOT_SP_REG,
		PARAM_LOW_REG_LS,
		PARAM_SP,
		PARAM_COND,
		PARAM_EXCL,
		PARAM_BRACE,
	};

	enum inst_param valid_lexeme_has_output[] = {
		true,
		true,
		true,
		true,
		true,
		true,
		false,
		false,
	};

	for (size_t i = 0;
	     i < sizeof(valid_lexemes) / sizeof(struct inst_arg_lexeme);
	     i++) {
		test_valid_lex(&valid_lexemes[i],
			       valid_lexeme_params[i],
			       valid_lexeme_has_output[i]);
	}

	struct inst_arg_lexeme invalid_lexemes[] = {
		{ .ty = ARG_LEX_REG, .seminfo.reg = 8 },
		{ .ty = ARG_LEX_REG, .seminfo.reg = 13 },
		{ .ty = ARG_LEX_REG, .seminfo.reg = 13 },
		{ .ty = ARG_LEX_REG, .seminfo.reg = 15 },
		{ .ty = ARG_LEX_REG, .seminfo.reg = 0 },
		{ .ty = ARG_LEX_REG, .seminfo.reg = 1 },

		{ .ty = ARG_LEX_REG_LS, .seminfo.reg_ls.pc = 1 },
		{ .ty = ARG_LEX_REG_LS, .seminfo.reg_ls.lr = 1 },
		{ .ty = ARG_LEX_REG_LS, .seminfo.reg_ls.pc = 1 },

		{ .ty = ARG_LEX_IMM, .seminfo.imm = 10 },
		{ .ty = ARG_LEX_IMM, .seminfo.imm = 8 },
		{ .ty = ARG_LEX_IMM, .seminfo.imm = 33 },
		{ .ty = ARG_LEX_IMM, .seminfo.imm = 5 },
		{ .ty = ARG_LEX_IMM, .seminfo.imm = 7 },
		{ .ty = ARG_LEX_IMM, .seminfo.imm = 256 },
		{ .ty = ARG_LEX_IMM, .seminfo.imm = 257 },
		{ .ty = ARG_LEX_IMM, .seminfo.simm = -514 },
		{ .ty = ARG_LEX_IMM, .seminfo.imm = 1256 },
		{ .ty = ARG_LEX_IMM, .seminfo.imm = 1256 },
		{ .ty = ARG_LEX_IMM, .seminfo.imm = UINT32_MAX },
		{ .ty = ARG_LEX_IMM, .seminfo.imm = UINT32_MAX },
	};

	enum inst_param invalid_lexeme_params[] = {
		PARAM_LOW_REG,
		PARAM_HIGH_NOT_SP_REG,
		PARAM_NOT_SP_REG,
		PARAM_NOT_PC_REG,
		PARAM_SP,
		PARAM_PC,

		PARAM_LOW_REG_LS,
		PARAM_POP_REG_LS,
		PARAM_PUSH_REG_LS,

		PARAM_IMM0,
		PARAM_IMM3,
		PARAM_IMM5,
		PARAM_IMM5_00,
		PARAM_IMM7_00,
		PARAM_IMM8,
		PARAM_IMM8_0,
		PARAM_SIMM8_0_LABEL,
		PARAM_IMM8_00,
		PARAM_IMM8_00_LABEL,
		PARAM_SIMM11_0_LABEL,
		PARAM_SIMM24_0_LABEL,
	};

	for (size_t i = 0;
	     i < sizeof(invalid_lexemes) / sizeof(struct inst_arg_lexeme);
	     i++) {
		test_invalid_lex(&invalid_lexemes[i],
				 invalid_lexeme_params[i],
				 MAS_ERR_INV_VALUE);
	}

	mas_deinit_lib();
}
