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
#include "hc256.h"

#ifndef HC256_CIPHER
#define HC256_CIPHER


bitset<MAX_SIZE> stream_cipher_HC256(bitset<MAX_SIZE>input,int n,int m){//256 bits input key
	const int keystream_len = m/32;int round;
	uint32 key[8],iv[8],DATA[16],P[1024],Q[1024];
	uint32 X[16],Y[16];
	uint32 counter2048; // counter2048 = i mod 2048;

	fill(key, key + 8, 0);
	fill(iv, iv + 8, 0);
	fill(DATA, DATA + keystream_len, 0);

	unsigned int key_len = 8;	// 256 bit key
	for (int it = 0; it < key_len; it++) {
		int pos = (key_len - it - 1)*32;
		bitset<32> temp = bitset<32>(0);
		for (int j = 0; j < 32; j++)
			temp[j] = input[pos + j];		
		key[it] = temp.to_ulong();
	}
    initialization(key,iv,P,Q,counter2048,X,Y);
	encrypt(DATA,counter2048,P,X,Q,Y);

	bitset<MAX_SIZE> result  = bitset<MAX_SIZE>(0);
    for(round = 0; round < keystream_len; round ++) {
		bitset<32>_byte = bitset<32> (DATA[round]);
		int pos = (keystream_len - round - 1)*32;
		for (int k = 0; k < 32; k++)
			result[pos + k] = _byte[k];
    }
	return result;
}


#endif