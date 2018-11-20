#include <iostream>
#include "wallet.h"

/**
 * For asserting runtime failures
 * Change -DERR1 to desired test case
 * g++-8 -Wall -Wextra -O2 -std=c++17 -DERR1 wallet.h err_test.cc -o wallet.o
 */

int main() {
#ifdef ERR1
    Wallet w1(-5);
#endif
#ifdef ERR2
    Wallet w1(3);
    w1 -= 4;
#endif
#ifdef ERR3
    Wallet w1(3);
    w1 += (-4);
#endif
#ifdef ERR4
    Wallet w1(3), w2(4);
    w1 -= w2;
#endif
#ifdef ERR5
    Wallet w1;
    w1 = 1 - 2;
#endif
#ifdef ERR6
    std::cout << "ERR6";
    Wallet w1(1); // Value zero does not produce the desired test case
    w1 *= -1;
#endif
#ifdef ERR7
    Wallet w1;
    w1 = Wallet(1) - Wallet(2);
#endif
#ifdef ERR8
    Wallet w("  1,231  1");
#endif
#ifdef ERR9
    Wallet w("")
#endif
#ifdef ERR10
    Wallet w("0.111111111")
#endif
    return 0;
}
