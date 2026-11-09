#include "detail/opcodes.h"
#include "detail/grammar.h"

#include <stddef.h>
#include <stdio.h>


/* (Opcode/mnemonic name map - ORDER THUMB2_OP) */
const char *const thumb2_mnemonics[INST_COUNT] = {
	"adcs", "adds", "add", "ands", "asrs", "b",
	"bics", "bkpt", "bl", "blx", "bx", "cmn", "cmp",
	"eors", "ldm", "ldr", "ldrb", "ldrh", "ldrsb", "ldrsh",
	"lsls", "lsrs", "mov", "movs", "muls", "mvns", "negs", "nop",
	"orrs", "pop", "push", "rors", "rsbs", "sbcs", "sev", "stm",
	"str", "strb", "strh", "subs", "sub", "tst", "udf"
};

/* (Condition suffix name map - ORDER THUMB2_COND) */
const char *const thumb2_mnemonic_cond_suffix[COND_SUFFIX_COUNT] = {
	"eq", "ne", "cs", "cc", "mi", "pl", "vs", "vc",
	"hi", "ls", "ge", "lt", "gt", "le", "al",
};

/* (Debug parameter names - ORDER THUMB2_PARAM) */
const char *const thumb2_param_names[PARAM_KIND_COUNT] = {
	"",

	"r0-r7", "r8-r14", "r0-r15", "r0-r13,r15", "r0-r14", "sp", "pc",

	"{ r0-r7 }", "{ r0-r7, pc }", "{ r0-r7, lr }",

	"#imm3",
	"#imm5", "#imm5_00",
	"#imm7_00",
	"#imm8", "#imm8_0", "#imm8_0_label", "#imm8_00", "#imm8_00_label",
	"#imm11_0_label",
	"#imm24_0_label",

	"!",
	"[",
	"<cond>",
	""
};

/* Instructions are represented as:
 * <mnemonic><mnemonic_cond_suffix> <parameters...>
 *
 * An instruction can have conditional execution mode IF its first parameter
 * is PARAM_COND. */

