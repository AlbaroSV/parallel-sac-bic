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

// TRIVIUM Constants
const int TRIVIUM_KEY_SIZE = 10;  // 80 bits
const int TRIVIUM_IV_SIZE = 10;   // 80 bits

// Trivium state: 288 bits = 3 shift registers (93 + 84 + 111)
std::bitset<288> trivium_state;

// Initialization of the state with key and IV
void trivium_init(const std::vector<uint8_t>& key, const std::vector<uint8_t>& iv) {
    trivium_state.reset();
    
    // Load key into state[0..79]
    for (int i = 0; i < 80; ++i) {
        trivium_state[i] = (key[i / 8] >> (i % 8)) & 1;
    }

    // Load IV into state[93..172]
    for (int i = 0; i < 80; ++i) {
        trivium_state[93 + i] = (iv[i / 8] >> (i % 8)) & 1;
    }

    // Set state[285..287] = 1
    trivium_state[285] = trivium_state[286] = trivium_state[287] = 1;

    // 4 × 288 initialization cycles (discard output)
    for (int i = 0; i < 4 * 288; ++i) {
        bool t1 = trivium_state[65] ^ trivium_state[92];
        bool t2 = trivium_state[161] ^ trivium_state[176];
        bool t3 = trivium_state[242] ^ trivium_state[287];

        bool z1 = trivium_state[90] & trivium_state[91];
        bool z2 = trivium_state[174] & trivium_state[175];
        bool z3 = trivium_state[285] & trivium_state[286];

        bool s1 = t1 ^ trivium_state[170] ^ z1;
        bool s2 = t2 ^ trivium_state[263] ^ z2;
        bool s3 = t3 ^ trivium_state[68]  ^ z3;

        // Shift registers
        for (int j = 92; j > 0; --j) trivium_state[j] = trivium_state[j - 1];
        trivium_state[0] = s3;

        for (int j = 176; j > 93; --j) trivium_state[j] = trivium_state[j - 1];
        trivium_state[93] = s1;

        for (int j = 287; j > 177; --j) trivium_state[j] = trivium_state[j - 1];
        trivium_state[177] = s2;
    }
}

// Generate one bit of keystream
bool trivium_keystream_bit() {
    bool t1 = trivium_state[65] ^ trivium_state[92];
    bool t2 = trivium_state[161] ^ trivium_state[176];
    bool t3 = trivium_state[242] ^ trivium_state[287];

    bool z = t1 ^ t2 ^ t3;

    bool z1 = trivium_state[90] & trivium_state[91];
    bool z2 = trivium_state[174] & trivium_state[175];
    bool z3 = trivium_state[285] & trivium_state[286];

    bool s1 = t1 ^ trivium_state[170] ^ z1;
    bool s2 = t2 ^ trivium_state[263] ^ z2;
    bool s3 = t3 ^ trivium_state[68]  ^ z3;

    // Shift registers
    for (int j = 92; j > 0; --j) trivium_state[j] = trivium_state[j - 1];
    trivium_state[0] = s3;

    for (int j = 176; j > 93; --j) trivium_state[j] = trivium_state[j - 1];
    trivium_state[93] = s1;

    for (int j = 287; j > 177; --j) trivium_state[j] = trivium_state[j - 1];
    trivium_state[177] = s2;

    return z;
}

// Trivium stream cipher wrapper
std::bitset<MAX_SIZE> stream_cipher_TRIVIUM(std::bitset<MAX_SIZE> input, int n, int m) {
    std::bitset<MAX_SIZE> output;
    std::vector<uint8_t> key(TRIVIUM_KEY_SIZE, 0x00); // Replace with actual key
    std::vector<uint8_t> iv(TRIVIUM_IV_SIZE, 0x00);   // Replace with actual IV

    trivium_init(key, iv);

    for (int i = 0; i < n; ++i) {
        bool keystream_bit = trivium_keystream_bit();
        output[i] = input[i] ^ keystream_bit;
    }

    return output;
}

#endif
