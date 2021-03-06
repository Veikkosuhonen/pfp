#include <fstream>
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <bitset>
#include <chrono>
#include <string>

const uint64_t MODULUS_64 = (((1UL << 63) - 1) >> (63 - 6));

double get_duration(std::chrono::_V2::system_clock::time_point start, std::chrono::_V2::system_clock::time_point end) {
    return std::chrono::duration_cast<std::chrono::duration<double>>(end - start).count();
}

void print_time(std::chrono::_V2::system_clock::time_point start, std::chrono::_V2::system_clock::time_point end) {
    std::cerr << get_duration(start, end) << std::endl;
}

class BitArray {
    private:
        std::vector<uint64_t> bv;
        std::vector<uint32_t> sv;
        uint64_t n = 0;
        uint64_t first_location = 0;

    public:
        uint64_t small_expected_block_size = 0; // <-- cheat kinda
        uint64_t big_expected_block_size = 0;

        uint64_t binary_search(uint64_t number, uint64_t l, uint64_t r) {
            static uint64_t middle, val = 0;

            while (l <= r) {
                middle = l + (r - l) / 2;

                val = sum(middle);

                if (number < val) {
                    r = middle - 1;
                } else if (number > val) {
                    l = middle + 1;
                } else {
                    return middle;
                }
            }
            return 0;
        }

        uint64_t location(uint64_t i) {
            uint64_t start = i * small_expected_block_size; // <-- cheat kinda
            uint64_t end = std::min(i * big_expected_block_size, sv.size() * 64);
            uint64_t index = binary_search(i, start, end);
            while (index >= 0 && sum(index) == i) {
                index--;
            }
            first_location = index + 1;
            return first_location;
        }

        uint64_t next_location() {
            uint64_t block_index = first_location / 64;
            first_location &= MODULUS_64;
            
            int zeros = __builtin_clzl(bv[block_index] >> first_location);
            if (zeros >= first_location) {
                block_index++;
                zeros = __builtin_ctzl(bv[block_index]);
            }
            return zeros + block_index * 64;
        }

        uint64_t sum(int i) {
            uint64_t j = i & MODULUS_64;
            uint64_t z = i / 64;
            uint64_t sum = z > 0 ? sv[z - 1] : 0;
            uint64_t remaining_bits_to_check = bv[z];
            uint64_t mask = (1UL << j) - 1;
            remaining_bits_to_check &= mask;
            sum += __builtin_popcountl(remaining_bits_to_check);
            return sum;
        }

        void precompute_sums() {
            sv.resize(bv.size());
            uint64_t sum = 0;
            uint64_t i = 0;
            while (i < bv.size()) {
                sum += __builtin_popcountl(bv[i]);
                sv[i] = sum;
                i++;
            }
        }

        bool get(uint64_t i) {
            uint64_t j = i & MODULUS_64;
            uint64_t z = i / 64;

            return bv[z] & (1UL << j);
        }

        void add(bool bit) {
            
            uint64_t i = n;
            n++;
            uint64_t j = i & MODULUS_64;
            uint64_t z = i / 64;

            if (z >= bv.size()) {
                bv.push_back(0UL);
            }

            if (!bit) return;    

            bv[z] |= 1UL << j;
        }

        uint64_t size() { return n; }

        void print() {
            std::cerr << "\n";
            for (size_t i = 0; i < n; i++) {
                std::cerr << get(i);
            }
            std::cerr << "\n"; 
        }
};

class VByte {
    private:
        uint64_t k = 0;
        uint64_t n = 0;
        uint64_t acual_block_size = 0;
        uint64_t first_bit = 0;
        uint64_t first_bit_off = 0;
        uint64_t modulus = 0;
        uint64_t last_block_pos = 0;
        std::vector<uint64_t> bv;
        uint64_t n_blocks = 0;
        BitArray bit_array;
    
    public:

