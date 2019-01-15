#include <lpc17xx_systick.h>

#define DEMO
#include "stage1.c"
#include "stage2a.c"
#include "stage2b.c"
#include "stage3.c"

void SysTick_Handler(void)
{
	static int n = 1;
	static int time = 0;

	SYSTICK_ClearCounterFlag();

	if (time < 10) {
		time++;
		return;
	}

	time = 0;

	switch (n++) {
		case 1:
			stage2a();
			break;
		case 2:
			stage2b();
			break;
		default:
			SYSTICK_IntCmd(DISABLE);
			stage3();
	}
}

void main(void)
{
	stage1();

	SYSTICK_InternalInit(100);
	SYSTICK_IntCmd(ENABLE);
	SYSTICK_Cmd(ENABLE);

	while (1) {}
}
