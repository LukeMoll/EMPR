#!/bin/bash

read -p "What week number is it? " WEEKNUM
FILENAME="$(date -I)_Spring-$( printf '%02d' "$WEEKNUM")-$(date +%a_%k).md"

DATE=$(date +%d/%m/%y)

read -p "Who is chair? " CHAIR
read -p "Who is secretary? " SEC

touch $FILENAME
cat << EOF > $FILENAME
Team Meeting $DATE
===

<!-- remember two spaces at end of line to break onto a new line -->
**Chair**: $CHAIR  
**Secretary**: $SEC

**Present**: AB, CW, JP, LM  
**Apologies**: None

## Agenda
 - List items here

## Minutes


<!-- ## Any other business -->

## Actions
 - **##** List assigned actions to member ## here

## Signatures
<!-- 
	Paste in entire GPG signed messages here 
	Messages should have initials and date
	Signatures should be surrounded with triple backticks (on their own line) and the full signature block should be copied. For example:
	```
	-----BEGIN PGP SIGNED MESSAGE-----
		...
	-----END PGP SIGNATURE-----
	```
-->

	
 
EOF

echo "[Spring/$WEEKNUM/$(date '+%a %H %p')]($FILENAME)" >> README.md
