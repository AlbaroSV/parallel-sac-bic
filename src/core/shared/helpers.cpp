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
#include <string>
#include <iomanip>
#include <sstream>

using namespace std; 
#ifndef HELPERS
#define HELPERS


static void parallelFor(unsigned n,function<void(int begin,int end)>func){

    unsigned k = std::thread::hardware_concurrency();
    unsigned n_threads = k == 0 ? 8 : k;
    unsigned chunk_size = n / n_threads;
    unsigned chunk_remainder = n % n_threads;

    std::vector< std::thread > my_threads(n_threads);

	for(unsigned i = 0; i < n_threads; ++i)	{
		int start = i * chunk_size;
		my_threads[i] = std::thread(func, start, start+chunk_size);
	}

    int start = n_threads * chunk_size;
    func( start, start + chunk_remainder);

    std::for_each(my_threads.begin(), my_threads.end(), std::mem_fn(&std::thread::join));
}


// Helper function to print binary representation
void print_binary(const uint8_t* data, size_t size, const string& label = "") {
    if (!label.empty()) cout << label << ": ";
    for (size_t i = 0; i < size; i++) {
        std::bitset<8> bits(data[i]);
        std::cout << bits << " ";
    }
    std::cout << std::endl;
}

// Helper function to print hexadecimal representation
void print_hex(const uint8_t* data, size_t size, const string& label = "") {
    if (!label.empty()) cout << label << ": ";
    for (size_t i = 0; i < size; i++) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') 
                  << static_cast<int>(data[i]) << " ";
    }
    std::cout << std::dec << std::endl;
}

#endif