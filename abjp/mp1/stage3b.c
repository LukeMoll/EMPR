#include <lpc17xx_gpio.h>
#include <lpc17xx_systick.h>

#define LED_0 (1 << 18)
#define LED_1 (1 << 20)
#define LED_2 (1 << 21)
#define LED_3 (1 << 23)

const uint32_t LEDS[4] = { LED_0, LED_1, LED_2, LED_3 };
const uint32_t ALL_LEDS = LED_0 | LED_1 | LED_2 | LED_3;

void display_number(int n)
{
	int i;
	for (i = 0; i < 4; i++) {
		if (n & (1 << i)) {
			GPIO_SetValue(1, LEDS[i]);
		} else {
			GPIO_ClearValue(1, LEDS[i]);
		}
	}
}

void SysTick_Handler(void)
{
	static int time = 0;
	static int n = 0;

	SYSTICK_ClearCounterFlag();

	if (time < 10) {
		time++;
		return;
	}

	time = 0;

	if (n >= 16) {
		GPIO_ClearValue(1, ALL_LEDS);
		SYSTICK_IntCmd(DISABLE);
	} else {
		display_number(n++); // display n and _then_ increment it
	}
}

void main(void)
{
	GPIO_SetDir(1, ALL_LEDS, 1);

	SYSTICK_InternalInit(100);
	SYSTICK_IntCmd(ENABLE);
	SYSTICK_Cmd(ENABLE);
}
