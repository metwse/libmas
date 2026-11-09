/**
 * @file extension.h
 * @brief Extensions for sharing data between layers of grammars.
 */

#ifndef MAS_EXTENSION_H
#define MAS_EXTENSION_H


/** @brief Get data in an extension. */
#define mas_extension_get_data_fn(ty, fat_pointer, extension_name) \
	(ty *) (fat_pointer)->method((fat_pointer)->self, extension_name);


/** @brief Fat function pointer for getting instruction data. */
struct mas_extension_get_data {
	/** @cond */
	void *self;
	void *(*method)(void *self, const char *);
	/** @endcond */
};

/**
 * @brief Create a new instance of extension.
 *
 * @return Pointer to instance data for the extension. */
typedef void *(*mas_extension_new_method_t)(
		struct mas_extension_get_data *);

/** @brief Free resources owned by the extension and free it. */
typedef void (*mas_extension_destroy_method_t)(
		void *extension_data);

/** @brief An assembler extension. */
struct mas_extension {
	/** @brief Extension name. */
	const char *name;

	/** @brief Function pointer for creating a new extension instance. */
	mas_extension_new_method_t new;
	/** @brief Function pointer to destroy a extension instance. */
	mas_extension_new_method_t destroy;
};


#endif
