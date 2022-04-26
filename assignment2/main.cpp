#include <fstream>
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <bitset>
#include <chrono>
#include <string>

#include "../include/nanobench.h"

#define benchmark ankerl::nanobench::Bench().run
#define keep ankerl::nanobench::doNotOptimizeAway

const uint8_t FIRST_BIT = (1 << 7);
const uint8_t FIRST_BIT_OFF = FIRST_BIT - 1;

class BitArray {
    private:
        int k = 8;

        std::vector<uint8_t> bv;
    
    public:
        BitArray() {
            // bv.resize(64);
        }

        void add(uint64_t num) {
            uint64_t block;

            while (true) {

                block = num % 128;

                // block &= FIRST_BIT_OFF;
                if (num < 128) {
                    bv.push_back(block + 128); 
                    break;
                }
                bv.push_back(block);
                // std::cout << (int)block << " ";

                num >>= 7;
            }
            // std::cout << num << " " << blocks << " " << leading_bits << std::endl;

        }

        void decode() {
            int i = 0;
            int j = 0;
            uint64_t sum = 0UL;

            uint64_t block;
            while (i < bv.size()) {
                block = 0UL;
                block += bv[i];
                bool stop = block & FIRST_BIT;
                block &= FIRST_BIT_OFF;

                block = block << (7 * j);
                sum |= block;

                j++;
                i++;

                if (stop) {
                    std::cout << sum << std::endl;
                    sum = 0UL;
                    j = 0;
                }
            }
            
        }

        std::size_t size() {
            return bv.size();
        }
};


int main(int argc, char const* argv[]) {
    std::ifstream in;
    std::vector<uint64_t> v;

    int n;
    std::cin.read((char*)&n, sizeof(uint64_t));
    int k;
    std::cin.read((char*)&k, sizeof(uint64_t));
    int s;
    std::cin.read((char*)&s, sizeof(uint64_t));

    std::cerr << n << " " << k << " " << s << std::endl;

    BitArray b;

    // benchmark("Read and encode", [&] {
        int i = 0;
        while (i++ < n) {
            uint64_t num;
            std::cin.read((char*)&num, sizeof(uint64_t));
            b.add(num);
        }
    // });


    std::cerr << b.size() << std::endl;

    // benchmark("Decode", [&] {
        b.decode();
    // });



    /*int s = 0;
    benchmark("Stuff", [&] {
        while (i < n) {
            // std::cerr << v[i] << " " << b.get(v[i]) << std::endl;
            // std::cout << "i = " << i << " sum = " << v[i] << " " << std::endl;;
            // std::cout << b.sum(v[i]) << std::endl;
            // s += b.sum(v[i]);
        }
        keep(s);
        
    });*/
    
    return 0;
}

