#include "detail/opcodes.h"
#include "detail/grammar.h"

#include "../../include/grammar-dsl.h"
#include "../../include/grammar.h"


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

const struct mas_grammar_symbol
thumb2_grammar[INST_COUNT][MAX_ALT_COUNT + 1][MAX_ALT_SIZE + 1] = {
		/* register */
r(OP_ADCS,	rp(PARAM_LOW_REG), rp(PARAM_LOW_REG)),

		/* immediate */
r(OP_ADDS,	rp(PARAM_LOW_REG), rp(PARAM_LOW_REG), rp(PARAM_IMM3)
	alt	rp(PARAM_LOW_REG), rp(PARAM_IMM8)
		/* register */
	alt	rp(PARAM_LOW_REG), rp(PARAM_LOW_REG), rp(PARAM_LOW_REG)),

r(OP_ADD,	rp(PARAM_NOT_SP_REG), rp(PARAM_ANY_REG)
		/* SP plus immediate */
	alt	rp(PARAM_LOW_REG), rp(PARAM_SP), rp(PARAM_IMM8_00)
	alt	rp(PARAM_SP), rp(PARAM_IMM7_00)
		/* SP plus register */
	alt	rp(PARAM_ANY_REG), rp(PARAM_SP)
	alt	rp(PARAM_SP), rp(PARAM_ANY_REG)
	alt	rp(PARAM_LOW_REG), rp(PARAM_PC), rp(PARAM_IMM8_00)),

		/* register */
r(OP_ANDS,	rp(PARAM_LOW_REG), rp(PARAM_LOW_REG)),

		/* immediate */
r(OP_ASRS,	rp(PARAM_LOW_REG), rp(PARAM_LOW_REG), rp(PARAM_IMM5)
		/* register */
	alt	rp(PARAM_LOW_REG), rp(PARAM_LOW_REG)),

r(OP_B,		rp(PARAM_COND), rp(PARAM_SIMM8_0_LABEL)
	alt	rp(PARAM_SIMM11_0_LABEL)),

		/* register */
r(OP_BICS,	rp(PARAM_LOW_REG), rp(PARAM_LOW_REG)),

r(OP_BKPT,	rp(PARAM_IMM8)),

r(OP_BL,	rp(PARAM_SIMM24_0_LABEL)),

		/* register */
r(OP_BLX,	rp(PARAM_NOT_PC_REG)),

r(OP_BX,	rp(PARAM_NOT_PC_REG)),

		/* register */
r(OP_CMN,	rp(PARAM_LOW_REG), rp(PARAM_LOW_REG)),
		/* immediate */
r(OP_CMP,	rp(PARAM_LOW_REG), rp(PARAM_IMM8)
		/* register */
	alt	rp(PARAM_LOW_REG), rp(PARAM_LOW_REG)
	alt	rp(PARAM_ANY_REG), rp(PARAM_ANY_REG)),

		/* register */
r(OP_EORS,	rp(PARAM_LOW_REG), rp(PARAM_LOW_REG)),

		/* load */
r(OP_LDM,	rp(PARAM_LOW_REG), rp(PARAM_LOW_REG_LS)
	alt	rp(PARAM_LOW_REG), rp(PARAM_EXCL), rp(PARAM_LOW_REG_LS)),
		/* immediate */
r(OP_LDR,	rp(PARAM_LOW_REG), rp(PARAM_LOW_REG)
	alt	rp(PARAM_LOW_REG), rp(PARAM_BRACE), rp(PARAM_LOW_REG), rp(PARAM_IMM5_00)
	alt	rp(PARAM_LOW_REG), rp(PARAM_SP)
	alt	rp(PARAM_LOW_REG), rp(PARAM_BRACE), rp(PARAM_SP), rp(PARAM_IMM8_00)
		/* literal */
	alt	rp(PARAM_LOW_REG), rp(PARAM_IMM8_00_LABEL)
	alt	rp(PARAM_LOW_REG), rp(PARAM_BRACE), rp(PARAM_PC), rp(PARAM_IMM8_00)
		/* register */
	alt	rp(PARAM_LOW_REG), rp(PARAM_BRACE), rp(PARAM_LOW_REG), rp(PARAM_LOW_REG)),

		/* immediate */
r(OP_LDRB,	rp(PARAM_LOW_REG), rp(PARAM_LOW_REG)
	alt	rp(PARAM_LOW_REG), rp(PARAM_BRACE), rp(PARAM_LOW_REG), rp(PARAM_IMM5)
		/* register */
	alt	rp(PARAM_LOW_REG), rp(PARAM_BRACE), rp(PARAM_LOW_REG), rp(PARAM_LOW_REG)),
		/* immediate */
r(OP_LDRH,	rp(PARAM_LOW_REG), rp(PARAM_LOW_REG)
	alt	rp(PARAM_LOW_REG), rp(PARAM_BRACE), rp(PARAM_LOW_REG), rp(PARAM_IMM5)
		/* register */
	alt	rp(PARAM_LOW_REG), rp(PARAM_BRACE), rp(PARAM_LOW_REG), rp(PARAM_LOW_REG)),

r(OP_LDRSB,	rp(PARAM_LOW_REG), rp(PARAM_BRACE), rp(PARAM_LOW_REG), rp(PARAM_LOW_REG)),
r(OP_LDRSH,	rp(PARAM_LOW_REG), rp(PARAM_BRACE), rp(PARAM_LOW_REG), rp(PARAM_LOW_REG)),

		/* immediate */
r(OP_LSLS,	rp(PARAM_LOW_REG), rp(PARAM_LOW_REG), rp(PARAM_IMM5)
		/* register */
	alt	rp(PARAM_LOW_REG), rp(PARAM_LOW_REG)),
		/* immediate */
r(OP_LSRS,	rp(PARAM_LOW_REG), rp(PARAM_LOW_REG), rp(PARAM_IMM5)
		/* register */
	alt	rp(PARAM_LOW_REG), rp(PARAM_LOW_REG)),

r(OP_MOV,	rp(PARAM_ANY_REG), rp(PARAM_ANY_REG)),
		/* immediate */
r(OP_MOVS,	rp(PARAM_LOW_REG), rp(PARAM_IMM8)
		/* register */
	alt	rp(PARAM_LOW_REG), rp(PARAM_LOW_REG)),

r(OP_MULS,	rp(PARAM_LOW_REG), rp(PARAM_LOW_REG)),

		/* register */
r(OP_MVNS,	rp(PARAM_LOW_REG), rp(PARAM_LOW_REG)),

r(OP_NEGS,	rp(PARAM_LOW_REG), rp(PARAM_LOW_REG)),

r(OP_NOP, 	rp(PARAM_NOPARAM)),

		/* register */
r(OP_ORRS,	rp(PARAM_LOW_REG), rp(PARAM_LOW_REG)),

r(OP_POP,	rp(PARAM_POP_REG_LS)),
r(OP_PUSH,	rp(PARAM_PUSH_REG_LS)),

		/* register */
r(OP_RORS,	rp(PARAM_LOW_REG), rp(PARAM_LOW_REG)),

		/* register */
r(OP_RSBS,	rp(PARAM_LOW_REG), rp(PARAM_LOW_REG)
	alt	rp(PARAM_LOW_REG), rp(PARAM_LOW_REG), rp(PARAM_IMM0)),
r(OP_SBCS,	rp(PARAM_LOW_REG), rp(PARAM_LOW_REG)),

r(OP_SEV, 	rp(PARAM_NOPARAM)),

		/* store */
r(OP_STM,	rp(PARAM_LOW_REG), rp(PARAM_EXCL), rp(PARAM_LOW_REG_LS)),
		/* immediate */
r(OP_STR,	rp(PARAM_LOW_REG), rp(PARAM_LOW_REG)
	alt	rp(PARAM_LOW_REG), rp(PARAM_BRACE), rp(PARAM_LOW_REG), rp(PARAM_IMM5)
	alt	rp(PARAM_LOW_REG), rp(PARAM_SP)
	alt	rp(PARAM_LOW_REG), rp(PARAM_BRACE), rp(PARAM_SP), rp(PARAM_IMM8_00)
		/* register */
	alt	rp(PARAM_LOW_REG), rp(PARAM_BRACE), rp(PARAM_LOW_REG), rp(PARAM_LOW_REG)),

		/* immediate */
r(OP_STRB,	rp(PARAM_LOW_REG), rp(PARAM_LOW_REG)
	alt	rp(PARAM_LOW_REG), rp(PARAM_BRACE), rp(PARAM_LOW_REG), rp(PARAM_IMM5)
		/* register */
	alt	rp(PARAM_LOW_REG), rp(PARAM_BRACE), rp(PARAM_LOW_REG), rp(PARAM_LOW_REG)),
		/* immediate */
r(OP_STRH,	rp(PARAM_LOW_REG), rp(PARAM_LOW_REG)
	alt	rp(PARAM_LOW_REG), rp(PARAM_BRACE), rp(PARAM_LOW_REG), rp(PARAM_IMM5)
		/* register */
	alt	rp(PARAM_LOW_REG), rp(PARAM_BRACE), rp(PARAM_LOW_REG), rp(PARAM_LOW_REG)),

		/* immediate */
r(OP_SUBS,	rp(PARAM_LOW_REG), rp(PARAM_LOW_REG), rp(PARAM_IMM3)
	alt	rp(PARAM_LOW_REG), rp(PARAM_IMM8)
		/* register */
	alt	rp(PARAM_LOW_REG), rp(PARAM_LOW_REG), rp(PARAM_LOW_REG)),
		/* rp(SP) minus immediate */
r(OP_SUB,	rp(PARAM_SP), rp(PARAM_SP), rp(PARAM_IMM7_00)),

		/* register */
r(OP_TST,	rp(PARAM_LOW_REG), rp(PARAM_LOW_REG)),

r(OP_UDF,	rp(PARAM_IMM8)
	alt	rp(PARAM_NOPARAM)),
};
