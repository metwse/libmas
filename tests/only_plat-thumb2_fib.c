#include "../include/mas.h"
#include "../plat/thumb2/thumb2.h"


static int fib(register int n)
{
	if (n <= 2)
		return 1;

	n -= 2;
	register int i = 1;
	register int j = 1;

	do {
		i = i + j;
		n -= 1;
		if (n == 0)
			goto return_i;

		j = i + j;
		n -= 1;
	} while (n);

	return j;
return_i:
	return i;
}

int main(void)
{
	mas_xinit_lib();
	struct mas *mas = mas_xnew();
	mas_xpush_grammar(mas, &mas_plat_thumb2_grammar);

	struct mas_handle *h = mas_xstart(mas);

	/* Function for calculating nth Fibonacci number, using fast doubling
	 * method. */
	mas_handle_yxfeed(h, "subs r0, #2");
	mas_handle_yxfeed(h, "bgt #2");
	mas_handle_yxfeed(h, "movs r0, #1");
	mas_handle_yxfeed(h, "mov pc, lr");

	mas_handle_yxfeed(h, "movs r1, #1");
	mas_handle_yxfeed(h, "movs r2, #1");

	mas_handle_yxfeed(h, "adds r1, r1, r2");
	mas_handle_yxfeed(h, "subs r0, r0, #1");
	mas_handle_yxfeed(h, "beq #8");
	mas_handle_yxfeed(h, "adds r2, r1, r2");
	mas_handle_yxfeed(h, "subs r0, r0, #1");
	mas_handle_yxfeed(h, "bne #-14");
	mas_handle_yxfeed(h, "movs r0, r2");
	mas_handle_yxfeed(h, "b #0");
	mas_handle_yxfeed(h, "movs r0, r1");

	mas_handle_yxfeed(h, "mov pc, lr");

	/*
	struct mas_execmem e;
	mas_execmem_xalloc(&e, mas_thumb2_as_output_size(as));
	mas_thumb2_as_yfinalize(as, e.mem);

	mas_execmem_set_mode(&e, MAS_EXECMEM_RE);

	int (*fib_jit)(int) = (int (*)(int)) e.execmem;

	for (int i = 1; i < 10; i++)
		assert(fib_jit(i) == fib(i));

	mas_execmem_free(&e);
	*/

	mas_handle_destroy(h);

	mas_destroy(mas);
	mas_deinit_lib();
}
