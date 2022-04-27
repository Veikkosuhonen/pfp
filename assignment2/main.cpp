#include <fstream>
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <bitset>
#include <chrono>
#include <string>

const uint8_t FIRST_BIT = (1 << 7);
const uint8_t FIRST_BIT_OFF = FIRST_BIT - 1;
const uint64_t MODULUS_128 = (((1UL << 63) - 1) >> (63 - 7));

void print_time(std::chrono::_V2::system_clock::time_point start, std::chrono::_V2::system_clock::time_point end) {
    std::cerr << std::chrono::duration_cast<std::chrono::duration<double>>(end - start).count() << std::endl;
}

class BitArray {
    private:
        int k = 8;

        std::vector<uint8_t> bv;
    
    public:

        void add(uint64_t num) {
            uint64_t block;

            while (true) {
                block = num & MODULUS_128;

                if (num < 128) {
                    bv.push_back(block + 128UL); 
                    break;
                }
                bv.push_back(block);
                num >>= 7;
            }
        }

        void decode() {
            int i = 0;
            int j = 0;
            uint64_t sum = 0UL;

            uint64_t block;
            while (i < bv.size()) {
                block = bv[i];
                bool stop = block & FIRST_BIT;
                block &= FIRST_BIT_OFF;

                sum |= block << j;

                j += 7;
                i++;

                if (stop) {
                    std::cout << sum << "\n";
                    sum = 0UL;
                    j = 0;
                }
            }
            
        }

        void decode_s() {
            int i = 0;
            int j = 0;
            uint64_t sum = 0UL;
            uint64_t prev = 0UL;

            uint64_t block;
            while (i < bv.size()) {
                block = 0UL;
                block = bv[i];
                bool stop = block & FIRST_BIT;
                block &= FIRST_BIT_OFF;

                sum |= block << j;

                j += 7;
                i++;

                if (stop) {
                    sum += prev;
                    std::cout << sum << "\n";
                    prev = sum;
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
    // std::freopen(NULL, "rb", stdin);

    int n;
    std::cin.read((char*)&n, sizeof(uint64_t));
    int k;
    std::cin.read((char*)&k, sizeof(uint64_t));
    int s;
    std::cin.read((char*)&s, sizeof(uint64_t));

    // std::cerr << n << " " << k << " " << s << std::endl;

    BitArray b;

    // std::chrono::high_resolution_clock clock;
    // auto start = clock.now();

    int i = 0;
    uint64_t prev = 0UL;

    if (s > 0) {
        uint64_t num;
        while (i < n) {
            std::cin.read((char*)&num, sizeof(uint64_t));
            b.add(num - prev);
            prev = num;
            i++;
        }
    } else {
        uint64_t num;
        while (i < n) {
            std::cin.read((char*)&num, sizeof(uint64_t));
            b.add(num);
            i++;
        }
    }

    // auto end = clock.now();
    // std::cerr << "Read and encode: ";
    // print_time(start, end);


    std::cout << b.size() << "\n";

    
    // start = clock.now();

    if (s > 0) {
        b.decode_s();
    } else {
        b.decode();
    }

    // end = clock.now();
    // std::cerr << "Decode: ";
    // print_time(start, end);
    
    
    return 0;
}

