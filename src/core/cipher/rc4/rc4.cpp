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
#include "rc4.h"


#ifndef RC4_CIPHER
#define RC4_CIPHER

bitset<MAX_SIZE> stream_cipher_RC4(bitset<MAX_SIZE>input,int n,int m){
	unsigned char key[256], K[256], S[256];	
	fill(key, key + 256, 0);
	fill(K, K + 256, 0);
	fill(S, S + 256, 0);	
	unsigned int key_len = n/8;
	int round, keystream_len = m/8;
	for (int i = 0; i < key_len; i++) {
		int pos = (key_len - i - 1)*8;
		bitset<8>temp = bitset<8>(0);
		for (int j = 0; j < 8; j++)
			temp[j] = input[pos + j];
		unsigned char uc = temp.to_ulong();
		key[i] = uc;
	}
	rc4_expandkey(key_len, key, K);
    rc4_ksa(S, K);
	int i = 0, j = 0;
	bitset<MAX_SIZE>result  = bitset<MAX_SIZE>(0);
    for(round = 0; round < keystream_len; round ++) {
        unsigned char v = rc4_prga(i, j, S);
		bitset<8>_byte = bitset<8>(v);
		int pos = (keystream_len - round - 1)*8;
		for (int k = 0; k < 8; k++)
			result[pos + k] = _byte[k];
    }
	return result;
}
#endif
