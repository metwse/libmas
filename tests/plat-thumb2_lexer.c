#include "../include/mas.h"
#include "../plat/thumb2/detail/grammar.h"
#include "../plat/thumb2/detail/opcodes.h"
#include "../src/common.h"


static void test_lex(const char *inst,
		     enum inst_opcode op,
		     struct inst_arg_tk *it)
{
	enum inst_opcode parse_op = 0;
	mas_err_unwrap(thumb2_consume_mnemonic(&inst, &parse_op));
	struct inst_arg_tk pit[MAX_PARAM_COUNT + 1];

	assert2(parse_op == op, "opcode mismatch");

	for (size_t i = 0; i < MAX_PARAM_COUNT + 1; i++) {
		mas_err_unwrap(thumb2_consume_arg_tk(&inst, i == 0, &pit[i]));

		assert2(pit[i].ty == it[i].ty,
			"parsed token type do not match with testsuite");

		switch (pit[i].ty) {
		case ARG_TK_REG:
			assert2(pit[i].seminfo.parsed.reg ==
				it[i].seminfo.parsed.reg,);
			break;

		case ARG_TK_REG_LS:
			assert2(pit[i].seminfo.parsed.reg_ls.reg_ls ==
				it[i].seminfo.parsed.reg_ls.reg_ls,);
			break;

		case ARG_TK_IMM:
			assert2(pit[i].seminfo.parsed.imm ==
				it[i].seminfo.parsed.imm,);
			break;

		case ARG_TK_COND:
			assert2(pit[i].seminfo.parsed.cond_suffix ==
				it[i].seminfo.parsed.cond_suffix,);
			break;

		case ARG_TK_REG_VA_STR:
		case ARG_TK_REG_VA_NUM:
		case ARG_TK_REG_LS_VA:
		case ARG_TK_IMM_VA:
		case ARG_TK_EXCL:
		case ARG_TK_BRACE:
		case ARG_TK_END:
			break;
		}

		if (pit[i].ty == ARG_TK_END)
			break;
	}
}

int main(void)
{
	mas_xinit();

	test_lex("adds r1, r3",
		 OP_ADDS,
		 (struct inst_arg_tk []) {
			 { .ty = ARG_TK_REG, .seminfo.parsed.reg = 1 },
			 { .ty = ARG_TK_REG, .seminfo.parsed.reg = 3 },
			 { .ty = ARG_TK_END, },
		 });

	test_lex("push { r2, r3, r4, lr }",
		 OP_PUSH,
		 (struct inst_arg_tk []) {
			 { .ty = ARG_TK_REG_LS, .seminfo.parsed.reg_ls.reg_ls = 540 },
			 { .ty = ARG_TK_END, },
		 });
	test_lex("ldr r1, [r3, #12]",
		 OP_LDR,
		 (struct inst_arg_tk []) {
			 { .ty = ARG_TK_REG, .seminfo.parsed.reg = 1 },
			 { .ty = ARG_TK_BRACE, },
			 { .ty = ARG_TK_REG, .seminfo.parsed.reg = 3 },
			 { .ty = ARG_TK_IMM, .seminfo.parsed.imm = 12 },
			 { .ty = ARG_TK_END, },
		 });
	test_lex("ldm r7!, { r1, r3, r5 }",
		 OP_LDM,
		 (struct inst_arg_tk []) {
			 { .ty = ARG_TK_REG, .seminfo.parsed.reg = 7 },
			 { .ty = ARG_TK_EXCL, },
			 { .ty = ARG_TK_REG_LS, .seminfo.parsed.reg_ls.reg_ls = 42 },
			 { .ty = ARG_TK_END, },
		 });
	test_lex("bne #10",
		 OP_B,
		 (struct inst_arg_tk []) {
			 { .ty = ARG_TK_COND, .seminfo.parsed.cond_suffix = COND_NE },
			 { .ty = ARG_TK_IMM, .seminfo.parsed.imm = 10 },
			 { .ty = ARG_TK_END, },
		 });
	test_lex("str %r, [%R, %i]",
		 OP_STR,
		 (struct inst_arg_tk []) {
			 { .ty = ARG_TK_REG_VA_NUM, },
			 { .ty = ARG_TK_BRACE, },
			 { .ty = ARG_TK_REG_VA_STR, },
			 { .ty = ARG_TK_IMM_VA, },
			 { .ty = ARG_TK_END, },
		 });
	test_lex("stm %r!, %l",
		 OP_STM,
		 (struct inst_arg_tk []) {
			 { .ty = ARG_TK_REG_VA_NUM, },
			 { .ty = ARG_TK_EXCL, },
			 { .ty = ARG_TK_REG_LS_VA, },
			 { .ty = ARG_TK_END, },
		 });

	mas_destroy();
}
