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
#ifndef BIC_TEST
#define BIC_TEST

int ComputeBIC(int i,bitset<MAX_SIZE>(*cipher)(bitset<MAX_SIZE>,int n,int m),int n,int m,int l){
	int t = 0; double expected = l/2,_d = l/4;
	vector<bitset<MAX_SIZE>> v;
	for (int r = 0; r < l; r++) {
		bitset<MAX_SIZE> X_r_i = D[r];
		X_r_i.flip(i);
		bitset<MAX_SIZE> Y_r_i = cipher(X_r_i,n,m);
		bitset<MAX_SIZE> V_r_i = Y_r_i ^ output[r];
		v.push_back(V_r_i);
	}
	for (int j = 1; j < m; j++) {
		for (int k = 0; k < j; k++) {
			double observed = 0.0;
			for (int ll = 0; ll < l; ll++) observed+=v[ll][j]^v[ll][k];
			double chiSqTest = ((observed - expected) * (observed - expected))/_d;
			if (chiSqTest > X2_critical_value) t++;
		}
	}
	return t;
}


bool BIC_stream_cipher___Parallel(std::bitset<MAX_SIZE>(*cipher)(std::bitset<MAX_SIZE>,int n,int m),double a1,int n,int m,int l,bitset<MAX_SIZE>D[]){
	double T = 0;
	parallelFor(n, [&](int start, int end){
	    for(int i = start; i < end; ++i)
	        rejections[i] = ComputeBIC(i,cipher,n,m,l);
	});
	T = accumulate(rejections, rejections + n, T);
	cout << "Numero de rechazos T = " << T << endl;
	double C_m_2 = (m * (m - 1))/2, _n = n, _m = m;
	cout << "valor alpha_1 = " << a1 << endl;
	double E_T_H0 =  a1 * _n * C_m_2;
	cout << "valor esperado E(T|H0) = " << E_T_H0 << endl;
	double variance = E_T_H0 * (1 - a1);
	cout << "varianza = " << variance << endl;
	double Z_T = (T - E_T_H0) / sqrt(variance);
	cout << "valor Z_T = " << Z_T << endl;
	return Z_T <= Z_critical_normal_distribution;
}

#endif