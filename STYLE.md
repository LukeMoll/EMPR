Code Style
===

## C
The following code style will be used for all C code:

### Line limit
Lines shall not exceed **100 characters**

### Indentation
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

### Braces
Braces should broadly follow **K&R style**. That is, opening braces should be on
the same line as their control statement with the exception of functions, where
the opening brace goes on the following line. In the case of control statements
following a closing brace (e.g. `else`, `else if`, and do-while), the control
statment should go on the same line as the closing brace.

```c
void foo(int x)
{
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
void foo(int x)
{
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

### Case
Functions and variables should both use `snake_case`. Constants should be
`ANGRY_SNAKE`. Preprocessor commands shall use `lowercase`

### Operators
Parentheses are free. Use them to disambiguate complex expressions. Do **not**
rely on order of operations.

### Spaces
Spacing should follow the [Linux Kernel style (§3.1)][linux-spacing]. In
short, spaces **should** go:

- after most keywords (e.g. `if (foo)`)
- before but not after a pointer asterisk (e.g. `char *foo`)
- around almost all binary operators (e.g. `a + b`)

Spaces **should not** go:

- directly inside parentheses (e.g. no `foo( x )`)
- between a function and its parameters (e.g. no `foo (x)`)
- between unary operators and their operand (e.g. `x++` and `-y`)
- around the `.` and `->` structure member operators

## Markdown
The following style will be used for Markdown

```md
Title
===

<!-- note space after hashes -->
## Heading 2

**bold** _italic_ _**both**_

```

### Line limit
Lines shall not exceed **80 characters**. Markdown files contain prose and are
meant to be easily readable _in their source form_. A line length limit of 80
helps with readability of prose since humans start clocking out if lines are too
long. Additionally, most editors soft-wrap prose badly (i.e. half-way through a
word) since they're not designed for that. Hard wrapping at a sane width
prevents both these issues.

If your editor can't be set to automatically hard-wrap at a given width, or
display a guide line at a given column to allow you to manually do it, then it's
a bad editor and you should replace it.

The exception to this rule is link references at the bottom of the file since
URLs can get hella long and can't be broken up.

## Encoding and line endings
All text files, regardless of type, should be encoded as UTF-8 and use unix line
endings (LF, no CR). If your editor doesn't support changing these then it's a
bad editor and you should replace it.

[linux-spacing]: https://www.kernel.org/doc/html/v4.10/process/coding-style.html#spaces
