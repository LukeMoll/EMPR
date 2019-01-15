Code Style
===

The following code style will be used for all C code.

## Line limit
There's no hard limit but try, if possible, to keep lines under 100 characters.

## Indentation
Blocks shall be indented using **tabs**

Switch-case shall be indented as such:
```c
switch(variable) {
	case value:
		// case body here
		break;
	default:
		// default body here
		break;
}
```

## Braces
That is, opening braces should be on the same line as their control statement.
In the case of control statements following a closing brace (e.g. `else`, `else
if`, and do-while), the control statment should go on the same line as the
closing brace.

```c
void foo(int x) {
	if (x > 0) {
		// do some things
	} else if (x == 0) {
		// do some other things
	} else {
		// do yet other things
	}
}
```

For single statement bodies, braces may be elided if none of the other bodies
need to be blocks (see below).

```c
void foo(int x) {
	// okay
	if (x > 0)
		do_a_thing();

	// also okay
	if (x > 0)
		do_a_thing();
	else
		do_another_thing;

        // bad. no.
	if (x > 0)
		do_a_thing();
	else {
		// do some other things
	}
}
```

## Case
Functions and variables should both use `snake_case`. Constants should be
`ANGRY_SNAKE`. Preprocessor commands shall use `lowercase`

## Operators
Parentheses are free. Use them to disambiguate complex expressions. Be careful
about things seeming "obvious".

## Spaces
Spacing should follow the [Linux Kernel style (§3.1)][linux-spacing]. In short,
spaces **should** go:

- after most keywords (e.g. `if (foo)`)
- before but not after a pointer asterisk (e.g. `char *foo`)
- around almost all binary operators (e.g. `a + b`)

Spaces **should not** go:

- directly inside parentheses (e.g. no `foo( x )`)
- between a function and its parameters (e.g. no `foo (x)`)
- between unary operators and their operand (e.g. `x++` and `-y`)
- around the `.` and `->` structure member operators

## Encoding and line endings
Files should be encoded as UTF-8 and use unix line endings (LF, no CR).

[linux-spacing]: https://www.kernel.org/doc/html/v4.10/process/coding-style.html#spaces