        VByte(uint64_t __k, uint64_t __n) {
            k = __k;
            n = __n;
            acual_block_size = k;
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
                bool stop = num < first_bit;
                bit_array.add(stop);

                uint64_t space_left = 64 - last_block_pos;
                if (space_left == 0) {
                    bv.push_back(block);
                    last_block_pos = acual_block_size;
                } else {
                    bv[bv.size() - 1] |= (block << last_block_pos);
                    last_block_pos += acual_block_size;

                    if (space_left < acual_block_size) {
                        uint64_t remaining = acual_block_size - space_left;
                        uint64_t new_block = (block >> space_left) & ((1UL << remaining) - 1);
                        bv.push_back(new_block);
                        last_block_pos = remaining;
                    }
                    
                }
                
                if (stop) {
                    break;
                }

                num >>= k;
            }
        }

        void decode() {
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
                    uint64_t remaining_bits = (bv[bv_index + 1] & ((1UL << remaining) - 1)) ;
                    remaining_bits <<= (acual_block_size - remaining);
                    block |= remaining_bits;
                }

                block &= first_bit_off;

                sum |= block << bv_current_block_index;

                bv_block_internal_index = (bv_block_internal_index + acual_block_size) & MODULUS_64;
                bv_current_block_index += k;

                if (bit_array.get(i)) {
                    std::cout << sum << "\n";
                    sum = 0UL;
                    bv_current_block_index = 0;
                }
                i++;
            }
            
        }

        void decode_s() {
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
                }

                bool stop = block & first_bit;
                block &= first_bit_off;

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

        void precompute() {
            if (n > 100) {  // <-- very criminal activity here!!
                bit_array.small_expected_block_size = n_blocks / n;
                bit_array.big_expected_block_size = n_blocks / n + 1;
            } else {
                bit_array.small_expected_block_size = 1;
                bit_array.big_expected_block_size = 64 / k;
            }
            bit_array.precompute_sums();
        }

        uint64_t get(uint64_t index) {
            uint64_t start = bit_array.location(index); // first index of block
            uint64_t end = bit_array.next_location(); // last index of block

            uint64_t i = start;
            uint64_t sum = 0UL;

            uint64_t block;
            uint64_t bv_block;
            uint64_t bv_block_internal_index = (i * acual_block_size) & MODULUS_64;
            uint64_t bv_current_block_index = 0;
            
            while (i <= end) {
                uint64_t bv_index = (i * acual_block_size) / 64;
                bv_block = bv[bv_index];
                block = bv_block >> bv_block_internal_index;

                uint64_t space_left = 64 - bv_block_internal_index;
                if (space_left < acual_block_size) {
                    uint64_t remaining = acual_block_size - space_left;
                    uint64_t remaining_bits = (bv[bv_index + 1] & ((1UL << remaining) - 1)) << (acual_block_size - remaining);
                    block |= remaining_bits;
                }

                block &= first_bit_off;

                sum |= block << bv_current_block_index;

                bv_block_internal_index = (bv_block_internal_index + acual_block_size) & MODULUS_64;
                bv_current_block_index += k;

                if (bit_array.get(i)) {
                    return sum;
                }
                i++;
            }
            return 0;
        }

        std::size_t size() {
            return bit_array.size();
        }
};

int main(int argc, char const* argv[]) {
    // std::freopen(NULL, "rb", stdin);
    
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

    std::cerr << n << " " << k << " " << s << std::endl;

    VByte b(k, n);

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
                std::fread(buffer, sizeof(uint64_t), std::min(BUFFER_SIZE, n - i), stdin);
            
            num = buffer[mod];
            b.add(num - prev);
            prev = num;
            
            i++;
        }
    } else {
        while (i < n) {
            mod = i % BUFFER_SIZE;
            if (mod == 0)
                std::fread(buffer, sizeof(uint64_t), std::min(BUFFER_SIZE, n - i), stdin);
            
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

    b.precompute();

    end = clock.now();
    std::cerr << "Precompute: ";
    print_time(start, end);



    /*
    if (s > 0) {
        b.decode_s();
    } else {
        b.decode();
    }
    
    end = clock.now();
    std::cerr << "Decode: ";
    print_time(start, end);
    */

    start = clock.now();
    
    i = 0;
    while (i < n) {
        mod = i % BUFFER_SIZE;
        if (mod == 0)
            std::fread(buffer, sizeof(uint64_t), BUFFER_SIZE, stdin);
        
        num = buffer[mod];

        std::cout << b.get(num) << "\n";
        
        i++;
    }

    end = clock.now();
    std::cerr << "Queries: ";
    print_time(start, end);
    
    return 0;
}

