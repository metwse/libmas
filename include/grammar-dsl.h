/**
 * @file grammar-dsl.h
 * @brief A set of macros to faciliate grammar definitions in libmas.
 */

#ifndef MAS_GRAMMAR_DSL_H
#define MAS_GRAMMAR_DSL_H


/** @brief An alternative with no argument. */
#define EPSILON { .ty = MAS_GRAMMAR_SYMBOL_NOPARAM }


/** @brief A macro to define grammar rule. */
#define r(opcode, ...) \
	[opcode] = { \
		{ __VA_ARGS__, { .ty = MAS_GRAMMAR_SYMBOL_ALT_END } }, \
		{ { .ty = MAS_GRAMMAR_SYMBOL_PRODUCTIONS_END } } \
	}

/** @brief A parameter. */
#define rp(param_id2) { \
		.ty = MAS_GRAMMAR_SYMBOL_PARAM_ID, \
		.data.param_id = param_id2 \
	}

/** @brief Zero or more qualifier. */
#define rzero_or_more(sym) { \
		.ty = MAS_GRAMMAR_SYMBOL_QUALIFICATION, \
		.data.qualification = MAS_GRAMMAR_SYMBOL_QUALIFICATION_ZERO_OR_MORE \
	}, sym

/** @brief One or more qualifier. */
#define rone_or_more(sym) { \
		.ty = MAS_GRAMMAR_SYMBOL_QUALIFICATION, \
		.data.qualification = MAS_GRAMMAR_SYMBOL_QUALIFICATION_ONE_OR_MORE \
	}, sym

/** @brief Zero or one qualifier. */
#define rzero_or_one(sym) { \
		.ty = MAS_GRAMMAR_SYMBOL_QUALIFICATION, \
		.data.qualification = MAS_GRAMMAR_SYMBOL_QUALIFICATION_ZERO_OR_ONE \
	}, sym

/** @brief Grouped symbols. */
#define rgroup(...) { \
		.ty = MAS_GRAMMAR_SYMBOL_QUALIFICATION, \
		.data.qualification = MAS_GRAMMAR_SYMBOL_QUALIFICATION_GROUP_START \
	}, __VA__ARGS, { \
		.ty = MAS_GRAMMAR_SYMBOL_QUALIFICATION, \
		.data.qualification = MAS_GRAMMAR_SYMBOL_QUALIFICATION_GROUP_END \
	}

/** @brief Seperates grammar alternatives. */
#define alt , { .ty = MAS_GRAMMAR_SYMBOL_ALT_END } }, {


#endif
