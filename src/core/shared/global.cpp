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

using namespace std; 
#ifndef GLOBAL
#define GLOBAL

mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());

typedef struct {
    int n;
    int m;
    int l;
    int alg_option;
    int cipher_opt;
} TestSetUp;

const int MAX_L = 1000000;
const int MAX_SIZE = 2048;

const double Z_critical_normal_distribution = 3.090;
const double X2_critical_value = 6.6349;

int bitCount[MAX_SIZE + 1][MAX_SIZE + 1];
int rejections[MAX_SIZE];
bitset<MAX_SIZE> D[MAX_L];
bitset<MAX_SIZE> output[MAX_L];

const int HC256_CHIPHER_OPTION = 1;
const int RC4_CHIPHER_OPTION = 2;
const int IDENTITY_CHIPHER_OPTION = 3;

#endif