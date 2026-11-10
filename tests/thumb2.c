#include "../include/mas.h"
#include "../plat/thumb2/thumb2.h"


int main(void)
{
	mas_xinit_lib();
	struct mas *mas = mas_xnew();
	mas_xpush_grammar(mas, &mas_plat_thumb2_grammar);

	struct mas_handle *h = mas_xstart(mas);

	mas_handle_yxfeed(h, "adcs r1, r2");

	mas_handle_yxfeed(h, "adds r1, r2, %i", 7);
	mas_handle_yxfeed(h, "adds r1, #255");
	mas_handle_yxfeed(h, "adds r3, r2, r1");

	mas_handle_yxfeed(h, "add lr, r15");
	mas_handle_yxfeed(h, "add lr, r5");
	mas_handle_yxfeed(h, "add r7, sp, #1016");
	mas_handle_yxfeed(h, "add sp, #504");
	mas_handle_yxfeed(h, "add pc, sp");
	mas_handle_yxfeed(h, "add r1, sp");
	mas_handle_yxfeed(h, "add sp, lr");
	mas_handle_yxfeed(h, "add sp, r2");
	mas_handle_yxfeed(h, "add r2, %R", "pc");

	mas_handle_yxfeed(h, "ands r2, r4");

	mas_handle_yxfeed(h, "asrs r2, r4, #31");
	mas_handle_yxfeed(h, "asrs r4, r3");

	mas_handle_yxfeed(h, "bne #4");
	mas_handle_yxfeed(h, "b #4");

	mas_handle_yxfeed(h, "bics r3, r7");
	mas_handle_yxfeed(h, "bkpt #255");

	mas_handle_yxfeed(h, "bl %i", -16777216);
	mas_handle_yxfeed(h, "bl #-12");
	mas_handle_yxfeed(h, "bl %i", 16777214);

	mas_handle_yxfeed(h, "blx %r", 11);
	mas_handle_yxfeed(h, "bx %r", 10);
	mas_handle_yxfeed(h, "cmn r3, r5");

	mas_handle_yxfeed(h, "cmp r3, %i", 255);
	mas_handle_yxfeed(h, "cmp r2, r1");
	mas_handle_yxfeed(h, "cmp pc, sp");
	mas_handle_yxfeed(h, "cmp r1, sp");
	mas_handle_yxfeed(h, "cmp pc, r1");

	mas_handle_yxfeed(h, "eors r1, r4");

	mas_handle_yxfeed(h, "ldm r1!, { r4 }");
	mas_handle_yxfeed(h, "ldm r1, { r2, r4 }");
	mas_handle_yxfeed(h, "ldm r2!, { r4 }");
	mas_handle_yxfeed(h, "ldm r3, { r4, r5, r6 }");

	mas_handle_yxfeed(h, "ldr r1, r2");
	mas_handle_yxfeed(h, "ldr r1, [r2, #124]");
	mas_handle_yxfeed(h, "ldr r3, sp");
	mas_handle_yxfeed(h, "ldr r4, [sp, #1016]");
	mas_handle_yxfeed(h, "ldr r5, #1012");
	mas_handle_yxfeed(h, "ldr r6, [pc, #1008]");
	mas_handle_yxfeed(h, "ldr r7, [r1, r2]");

	mas_handle_yxfeed(h, "ldrb r7, [r2, #12]");
	mas_handle_yxfeed(h, "ldrb r7, r2");
	mas_handle_yxfeed(h, "ldrb r7, [r1, r2]");

	mas_handle_yxfeed(h, "ldrh r6, [r1, #13]");
	mas_handle_yxfeed(h, "ldrh r6, r3");
	mas_handle_yxfeed(h, "ldrh r6, [r0, r3]");

	mas_handle_yxfeed(h, "ldrsh r6, [r0, r3]");
	mas_handle_yxfeed(h, "ldrsb r6, [r0, r3]");

	mas_handle_yxfeed(h, "lsls r5, r1, #31");
	mas_handle_yxfeed(h, "lsls r5, r1");
	mas_handle_yxfeed(h, "lsrs r5, r1, #31");
	mas_handle_yxfeed(h, "lsrs r5, r1");

	mas_handle_yxfeed(h, "movs r3, #255");
	mas_handle_yxfeed(h, "mov r10, r3");
	mas_handle_yxfeed(h, "movs r1, r3");

	mas_handle_yxfeed(h, "muls r3, r4");
	mas_handle_yxfeed(h, "mvns r5, r3");
	mas_handle_yxfeed(h, "nop");

	mas_handle_yxfeed(h, "orrs r6, r3");

	mas_handle_yxfeed(h, "push { r1, r2, r7 }");
	mas_handle_yxfeed(h, "push { r2, r4, r7, lr }");
	mas_handle_yxfeed(h, "pop { r1, r2, r7 }");
	mas_handle_yxfeed(h, "pop { r2, r4, r7, pc }");

	mas_handle_yxfeed(h, "negs r1, r3");
	mas_handle_yxfeed(h, "rsbs r1, r3, #0");
	mas_handle_yxfeed(h, "rsbs r2, r1, #0");
	mas_handle_yxfeed(h, "rsbs r2, r1");
	mas_handle_yxfeed(h, "sbcs r4, r7");
	mas_handle_yxfeed(h, "sev ");
	mas_handle_yxfeed(h, "sev");

	mas_handle_yxfeed(h, "stm r1!, { r4 }");
	mas_handle_yxfeed(h, "stm r1!, { r1, r2, r4 }");
	mas_handle_yxfeed(h, "stm r3!, { r3, r5, r6 }");

	mas_handle_yxfeed(h, "str r1, r2");
	mas_handle_yxfeed(h, "str r1, [r2, #8]");
	mas_handle_yxfeed(h, "str r2, sp");
	mas_handle_yxfeed(h, "str r2, [sp, #8]");
	mas_handle_yxfeed(h, "str r2, [r3, r4]");

	mas_handle_yxfeed(h, "strb r3, r4");
	mas_handle_yxfeed(h, "strb r3, [r4, #7]");
	mas_handle_yxfeed(h, "strb r3, [r4, r3]");

	mas_handle_yxfeed(h, "strh r3, r4");
	mas_handle_yxfeed(h, "strh r3, [r4, #14]");
	mas_handle_yxfeed(h, "strh r3, [r4, r3]");

	mas_handle_yxfeed(h, "subs r3, r4, #7");
	mas_handle_yxfeed(h, "subs r3, #255");
	mas_handle_yxfeed(h, "subs r3, r4, r7");
	mas_handle_yxfeed(h, "sub sp, sp, #128");

	mas_handle_yxfeed(h, "udf #123");
	mas_handle_yxfeed(h, "udf");

	mas_handle_destroy(h);

	mas_destroy(mas);
	mas_deinit_lib();
}
