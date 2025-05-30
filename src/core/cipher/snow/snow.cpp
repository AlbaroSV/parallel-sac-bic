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
#include <iomanip>
#include <sstream>
#include "../../shared/global.cpp"
#include "../../shared/helpers.cpp"

#ifndef SNOW_V_CIPHER
#define SNOW_V_CIPHER

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

using namespace std;
using namespace std::chrono;

u8 snow_key[32] = {0x00};
u8 snow_iv[16] = {0x00};
int is_aead_mode = 0;
int snow_is_debug_mode = 0; 


u8 SBox[256] = {
  0x63,0x7C,0x77,0x7B,0xF2,0x6B,0x6F,0xC5,0x30,0x01,0x67,0x2B,0xFE,0xD7,0xAB,0x76,
  0xCA,0x82,0xC9,0x7D,0xFA,0x59,0x47,0xF0,0xAD,0xD4,0xA2,0xAF,0x9C,0xA4,0x72,0xC0,
  0xB7,0xFD,0x93,0x26,0x36,0x3F,0xF7,0xCC,0x34,0xA5,0xE5,0xF1,0x71,0xD8,0x31,0x15,
  0x04,0xC7,0x23,0xC3,0x18,0x96,0x05,0x9A,0x07,0x12,0x80,0xE2,0xEB,0x27,0xB2,0x75,
  0x09,0x83,0x2C,0x1A,0x1B,0x6E,0x5A,0xA0,0x52,0x3B,0xD6,0xB3,0x29,0xE3,0x2F,0x84,
  0x53,0xD1,0x00,0xED,0x20,0xFC,0xB1,0x5B,0x6A,0xCB,0xBE,0x39,0x4A,0x4C,0x58,0xCF,
  0xD0,0xEF,0xAA,0xFB,0x43,0x4D,0x33,0x85,0x45,0xF9,0x02,0x7F,0x50,0x3C,0x9F,0xA8,
  0x51,0xA3,0x40,0x8F,0x92,0x9D,0x38,0xF5,0xBC,0xB6,0xDA,0x21,0x10,0xFF,0xF3,0xD2,
  0xCD,0x0C,0x13,0xEC,0x5F,0x97,0x44,0x17,0xC4,0xA7,0x7E,0x3D,0x64,0x5D,0x19,0x73,
  0x60,0x81,0x4F,0xDC,0x22,0x2A,0x90,0x88,0x46,0xEE,0xB8,0x14,0xDE,0x5E,0x0B,0xDB,
  0xE0,0x32,0x3A,0x0A,0x49,0x06,0x24,0x5C,0xC2,0xD3,0xAC,0x62,0x91,0x95,0xE4,0x79,
  0xE7,0xC8,0x37,0x6D,0x8D,0xD5,0x4E,0xA9,0x6C,0x56,0xF4,0xEA,0x65,0x7A,0xAE,0x08,
  0xBA,0x78,0x25,0x2E,0x1C,0xA6,0xB4,0xC6,0xE8,0xDD,0x74,0x1F,0x4B,0xBD,0x8B,0x8A,
  0x70,0x3E,0xB5,0x66,0x48,0x03,0xF6,0x0E,0x61,0x35,0x57,0xB9,0x86,0xC1,0x1D,0x9E,
  0xE1,0xF8,0x98,0x11,0x69,0xD9,0x8E,0x94,0x9B,0x1E,0x87,0xE9,0xCE,0x55,0x28,0xDF,
  0x8C,0xA1,0x89,0x0D,0xBF,0xE6,0x42,0x68,0x41,0x99,0x2D,0x0F,0xB0,0x54,0xBB,0x16
};

u8 Sigma[16] = {0, 4, 8, 12, 1, 5, 9, 13, 2, 6, 10, 14, 3, 7, 11, 15};
u32 AesKey1[4] = { 0, 0, 0, 0 };
u32 AesKey2[4] = { 0, 0, 0, 0 };

#define SNOW_MAKEU32(a, b) (((u32)(a) << 16) | ((u32)(b) ))
#define SNOW_MAKEU16(a, b) (((u16)(a) << 8) | ((u16)(b) ))

struct SnowV32 {
  u16 A[16], B[16];
  u32 R1[4], R2[4], R3[4];

