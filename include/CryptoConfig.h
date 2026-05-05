#ifndef CRYPTOCONFIG_H
#define CRYPTOCONFIG_H

#pragma once
#include <aes.h>
#include <sha.h>

namespace CryptoConfig {
    inline constexpr size_t IV_SIZE   = CryptoPP::AES::BLOCKSIZE;
    inline constexpr size_t HASH_SIZE = CryptoPP::SHA256::DIGESTSIZE;
    inline constexpr const char* MAGIC_HEADER = "ENC!";
}

#endif // CRYPTOCONFIG_H
