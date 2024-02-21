# SFED-AES-256

Single File Encryption & Decryption: AES 256

This utility provides a simple command-line interface for encrypting and decrypting files using the AES (Advanced Encryption Standard) algorithm. It is designed to be easy to use for basic encryption tasks, offering the flexibility to work with different key inputs and modes.

## Features

- Encrypt or decrypt files using AES-256.
- Generate a random AES key for encryption.
- Allow specifying a key via file, directly as a string, or generate a new one.
- Output encrypted/decrypted content to a specified file.
- Include the initialization vector (IV) in the encrypted file for proper decryption.

## Dependencies

- This utility requires the `aes.h` header file for AES functionality, which is part of a separate, custom mini AES library.
- Standard C++ libraries: `<iostream>`, `<fstream>`, `<vector>`, `<string>`, `<cstdlib>`, `<ctime>`, `<filesystem>`.


## Usage

```bash
./SFED-AES-256 -i <inputfile> -o <outputfile> -m <encrypt|decrypt> [-k <keyfile>] [-ko <keyoutputfile>] [-ks <keystring>]
```

- `-i <inputfile>`: Specifies the input file to encrypt or decrypt.
- `-o <outputfile>`: Specifies the output file to write the encrypted or decrypted content.
- `-m <encrypt|decrypt>`: Mode of operation, either `encrypt` or `decrypt`.
- `-k <keyfile>`: (Optional) Specifies a file containing the AES key to use.
- `-ko <keyoutputfile>`: (Optional) Specifies a file to save the generated AES key when not using an existing key.
- `-ks <keystring>`: (Optional) Directly specifies the AES key as a string.

Note: The utility generates a random AES key if neither `-k` nor `-ks` is provided and `-ko` is used to specify where to save it.

## Example

Encrypting a file:

```bash
./SFED-AES-256 -i test.txt -o encrypted.aes -m encrypt -ko key.aes
```

Decrypting a file:

```bash
./SFED-AES-256 -i encrypted.aes -o decrypted.txt -m decrypt -k key.aes
```

## Security Notes

- Ensure the security of your key file and key string.
- The utility uses a random IV for encryption, which is prepended to the output file. This does not need to be kept secret but is necessary for decryption.


## Compile Example

```
g++ main.cpp aes.c -o SFED-AES-256.exe  -static
```
