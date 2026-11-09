#include "../include/mas.h"
#include "../plat/thumb2/detail/static-init.h"


void mas_xinit_lib(void)
{
	thumb2_lexer_static_xinit();
}

void mas_deinit_lib(void)
{
	thumb2_lexer_static_destroy();
}