  void aes_enc_round(u32 * result, u32 * state, u32 * roundKey) {
    if (snow_is_debug_mode) {
      cout << "\n=== AES Encryption Round ===" << endl;
      print_hex((u8*)state, 16, "Input State");
      print_hex((u8*)roundKey, 16, "Round snow_Key");
    }

    #define ROTL32(word32, offset) ((word32 << offset) | (word32 >> (32 - offset)))
    #define SB(index, offset) (((u32)(sb[(index) % 16])) << (offset * 8))
    #define MKSTEP(j)\
    w = SB(j * 4 + 0, 3) | SB(j * 4 + 5, 0) | SB(j * 4 + 10, 1) | SB(j * 4 + 15, 2);\
    t = ROTL32(w, 16) ^ ((w << 1) & 0xfefefefeUL) ^ (((w >> 7) & 0x01010101UL) * 0x1b);\
    result[j] = roundKey[j] ^ w ^ t ^ ROTL32(t, 8)
    u32 w, t;
    u8 sb[16];

    for (int i = 0; i < 4; i++)
      for (int j = 0; j < 4; j++)
        sb[i * 4 + j] = SBox[(state[i] >> (j * 8)) & 0xff];
    MKSTEP(0);
    MKSTEP(1);
    MKSTEP(2);
    MKSTEP(3);

    if (snow_is_debug_mode) {
      print_hex((u8*)result, 16, "Output State");
      cout << "==========================\n" << endl;
    }
  }

  u16 mul_x(u16 v, u16 c) {
    if (v & 0x8000)
      return(v << 1) ^ c;
    else
      return (v << 1);
  }

  u16 mul_x_inv(u16 v, u16 d) { 
    if (v & 0x0001)
      return(v >> 1) ^ d;
    else
      return (v >> 1);
  }

  void permute_sigma(u32 * state) { 
    u8 tmp[16];
    for (int i = 0; i < 16; i++)
      tmp[i] = (u8)(state[Sigma[i] >> 2] >> ((Sigma[i] & 3) << 3));
    for (int i = 0; i < 4; i++)
      state[i] = SNOW_MAKEU32(SNOW_MAKEU16(tmp[4 * i + 3], tmp[4 * i + 2]),
    SNOW_MAKEU16(tmp[4 * i + 1], tmp[4 * i]));
  }

  void fsm_update(void) {
    if (snow_is_debug_mode) {
      cout << "\n=== FSM Update ===" << endl;
      cout << "Before update:" << endl;
      print_hex((u8*)R1, 16, "R1");
      print_hex((u8*)R2, 16, "R2");
      print_hex((u8*)R3, 16, "R3");
    }
    u32 R1temp[4];
    memcpy(R1temp, R1, sizeof(R1));
    for (int i = 0; i < 4; i++) {
      u32 T2 = SNOW_MAKEU32(A[2 * i + 1], A[2 * i]);
      R1[i] = (T2 ^ R3[i]) + R2[i];
    }
    permute_sigma(R1);
    aes_enc_round(R3, R2, AesKey2);
    aes_enc_round(R2, R1temp, AesKey1);
    
    if (snow_is_debug_mode) {
      cout << "After update:" << endl;
      print_hex((u8*)R1, 16, "R1");
      print_hex((u8*)R2, 16, "R2");
      print_hex((u8*)R3, 16, "R3");
      cout << "================\n" << endl;
    }
  }

  void lfsr_update(void) {
    if (snow_is_debug_mode) {
      cout << "\n=== LFSR Update ===" << endl;
      cout << "Before update:" << endl;
      print_hex((u8*)A, 32, "LFSR-A");
      print_hex((u8*)B, 32, "LFSR-B");
    }
    for (int i = 0; i < 8; i++) { 
      u16 u = mul_x(A[0], 0x990f) ^ A[1] ^ mul_x_inv(A[8], 0xcc87) ^ B[0];
      u16 v = mul_x(B[0], 0xc963) ^ B[3] ^ mul_x_inv(B[8], 0xe4b1) ^ A[0];
      for (int j = 0; j < 15; j++) { 
        A[j] = A[j + 1];
        B[j] = B[j + 1];
      }
      A[15] = u;
      B[15] = v;
    }
    
    if (snow_is_debug_mode) {
      cout << "After update:" << endl;
      print_hex((u8*)A, 32, "LFSR-A");
      print_hex((u8*)B, 32, "LFSR-B");
      cout << "===============\n" << endl;
    }
  }

