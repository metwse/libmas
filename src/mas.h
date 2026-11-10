#ifndef MAS_MAS_INTERNAL_H
#define MAS_MAS_INTERNAL_H

#include "../include/grammar.h"

#include <stddef.h>

#define T struct mas_grammar, grammars
#include "../vendor/libfun/include/stack.h"

#define T char, asm_items
#include "../vendor/libfun/include/stack.h"

#define T char, arg_lexeme_buf
#include "../vendor/libfun/include/stack.h"

#define T char, arg_buf
#include "../vendor/libfun/include/stack.h"



/* The assembler. */
struct mas {
	struct fstack_grammars grammars;
};

/* Instruction of an grammar that is ready for being emitted. */
struct asm_item_inst {
	size_t grammar_idx;
	size_t size;

	mas_grammar_opcode_t opcode;
	mas_grammar_alt_idx_t alt_idx;
	char args[];
};

/* An assembly item. */
struct asm_item {
	enum asm_item_ty {
		ASM_ITEM_INST,
	} ty;

	char data[];
};

/* An assembler instance. */
struct mas_handle {
	const struct mas *mas;
	struct fstack_asm_items asm  /* compact asm items stack */;
	struct fstack_arg_lexeme_buf arg_lexemes  /* extracted arguments */;
	struct fstack_arg_buf args  /* parsed & validated arguments */;
	size_t output_size;
};


#endif
