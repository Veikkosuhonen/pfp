#include <vector>
#include <algorithm>
#include <bitset>

class constrained_set {
    private:
        std::vector<bool> set;

    public:
        // constrained_set
        void insert(int number) {
            if (number >= set.size()) {
                set.resize(number + 1);
            }
            set[number] = true;
            // std::cerr << set.size() << " " << set[number] << std::endl;
        }

        int count(int number) {
            // std::cerr << set.size() << std::endl;
            if (number >= set.size())
                return false;
            return set[number];
        }
};