  void keystream(u8 * z) {
    if (snow_is_debug_mode) {
      cout << "\n=== Keystream Generation ===" << endl;
    }
    for (int i = 0; i < 4; i++) {
      u32 T1 = SNOW_MAKEU32(B[2 * i + 9], B[2 * i + 8]);
      u32 v = (T1 + R1[i]) ^ R2[i];
      z[i * 4 + 0] = (v >> 0) & 0xff;
      z[i * 4 + 1] = (v >> 8) & 0xff;
      z[i * 4 + 2] = (v >> 16) & 0xff;
      z[i * 4 + 3] = (v >> 24) & 0xff;
    }
    //printZ(z);
    //printLFSRA(A);
    fsm_update();
    lfsr_update();

    if (snow_is_debug_mode) {
      print_hex(z, 16, "Generated Keystream");
      cout << "=========================\n" << endl;
    }
  }

  void printZ(u8 * z) {
    for (int i = 0; i < 16; i++) {
      printf ("%x|", z[i]); 
    }
    cout << "\n"; 
  }
    
  void printLFSRA(u16 * A) {
    cout << "*****Mostrar LFSR********\n";
      for (int i = 0; i < 16; i++) {
        printf ("%x|", A[i]); 
      }
    cout << "\n"; 
  }

  void keyiv_setup(u8 * snow_key, u8 * snow_iv, int is_aead_mode) {
    if (snow_is_debug_mode) {
      cout << "\n=== snow_Key/snow_IV Setup ===" << endl;
      print_hex(snow_key, 32, "Input snow_Key");
      print_hex(snow_iv, 16, "Input snow_IV");
      cout << "AEAD Mode: " << is_aead_mode << endl;
    }
    
    for (int i = 0; i < 8; i++) {
      A[i] = SNOW_MAKEU16(snow_iv[2 * i + 1], snow_iv[2 * i]);
      A[i + 8] = SNOW_MAKEU16(snow_key[2 * i + 1], snow_key[2 * i]);
      B[i] = 0x0000;
      B[i + 8] = SNOW_MAKEU16(snow_key[2 * i + 17], snow_key[2 * i + 16]);
    }

    if(is_aead_mode == 1) {
      B[0] = 0x6C41;
      B[1] = 0x7865;
      B[2] = 0x6B45;
      B[3] = 0x2064;
      B[4] = 0x694A;
      B[5] = 0x676E;
      B[6] = 0x6854;
      B[7] = 0x6D6F;
    }

    for (int i = 0; i < 4; i++) {
      R1[i] = R2[i] = R3[i] = 0x00000000;
    }

    for (int i = 0; i < 16; i++) { 
      u8 z[16];
      keystream(z);
      
      for (int j = 0; j < 8; j++) {
        A[j + 8] ^= SNOW_MAKEU16(z[2 * j + 1], z[2 * j]);
      }
      if (i == 14) {
        for (int j = 0; j < 4; j++) {
          R1[j] ^= SNOW_MAKEU32(SNOW_MAKEU16(snow_key[4 * j + 3], snow_key[4 * j + 2]),
          SNOW_MAKEU16(snow_key[4 * j + 1], snow_key[4 * j + 0]));
        }
      }
      if (i == 15) {
        for (int j = 0; j < 4; j++) {
          R1[j] ^= SNOW_MAKEU32(SNOW_MAKEU16(snow_key[4 * j + 19], snow_key[4 * j + 18]),
          SNOW_MAKEU16(snow_key[4 * j + 17], snow_key[4 * j + 16]));
        }
      }
    }
    if (snow_is_debug_mode) {
      cout << "After initialization:" << endl;
      print_hex((u8*)A, 32, "LFSR-A");
      print_hex((u8*)B, 32, "LFSR-B");
      print_hex((u8*)R1, 16, "R1");
      print_hex((u8*)R2, 16, "R2");
      print_hex((u8*)R3, 16, "R3");
      cout << "===================\n" << endl;
    }
  }
};


SnowV32 SNOW_V;

