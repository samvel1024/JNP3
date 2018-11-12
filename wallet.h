#ifndef JNP3_WALLET_H
#define JNP3_WALLET_H

#include <vector>
#include <ostream>

/*
 * Side notes for implementation
 *    Check for self-assignment in assignment operations
 *    Define your binary (+) arithmetic operators using your compound assignment operators (+=)
 */

class WalletOperation {
public:
    int getUnits() const;

    bool operator==(const WalletOperation &other) const;

    bool operator!=(const WalletOperation &other) const;

    bool operator<(const WalletOperation &other) const;

    bool operator<=(const WalletOperation &other) const;

    bool operator>(const WalletOperation &other) const;

    bool operator>=(const WalletOperation &other) const;

    friend std::ostream &operator<<(std::ostream &os, const WalletOperation &dt);

};

class Wallet {
private:
    std::vector<WalletOperation> operations;
public:
    Wallet();

    Wallet(int);

    Wallet(std::string);

    Wallet(Wallet &&w);

    Wallet(Wallet &&a, Wallet &&b);


    int getUnits() const;

    int opSize() const;


    Wallet &operator=(const Wallet &rhs);


    bool operator==(const Wallet &other) const;

    bool operator!=(const Wallet &other) const;

    bool operator<(const Wallet &other) const;

    bool operator<=(const Wallet &other) const;

    bool operator>(const Wallet &other) const;

    bool operator>=(const Wallet &other) const;

    Wallet &operator+=(const Wallet &rhs);

    Wallet &operator+=(const int &rhs);

    Wallet &operator*=(const int &rhs);


    const Wallet operator+(const Wallet &other) const;

    const Wallet operator-(const Wallet &other) const;

    const Wallet operator*(const Wallet &other) const;

    const WalletOperation &operator[](int x) const;

    friend std::ostream &operator<<(std::ostream &os, const Wallet &dt);

    friend bool operator==(const int &lhs, const Wallet &rhs);


    static const Wallet &fromBinary(const std::string &val);


};


const Wallet &Empty();

#endif
