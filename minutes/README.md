Minutes
===

## How to write minutes
1. The _secretary_ runs `./new.sh` and fills in the details for _secretary_ and _chair_
2. Open the newly created file and make sure the attendance section is correct
3. Fill in Agenda, Minutes, and Actions as appropriate.
4. Get members to sign minutes -- see **Signing** below

## Signing
Minutes are signed with GPG signatures from each member. Keys _must_ be protected by a passphrase and kept on relevant computers.

### Generating
Run `gpg --full-generate-key` to generate your signing key. Fill out the relevant info (consider adding that it's a key for EMPR in the Comment section):  
 - Kind of key: **(4) RSA (sign only)**
 - Keysize: Any size, **1024** will be fine.
 - Validity: Does not expire - **0**

Then run `gpg --list-keys` and copy the id into `keys/ids.tsv`, in the following format:
```
xy\t<key id>
```
 - `xy` is your lowercase initials
 - `\t`: press TAB
 - `<key id>` the ID of the key you just generated

And finally run `gpg --armor --export <key id>`, and save the content in `keys/xy.gpg` where `xy` is your lowercase initials. Don't forget to commit and push!

### Signing minutes
Once you've generated your keys, you can run `./sign.sh`. This will prompt you for your lowercase initials and a message. You'll likely be asked for your key passphrase. It will output a PGP block, copy the ENTIRE block and either send it to the secretary or paste it yourself into the Signatures section

## All minutes
Minutes from all the minutes can be found here:

<!-- Do not write any content below this line -->
[Spring/2/Wed 11 am](2019-01-15_Spring-02-Tue_11.md)  
[Spring/2/Thu 10 am](2019-01-17_Spring-02-Thu_10.md)  
