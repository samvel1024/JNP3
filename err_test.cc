#include <iostream>
#include "wallet.h"

/**
 * For asserting compilation failures
 * Change -DERR1 to desired test case
 * g++-8 -Wall -Wextra -O2 -std=c++17 -DERR1 wallet.h err_test.cc -o wallet.o
 */

int main() {
#ifdef ERR1
    Wallet w1, w2;
    w1 = Wallet(1);
    w1 = w2;
#endif
#ifdef ERR2
    Wallet w1(1), w2(2);
    Wallet suma1 = w1 + Wallet(1);
#endif
#ifdef ERR3
    Wallet suma1 = Wallet(1);
    Wallet suma2 = Wallet(2) ;
    Wallet suma3 = suma1 + suma2;
#endif
#ifdef ERR4
    Empty() += Wallet(1);
#endif
    return 0;

}
