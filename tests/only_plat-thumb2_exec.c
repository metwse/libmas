#include "../include/mas.h"
#include "../osal/osal.h"
#include "../plat/thumb2/as.h"


int main(void)
{
	mas_xinit();

	struct mas_thumb2_as *as;
	as = mas_thumb2_as_xnew(0);

	mas_thumb2_as_yxfeed(as, "mov pc, lr");

	struct mas_execmem e;
	mas_execmem_xalloc(&e, mas_thumb2_as_output_size(as));
	mas_thumb2_as_yfinalize(as, e.mem);

	mas_execmem_set_mode(&e, MAS_EXECMEM_RE);
	e.execmem();

	mas_destroy();
}
