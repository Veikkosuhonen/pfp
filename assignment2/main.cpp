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
const uint64_t MODULUS_64 = (((1UL << 63) - 1) >> (63 - 6));

void print_time(std::chrono::_V2::system_clock::time_point start, std::chrono::_V2::system_clock::time_point end) {
    std::cerr << std::chrono::duration_cast<std::chrono::duration<double>>(end - start).count() << std::endl;
}

class BitArray {
    private:
        uint64_t k = 0;
        uint64_t acual_block_size = 0;
        uint64_t first_bit = 0;
        uint64_t first_bit_off = 0;
        uint64_t modulus = 0;
        uint64_t last_block_pos = 0;
        std::vector<uint64_t> bv;
        uint64_t n_blocks = 0;
    
    public:

        BitArray(uint64_t __k) {
            k = __k;
            acual_block_size = k + 1;
            modulus = (((1UL << 63) - 1) >> (63 - k));
            first_bit = (1UL << k);
            first_bit_off = first_bit - 1;
            bv.push_back(0UL);
        }

        void add(uint64_t num) {
            uint64_t block;

            while (true) {
                n_blocks++;
                // what to add next
                block = num & modulus;
                // if less than 1 >> k, set stop bit
                if (num < first_bit) {
                    block += first_bit;
                    // std::cerr << "stop bit " << block-first_bit << " ";
                }


                // std::cerr << last_block_pos << " ";

                uint64_t space_left = 64 - last_block_pos;
                if (space_left == 0) {
                    bv.push_back(block);
                    last_block_pos = acual_block_size;
                    // std::cout << "B " << " ";
                } else {
                    bv[bv.size() - 1] |= (block << last_block_pos);
                    last_block_pos += acual_block_size;

                    if (space_left < acual_block_size) {
                        uint64_t remaining = acual_block_size - space_left;
                        uint64_t new_block = (block >> space_left) & ((1UL << remaining) - 1);
                        bv.push_back(new_block);
                        last_block_pos = remaining;
                        // std::cout << "Br " << block << " " << space_left << " " << remaining << " " << new_block << "\n";
                    }
                    
                }
                
                // std::cerr << bv[bv.size() - 1] << "\n";
                if (num < first_bit) {
                    break;
                }
                

                num >>= k;
            }
        }

        void decode() {
            // std::cerr << (bv[0] & first_bit_off) << "\n";
            uint64_t i = 0;
            uint64_t sum = 0UL;

            uint64_t block;
            uint64_t bv_block;
            uint64_t bv_block_internal_index = 0;
            uint64_t bv_current_block_index = 0;
            
            while (i < n_blocks) {
                uint64_t bv_index = (i * acual_block_size) / 64;
                bv_block = bv[bv_index];
                block = bv_block >> bv_block_internal_index;

                uint64_t space_left = 64 - bv_block_internal_index;
                if (space_left < acual_block_size) {
                    uint64_t remaining = acual_block_size - space_left;
                    uint64_t remaining_bits = (bv[bv_index + 1] & ((1UL << remaining) - 1)) << (acual_block_size - remaining);
                    block |= remaining_bits;
                    // std::cerr << "While decoding, one block spanned multiple bv_blocks " << remaining << " " << remaining_bits << " " << block << " " << (bv[bv_index + 1] & ((1UL << remaining) - 1)) << "\n";
                }

                bool stop = block & first_bit;
                block &= first_bit_off;

                // std::cout << bv_index << " " << bv_block_internal_index << " " << bv_current_block_index << " " << bv_block << " " << block << " " << stop << "\n";

                sum |= block << bv_current_block_index;

                bv_block_internal_index = (bv_block_internal_index + acual_block_size) & MODULUS_64;
                bv_current_block_index += k;
                i++;

                if (stop) {
                    std::cout << sum << "\n";
                    sum = 0UL;
                    bv_current_block_index = 0;
                }
            }
            
        }

