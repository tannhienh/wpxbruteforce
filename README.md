# Brute force wordpress websites with xmlrpc enabled

## Requirement
xmlrpc enabled

Check xmlrpc enabled: https://example.com/xmlrpc.php

Display: 'XML-RPC server accepts POST requests only.'

## Build:
`$ g++ wpxbruteforce.cpp -o wpxbruteforce -lcurl`

## Run:
`$ ./wpxbruteforce [url/xmlrpc.php] [usernames file] [passwords file]`

---

example:

`$ ./wpxbruteforce https://example.com/xmlrpc.php usernames.txt passwords.txt`
