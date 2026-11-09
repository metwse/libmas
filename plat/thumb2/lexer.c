#include "detail/grammar.h"
#include "detail/methods.h"
#include "detail/opcodes.h"
#include "detail/static-init.h"

#include "../../include/error.h"
#include "../../include/error.h"
#include "../../src/common.h"
#include "../../src/error.h"

#include <stddef.h>
#include <stdint.h>
#include <inttypes.h>
#include <ctype.h>


#define T char, enum inst_opcode, mnemonic_name
#include "../../vendor/libfun/include/hmap.h"

static struct fhmap_mnemonic_name mnemonic_name_map;


void thumb2_lexer_static_xinit(void)
{
	fhmap_mnemonic_name_xinit(&mnemonic_name_map);

	for (enum inst_opcode i = 0; i < INST_COUNT; i++)
		fhmap_mnemonic_name_xinsert(&mnemonic_name_map,
					    thumb2_mnemonics[i],
					    strlen(thumb2_mnemonics[i]),
					    &i);
}

void thumb2_lexer_static_destroy(void)
{
	fhmap_mnemonic_name_destroy(&mnemonic_name_map);
}

mas_err_t *thumb2_consume_opcode(
		const char **inst,
		mas_grammar_opcode_t *out_opcode,
		_unused void *out_op_seminfo,
		_unused struct mas_extension_get_data *ge)
{
	enum inst_opcode *out = out_opcode;

	size_t opcode_len = 0;
	for (; isalnum((*inst)[opcode_len]); opcode_len++)
		;

	if (opcode_len == 0)
		return_err(inv_mnemonic, "Invalid mnemonic!", "empty");

	char mnemonic_str[opcode_len + 1];
	strncpy(mnemonic_str, *inst, opcode_len);

	const enum inst_opcode *opcode;

	for (int i = 0; i < 2; i++) {
		/* first try to match full alnum chars (mnemonic)
		 * if there's no match, try to remove last 2 letters (cond
		 * suffix) and try again. */
		opcode = fhmap_mnemonic_name_get(&mnemonic_name_map,
						 (*inst),
						 opcode_len);

		if (opcode != NULL) {
			(*inst) += opcode_len;
			return_ok(*opcode);
		}

		if (opcode_len > 2)
			opcode_len -= 2;
		else
			break;
	}

	return_err(inv_mnemonic, "Invalid mnemonic!", mnemonic_str);
}

static mas_err_t *thumb2_consume_arg_reg_lexeme(const char **inst, uint32_t *out)
{
	switch ((*inst)[0]) {
	case 'r': {
		(*inst)++;
		uint32_t reg = strtol(*inst, (char **) inst, 0);

		if (reg < 16)
			return_ok(reg);
		break;
	}
	case 's':
		if ((*inst)[1] == 'p') {
			*inst += 2;
			return_ok(13);
		}
		break;

	case 'l':
		if ((*inst)[1] == 'r') {
			*inst += 2;
			return_ok(14);
		}
		break;

	case 'p':
		if ((*inst)[1] == 'c') {
			*inst += 2;
			return_ok(15);
		}
		break;
	}

	return_err(inv_value, "Invalid register (r0-12,sp,lr,pc).", 0, 15, 0);
}

static mas_err_t *thumb2_consume_arg_lexeme(const char **inst,
					    struct inst_arg_lexeme *out)
{
	const char *start = *inst;

	if ((*inst)[0] == '\0')
		return_ok((struct inst_arg_lexeme) { .ty = ARG_LEX_END });

	/* cond suffix */
	if (isalpha(start[0])) {
		if (strlen(start) >= 2 && isalpha(start[1]))
			for (size_t i = 0; i < COND_AL + 1; i++)
				if (start[0] == thumb2_mnemonic_cond_suffix[i][0] &&
				    start[1] == thumb2_mnemonic_cond_suffix[i][1]) {
					*inst += 2;
					return_ok((struct inst_arg_lexeme) {
						.ty = ARG_LEX_COND,
						.seminfo.cond_suffix = i
					});
				}
	}

	while (isspace((*inst)[0]) || (*inst)[0] == ',' || (*inst)[0] == ']')
		(*inst)++;