bitset<MAX_SIZE> stream_cipher_SNOW3G(bitset<MAX_SIZE> input, int n, int /*m*/)
{
    if (snow_is_debug_mode) {
      cout << "\n=== Stream Cipher Operation ===" << endl;
      cout << "Input size: " << n << " bits" << endl;
    }
    // SnowV32 SNOW_V;
    bitset<MAX_SIZE> output;
    
    // Initialize the cipher with snow_key and snow_IV
    // SNOW_V.keyiv_setup(snow_key, snow_iv, is_aead_mode);
    
    // Calculate how many 128-bit (16-byte) blocks we need to process
    int num_blocks = (n + 127) / 128;
    int remaining_bits = n % 128;
    
    if (snow_is_debug_mode) {
      cout << "Processing " << num_blocks << " blocks (" 
           << num_blocks * 128 << " bits)" << endl;
      if (remaining_bits > 0) {
          cout << "Plus " << remaining_bits << " remaining bits" << endl;
      }
    }
    // Process complete 128-bit blocks
    for (int i = 0; i < num_blocks; i++) {
        if (snow_is_debug_mode) {
          cout << "\nProcessing block " << i+1 << "/" << num_blocks << endl;
        }
        u8 keystream[16];
        SNOW_V.keystream(keystream);
      
        // Convert the keystream to a bitset
        bitset<128> ks_block;
        for (int j = 0; j < 16; j++) {
            for (int k = 0; k < 8; k++) {
                ks_block.set(j * 8 + k, (keystream[j] >> k) & 1);
            }
        }
        
        // Get the corresponding input block
        bitset<128> input_block;
        int start_bit = i * 128;
        int bits_to_process = (i == num_blocks - 1 && remaining_bits != 0) ? remaining_bits : 128;
        
        for (int j = 0; j < bits_to_process; j++) {
            input_block.set(j, input[start_bit + j]);
        }
        if (snow_is_debug_mode) {
          // Print input block
          u8 input_bytes[16] = {0};
          for (int j = 0; j < bits_to_process; j++) {
              if (input_block[j]) {
                  input_bytes[j/8] |= (1 << (j%8));
              }
          }
          print_hex(input_bytes, (bits_to_process + 7)/8, "Input Block");
          print_hex(keystream, 16, "Keystream Block");
        }
        
        // XOR the input block with the keystream
        bitset<128> output_block = input_block ^ ks_block;
        
        // Store the result in the output bitset
        for (int j = 0; j < bits_to_process; j++) {
            output.set(start_bit + j, output_block[j]);
        }
        if (snow_is_debug_mode) {
          // Print output block
          u8 output_bytes[16] = {0};
          for (int j = 0; j < bits_to_process; j++) {
              if (output_block[j]) {
                  output_bytes[j/8] |= (1 << (j%8));
              }
          }
          print_hex(output_bytes, (bits_to_process + 7)/8, "Output Block");
        }
    }
    
    if (snow_is_debug_mode) {
      cout << "\n=== Stream Cipher Complete ===" << endl;
    }
    
    return output;
}


std::vector<char> stream_cipher_SNOW3G_BYTES(std::vector<char>& input) {
    if (input.empty()) {
        return {};
    }
    
    std::vector<char> output;
    output.resize(input.size());
    
    const size_t block_size = 16; // Tamaño de bloque en bytes (128 bits)
    const size_t num_blocks = input.size() / block_size;
    const size_t remaining_bytes = input.size() % block_size;
    
    if (snow_is_debug_mode) {
        std::cout << "\n=== Stream Cipher Operation ===" << std::endl;
        std::cout << "Input size: " << input.size() << " bytes" << std::endl;
        std::cout << "Processing " << num_blocks << " full blocks ("
                  << num_blocks * block_size << " bytes)" << std::endl;
        if (remaining_bytes > 0) {
            std::cout << "Plus " << remaining_bytes << " remaining bytes" << std::endl;
        }
    }
    
    // Procesar bloques completos
    for (size_t i = 0; i < num_blocks; ++i) {
        if (snow_is_debug_mode) {
            std::cout << "\nProcessing block " << i+1 << "/" << num_blocks << std::endl;
        }
        
        uint8_t keystream[block_size];
        SNOW_V.keystream(keystream);
        
        const size_t offset = i * block_size;
        
        if (snow_is_debug_mode) {
            print_hex(keystream, block_size, "Keystream Block");
        }
        
        // XOR entre el bloque de entrada y el keystream
        for (size_t j = 0; j < block_size; ++j) {
            output[offset + j] = input[offset + j] ^ keystream[j];
        }
        
    }
    
    // Procesar bytes restantes (si los hay)
    if (remaining_bytes > 0) {
        if (snow_is_debug_mode) {
            std::cout << "\nProcessing final partial block (" 
                      << remaining_bytes << " bytes)" << std::endl;
        }
        
        uint8_t keystream[block_size];
        SNOW_V.keystream(keystream);
        
        const size_t offset = num_blocks * block_size;
        
        if (snow_is_debug_mode) {
            print_hex(keystream, remaining_bytes, "Keystream Block");
        }
        
        // XOR solo para los bytes restantes
        for (size_t j = 0; j < remaining_bytes; ++j) {
            output[offset + j] = input[offset + j] ^ keystream[j];
        }
        
    }
    
    if (snow_is_debug_mode) {
        std::cout << "\n=== Stream Cipher Complete ===" << std::endl;
    }
    
    return output;
}

#endif
