#include "../include/error.h"
#include "../include/mas.h"
#include "../plat/thumb2/detail/opcodes.h"
#include "../plat/thumb2/detail/grammar.h"
#include "../plat/thumb2/detail/methods.h"
#include "../src/common.h"

#include <assert.h>
#include <stddef.h>
#include <stdarg.h>


static void test_arg_lexer(const struct inst_arg_lexeme *lexemes,
			   const char *inst,
			   ...)
{
	va_list argp;
	va_start(argp, inst);

	const char **inst_str = &inst;

	struct inst_arg_lexeme lexeme = { .ty = -1 };
	for (size_t i = 0; inst[0]; i++) {
		mas_err_unwrap(thumb2_consume_arg(inst_str,
				   0, NULL, &lexeme, argp, NULL));

		assert(lexeme.ty == lexemes[i].ty);

		switch (lexemes[i].ty) {
		case ARG_LEX_REG:
			assert(lexeme.seminfo.reg ==
				lexemes[i].seminfo.reg);
			break;

		case ARG_LEX_REG_LS:
			assert(lexeme.seminfo.reg_ls.reg_ls ==
				lexemes[i].seminfo.reg_ls.reg_ls);
			break;

		case ARG_LEX_IMM:
			assert(lexeme.seminfo.imm ==
				lexemes[i].seminfo.imm);
			break;

		case ARG_LEX_COND:
			assert(lexeme.seminfo.cond_suffix ==
				lexemes[i].seminfo.cond_suffix);
			break;

		case ARG_LEX_REG_VA_STR:
		case ARG_LEX_REG_VA_NUM:
		case ARG_LEX_REG_LS_VA:
		case ARG_LEX_IMM_VA:
		case ARG_LEX_EXCL:
		case ARG_LEX_BRACE:
			break;

		case ARG_LEX_END:
			fatal("Unexpected early end of input.");
		}
	}

	assert2(thumb2_consume_arg(inst_str,
				   0, NULL, &lexeme, argp, NULL) ==
					(void *) -1,
		"Instruction did not finished!");

	va_end(argp);
}

int main(void)
{
	mas_xinit_lib();

	enum inst_opcode opcode = -1;
	enum inst_opcode opcodes[] = { OP_ADD, OP_ADDS, OP_B, OP_B, OP_BL };
	const char *mnemonics[] = { "add  ", "adds ", "bne", "b", "bl" };

	for (size_t i = 0; i < sizeof(opcodes) / sizeof(enum inst_opcode); i++) {
		mas_err_unwrap(thumb2_consume_opcode(&mnemonics[i],
						     &opcode,
						     NULL,
						     NULL));
		assert(opcode == opcodes[i]);
	}

	const char *inv_mnemonics[] = { "aaaaaa  ", "invalid ", "", "ad" };
	for (size_t i = 0; i < sizeof(inv_mnemonics) / sizeof(char *); i++) {
		mas_err_t *err = thumb2_consume_opcode(&inv_mnemonics[i],
						       &opcode,
						       NULL,
						       NULL);
		assert(mas_err_code(err) == MAS_ERR_INV_MNEMONIC);
		mas_err_free(err);
	}

	test_arg_lexer((struct inst_arg_lexeme []) {
				{ .ty = ARG_LEX_COND, .seminfo.cond_suffix = COND_NE },
				{ .ty = ARG_LEX_REG, .seminfo.reg = 1 },
				{ .ty = ARG_LEX_REG, .seminfo.reg = 15 },
				{ .ty = ARG_LEX_REG, .seminfo.reg = 14 },
				{ .ty = ARG_LEX_REG_LS, .seminfo.reg_ls = { .r0 = 1, .r1 = 1 } },
				{ .ty = ARG_LEX_REG_LS, .seminfo.reg_ls = { .pc = 1, .r4 = 1 } },
				{ .ty = ARG_LEX_IMM, .seminfo.simm = -3 },
			},
			"ne r1, pc, lr, %l, { pc, r4 }, #-3", 1 | 2);

	test_arg_lexer((struct inst_arg_lexeme []) {
				{ .ty = ARG_LEX_IMM, .seminfo.imm = 123 },
				{ .ty = ARG_LEX_REG, .seminfo.reg = 11 },
				{ .ty = ARG_LEX_IMM, .seminfo.simm = -321 },
				{ .ty = ARG_LEX_REG, .seminfo.reg = 2 },
				{ .ty = ARG_LEX_REG, .seminfo.reg = 13 },
			},
			"%i, r11, %i, %r, %R", 123, -321, 2, "sp");

	test_arg_lexer((struct inst_arg_lexeme []) {
				{ .ty = ARG_LEX_COND, .seminfo.cond_suffix = COND_EQ },
				{ .ty = ARG_LEX_REG, .seminfo.reg = 1 },
				{ .ty = ARG_LEX_EXCL },
				{ .ty = ARG_LEX_BRACE },
				{ .ty = ARG_LEX_REG, .seminfo.reg = 15 },
				{ .ty = ARG_LEX_IMM, .seminfo.imm = 32 },
			},
			"eq r1!, [pc, #32]");

	mas_deinit_lib();
}
