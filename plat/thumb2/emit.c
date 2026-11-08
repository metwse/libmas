#include "detail/opcodes.h"

#include "../../src/binary-literal.h"

#include <stdbool.h>
#include <stdint.h>


/* emit bits to an offset */
#define emit(bits, n, o) do { \
		bytes |= ((bits) & ((1 << (n)) - 1)) << o; \
	} while (0)
/* emit bits incrementing saved offset */
#define emit2(bits, n) do { \
		emit(bits, n, write_offset); \
		write_offset += n; \
	} while (0)
/* emit for the rest of instruction */
#define emit3(bits) do { \
		emit2(bits, (is_32_bit ? 32 : 16) - write_offset); \
	} while (0)


void thumb2_inst_emit(void *out, const struct inst *inst)
{
	size_t alt_idx = inst->alt_idx;
	const union inst_arg_data *args = inst->args;

	uint32_t bytes = 0;
	bool is_32_bit = thumb2_inst_size(inst) == 4;
	uint32_t write_offset = 0;

	switch (inst->opcode) {
	case OP_ADCS:
		switch (alt_idx) {
		case 0:
			emit2(args[0].reg, 3);
			emit2(args[1].reg, 3);
			emit3(b(0100000101));
			break;
		}
		break;

	case OP_ADDS:
		switch (alt_idx) {
		case 0:
			emit2(args[0].reg, 3);
			emit2(args[1].reg, 3);
			emit2(args[2].imm, 3);
			emit3(b(0001110));
			break;
		case 1:
			emit2(args[1].imm, 8);
			emit2(args[0].reg, 3);
			emit3(b(00110));
			break;
		case 2:
			emit2(args[0].reg, 3);
			emit2(args[1].reg, 3);
			emit2(args[2].reg, 3);
			emit3(b(0001100));
			break;
		}
		break;

	case OP_ADD:
		switch (alt_idx) {
		case 0:
			emit2(args[0].reg, 3);
			emit2(args[1].reg, 4);
			emit2((args[0].reg & 0x8) ? 1 : 0, 1);
			emit3(b(01000100));
			break;
		case 1:
			emit2(args[2].imm >> 2, 8);
			emit2(args[0].reg, 3);
			emit3(b(10101));
			break;
		case 2:
			emit2(args[1].imm >> 2, 7);
			emit3(b(101100000));
			break;
		case 3:
			emit2(args[0].reg, 3);
			emit2(b(1101), 4);
			emit2((args[0].reg & 0x8) ? 1 : 0, 1);
			emit3(b(01000100));
			break;
		case 4:
			emit2(b(101), 3);
			emit2(args[1].reg, 4);
			emit3(b(010001001));
			break;
		case 5:
			emit2(args[1].imm, 8);
			emit2(args[0].reg, 3);
			emit3(b(10100));
			break;
		}
		break;

	case OP_ANDS:
		switch (alt_idx) {
		case 0:
			emit2(args[0].reg, 3);
			emit2(args[1].reg, 3);
			emit3(b(0100000000));
			break;
		}
		break;

	case OP_ASRS:
		switch (alt_idx) {
		case 0:
			emit2(args[0].reg, 3);
			emit2(args[1].reg, 3);
			emit2(args[2].imm, 5);
			emit3(b(00010));
			break;
		case 1:
			emit2(args[0].reg, 3);
			emit2(args[1].reg, 3);
			emit3(b(0100000100));
			break;
		}
		break;

	case OP_B:
		switch (alt_idx) {
		case 0:
			emit2(args[1].simm >> 1, 8);
			emit2(args[0].cond_suffix, 4);
			emit3(b(1101));
			break;
		case 1:
			emit2(args[0].simm >> 1, 11);
			emit3(b(11100));
			break;
		}
		break;

	case OP_BICS:
		switch (alt_idx) {
		case 0:
			emit2(args[0].reg, 3);
			emit2(args[1].reg, 3);
			emit3(b(0100001110));
			break;
		}
		break;

	case OP_BKPT:
		switch (alt_idx) {
		case 0:
			emit2(args[0].imm, 8);
			emit3(b(10111110));
			break;
		}
		break;

	case OP_BL:
		switch (alt_idx) {
		case 0: {
			int s = (args[0].simm & (1 << 24)) ? 1 : 0;
			int j1 = s ^ ((args[0].simm & (1 << 23)) ? 1 : 0);
			int j2 = s ^ ((args[0].simm & (1 << 22)) ? 1 : 0);

			emit2(args[0].simm >> 12, 10);
			emit2(s, 1);
			emit2(b(11110), 5);

			emit2(args[0].simm >> 1, 11);
			emit2(j2, 1);
			emit2(1, 1);
			emit2(j1, 1);
			emit2(b(11), 2);
			break;
		}
		}
		break;

	case OP_BLX:
		switch (alt_idx) {
		case 0:
			emit2(b(000), 3);
			emit2(args[0].reg, 4);
			emit3(b(010001111));
			break;
		}
		break;

	case OP_BX:
		switch (alt_idx) {
		case 0:
			emit2(b(000), 3);
			emit2(args[0].reg, 4);
			emit3(b(010001110));
			break;
		}
		break;

	case OP_CMN:
		switch (alt_idx) {
		case 0:
			emit2(args[0].reg, 3);
			emit2(args[1].reg, 3);
			emit3(b(0100001011));
			break;
		}
		break;

	case OP_CMP:
		switch (alt_idx) {
		case 0:
			emit2(args[1].imm, 8);
			emit2(args[0].reg, 3);
			emit3(b(00101));
			break;
		case 1:
			emit2(args[0].reg, 3);
			emit2(args[1].reg, 3);
			emit3(b(0100001010));
			break;
		case 2:
			emit2(args[0].reg, 3);
			emit2(args[1].reg, 4);
			emit2((args[0].reg & 0x8) ? 1 : 0, 1);
			emit3(b(01000101));
			break;
		}
		break;

	case OP_EORS:
		switch (alt_idx) {
		case 0:
			emit2(args[0].reg, 3);
			emit2(args[1].reg, 3);
			emit3(b(0100000001));
			break;
		}
		break;

	case OP_LDM: {
		uint32_t reg_ls = 0;
		switch (alt_idx) {
		case 0:
			reg_ls = args[1].reg_ls.reg_ls;
			reg_ls &= ~(1 << args[0].reg);
			// fallthrough
		case 1:
			if (reg_ls == 0) {
				reg_ls = args[2].reg_ls.reg_ls;
				reg_ls |= 1 << args[0].reg;
			}
			emit2(reg_ls, 8);
			emit2(args[0].reg, 3);
			emit3(b(11001));
			break;
		}
		break;
	}

	case OP_LDR:
		switch (alt_idx) {
		case 0:
			emit2(args[0].reg, 3);
			emit2(args[1].reg, 3);
			emit2(0, 5);
			emit3(b(01101));
			break;
		case 1:
			emit2(args[0].reg, 3);
			emit2(args[2].reg, 3);
			emit2(args[3].imm >> 2, 5);
			emit3(b(01101));
			break;
		case 2:
			emit2(0, 8);
			emit2(args[0].reg, 3);
			emit3(b(10011));
			break;
		case 3:
			emit2(args[3].imm >> 2, 8);
			emit2(args[0].reg, 3);
			emit3(b(10011));
			break;
		case 4:
			emit2(args[1].imm >> 2, 8);
			emit2(args[0].reg, 3);
			emit3(b(01001));
			break;
		case 5:
			emit2(args[3].imm >> 2, 8);
			emit2(args[0].reg, 3);
			emit3(b(01001));
			break;
		case 6:
			emit2(args[0].reg, 3);
			emit2(args[1].reg, 3);
			emit2(args[2].reg, 3);
			emit3(b(0101100));
			break;
		}
		break;

	case OP_LDRB:
		switch (alt_idx) {
		case 0:
			emit2(args[0].reg, 3);
			emit2(args[1].reg, 3);
			emit2(0, 5);
			emit3(b(01111));
			break;
		case 1:
			emit2(args[0].reg, 3);
			emit2(args[2].reg, 3);
			emit2(args[3].imm, 5);
			emit3(b(01111));
			break;
		case 2:
			emit2(args[0].reg, 3);
			emit2(args[2].reg, 3);
			emit2(args[3].reg, 3);
			emit3(b(0101110));
			break;
		}
		break;

	case OP_LDRH:
		switch (alt_idx) {
		case 0:
			emit2(args[0].reg, 3);
			emit2(args[1].reg, 3);
			emit2(0, 5);
			emit3(b(10001));
			break;
		case 1:
			emit2(args[0].reg, 3);
			emit2(args[2].reg, 3);
			emit2(args[3].imm, 5);
			emit3(b(10001));
			break;
		case 2:
			emit2(args[0].reg, 3);
			emit2(args[2].reg, 3);
			emit2(args[3].reg, 3);
			emit3(b(0101101));
			break;
		}
		break;

	case OP_LDRSB:
		switch (alt_idx) {
		case 0:
			emit2(args[0].reg, 3);
			emit2(args[2].reg, 3);
			emit2(args[3].reg, 3);
			emit3(b(0101011));
			break;
		}
		break;

	case OP_LDRSH:
		switch (alt_idx) {
		case 0:
			emit2(args[0].reg, 3);
			emit2(args[2].reg, 3);
			emit2(args[3].reg, 3);
			emit3(b(0101111));
			break;
		}
		break;

	case OP_LSLS:
		switch (alt_idx) {
		case 0:
			emit2(args[0].reg, 3);
			emit2(args[1].reg, 3);
			emit2(args[2].imm, 5);
			emit3(b(00000));
			break;
		case 1:
			emit2(args[0].reg, 3);
			emit2(args[1].reg, 3);
			emit3(b(0100000010));
			break;
		}
		break;

	case OP_LSRS:
		switch (alt_idx) {
		case 0:
			emit2(args[0].reg, 3);
			emit2(args[1].reg, 3);
			emit2(args[2].imm, 5);
			emit3(b(00001));
			break;
		case 1:
			emit2(args[0].reg, 3);
			emit2(args[1].reg, 3);
			emit3(b(0100000011));
			break;
		}
		break;

	case OP_MOV:
		switch (alt_idx) {
		case 0:
			emit2(args[0].reg, 3);
			emit2(args[1].reg, 4);
			emit2((args[0].reg & 0x8) ? 1 : 0, 1);
			emit3(b(01000110));
			break;
		}
		break;

	case OP_MOVS:
		switch (alt_idx) {
		case 0:
			emit2(args[1].imm, 8);
			emit2(args[0].reg, 3);
			emit3(b(00100));
			break;
		case 1:
			emit2(args[0].reg, 3);
			emit2(args[1].reg, 3);
			emit3(b(0000000000));
			break;
		}
		break;

	case OP_MULS:
		switch (alt_idx) {
		case 0:
			emit2(args[0].reg, 3);
			emit2(args[1].reg, 3);
			emit3(b(0100001101));
			break;
		}
		break;

	case OP_MVNS:
		switch (alt_idx) {
		case 0:
			emit2(args[0].reg, 3);
			emit2(args[1].reg, 3);
			emit3(b(0100001111));
			break;
		}
		break;

	case OP_NOP:
		switch (alt_idx) {
		case 0:
			emit3(b(1011111100000000));
			break;
		}
		break;

	case OP_ORRS:
		switch (alt_idx) {
		case 0:
			emit2(args[0].reg, 3);
			emit2(args[1].reg, 3);
			emit3(b(0100001100));
			break;
		}
		break;

	case OP_POP:
		switch (alt_idx) {
		case 0:
			emit2(args[0].reg_ls.reg_ls, 8);
			emit2(args[0].reg_ls.pc, 1);
			emit3(b(1011110));
			break;
		}
		break;

	case OP_PUSH:
		switch (alt_idx) {
		case 0:
			emit2(args[0].reg_ls.reg_ls, 8);
			emit2(args[0].reg_ls.lr, 1);
			emit3(b(1011010));
			break;
		}
		break;

	case OP_RORS:
		switch (alt_idx) {
		case 0:
			emit2(args[0].reg, 3);
			emit2(args[1].reg, 3);
			emit3(b(0100000111));
			break;
		}
		break;

	case OP_NEGS:
	case OP_RSBS:
		switch (alt_idx) {
		case 0:
		case 1:
			emit2(args[0].reg, 3);
			emit2(args[1].reg, 3);
			emit3(b(0100001001));
			break;
		}
		break;

	case OP_SBCS:
		switch (alt_idx) {
		case 0:
			emit2(args[0].reg, 3);
			emit2(args[1].reg, 3);
			emit3(b(0100000110));
			break;
		}
		break;

	case OP_SEV:
		switch (alt_idx) {
		case 0:
			emit3(b(1011111101000000));
			break;
		}
		break;

	case OP_STM:
		switch (alt_idx) {
		case 0:
			emit2(args[2].reg_ls.reg_ls, 8);
			emit2(args[0].reg, 3);
			emit3(b(11000));
			break;
		}
		break;

	case OP_STR:
		switch (alt_idx) {
		case 0:
			emit2(args[0].reg, 3);
			emit2(args[1].reg, 3);
			emit2(0, 5);
			emit3(b(01100));
			break;
		case 1:
			emit2(args[0].reg, 3);
			emit2(args[2].reg, 3);
			emit2(args[3].imm >> 2, 5);
			emit3(b(01100));
			break;
		case 2:
			emit2(0, 8);
			emit2(args[0].reg, 3);
			emit3(b(10010));
			break;
		case 3:
			emit2(args[3].imm >> 2, 8);
			emit2(args[0].reg, 3);
			emit3(b(10010));
			break;
		case 4:
			emit2(args[0].reg, 3);
			emit2(args[2].reg, 3);
			emit2(args[3].reg, 3);
			emit3(b(0101000));
			break;
		}
		break;

	case OP_STRB:
		switch (alt_idx) {
		case 0:
			emit2(args[0].reg, 3);
			emit2(args[1].reg, 3);
			emit2(0, 5);
			emit3(b(01110));
			break;
		case 1:
			emit2(args[0].reg, 3);
			emit2(args[2].reg, 3);
			emit2(args[3].imm, 5);
			emit3(b(01110));
			break;
		case 2:
			emit2(args[0].reg, 3);
			emit2(args[2].reg, 3);
			emit2(args[3].reg, 3);
			emit3(b(0101010));
			break;
		}
		break;

	case OP_STRH:
		switch (alt_idx) {
		case 0:
			emit2(args[0].reg, 3);
			emit2(args[1].reg, 3);
			emit2(0, 5);
			emit3(b(10000));
			break;
		case 1:
			emit2(args[0].reg, 3);
			emit2(args[2].reg, 3);
			emit2(args[3].imm >> 1, 5);
			emit3(b(10000));
			break;
		case 2:
			emit2(args[0].reg, 3);
			emit2(args[2].reg, 3);
			emit2(args[3].reg, 3);
			emit3(b(0101001));
			break;
		}
		break;

	case OP_SUBS:
		switch (alt_idx) {
		case 0:
			emit2(args[0].reg, 3);
			emit2(args[1].reg, 3);
			emit2(args[2].imm, 3);
			emit3(b(0001111));
			break;
		case 1:
			emit2(args[1].imm, 8);
			emit2(args[0].reg, 3);
			emit3(b(00111));
			break;
		case 2:
			emit2(args[0].reg, 3);
			emit2(args[1].reg, 3);
			emit2(args[2].reg, 3);
			emit3(b(0001101));
			break;
		}
		break;

	case OP_SUB:
		switch (alt_idx) {
		case 0:
			emit2(args[2].imm >> 2, 7);
			emit3(b(101100001));
			break;
		}
		break;

	case OP_TST:
		switch (alt_idx) {
		case 0:
			emit2(args[0].reg, 3);
			emit2(args[1].reg, 3);
			emit3(b(0100001000));
			break;
		}
		break;

	case OP_UDF:
		switch (alt_idx) {
		case 0:
			emit2(args[0].imm, 8);
			break;
		case 1:
			emit2(0, 8);
			break;
		}
		emit3(b(11011110));
		break;
	}

	uint16_t *addr = (uint16_t *) out;

	if (!is_32_bit) {
		*addr = (uint16_t) bytes;
	} else {
		*addr = (uint16_t) bytes;
		*(addr + 1) = (uint16_t) (bytes >> 16);
	}
}

size_t thumb2_inst_size(const struct inst *inst)
{
	size_t alt_idx = inst->alt_idx;

	switch (inst->opcode) {
	case OP_BL:
		switch (alt_idx) {
		case 0:
			return 4;
		}

	default: break;
	}

	return 2;
}
