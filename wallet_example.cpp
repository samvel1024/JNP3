#include "wallet.h"
#include <iostream>
#include <cassert>

//TODO bring this back after moving the class declaration to .h file
static const unsigned int UNITS_IN_B = 100000000;

void custom();

void official();

using std::move;

int main() {
    custom();
    official();
}

void official() {
    // Wallet w1111(true); // exception
    // Wallet ssss = Wallet(1) + Wallet(2); // TODO remember about that!
    // assert(ssss.opSize() == 2);

    Wallet ww1;
    ww1 = std::move(ww1);
    assert(ww1.opSize() == 1);
    std::cout << "abcde\n";


    assert(Empty() == 0);
    assert(Empty() < Wallet(1));
    assert(Empty().getUnits() == 0);
    assert(Empty().opSize() == 1);
    assert(Empty()[0].getUnits() == 0);
    Wallet suma4 = Wallet(1) + Wallet(2);

    assert(suma4.opSize() == 3);

    Wallet w1;
    assert(w1 == Empty());
    assert(w1.opSize() == 1);

    Wallet w2(1), w3(1);
    assert(w2.opSize() == 1);
    assert(w2.getUnits() == UNITS_IN_B);


    assert(Wallet::fromBinary("100") == Wallet(4));
    assert(Wallet(" 1.2000 ") == Wallet("1,2"));

    assert(Wallet(2) + w2 == Wallet(3));


    assert(Wallet(1) + Wallet(2) == Wallet(3));
    assert((1 + 2) == Wallet(3));

    Wallet w4;
    assert(w4.opSize() == 1);

    w4 += 2;
    assert(w4.opSize() == 2);
    assert(w4[0] <= w4[1]);

    assert(w4[1].getUnits() == w4.getUnits());

    w4 *= 3;
    assert(w4.opSize() == 3);

    assert(w4[2].getUnits() == 6 * UNITS_IN_B);

    assert(Wallet(move(w4)).opSize() == 4);

    Wallet w5{2};
    Wallet w6 = Wallet(10) - w5;
    assert(w5 == Wallet(4));
    assert(w6 == Wallet(8));
}

void custom() {

    Wallet q1, q2;
    q1 = Wallet(1); // OK
//    q1 = q2; // błąd kompilacji

    Wallet r1(1), r2(2);
    //r1 + r2; // błąd kompilacji
    //Wallet suma1 = r1 + Wallet(1); // błąd kompilacji
    Wallet suma2 = Wallet(2) + r2; // OK
    assert(r2.getUnits() == 0);
    assert(r2.opSize() == 2);
    assert(suma2.getUnits() == 4 * UNITS_IN_B);
    //Wallet suma3 = suma1 + suma2;  // błąd kompilacji
    Wallet suma4 = Wallet(1) + Wallet(2);  // OK
    // assert(suma4.opSize() == 2);
    assert(suma4.getUnits() == 3 * UNITS_IN_B);

    Wallet s1, s2;
    bool b;

    s1 += Wallet(3);
    s1 *= 3;
    b = 2 < s2;
    Wallet suma5 = 2 + s2;
    Wallet suma6 = Wallet(1) + 2;

    Wallet t1(10000000), t2(10000000);
//    Wallet t3(2000000); // exception

    Wallet u1(1), u2(2);
    u1 += 1;
    u2 += Wallet(1);
    u1 += 0;

    // u2 += 1 += Wallet(1); // wrong
    Wallet u3(std::move(u1), std::move(u2));

    assert(u3.opSize() == 6);
    assert(u3.getUnits() == 5 * UNITS_IN_B);

    Wallet x1(1), x2(2);
    assert(x1 < x2);
    assert(x1 <= x2);
    assert(x1 <= x1);
    assert(x1 != x2);
    assert(x2 >= x1);
    assert(x2 >= x2);
    assert(x2 > x1);

    assert(Wallet(1) < x2);
    assert(Wallet(1) <= x2);
    assert(Wallet(1) <= Wallet(1));
    assert(Wallet(1) != x2);
    assert(x2 >= Wallet(1));
    assert(x2 >= x2);
    assert(x2 > Wallet(1));

    assert(Wallet(1) < Wallet(2));
    assert(Wallet(1) <= Wallet(2));
    assert(Wallet(1) <= Wallet(1));
    assert(Wallet(1) != Wallet(2));
    assert(Wallet(2) >= Wallet(1));
    assert(Wallet(2) >= Wallet(2));
    assert(Wallet(2) > Wallet(1));

    assert(Empty() == 0);
    assert(Empty() < Wallet(1));
    assert(Empty().getUnits() == 0);
    assert(Empty().opSize() == 1);
    assert(Empty()[0].getUnits() == 0);

    Wallet w1;
    assert(w1 == Empty());
    assert(w1.opSize() == 1);

    Wallet w2(1), w3(1);
    assert(w2.opSize() == 1);
    assert(w2.getUnits() == UNITS_IN_B);
    assert(w2 == 1);
    assert(1 == w2);
    assert(w2 == w3);

    assert(Wallet::fromBinary("100") == Wallet(4));
    std::string str(" 1.2000 ");
    char a[7] = {'1', '.', '2', '0', '0', '0', '\0'};
    assert(Wallet(" 1.2000 ") == Wallet("1,2"));
    assert(Wallet(" 1.2000 ") == Wallet(a));
    assert(Wallet(str) == Wallet("1,2"));

    assert(Wallet(2) + w2 == Wallet(3));
    assert(Wallet(1) + Wallet(2) == Wallet(3));
    assert(1 + 2 == Wallet(3));

    Wallet w4;
    assert(w4.opSize() == 1);

    w4 += 2;
    assert(w4.opSize() == 2);
    assert(w4[0] <= w4[1]);
    assert(w4[1].getUnits() == w4.getUnits());

    w4 *= 3;
    assert(w4.opSize() == 3);
    assert(w4[2].getUnits() == 6 * UNITS_IN_B);

    assert(Wallet(move(w4)).opSize() == 4);

    Wallet w5{2};
    Wallet w7 = Wallet(10) - w5;
    assert(w5 == Wallet(4));
    assert(w7 == Wallet(8));
}