const enum inst_param
thumb2_inst_params[INST_COUNT][MAX_INST_ALT_COUNT + 1][MAX_PARAM_COUNT + 1] = {
			/* register */
	[OP_ADCS] = {	{ PARAM_LOW_REG, PARAM_LOW_REG } },

			/* immediate */
	[OP_ADDS] = {	{ PARAM_LOW_REG, PARAM_LOW_REG, PARAM_IMM3 } ,
			{ PARAM_LOW_REG, PARAM_IMM8 },
			/* register */
			{ PARAM_LOW_REG, PARAM_LOW_REG, PARAM_LOW_REG } },

	[OP_ADD] = {	{ PARAM_NOT_SP_REG, PARAM_ANY_REG },
			/* SP plus immediate */
			{ PARAM_LOW_REG, PARAM_SP, PARAM_IMM8_00 },
			{ PARAM_SP, PARAM_IMM7_00 },
			/* SP plus register */
			{ PARAM_ANY_REG, PARAM_SP },
			{ PARAM_SP, PARAM_ANY_REG },
			{ PARAM_LOW_REG, PARAM_PC, PARAM_IMM8_00 } },

			/* register */
	[OP_ANDS] = {	{ PARAM_LOW_REG, PARAM_LOW_REG } },

			/* immediate */
	[OP_ASRS] = {	{ PARAM_LOW_REG, PARAM_LOW_REG, PARAM_IMM5 },
			/* register */
			{ PARAM_LOW_REG, PARAM_LOW_REG } },

	[OP_B] = {	{ PARAM_COND, PARAM_SIMM8_0_LABEL },
			{ PARAM_SIMM11_0_LABEL } },

			/* register */
	[OP_BICS] = {	{ PARAM_LOW_REG, PARAM_LOW_REG } },

	[OP_BKPT] = {	{ PARAM_IMM8 } },

	[OP_BL] = {	{ PARAM_SIMM24_0_LABEL } },

			/* register */
	[OP_BLX] = {	{ PARAM_NOT_PC_REG } },

	[OP_BX] = {	{ PARAM_NOT_PC_REG } },

			/* register */
	[OP_CMN] = {	{ PARAM_LOW_REG, PARAM_LOW_REG } },
			/* immediate */
	[OP_CMP] = {	{ PARAM_LOW_REG, PARAM_IMM8 },
			/* register */
			{ PARAM_LOW_REG, PARAM_LOW_REG },
			{ PARAM_ANY_REG, PARAM_ANY_REG } },

			/* register */
	[OP_EORS] = {	{ PARAM_LOW_REG, PARAM_LOW_REG } },

			/* load */
	[OP_LDM] = {	{ PARAM_LOW_REG, PARAM_LOW_REG_LS },
			{ PARAM_LOW_REG, PARAM_EXCL, PARAM_LOW_REG_LS }, },
			/* immediate */
	[OP_LDR] = {	{ PARAM_LOW_REG, PARAM_LOW_REG },
			{ PARAM_LOW_REG, PARAM_BRACE, PARAM_LOW_REG, PARAM_IMM5_00 },
			{ PARAM_LOW_REG, PARAM_SP },
			{ PARAM_LOW_REG, PARAM_BRACE, PARAM_SP, PARAM_IMM8_00 },
			/* literal */
			{ PARAM_LOW_REG, PARAM_IMM8_00_LABEL },
			{ PARAM_LOW_REG, PARAM_BRACE, PARAM_PC, PARAM_IMM8_00 },
			/* register */
			{ PARAM_LOW_REG, PARAM_BRACE, PARAM_LOW_REG, PARAM_LOW_REG } },

			/* immediate */
	[OP_LDRB] = {	{ PARAM_LOW_REG, PARAM_LOW_REG },
			{ PARAM_LOW_REG, PARAM_BRACE, PARAM_LOW_REG, PARAM_IMM5 },
			/* register */
			{ PARAM_LOW_REG, PARAM_BRACE, PARAM_LOW_REG, PARAM_LOW_REG } },
			/* immediate */
	[OP_LDRH] = {	{ PARAM_LOW_REG, PARAM_LOW_REG },
			{ PARAM_LOW_REG, PARAM_BRACE, PARAM_LOW_REG, PARAM_IMM5 },
			/* register */
			{ PARAM_LOW_REG, PARAM_BRACE, PARAM_LOW_REG, PARAM_LOW_REG } },

	[OP_LDRSB] = {	{ PARAM_LOW_REG, PARAM_BRACE, PARAM_LOW_REG, PARAM_LOW_REG } },
	[OP_LDRSH] = {	{ PARAM_LOW_REG, PARAM_BRACE, PARAM_LOW_REG, PARAM_LOW_REG } },

			/* immediate */
	[OP_LSLS] = {	{ PARAM_LOW_REG, PARAM_LOW_REG, PARAM_IMM5 },
			/* register */
			{ PARAM_LOW_REG, PARAM_LOW_REG } },
			/* immediate */
	[OP_LSRS] = {	{ PARAM_LOW_REG, PARAM_LOW_REG, PARAM_IMM5 },
			/* register */
			{ PARAM_LOW_REG, PARAM_LOW_REG } },

	[OP_MOV] = {	{ PARAM_ANY_REG, PARAM_ANY_REG } },
			/* immediate */
	[OP_MOVS] = {	{ PARAM_LOW_REG, PARAM_IMM8 },
			/* register */
			{ PARAM_LOW_REG, PARAM_LOW_REG } },

	[OP_MULS] = {	{ PARAM_LOW_REG, PARAM_LOW_REG } },

			/* register */
	[OP_MVNS] = {	{ PARAM_LOW_REG, PARAM_LOW_REG } },

	[OP_NEGS] = {	{ PARAM_LOW_REG, PARAM_LOW_REG } },

	[OP_NOP] = { 	{ PARAM_NOPARAM } },

			/* register */
	[OP_ORRS] = {	{ PARAM_LOW_REG, PARAM_LOW_REG } },

	[OP_POP] = {	{ PARAM_POP_REG_LS } },
	[OP_PUSH] = {	{ PARAM_PUSH_REG_LS } },

			/* register */
	[OP_RORS] = {	{ PARAM_LOW_REG, PARAM_LOW_REG } },

			/* register */
	[OP_RSBS] = {	{ PARAM_LOW_REG, PARAM_LOW_REG },
			{ PARAM_LOW_REG, PARAM_LOW_REG, PARAM_IMM0 } },
	[OP_SBCS] = {	{ PARAM_LOW_REG, PARAM_LOW_REG } },

	[OP_SEV] = { 	{ PARAM_NOPARAM } },

			/* store */
	[OP_STM] = {	{ PARAM_LOW_REG, PARAM_EXCL, PARAM_LOW_REG_LS } },
			/* immediate */
	[OP_STR] = {	{ PARAM_LOW_REG, PARAM_LOW_REG },
			{ PARAM_LOW_REG, PARAM_BRACE, PARAM_LOW_REG, PARAM_IMM5 },
			{ PARAM_LOW_REG, PARAM_SP },
			{ PARAM_LOW_REG, PARAM_BRACE, PARAM_SP, PARAM_IMM8_00 },
			/* register */
			{ PARAM_LOW_REG, PARAM_BRACE, PARAM_LOW_REG, PARAM_LOW_REG } },

			/* immediate */
	[OP_STRB] = {	{ PARAM_LOW_REG, PARAM_LOW_REG },
			{ PARAM_LOW_REG, PARAM_BRACE, PARAM_LOW_REG, PARAM_IMM5 },
			/* register */
			{ PARAM_LOW_REG, PARAM_BRACE, PARAM_LOW_REG, PARAM_LOW_REG } },
			/* immediate */
	[OP_STRH] = {	{ PARAM_LOW_REG, PARAM_LOW_REG },
			{ PARAM_LOW_REG, PARAM_BRACE, PARAM_LOW_REG, PARAM_IMM5 },
			/* register */
			{ PARAM_LOW_REG, PARAM_BRACE, PARAM_LOW_REG, PARAM_LOW_REG } },

			/* immediate */
	[OP_SUBS] = {	{ PARAM_LOW_REG, PARAM_LOW_REG, PARAM_IMM3 },
			{ PARAM_LOW_REG, PARAM_IMM8 },
			/* register */
			{ PARAM_LOW_REG, PARAM_LOW_REG, PARAM_LOW_REG } },
			/* SP minus immediate */
	[OP_SUB] = {	{ PARAM_SP, PARAM_SP, PARAM_IMM7_00 } },

			/* register */
	[OP_TST] = {	{ PARAM_LOW_REG, PARAM_LOW_REG } },

	[OP_UDF] = {	{ PARAM_IMM8 },
			{ PARAM_NOPARAM } },
};


