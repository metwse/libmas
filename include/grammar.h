/**
 * @file grammar.h
 * @brief libmas grammar object interface.
 */

#ifndef MAS_GRAMMAR_H
#define MAS_GRAMMAR_H

#include "error.h"

#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>

struct mas_extension_get_data  /* defined in extension.h */;


/** @brief Numeric identifier for a opcode. */
typedef uint32_t mas_grammar_opcode_t;

/** @brief Numeric identifier for a parameter. */
typedef uint32_t mas_grammar_param_id_t;

/** @brief Index of the matched alternative in the grammar. */
typedef size_t mas_grammar_alt_idx_t;

/**
 * @brief Parse and consume the instruction opcode.
 *
 * Advances @p inst to end of the opcode on success.
 *
 * @param inst Pointer to the instruction string, updated as the opcode is consumed.
 * @param out_opcode Numeric opcode for the mnemonic.
 * @param out_op_seminfo Extra information for parsed mnemonic.
 *
 * @return NULL on sucess, `(void *) -1` if the instruction does not
 *         belong to this grammar, and an error pointer on failure.
 */
typedef mas_err_t *(*mas_grammar_consume_opcode_method_t)(
		const char **inst,
		mas_grammar_opcode_t *out_opcode,
		void *out_op_seminfo,
		struct mas_extension_get_data *);
/**
 * @brief Parse and consume the next argument lexeme.
 *
 * Advances @p inst to end of the opcode on success.
 *
 * @param inst Pointer to the remaining instruction string.
 * @param opcode Opcode of the parsed instruction.
 * @param op_seminfo Extra semantic information for the parsed instruction.
 * @param out_lexeme Output buffer for the parsed lexeme.
 * @param argp Variable arg list pointer to consume by lexer.
 *
 * @return NULL on success or error pointer on failure.
 */
typedef mas_err_t *(*mas_grammar_consume_arg_method_t)(
		const char **inst,
		mas_grammar_opcode_t opcode,
		const void *op_seminfo,
		void *out_lexeme,
		va_list argp,
		struct mas_extension_get_data *);

/**
 * @brief Validate a lexeme and convert it into an argument.
 *
 * Validates lexeme against `param` and `arg_idx`, return resulting
 * argument via `out_arg`.
 *
 * @param lexeme Lexeme to validate.
 * @param param_id Parameter identifier to check lexeme with.
 * @param arg_idx Index of the lexeme in arguments.
 * @param out_arg Output buffer for the produced argument.
 *
 * @return NULL on sucess, `(void *) -1` if the lexeme does not produce
 *         a token, and an error pointer on failure.
 */
typedef mas_err_t *(*mas_grammar_validate_arg_method_t)(
		const void *lexeme,
		mas_grammar_param_id_t param_id,
		size_t arg_idx,
		void *out_arg,
		struct mas_extension_get_data *);

/**
 * @brief Emit instruction machine code.
 *
 * @param opcode Numeric identifier for the mnemonic.
 * @param args Pointer to instruction arguments.
 * @parma alt_idx Index of matched alternative in the grammar.
 * @param out Output machine code.
 *
 * @return NULL on succes and an error pointer on failure.
 */
typedef mas_err_t *(*mas_grammar_emit_method_t)(
		mas_grammar_opcode_t opcode,
		const void *args,
		mas_grammar_alt_idx_t alt_idx,
		char *out,
		struct mas_extension_get_data *);

/** @brief Returns size of the instruction in bytes. */
typedef size_t (*mas_grammar_get_inst_size_method_t)(
		mas_grammar_opcode_t opcode,
		const void *args,
		mas_grammar_alt_idx_t alt_idx,
		struct mas_extension_get_data *);


/** @brief Assembly grammar definitions. */
struct mas_grammar_symbol {
	enum mas_grammar_symbol_ty {
		MAS_GRAMMAR_SYMBOL_PARAM_ID  /** A parameter is specified. */,
		MAS_GRAMMAR_SYMBOL_QUALIFICATION  /** A qualification is specified. */,
		MAS_GRAMMAR_SYMBOL_ALT_END /** End of an alternative. */,
		MAS_GRAMMAR_SYMBOL_PRODUCTIONS_END /** End of the list of alternatives. */,
		MAS_GRAMMAR_SYMBOL_NOPARAM /** An alternative with no argument. */,
	} ty  /** Parameter type. */;

	union mas_grammar_symbol_data {
		mas_grammar_param_id_t param_id  /** A parameter ID. */;

		enum mas_grammar_symbol_qualification {
			MAS_GRAMMAR_SYMBOL_QUALIFICATION_ZERO_OR_MORE /** `*` */,
			MAS_GRAMMAR_SYMBOL_QUALIFICATION_ONE_OR_MORE /** `+` */,
			MAS_GRAMMAR_SYMBOL_QUALIFICATION_ZERO_OR_ONE /** `?` */,
			MAS_GRAMMAR_SYMBOL_QUALIFICATION_GROUP_START /** `(` */,
			MAS_GRAMMAR_SYMBOL_QUALIFICATION_GROUP_END /** `)` */,
		} qualification  /** A regex qualification. */;
	} data  /** Grammar rule. */;
};

/** @brief An assembler grammar. */
struct mas_grammar {
	/** @brief Grammar name. */
	const char *name;

	/** @brief Regex for the grammar. */
	const struct mas_grammar_symbol *grammar;

	/** @brief Function pointer for consuming the opcode from instruction
	 * string. */
	mas_grammar_consume_opcode_method_t consume_opcode;
	/** @brief Fuction pointer for consuming an argument from the remaining
	 * part of instruction. */
	mas_grammar_consume_arg_method_t consume_arg;
	/** @brief Funciton pointer for validating an argument. */
	mas_grammar_validate_arg_method_t validate_arg;
	/** @brief Funciton pointer for emitting machine code. */
	mas_grammar_emit_method_t emit;
	/** @brief Function pointer for querying machine code size of an
	 * instruction. */
	mas_grammar_emit_method_t get_inst_size;
};


#endif
