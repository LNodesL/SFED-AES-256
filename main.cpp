#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include "include/aes.h" 

std::vector<uint8_t> GenerateRandomAESKey(size_t length) {
    std::vector<uint8_t> key(length);
    for (size_t i = 0; i < length; ++i) {
        key[i] = rand() % 256;
    }
    return key;
}

size_t GetFileSize(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file) {
        std::cerr << "Could not open file: " << filename << std::endl;
        return 0;
    }
    return static_cast<size_t>(file.tellg());
}

void AESEncryptDecrypt(const std::string& inputFile, const std::string& outputFile, const std::vector<uint8_t>& key, bool encrypt) {
    std::ifstream file(inputFile, std::ios::binary);
    if (!file) {
        std::cerr << "Could not open input file for reading.\n";
        return;
    }

    size_t fileSize = GetFileSize(inputFile);
    std::vector<uint8_t> buffer(fileSize);
    file.read(reinterpret_cast<char*>(buffer.data()), fileSize);
    file.close();

    AES_ctx ctx;
    std::vector<uint8_t> iv(AES_BLOCKLEN);
    if (encrypt) {
        for (auto &i : iv) {
            i = rand() % 256;
        }
        size_t paddingLength = AES_BLOCKLEN - (buffer.size() % AES_BLOCKLEN);
        buffer.insert(buffer.end(), paddingLength, static_cast<uint8_t>(paddingLength));
    } else {
        std::copy(buffer.begin(), buffer.begin() + AES_BLOCKLEN, iv.begin());
        buffer.erase(buffer.begin(), buffer.begin() + AES_BLOCKLEN);
        fileSize -= AES_BLOCKLEN;
    }

    AES_init_ctx_iv(&ctx, key.data(), iv.data());

    if (encrypt) {
        AES_CBC_encrypt_buffer(&ctx, buffer.data(), buffer.size());
        buffer.insert(buffer.begin(), iv.begin(), iv.end());
    } else {
        AES_CBC_decrypt_buffer(&ctx, buffer.data(), buffer.size());
        uint8_t paddingLength = buffer.back();
        if(paddingLength <= AES_BLOCKLEN) {
            buffer.erase(buffer.end() - paddingLength, buffer.end());
        }
    }

    std::ofstream outFile(outputFile, std::ios::binary);
    if (!outFile) {
        std::cerr << "Could not open output file for writing.\n";
        return;
    }
    outFile.write(reinterpret_cast<const char*>(buffer.data()), buffer.size());
    outFile.close();

    std::cout << "Operation complete. Output file: " << outputFile << std::endl;
}

int main(int argc, char* argv[]) {
    std::string input, output, keyFile, keyOutput, keyString, mode;

    if (argc < 5) {
        std::cerr << "Usage: " << argv[0] << " -i <inputfile> -o <outputfile> -m <mode> [-k <keyfile>] [-ko <keyoutputfile>] [-ks <keystring>]\n";
        return 1;
    }

    srand(static_cast<unsigned int>(time(nullptr))); // Seed for random number generation

    for (int i = 1; i < argc; i += 2) {
        std::string flag = argv[i];
        if (flag == "-i") {
            input = argv[i + 1];
        } else if (flag == "-o") {
            output = argv[i + 1];
        } else if (flag == "-m") {
            mode = argv[i + 1];
        } else if (flag == "-k") {
            keyFile = argv[i + 1];
        } else if (flag == "-ko") {
            keyOutput = argv[i + 1];
        } else if (flag == "-ks") {
            keyString = argv[i + 1];
        }
    }

    if (input.empty() || output.empty() || mode.empty()) {
        std::cerr << "Input, output files, and mode must be specified.\n";
        return 1;
    }

    if (mode != "encrypt" && mode != "decrypt") {
        std::cerr << "Invalid mode. Use 'encrypt' or 'decrypt'.\n";
        return 1;
    }

    std::vector<uint8_t> key;
    if (!keyFile.empty()) {
        // Load key from file
        std::ifstream keyIn(keyFile, std::ios::binary);
        if (!keyIn) {
            std::cerr << "Could not open key file for reading.\n";
            return 1;
        }
        key = std::vector<uint8_t>(std::istreambuf_iterator<char>(keyIn), {});
    } else if (!keyString.empty()) {
        // Use key string directly
        key.assign(keyString.begin(), keyString.end());
    } else {
        // Generate a new key
        key = GenerateRandomAESKey(AES_KEYLEN);
        if (!keyOutput.empty()) {
            // Save generated key to file
            std::ofstream keyOut(keyOutput, std::ios::binary);
            if (!keyOut) {
                std::cerr << "Could not open key output file for writing.\n";
                return 1;
            }
            keyOut.write(reinterpret_cast<const char*>(key.data()), key.size());
            keyOut.close();
            std::cout << "Key saved to file: " << keyOutput << std::endl;
        }
    }

    AESEncryptDecrypt(input, output, key, mode == "encrypt");

    return 0;
}
