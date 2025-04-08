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
#include "../../shared/helpers.cpp"
#include "../../shared/global.cpp"

using namespace std; 
#ifndef SAC_TEST
#define SAC_TEST

int ComputeSAC(int i,bitset<MAX_SIZE>(*cipher)(bitset<MAX_SIZE>,int n,int m),int n,int m,int l){
	int t = 0;double expected=l/2;
	for (int r = 0; r < l; r++) {
		bitset<MAX_SIZE> X_r_i = D[r];
		X_r_i.flip(i);
		bitset<MAX_SIZE> Y_r_i = cipher(X_r_i,n,m);
		bitset<MAX_SIZE> V_r_i = Y_r_i ^ output[r];
		for (int k = 0; k < m; k++)
			bitCount[i][k] += V_r_i.test(k);
	}
	for (int j = 0; j < m; j++) {
		double observed = bitCount[i][j];	//cant 1s en la columna j
		double chiSqTest = ((observed - expected) * (observed - expected)) / expected;
		if (chiSqTest > X2_critical_value) t++;
	}
	return t;
}


bool SAC_stream_cipher___Parallel(bitset<MAX_SIZE>(*cipher)(bitset<MAX_SIZE>,int n,int m),double a1,int n,int m,int l,bitset<MAX_SIZE>D[]){
	double T = 0;
	parallelFor(n, [&](int start, int end){ 
	    for(int i = start; i < end; ++i)
	        rejections[i] = ComputeSAC(i,cipher,n,m,l);
	});
	T = accumulate(rejections, rejections + n, T);
	cout << "Numero de rechazos T = " << T << endl;
	double _n = n, _m = m;
	cout << "valor alpha_1 = " << a1 << endl;
	double E_T_H0 =  a1 * _n * _m;
	cout << "valor esperado E(T|H0) = " << E_T_H0 << endl;
	double variance = E_T_H0 * (1 - a1);
	cout << "varianza = " << variance << endl;
	double Z_T = (T - E_T_H0) / sqrt(variance);
	cout << "valor Z_T = " << Z_T << endl;
	return Z_T <= Z_critical_normal_distribution;
}


#endif