        void decode_s() {
            // std::cerr << (bv[0] & first_bit_off) << "\n";
            uint64_t i = 0;
            uint64_t sum = 0UL;

            uint64_t block;
            uint64_t bv_block;
            uint64_t bv_block_internal_index = 0;
            uint64_t bv_current_block_index = 0;
            uint64_t prev = 0;
            while (i < n_blocks) {
                uint64_t bv_index = (i * acual_block_size) / 64;
                bv_block = bv[bv_index];
                block = bv_block >> bv_block_internal_index;

                uint64_t space_left = 64 - bv_block_internal_index;
                if (space_left < acual_block_size) {
                    uint64_t remaining = acual_block_size - space_left;
                    uint64_t remaining_bits = (bv[bv_index + 1] & ((1UL << remaining) - 1)) << (acual_block_size - remaining);
                    block |= remaining_bits;
                    // std::cerr << "While decoding, one block spanned multiple bv_blocks " << remaining << " " << remaining_bits << " " << block << " " << (bv[bv_index + 1] & ((1UL << remaining) - 1)) << "\n";
                }

                bool stop = block & first_bit;
                block &= first_bit_off;

                // std::cout << bv_index << " " << bv_block_internal_index << " " << bv_current_block_index << " " << bv_block << " " << block << " " << stop << "\n";

                sum |= block << bv_current_block_index;

                bv_block_internal_index = (bv_block_internal_index + acual_block_size) & MODULUS_64;
                bv_current_block_index += k;
                i++;

                if (stop) {
                    std::cout << sum+prev << "\n";
                    prev = sum+prev;
                    sum = 0UL;
                    bv_current_block_index = 0;
                }
            }
        }

        std::size_t size() {
            return n_blocks;
        }
};

int main(int argc, char const* argv[]) {
    std::freopen(NULL, "rb", stdin);
    
    // https://stackoverflow.com/questions/18412164/fast-c-string-output
    std::ios_base::sync_with_stdio(false);
    // std::cin.tie(NULL);
    // std::cout.tie(NULL);
    std::cout << std::nounitbuf;
    // std::setvbuf(stdin, nullptr, _IOFBF, 65536 / 2);
    // std::setvbuf(stdout, nullptr, _IOFBF, 65536);

    uint64_t n;
    std::fread((char*)&n, sizeof(uint64_t), 1, stdin);
    uint64_t k;
    std::fread((char*)&k, sizeof(uint64_t), 1, stdin);
    uint64_t s;
    std::fread((char*)&s, sizeof(uint64_t), 1, stdin);

    // k = k < 2 ? 7 : k;
    // k = 7;
    std::cerr << n << " " << k << " " << s << std::endl;

    BitArray b(k);

    std::chrono::high_resolution_clock clock;
    auto start = clock.now();

    uint64_t i = 0;
    uint64_t prev = 0UL;

    const size_t BUFFER_SIZE = 64;
    uint64_t num;
    uint64_t buffer[BUFFER_SIZE];
    uint64_t mod = 0;
    if (s > 0) {
        while (i < n) {
            mod = i % BUFFER_SIZE;
            if (mod == 0)
                std::fread(buffer, sizeof(uint64_t), BUFFER_SIZE, stdin);
            
            num = buffer[mod];
            b.add(num - prev);
            prev = num;
            
            i++;
        }
    } else {
        while (i < n) {
            mod = i % BUFFER_SIZE;
            if (mod == 0)
                std::fread(buffer, sizeof(uint64_t), BUFFER_SIZE, stdin);
            
            b.add(buffer[mod]);
            
            i++;
        }
    }

    auto end = clock.now();
    std::cerr << "Read and encode: ";
    print_time(start, end);


    std::cout << b.size() << "\n";
    u_int64_t size = b.size() * k;
    double compression = size / (1.0 * n * 64);
    std::cerr << "Blocks: " << b.size() << " Compression ratio: " << compression << "\n";
    
    start = clock.now();

    if (s > 0) {
        b.decode_s();
    } else {
        b.decode();
    }

    end = clock.now();
    std::cerr << "Decode: ";
    print_time(start, end);
    
    
    return 0;
}

