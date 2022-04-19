#include <fstream>
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <bitset>

class BitArray {
    private:
        std::vector<uint64_t> bv;
        std::vector<uint32_t> sv;

    public:

        BitArray(int i) {
            int j = i % 64;
            int z = i / 64;

            // std::cerr << __builtin_popcountl(0UL << 64 - 0) << std::endl;

            bv.resize(z + 1);
            sv.resize(z + 1);
        }

        int binary_search(int number) {
            int size = bv.size() * 64;
            int l = 0;
            int r = size - 1;
            static int middle, val = 0;

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

        int location(int i) {
            int index = binary_search(i);
            while (index >= 0 && sum(index) == i) {
                index--;
            }
            return index + 1;
        }

        int sum(int i) {
            uint64_t j = i % 64;
            uint64_t z = i / 64;

            uint64_t sum = z > 0 ? sv[z - 1] : 0;

            uint64_t remaining_bits_to_check = bv[z];

            uint64_t mask = (1UL << j) - 1;

            // std::cerr << i << " " << j << " " << z << " " << sum << " " << bv[z] << " " << mask << std::endl;

            remaining_bits_to_check &= mask;

            sum += __builtin_popcountl(remaining_bits_to_check);
            // std::cerr << "__" << __builtin_popcountl(remaining_bits_to_check) << std::endl;
            return sum;
        }

        void precompute_sums() {
            int sum = 0;
            int i = 0;
            while (i < bv.size()) {
                sum += __builtin_popcountl(bv[i]);
                sv[i] = sum;
                i++;
            }
        }
        
        bool get(int i) {
            int j = i % 64;
            int z = i / 64;

            return bv[z] & (1UL << j);
        }

        void set(int i) {
            int j = i % 64;
            int z = i / 64;

            bv[z] |= 1UL << j;
        }
};

class PackedArray {
    private:
        int k;
        int n;
        std::vector<bool> v;
    
    public:
        PackedArray(int _n, int _k) {
            n = _n;
            k = _k;
            v.resize(n * k);
        }

        void set(u_int64_t num) {
            int i = 0;
            while (i < k) {
                
            }
        }

        bool get(u_int64_t) {
            return false;
        }
};

int main(int argc, char const* argv[]) {
    std::ifstream in;

    std::vector<uint64_t> v;



    int n;
    std::cin.read((char*)&n, sizeof(uint64_t));
    int m;
    std::cin.read((char*)&m, sizeof(uint64_t));
    // std::cerr << n << " " << m << std::endl;

    BitArray b(m);

    int i = 0;
    while (i++ < n) {
        uint64_t num;
        std::cin.read((char*)&num, sizeof(uint64_t));
        v.push_back(num);
    }

    i = 0;
    while (i < n) {
        b.set(v[i]);
        i++;
    }

    v.clear();
    b.precompute_sums();

    i = 0;
    while (i++ < n) {
        uint64_t num;
        std::cin.read((char*)&num, sizeof(uint64_t));
        v.push_back(num);
    }

    i = 0;
    while (i < n) {
        // std::cerr << v[i] << " " << b.get(v[i]) << std::endl;
        // std::cout << "i = " << i << " sum = " << v[i] << " " << std::endl;;
        std::cout << b.location(v[i]) << std::endl;
        i++;
    }

	// in.close();
    
    return 0;
}
