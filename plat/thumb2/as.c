#include "as.h"
#include "detail/detail.h"
#include "detail/grammar.h"
#include "detail/opcodes.h"

#include "../../src/common.h"
#include "../../src/error.h"

#include <stdarg.h>
#include <stdint.h>


#define cur ((uint16_t *) as->e.mem)[as->i]
#define inc as->i++


/* TODO: hints */
struct mas_thumb2_as *mas_thumb2_as_xnew(enum mas_as_hint _unused hints)
{
	struct mas_thumb2_as *as = xmalloc(sizeof(struct mas_thumb2_as));

	fstack_asm_item_xinit(&as->asm);
	as->output_size = 0;

	fhmap_label_idx_map_xinit(&as->label_idx_map);
	as->up_as = NULL;

	return as;
}

static inline mas_err_t *thumb2_as_xfeed_inner(struct mas_thumb2_as *as,
					       const char *inst_str,
					       va_list ap)
{
	enum inst_opcode op;
	return_try(thumb2_consume_mnemonic(&inst_str, &op));

	struct inst_arg_tk arg_tks[MAX_PARAM_COUNT + 1];

	for (size_t i = 0; i < MAX_PARAM_COUNT + 1; i++) {
		struct inst_arg_tk arg_tk;
		return_try(thumb2_consume_arg_tk(&inst_str, i == 0, &arg_tk));

		switch (arg_tk.ty) {
		case ARG_TK_REG_VA_STR:
			arg_tk.ty = ARG_TK_REG;
			return_try(thumb2_consume_arg_reg_tk(
				   &(const char *) { va_arg(ap, char *) },
				   &arg_tk.seminfo.parsed.reg));
			break;

		case ARG_TK_REG_VA_NUM:
			arg_tk.ty = ARG_TK_REG;
			arg_tk.seminfo.parsed.reg = va_arg(ap, int);
			break;

		case ARG_TK_REG_LS_VA:
			arg_tk.ty = ARG_TK_REG_LS;
			arg_tk.seminfo.parsed.reg_ls.reg_ls = va_arg(ap, int);
			break;

		case ARG_TK_IMM_VA:
			arg_tk.ty = ARG_TK_IMM;
			arg_tk.seminfo.parsed.imm = va_arg(ap, int);
			break;

		default:
			break;
		}

		arg_tks[i] = arg_tk;
		if (arg_tks[i].ty == ARG_TK_END)
			break;
	}

	struct inst inst;
	return_try(thumb2_validate(op, arg_tks, &inst));

	fstack_asm_item_xpush(&as->asm, &(const struct asm_item) {
				     .ty = ASM_ITEM_INST,
				     .item.inst = inst
			      });
	as->output_size += thumb2_inst_size(&inst);

	return NULL;
}

void mas_thumb2_as_yxfeed(struct mas_thumb2_as *as,
			  const char *inst_str,
			  ...)
{
	va_list ap;
	va_start(ap, inst_str);
	mas_err_unwrap(thumb2_as_xfeed_inner(as, inst_str, ap));
	va_end(ap);
}

mas_err_t *mas_thumb2_as_xfeed(struct mas_thumb2_as *as,
			       const char *inst_str,
			       ...)
{
	va_list ap;
	va_start(ap, inst_str);
	mas_err_t *res = thumb2_as_xfeed_inner(as, inst_str, ap);
	va_end(ap);

	return res;
}

mas_err_t *mas_thumb2_as_xappend(struct mas_thumb2_as *as,
				 struct mas_thumb2_as *extend_with,
				 bool isolate_scope)
{
	extend_with->up_as = isolate_scope ? NULL : as;

	fstack_asm_item_xpush(&as->asm, &(const struct asm_item) {
				     .ty = ASM_ITEM_AS,
				     .item.as = extend_with
			     });
	as->output_size += extend_with->output_size;

	return NULL;
}

size_t mas_thumb2_as_output_size(const struct mas_thumb2_as *as)
{
	return as->output_size;
}

mas_err_t *mas_thumb2_as_finalize(struct mas_thumb2_as *as, void *out_)
{
	char *out = out_;

	for (size_t i = 0; i < fstack_asm_item_len(&as->asm); i++) {
		struct asm_item *item = fstack_asm_item_at_mut(&as->asm, i);

		if (item->ty == ASM_ITEM_INST) {
			thumb2_inst_emit(out, &item->item.inst);
			out += thumb2_inst_size(&item->item.inst);
		} else if (item->item.as) {
			out += item->item.as->output_size;
			mas_err_t *err = mas_thumb2_as_finalize(item->item.as,
								out);

			if (err)
				return err;

			mas_thumb2_as_destroy(item->item.as);
			item->item.as = NULL;
		}
	}

	fstack_asm_item_clear(&as->asm);
	mas_thumb2_as_destroy(as);

	return NULL;
}

void mas_thumb2_as_destroy(struct mas_thumb2_as *as)
{
	for (size_t i = 0; i < fstack_asm_item_len(&as->asm); i++) {
		struct asm_item *item = fstack_asm_item_at_mut(&as->asm, i);

		if (item->item.as)
			mas_thumb2_as_destroy(item->item.as);
	}

	fstack_asm_item_destroy(&as->asm);
	fhmap_label_idx_map_destroy(&as->label_idx_map);
	free(as);
}