void thumb2_dump_grammar(void)
{
	for (size_t inst = 0;
	     inst < INST_COUNT;
	     inst++) {
		for (size_t alt = 0; thumb2_inst_params[inst][alt][0]; alt++) {
			size_t i = 0;
#define cur_param thumb2_inst_params[inst][alt][i]
#define next_param thumb2_inst_params[inst][alt][i + 1]

			if (cur_param == PARAM_COND) {
				printf("%s<cond> ", thumb2_mnemonics[inst]);
				i++;
			} else {
				printf("%s ", thumb2_mnemonics[inst]);
			}

			size_t brace_count = 0;
			for (;
			     cur_param != PARAME;
			     i++) {
				if (next_param != PARAME &&
				    (cur_param == PARAM_EXCL || (
					(is_reg(cur_param) || is_imm(cur_param)) &&
					(next_param != PARAM_EXCL))
				    )) {
					printf("%s, ",
					       thumb2_param_names[cur_param]);
				} else if (cur_param == PARAM_BRACE) {
					printf("[");
					brace_count++;
				} else if (next_param == PARAME || next_param == PARAM_EXCL) {
					printf("%s",
					       thumb2_param_names[cur_param]);
				} else {
					printf("%s ",
					       thumb2_param_names[cur_param]);
				}
			}

			while (brace_count--)
				printf("]");

			printf("\n");
		}
	}
}
