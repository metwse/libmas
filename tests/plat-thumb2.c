#include "../include/mas.h"
#include "../plat/thumb2/as.h"
#include "../src/common.h"


int main(void)
{
	mas_xinit();

	struct mas_thumb2_as *as;
	as = mas_thumb2_as_xnew(0);

	mas_thumb2_as_yxfeed(as, "adcs r1, r2");

	mas_thumb2_as_yxfeed(as, "adds r1, r2, %i", 7);
	mas_thumb2_as_yxfeed(as, "adds r1, #255");
	mas_thumb2_as_yxfeed(as, "adds r3, r2, r1");

	mas_thumb2_as_yxfeed(as, "add lr, r15");
	mas_thumb2_as_yxfeed(as, "add lr, r5");
	mas_thumb2_as_yxfeed(as, "add r7, sp, #1016");
	mas_thumb2_as_yxfeed(as, "add sp, #504");
	mas_thumb2_as_yxfeed(as, "add pc, sp");
	mas_thumb2_as_yxfeed(as, "add r1, sp");
	mas_thumb2_as_yxfeed(as, "add sp, lr");
	mas_thumb2_as_yxfeed(as, "add sp, r2");
	mas_thumb2_as_yxfeed(as, "add r2, %R", "pc");

	mas_thumb2_as_yxfeed(as, "ands r2, r4");

	mas_thumb2_as_yxfeed(as, "asrs r2, r4, #31");
	mas_thumb2_as_yxfeed(as, "asrs r4, r3");

	mas_thumb2_as_yxfeed(as, "bne #4");
	mas_thumb2_as_yxfeed(as, "b #4");

	mas_thumb2_as_yxfeed(as, "bics r3, r7");
	mas_thumb2_as_yxfeed(as, "bkpt #170");

	mas_thumb2_as_yxfeed(as, "bl #16777216");

	mas_thumb2_as_yxfeed(as, "blx %r", 11);
	mas_thumb2_as_yxfeed(as, "bx %r", 10);
	mas_thumb2_as_yxfeed(as, "cmn r3, r5");

	mas_thumb2_as_yxfeed(as, "cmp r3, %i", 255);
	mas_thumb2_as_yxfeed(as, "cmp r2, r1");
	mas_thumb2_as_yxfeed(as, "cmp pc, sp");
	mas_thumb2_as_yxfeed(as, "cmp r1, sp");
	mas_thumb2_as_yxfeed(as, "cmp pc, r1");

	mas_thumb2_as_yxfeed(as, "eors r1, r4");

	mas_thumb2_as_yxfeed(as, "ldm r1!, { r4 }");
	mas_thumb2_as_yxfeed(as, "ldm r1, { r2, r4 }");
	mas_thumb2_as_yxfeed(as, "ldm r2!, { r4 }");
	mas_thumb2_as_yxfeed(as, "ldm r3, { r4, r5, r6 }");

	mas_thumb2_as_yxfeed(as, "ldr r1, r2");
	mas_thumb2_as_yxfeed(as, "ldr r1, [r2, #124]");
	mas_thumb2_as_yxfeed(as, "ldr r3, sp");
	mas_thumb2_as_yxfeed(as, "ldr r4, [sp, #1016]");
	mas_thumb2_as_yxfeed(as, "ldr r5, #1012");
	mas_thumb2_as_yxfeed(as, "ldr r6, [pc, #1008]");
	mas_thumb2_as_yxfeed(as, "ldr r7, [r1, r2]");

	mas_thumb2_as_yxfeed(as, "ldrb r7, [r2, #12]");
	mas_thumb2_as_yxfeed(as, "ldrb r7, r2");
	mas_thumb2_as_yxfeed(as, "ldrb r7, [r1, r2]");

	mas_thumb2_as_yxfeed(as, "ldrh r6, [r1, #13]");
	mas_thumb2_as_yxfeed(as, "ldrh r6, r3");
	mas_thumb2_as_yxfeed(as, "ldrh r6, [r0, r3]");

	mas_thumb2_as_yxfeed(as, "ldrsh r6, [r0, r3]");
	mas_thumb2_as_yxfeed(as, "ldrsb r6, [r0, r3]");

	mas_thumb2_as_yxfeed(as, "lsls r5, r1, #31");
	mas_thumb2_as_yxfeed(as, "lsls r5, r1");
	mas_thumb2_as_yxfeed(as, "lsrs r5, r1, #31");
	mas_thumb2_as_yxfeed(as, "lsrs r5, r1");

	mas_thumb2_as_yxfeed(as, "movs r3, #255");
	mas_thumb2_as_yxfeed(as, "mov r10, r3");
	mas_thumb2_as_yxfeed(as, "movs r1, r3");

	mas_thumb2_as_yxfeed(as, "muls r3, r4");
	mas_thumb2_as_yxfeed(as, "mvns r5, r3");
	mas_thumb2_as_yxfeed(as, "nop");

	mas_thumb2_as_yxfeed(as, "orrs r6, r3");

	mas_thumb2_as_yxfeed(as, "push { r1, r2, r7 }");
	mas_thumb2_as_yxfeed(as, "push { r2, r4, r7, lr }");
	mas_thumb2_as_yxfeed(as, "pop { r1, r2, r7 }");
	mas_thumb2_as_yxfeed(as, "pop { r2, r4, r7, pc }");

	mas_thumb2_as_yxfeed(as, "negs r1, r3");
	mas_thumb2_as_yxfeed(as, "rsbs r1, r3, #0");
	mas_thumb2_as_yxfeed(as, "rsbs r2, r1, #0");
	mas_thumb2_as_yxfeed(as, "rsbs r2, r1");
	mas_thumb2_as_yxfeed(as, "sbcs r4, r7");
	mas_thumb2_as_yxfeed(as, "sev ");
	mas_thumb2_as_yxfeed(as, "sev");

	mas_thumb2_as_yxfeed(as, "stm r1!, { r4 }");
	mas_thumb2_as_yxfeed(as, "stm r1!, { r1, r2, r4 }");
	mas_thumb2_as_yxfeed(as, "stm r3!, { r3, r5, r6 }");

	mas_thumb2_as_yxfeed(as, "str r1, r2");
	mas_thumb2_as_yxfeed(as, "str r1, [r2, #8]");
	mas_thumb2_as_yxfeed(as, "str r2, sp");
	mas_thumb2_as_yxfeed(as, "str r2, [sp, #8]");
	mas_thumb2_as_yxfeed(as, "str r2, [r3, r4]");

	mas_thumb2_as_yxfeed(as, "strb r3, r4");
	mas_thumb2_as_yxfeed(as, "strb r3, [r4, #7]");
	mas_thumb2_as_yxfeed(as, "strb r3, [r4, r3]");

	mas_thumb2_as_yxfeed(as, "strh r3, r4");
	mas_thumb2_as_yxfeed(as, "strh r3, [r4, #14]");
	mas_thumb2_as_yxfeed(as, "strh r3, [r4, r3]");

	mas_thumb2_as_yxfeed(as, "subs r3, r4, #7");
	mas_thumb2_as_yxfeed(as, "subs r3, #255");
	mas_thumb2_as_yxfeed(as, "subs r3, r4, r7");
	mas_thumb2_as_yxfeed(as, "sub sp, sp, #128");

	mas_thumb2_as_yxfeed(as, "udf #123");
	mas_thumb2_as_yxfeed(as, "udf");

	void *out = xmalloc(mas_thumb2_as_output_size(as));
	mas_thumb2_as_yfinalize(as, out);
	free(out);

	mas_destroy();
}
