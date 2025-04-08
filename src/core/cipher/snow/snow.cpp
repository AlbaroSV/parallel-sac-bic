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

// SNOW 3G Constants
const int SNOW3G_KEY_SIZE = 16; // 128 bits
const int SNOW3G_IV_SIZE = 16;  // 128 bits

// LFSR of 16 32-bit words
uint32_t LFSR[16];
uint32_t FSM_R1 = 0, FSM_R2 = 0, FSM_R3 = 0;

// Irreducible polynomial used in SNOW 3G
uint32_t mul_alpha(uint32_t x) {
    return ((x << 8) ^ ((x >> 23) * 0x1B)); // x * α in GF(2^32)
}

// FSM update
uint32_t fsm_output() {
    uint32_t F = (FSM_R1 ^ LFSR[15]) + FSM_R2;
    uint32_t r = FSM_R3;
    FSM_R3 = S2(FSM_R2);
    FSM_R2 = S1(FSM_R1);
    FSM_R1 = r;
    return F;
}

// Dummy S-boxes (placeholders – real ones are table-based)
uint32_t S1(uint32_t x) {
    return ((x ^ 0xA5A5A5A5) + 0x12345678); // fake S-box 1
}
uint32_t S2(uint32_t x) {
    return ((x ^ 0x5A5A5A5A) + 0x87654321); // fake S-box 2
}

// LFSR update in mode keystream generation
void lfsr_update() {
    uint32_t v = mul_alpha(LFSR[0]) ^ LFSR[2] ^ mul_alpha(LFSR[11]) ^ LFSR[15];
    for (int i = 0; i < 15; ++i)
        LFSR[i] = LFSR[i + 1];
    LFSR[15] = v;
}

// Initialization with key and IV
void snow3g_init(const std::vector<uint8_t>& key, const std::vector<uint8_t>& iv) {
    for (int i = 0; i < 16; ++i) {
        LFSR[i] = ((uint32_t)key[i] << 24) | ((uint32_t)iv[i] << 16) | (key[i] << 8) | iv[i];
    }
    FSM_R1 = FSM_R2 = FSM_R3 = 0;
    for (int i = 0; i < 32; ++i) {
        uint32_t dummy = fsm_output();
        lfsr_update();
    }
}

// Generate one keystream word
uint32_t snow3g_keystream_word() {
    uint32_t z = fsm_output() ^ LFSR[0];
    lfsr_update();
    return z;
}

// Stream cipher wrapper
std::bitset<MAX_SIZE> stream_cipher_SNOW3G(std::bitset<MAX_SIZE> input, int n, int m) {
    std::bitset<MAX_SIZE> output;
    std::vector<uint8_t> key(SNOW3G_KEY_SIZE, 0x00); // Replace with actual key
    std::vector<uint8_t> iv(SNOW3G_IV_SIZE, 0x00);   // Replace with actual IV

    snow3g_init(key, iv);

    // Convert bitset to bytes
    std::vector<uint8_t> input_bytes((n + 7) / 8);
    for (int i = 0; i < n; ++i)
        if (input[i]) input_bytes[i / 8] |= (1 << (i % 8));

    std::vector<uint8_t> output_bytes(input_bytes.size());

    // Generate keystream and apply XOR
    for (size_t i = 0; i < input_bytes.size(); ++i) {
        if (i % 4 == 0) {
            uint32_t word = snow3g_keystream_word();
            for (int j = 0; j < 4 && i + j < input_bytes.size(); ++j) {
                output_bytes[i + j] = input_bytes[i + j] ^ ((word >> (8 * j)) & 0xFF);
            }
        }
    }

    // Convert back to bitset
    for (int i = 0; i < n; ++i)
        if ((output_bytes[i / 8] >> (i % 8)) & 1) output.set(i);

    return output;
}

#endif
