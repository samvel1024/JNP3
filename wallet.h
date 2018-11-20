#ifndef JNP3_WALLET_H
#define JNP3_WALLET_H

#include <vector>
#include <ostream>
#include <chrono>
#include <algorithm>
#include <regex>
#include <exception>
#include <iomanip>

using number = long long int;

class WalletOperation {

private:
    number units;
    std::chrono::milliseconds performed;
    std::string date_str;

    const std::string performed_date(time_t time) const;

public:
    WalletOperation(number);

    number getUnits() const;

    bool operator==(const WalletOperation &) const;

    bool operator<(const WalletOperation &) const;

    bool operator!=(const WalletOperation &) const;

    bool operator<=(const WalletOperation &) const;

    bool operator>(const WalletOperation &) const;

    bool operator>=(const WalletOperation &) const;

    friend std::ostream &operator<<(std::ostream &, const WalletOperation &);
};

class Wallet {

private:
    std::vector<WalletOperation> operations;
    number units;
    static number TOTAL_B_UNITS;

    void create_and_add(number);

    void add_operation(number);

    void check_for_underflow(number, number);

    void check_for_overflow(number);

public:
    template<typename T>
    Wallet(T n) = delete;

    Wallet();

    Wallet(int);

    Wallet(number);

    explicit Wallet(const std::string &);

    explicit Wallet(const char *);

    explicit Wallet(char *);

    Wallet(Wallet &&);

    Wallet(Wallet &&, Wallet &&);

    ~Wallet();

    number getUnits() const;

    size_t opSize() const;

    Wallet operator=(Wallet &&);

    Wallet &operator-=(Wallet &&);

    Wallet &operator+=(Wallet &&);

    Wallet &operator+=(Wallet &);

    Wallet &operator-=(Wallet &);

    Wallet &operator+=(number);

    Wallet &operator-=(number);

    Wallet &operator*=(number);

    const WalletOperation &operator[](size_t) const;

    static Wallet fromBinary(const std::string &);
};

std::ostream &operator<<(std::ostream &, const Wallet &);

Wallet operator*(Wallet &, number);

Wallet operator*(number, Wallet &);

Wallet operator+(Wallet &&, Wallet &&);

Wallet operator+(Wallet &&, Wallet &);

Wallet operator-(Wallet &&, Wallet &&);

Wallet operator-(Wallet &&, Wallet &);

bool operator==(const Wallet &, const Wallet &);

bool operator!=(const Wallet &, const Wallet &);

bool operator<=(const Wallet &, const Wallet &);

bool operator>=(const Wallet &, const Wallet &);

bool operator<(const Wallet &, const Wallet &);

bool operator>(const Wallet &, const Wallet &);

const Wallet &Empty();

#endif
