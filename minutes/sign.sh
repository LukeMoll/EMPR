#!/bin/bash

set -e

read -p "Who is signing? (lowercase): " INITIAL
KEYID=$(grep $INITIAL keys/ids.tsv | cut -f2)
echo "Using key:"
gpg2 --list-keys $KEYID

echo "Enter your message on a single line."
echo "You should include your initials and the current date"
read -p "> " MSG
echo "Copy the following block and send to the secretary to paste in the Signatures section"
echo $MSG | gpg2 --clearsign --default-key $KEYID
