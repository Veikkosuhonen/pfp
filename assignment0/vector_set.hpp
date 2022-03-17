#include <vector>
#include <algorithm>
#include <iostream>
#include <cmath>

class vector_set {
    private:
        const static int index_length = 64;
        int index_page_size;

        std::vector<int> v;
        std::vector<int> index = std::vector<int>(index_length);

        bool query_mode = false;
        int l = 0;
        int r = 0;
        int found = 0;

        void switch_to_query_mode() {
            std::sort(v.begin(), v.end());
            build_index();
        }

        void build_index() {
            
            index_page_size = std::floor(1.0 * v.size() / index_length);
            index_page_size = index_page_size == 0 ? 1 : index_page_size;
            for (int i = 0; i < index_length; i++) {
                index[i] = v[std::min(i * index_page_size, (int)v.size())];
            }
        }

        int search(int number) {
            // search index
            int start = index_search(number);
            int end = std::min(start + index_page_size, (int)v.size());

            if (number == 2117502619) {
                1715126711;
                1748042173;
                2117502619;
                std::cerr << v[start] << " " << v[end] << " " << start / index_page_size << " " << std::endl;
                return sequential_search(number, start, end);
            }
            // 2147483647;
            // 1323470365;
            return binary_search(number, 0, v.size());
        }

        int index_search(int number) {
            for (int i = 0; i < index_length; i++) {
                if (number <= index[i]) {
                    return (i) * index_page_size;
                }
            }
        }

        int binary_search(int number, int start, int end) {
            
            static int middle, val = 0;
            l = start;
            r = end;
            found = 0;

            while (l <= r) {
                middle = l + (r - l) / 2;
                val = v[middle];

                if (number < val) {
                    r = middle - 1;
                } else if (number > val) {
                    l = middle + 1;
                } else {
                    l = middle +1;
                    found = val;
                }
            }

            return found;
        }

        int sequential_search(int number, int start, int end) {
            for (int i = start; i < end; i++) {
                if (number == v[i]) return number;
            }
            return 0;
        }

    public: 
        void insert(int number) {
            v.push_back(number);
        }

        int count(int number) {
            if (!query_mode) {
                switch_to_query_mode();
                query_mode = true;
            }

            return search(number);
        }
};