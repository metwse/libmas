/* Opcode & instruction definitions. */

#ifndef THUMB2_OPCODES_H
#define THUMB2_OPCODES_H

#include <stddef.h>
#include <stdint.h>


#define MAX_ALT_COUNT 7
#define MAX_ALT_SIZE 4

/* (Number of opcodes - ORDER THUMB2_OP) */
#define INST_COUNT (OP_UDF + 1)
/* (Number of condition suffixes - ORDER THUMB2_COND) */
#define COND_SUFFIX_COUNT (COND_AL + 1)

/* (Number of parameter kinds and their reg, reg_ls, imm ranges - ORDER THUMB2_PARAM) */
#define PARAM_KIND_COUNT (PARAM_NOPARAM + 1)
#define is_reg(param) \
	((param) >= PARAM_LOW_REG && (param) <= PARAM_PC)
#define is_reg_ls(param) \
	((param) >= PARAM_LOW_REG_LS && (param) <= PARAM_PUSH_REG_LS)
#define is_imm(param) \
	((param) >= PARAM_IMM0 && (param) <= PARAM_SIMM24_0_LABEL)


enum inst_opcode {  // GREP "ORDER THUMB2_OP" if you change this enum
	OP_ADCS, OP_ADDS, OP_ADD, OP_ANDS, OP_ASRS, OP_B,
	OP_BICS, OP_BKPT, OP_BL, OP_BLX, OP_BX, OP_CMN, OP_CMP,
	OP_EORS, OP_LDM, OP_LDR, OP_LDRB, OP_LDRH, OP_LDRSB, OP_LDRSH,
	OP_LSLS, OP_LSRS, OP_MOV, OP_MOVS, OP_MULS, OP_MVNS, OP_NEGS, OP_NOP,
	OP_ORRS, OP_POP, OP_PUSH, OP_RORS, OP_RSBS, OP_SBCS, OP_SEV, OP_STM,
	OP_STR, OP_STRB, OP_STRH, OP_SUBS, OP_SUB, OP_TST, OP_UDF
};

/* Conditional execution flags. */
enum inst_cond {  // GREP "ORDER THUMB2_COND" if you change this enum
	COND_EQ, COND_NE, COND_CS, COND_CC, COND_MI, COND_PL, COND_VS, COND_VC,
	COND_HI, COND_LS, COND_GE, COND_LT, COND_GT, COND_LE, COND_AL,
};

/* Instruction parameters. */
enum inst_param {  // GREP "ORDER THUMB2_PARAM" if you change this enum
	PARAM_LOW_REG,  /* r0-r7 */
	PARAM_HIGH_NOT_SP_REG,  /* r8-r14 */
	PARAM_ANY_REG,  /* r0-r15 */
	PARAM_NOT_SP_REG,  /* r0-r13,r15 */
	PARAM_NOT_PC_REG,  /* r0-r14 */
	PARAM_SP,  /* r13 */
	PARAM_PC,  /* r15 */

	PARAM_LOW_REG_LS,  /* { r0-r7 } */
	PARAM_POP_REG_LS,  /* { r0-r7, pc } */
	PARAM_PUSH_REG_LS,  /* { r0-r7, lr } */

	PARAM_IMM0,  /* constant 0 */
	PARAM_IMM3,  /* 3-bit imm */
	PARAM_IMM5, PARAM_IMM5_00,
	PARAM_IMM7_00,
	PARAM_IMM8, PARAM_IMM8_0, PARAM_SIMM8_0_LABEL,
		PARAM_IMM8_00, PARAM_IMM8_00_LABEL,
	PARAM_SIMM11_0_LABEL,
	PARAM_SIMM24_0_LABEL,

	PARAM_EXCL,
	PARAM_BRACE,
	PARAM_COND,
	PARAM_NOPARAM,
};

/* Parsed instruction argument. */
union inst_arg {
	enum inst_arg_data_reg {
		ARG_REG_R0, ARG_REG_R1, ARG_REG_R2, ARG_REG_R3,
		ARG_REG_R4, ARG_REG_R5, ARG_REG_R6, ARG_REG_R7,
		ARG_REG_R8, ARG_REG_R9, ARG_REG_R10, ARG_REG_R11,
		ARG_REG_R12, ARG_REG_R13, ARG_REG_R14, ARG_REG_R15,
	} reg;

	struct inst_arg_data_reg_ls {
		union {
			struct {
			uint32_t r0 : 1, r1 : 1, r2 : 1, r3 : 1,
				 r4 : 1, r5 : 1, r6 : 1, r7 : 1,
				 pc : 1, lr : 1;
			};

			uint32_t reg_ls;
		};
	} reg_ls;

	uint32_t imm  /* unsingned imm */;
	int32_t simm  /* signed imm */;

	enum inst_cond cond_suffix;
};


#endif
