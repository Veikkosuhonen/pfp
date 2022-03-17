#include <iostream>
#include "header.hpp"

using namespace std;

void testi(int &x) {
    x = multiply(x, 2);
}

int main() {
    int x = 2;
    testi(x);
    cout << x << "\n"; // 5
}
