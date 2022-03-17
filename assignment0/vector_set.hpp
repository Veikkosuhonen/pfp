#include <vector>
#include <algorithm>
#include <iostream>
#include <cmath>

class vector_set {
    private:
        std::vector<int> v;

        bool query_mode = false;
        int l = 0;
        int r = 0;
        int found = 0;

        void switch_to_query_mode() {
            std::sort(v.begin(), v.end());
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

    public: 
        void insert(int number) {
            v.push_back(number);
        }

        int count(int number) {
            if (!query_mode) {
                switch_to_query_mode();
                query_mode = true;
            }

            return binary_search(number, 0, v.size());
        }
};