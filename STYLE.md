Code Style
===

# C
The following code style will be used for all C code:

## Line limit
Lines shall not exceed **_n_ characters**

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
```c
// example function braces
void main(void) {
	// example statment block braces
	if(true) {
		
	}
}
```

## Case
Functions and variables should both use `snake_case`. Constants should be `ANGRY_SNAKE`. Preprocessor commands shall use `lowercase`

## Operators
There **should** be spaces between operators. (`x = y * 2;` not `x=y*2`).  
Parentheses shall be used to fully disambiguate operators (order of operations should **not** be relied upon).

# Markdown
The following style will be used for Markdown

```md
Title
===

<!-- note space after hashes -->
## Heading 2

**bold** _italic_ _**both**_

```

No line limit - set your editor to wrap (`esc` followed by `Shift-4` in Nano)
