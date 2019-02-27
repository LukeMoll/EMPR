#include <stdint.h>
#include <stdbool.h>

#include "keypad.h"
#include "i2c.h"

#define KEYPAD_ADDR 0x21
                                                   
static const char KEYS[4][4] = {
	{ 'A', '3', '2', '1' },
	{ 'B', '6', '5', '4' },
	{ 'C', '9', '8', '7' },
	{ 'D', '#', '0', '*' },
};

/**
 * Returns currently pressed key.
 */
char keypad_get_key(void) {
	uint8_t buf, col, row;
	row = 0;

	for (col = 0; col < 4 && !row; col++) {
		buf = 0xff - (1 << (4 + col));
		i2c_write_byte(KEYPAD_ADDR, buf);
		i2c_read_byte(KEYPAD_ADDR, &buf);

		switch (buf & 0x0f) {
			case 0x07:
				row = 1;
				break;
			case 0x0b:
				row = 2;
				break;
			case 0x0d:
				row = 3;
				break;
			case 0x0e:
				row = 4;
				break;
			default:
				row = 0;
		}
	}

	if (!row)
		return 0;

	return KEYS[row - 1][col - 1];
}

char keypad_wait_for_key(void) {
	static bool key_pressed = false;

	char key = 0;
	while (!key) {
		if (!(key = keypad_get_key()))
			key_pressed = false;
		else if (!key_pressed)
			key_pressed = true;
	}

	return key;
}