	switch ((*inst)[0]) {
	case '#': {  /* collect imm */
		(*inst)++;
		intmax_t simm = strtoimax(*inst, (char **) inst, 0);

		return_ok((struct inst_arg_lexeme) {
			.ty = ARG_LEX_IMM,
			.seminfo.simm = simm
		});
	}
	case 'r':
	case 's':
	case 'l':
	case 'p': {  /* collect reg */
		uint32_t reg;
		return_try(thumb2_consume_arg_reg_lexeme(inst, &reg));

		return_ok((struct inst_arg_lexeme) {
			.ty = ARG_LEX_REG,
			.seminfo.reg = reg
		});
	}

	case '{': {  /* collect reg_ls */
		struct inst_arg_data_reg_ls reg_ls = { 0 };

		(*inst)++;  /* consume the { */
		while (isspace((*inst)[0]))
			(*inst)++;

		while ((*inst)[0] != '}') {
			uint32_t reg;
			return_try(thumb2_consume_arg_reg_lexeme(inst, &reg));

			while (isspace((*inst)[0]) || (*inst)[0] == ',')
				(*inst)++;

			if ((*inst)[0] == '\0')
				return_err(syntax, "Unterminated register list.");

			if (!(reg < 8 || reg == 14 || reg == 15))
				return_err(inv_value,
					   "Only low registers and pc & lr "
					   "allowed in register list.",
					   0, 0, 0);
			if (reg < 8)
				reg_ls.reg_ls |= 1 << reg;
			if (reg == 14)
				reg_ls.lr = 1;
			if (reg == 15)
				reg_ls.pc = 1;
		}

		(*inst)++;  /* consume the } */

		return_ok((struct inst_arg_lexeme) {
			.ty = ARG_LEX_REG_LS,
			.seminfo.reg_ls = reg_ls
		});
	}

	case '%':  /* vaarg placeholders */
		*inst += 2;  /* collect %x */
		switch ((*inst)[-1]) {
		case 'r':
			return_ok((struct inst_arg_lexeme) { .ty = ARG_LEX_REG_VA_NUM });
		case 'R':
			return_ok((struct inst_arg_lexeme) { .ty = ARG_LEX_REG_VA_STR });
		case 'l':
			return_ok((struct inst_arg_lexeme) { .ty = ARG_LEX_REG_LS_VA });
		case 'i':
			return_ok((struct inst_arg_lexeme) { .ty = ARG_LEX_IMM_VA });
		default:
			return_err(syntax, "Invalid placeholder for %.");
		}
		break;

	case '[':  /* collect brace */
		(*inst)++;
		return_ok((struct inst_arg_lexeme) { .ty = ARG_LEX_BRACE });
	case '!':  /* excl */
		(*inst)++;
		return_ok((struct inst_arg_lexeme) { .ty = ARG_LEX_EXCL });
	case '\0':  /* end */
		return_ok((struct inst_arg_lexeme) { .ty = ARG_LEX_END });

	default:
		return_err(syntax, "Invalid char at input stream.");
	}

	_unreachable;
}

mas_err_t *thumb2_consume_arg(
		const char **inst,
		_unused mas_grammar_opcode_t opcode,
		_unused const void *op_seminfo,
		void *out_arg_lexeme,
		va_list argp,
		_unused struct mas_extension_get_data *ge)
{
	struct inst_arg_lexeme *out = out_arg_lexeme;

	struct inst_arg_lexeme arg_lexeme;
	return_try(thumb2_consume_arg_lexeme(inst, &arg_lexeme));

	switch (arg_lexeme.ty) {
	case ARG_LEX_REG_VA_STR:
		arg_lexeme.ty = ARG_LEX_REG;
		return_try(thumb2_consume_arg_reg_lexeme(
			   &(const char *) { va_arg(argp, char *) },
			   &arg_lexeme.seminfo.reg));
		break;

	case ARG_LEX_REG_VA_NUM:
		arg_lexeme.ty = ARG_LEX_REG;
		arg_lexeme.seminfo.reg = va_arg(argp, int);
		break;

	case ARG_LEX_REG_LS_VA:
		arg_lexeme.ty = ARG_LEX_REG_LS;
		arg_lexeme.seminfo.reg_ls.reg_ls = va_arg(argp, int);
		break;

	case ARG_LEX_IMM_VA:
		arg_lexeme.ty = ARG_LEX_IMM;
		arg_lexeme.seminfo.imm = va_arg(argp, int);
		break;

	default:
		break;
	}

	*out = arg_lexeme;
	return arg_lexeme.ty == ARG_LEX_END ? (void *) -1 : NULL;
}
