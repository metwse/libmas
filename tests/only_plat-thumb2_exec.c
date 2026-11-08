#include "../include/mas.h"
#include "../osal/osal.h"
#include "../plat/thumb2/as.h"


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
	mas_xinit();

	struct mas_thumb2_as *as;
	as = mas_thumb2_as_xnew(0);

	/* Function for calculating nth Fibonacci number, using fast doubling
	 * method. */
	mas_thumb2_as_yxfeed(as, "subs r0, #2");
	mas_thumb2_as_yxfeed(as, "bgt #2");
	mas_thumb2_as_yxfeed(as, "movs r0, #1");
	mas_thumb2_as_yxfeed(as, "mov pc, lr");

	mas_thumb2_as_yxfeed(as, "movs r1, #1");
	mas_thumb2_as_yxfeed(as, "movs r2, #1");

	mas_thumb2_as_yxfeed(as, "adds r1, r1, r2");
	mas_thumb2_as_yxfeed(as, "subs r0, r0, #1");
	mas_thumb2_as_yxfeed(as, "beq #8");
	mas_thumb2_as_yxfeed(as, "adds r2, r1, r2");
	mas_thumb2_as_yxfeed(as, "subs r0, r0, #1");
	mas_thumb2_as_yxfeed(as, "bne #-14");
	mas_thumb2_as_yxfeed(as, "movs r0, r2");
	mas_thumb2_as_yxfeed(as, "b #0");
	mas_thumb2_as_yxfeed(as, "movs r0, r1");

	mas_thumb2_as_yxfeed(as, "mov pc, lr");

	struct mas_execmem e;
	mas_execmem_xalloc(&e, mas_thumb2_as_output_size(as));
	mas_thumb2_as_yfinalize(as, e.mem);

	mas_execmem_set_mode(&e, MAS_EXECMEM_RE);

	int (*fib_jit)(int) = (int (*)(int)) e.execmem;

	for (int i = 1; i < 10; i++)
		assert(fib_jit(i) == fib(i));

	mas_execmem_free(&e);

	mas_destroy();
}
