#include "thumb2.h"
#include "detail/grammar.h"
#include "detail/methods.h"
#include "detail/opcodes.h"

#include "../../include/grammar.h"


const struct mas_grammar mas_plat_thumb2_grammar = {
	.name = "thumb2",

	.grammar = (const struct mas_grammar_symbol *) thumb2_grammar,

	.inst_count = INST_COUNT,
	.max_alt_count = MAX_ALT_COUNT,
	.max_alt_size = MAX_ALT_SIZE,

	.sizeof_op_seminfo = 0,
	.sizeof_arg_lexeme = sizeof(struct inst_arg_lexeme),
	.sizeof_arg = sizeof(union inst_arg),

	.consume_opcode = thumb2_consume_opcode,
	.consume_arg = thumb2_consume_arg,
	.validate_arg = thumb2_validate_arg,
	.emit = thumb2_emit,
	.get_inst_size = thumb2_get_inst_size,
};
