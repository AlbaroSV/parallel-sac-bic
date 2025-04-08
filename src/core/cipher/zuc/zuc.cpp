#include <iostream>
#include <algorithm>
#include <bitset>
#include <set>
#include <vector>
#include <chrono>
#include <random>
#include <thread>
#include <functional>
#include <numeric>
#include <string>
#include "../../shared/global.cpp"

#ifndef test_cipher_h
#define test_cipher_h

// ZUC parameters
const uint32_t D = 0x44D7; // Constant used in initialization
const int ZUC_KEY_SIZE = 16; // 128-bit key
const int ZUC_IV_SIZE = 16;  // 128-bit IV
const int ZUC_WORD_SIZE = 31; // Internal registers are 31-bit

// LFSR
uint32_t LFSR_S[16]; // LFSR registers (31-bit values)

// Helper: mod (2^31 - 1)
uint32_t mod31(uint64_t x) {
    uint32_t res = (x & 0x7FFFFFFF) + (x >> 31);
    return (res >= 0x7FFFFFFF) ? (res - 0x7FFFFFFF) : res;
}

// Helper: Rotate left
uint32_t rotl32(uint32_t x, int n) {
    return ((x << n) | (x >> (32 - n)));
}

// Simple placeholder S-box function (ZUC uses 2 S-boxes in real spec)
uint32_t S_box(uint32_t x) {
    return (x ^ rotl32(x, 11)) + 0x12345678; // Fake S-box for simplicity
}

// Initialization function (simplified)
void zuc_init(const std::vector<uint8_t>& key, const std::vector<uint8_t>& iv) {
    for (int i = 0; i < 16; ++i) {
        LFSR_S[i] = ((uint32_t)key[i] << 23) | ((uint32_t)D << 8) | iv[i];
        LFSR_S[i] &= 0x7FFFFFFF; // Ensure 31-bit
    }
}

// Generate one 32-bit keystream word (simplified)
uint32_t zuc_generate_keystream_word() {
    // Non-linear combining + S-box
    uint32_t W = (LFSR_S[0] ^ LFSR_S[4]) + LFSR_S[10];
    W = S_box(W);

    // LFSR update (simplified version of ZUC update mode)
    uint64_t new_s = (uint64_t)LFSR_S[0] + LFSR_S[2] + LFSR_S[11];
    for (int i = 0; i < 15; ++i) LFSR_S[i] = LFSR_S[i + 1];
    LFSR_S[15] = mod31(new_s);

    return W;
}

// ZUC cipher implementation (bitset-based)
std::bitset<MAX_SIZE> stream_cipher_ZUC(std::bitset<MAX_SIZE> input, int n, int m) {
    std::bitset<MAX_SIZE> output;
    std::vector<uint8_t> key(ZUC_KEY_SIZE, 0x00); // Replace with real key
    std::vector<uint8_t> iv(ZUC_IV_SIZE, 0x00);   // Replace with real IV

    zuc_init(key, iv);

    // Convert bitset to bytes
    std::vector<uint8_t> input_bytes((n + 7) / 8);
    for (int i = 0; i < n; ++i) {
        if (input[i]) input_bytes[i / 8] |= (1 << (i % 8));
    }

    std::vector<uint8_t> output_bytes(input_bytes.size());

    // Generate keystream and XOR
    for (size_t i = 0; i < input_bytes.size(); ++i) {
        if (i % 4 == 0) {
            uint32_t word = zuc_generate_keystream_word();
            for (int j = 0; j < 4 && i + j < input_bytes.size(); ++j) {
                output_bytes[i + j] = input_bytes[i + j] ^ ((word >> (8 * j)) & 0xFF);
            }
        }
    }

    // Convert back to bitset
    for (int i = 0; i < n; ++i) {
        if ((output_bytes[i / 8] >> (i % 8)) & 1) output.set(i);
    }

    return output;
}

#endif
