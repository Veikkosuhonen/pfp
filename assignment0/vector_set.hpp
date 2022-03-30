#include <vector>
#include <bitset>
#include <algorithm>

class vector_set {
    private:
        std::vector<int> v;

        bool query_mode = false;
        int size = 0;
        int l = 0;
        int r = 0;
        int range = 0;
        int middle_val = 0;

        void switch_to_query_mode() {
            size = v.size();
            std::sort(v.begin(), v.end());
            middle_val = v[v.size() / 2]; // every query needs it so it might just help a tiny bit
        }

        int binary_search(int number) {
            
            static int middle, val = 0;

            if (number < middle_val) {
                r = size / 2 - 1;
                l = 0;
            } else {
                l = size / 2 + 1;
                r = size;
            }
            range = size / 4;
            middle = l + range;

            while (l <= r) {
                val = v[middle];

                if (number < val) {
                    r = middle - 1;
                } else if (number > val) {
                    l = middle + 1;
                } else {
                    return 1;
                }
                range /= 2;
                middle = l + range;
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
            return binary_search(number);
        }
};