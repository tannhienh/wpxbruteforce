# Brute force wordpress websites with xmlrpc enabled

wpxbruteforce 1.2.0 (c) 2022 by Xblackey

Disclaimer: For penetration testing or educational purposes only

## Requirement
It is exploitable if the site's xmlrpc is enabled

Check if xmlrpc is enabled or not at: `https://example.com/xmlrpc.php`

display: `XML-RPC server accepts POST requests only.`

## Build:
`$ g++ wpxbruteforce.cpp -o wpxbruteforce -lcurl`

## Usage:
`$ ./wpxbruteforce [url] [usernames file] [passwords file]`

---

example:

`$ ./wpxbruteforce https://example.com usernames.txt passwords.txt`
