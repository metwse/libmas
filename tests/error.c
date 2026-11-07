#include "../include/error.h"
#include "../src/error.h"

#include <stdio.h>


int main(void)
{
	mas_err_t *err = err_xnew_list("This is a test error list.");

	mas_err_t *inv_val = err_xnew_inv_value(NULL, 0, 10, 0);
	err_push(inv_val, err_xnew_inv_mnemonic("A mnemonic error message.", "a_mnemonic"));

	mas_err_t *tail = err_push(err, inv_val);
	err_push(tail, err_xnew_label_not_found(NULL, "label404"));

	mas_err_message(err, stdout);

	mas_err_free(err);
}
