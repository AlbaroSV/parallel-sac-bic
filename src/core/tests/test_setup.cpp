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
#include "../cipher/identity/identity_cipher.cpp"
#include "../tests/bic/bic.cpp"
#include "../tests/sac/sac.cpp"
#include "../shared/global.cpp"
#include "../cipher/hc256/hc256.cpp" 
#include "../cipher/rc4/rc4.cpp" 


using namespace std;
using namespace chrono;

#ifndef TEST_SETUP
#define TEST_SETUP

void make_data_test(int n,int l){	
	set<string> vectors; int ll = 0;
	while (vectors.size() < l){
		string s = "";
		for (int j = 0; j < n; j++){
			int rd = rng();
			if (rd % 2) s+="1";
			else s+="0";
		}
		vectors.insert(s);
	}
	for (auto &&v : vectors)D[ll++]=bitset<MAX_SIZE>(v);
}


void pre_compute(bitset<MAX_SIZE>(*cipher)(bitset<MAX_SIZE>,int n,int m),int n,int m,int l){
	make_data_test(n,l);//build D
	for (int i = 0; i < l; i++)output[i]=cipher(D[i],n,m);
}

void make_test(TestSetUp* testSetUp){   	
	
	int alg_option = (*testSetUp).alg_option; 
	int cipher_opt = (*testSetUp).cipher_opt; 
	int n = (*testSetUp).n, m = (*testSetUp).m, l = (*testSetUp).l;
	
	if(cipher_opt==1){//Build D[r], output[r] sets
		pre_compute(&stream_cipher_IDENTITY,n,m,l);
	} else pre_compute(&stream_cipher_IDENTITY,n,m,l);	
	cout << "entrada Xr, Yr computada" << endl;
	
	bool TEST_RESULT;
	auto start = high_resolution_clock::now();
	string message = ""; 

	switch (alg_option)
	{
		case 1:
			//BIC
			if(cipher_opt==1) TEST_RESULT = BIC_stream_cipher___Parallel(&stream_cipher_RC4,0.01,n,m,l,D);
			else TEST_RESULT = BIC_stream_cipher___Parallel(&stream_cipher_HC256,0.01,n,m,l,D);
			message = "el BIC";			
		break;
		case 2: 
			// SAC
			if(cipher_opt==1) TEST_RESULT = SAC_stream_cipher___Parallel(&stream_cipher_RC4,0.01,n,m,l,D);
			else TEST_RESULT = SAC_stream_cipher___Parallel(&stream_cipher_HC256,0.01,n,m,l,D);
			message = "el SAC"; 
		break; 
	}

	auto stop = high_resolution_clock::now();
	auto secondsDuration = duration_cast<seconds>(stop - start);
	cout << "Tiempo de computo en segundos: " << secondsDuration.count() << " seconds" << "\n";
	auto minutesDuration = duration_cast<minutes>(stop - start);
	cout << "Tiempo de computo en minutos: " << minutesDuration.count() << " minutos" << "\n";
	cout << (TEST_RESULT ? "El cifrador satisface " : "El cifrador no satisface ") + message << endl; 
	
	return; 
}
#endif