#ifndef JNP3_WALLET_H
#define JNP3_WALLET_H

#include <vector>
#include <ostream>
#include <chrono>
#include <algorithm>
#include <regex>


#include <iostream> // TODO remove that, to debug only
#include <cassert>
#include <iomanip>

/*
 * Side notes for implementation
 *    Check for self-assignment in assignment operations
 *    Define your binary (+) arithmetic operators using your compound assignment operators (+=)
 *    Double check if there is no copy constructor/copy assignment and delete if there is
 *    Double check that we are not repeating adding history operations anywhere
 *    Double check that we are not multiplying UNITS_IN_B by UNITS_IN_B (it happens very often...)
 */


using number = int;
const number UNITS_IN_B = 100000000;


class WalletOperation {

public:
    WalletOperation(number u) : units(u) {
        performed = std::time(nullptr);
        performed_str = performed_date(performed);
    }

    int getUnits() const {
        return units;
    };

    bool operator==(const WalletOperation &other) const {
        return performed == other.performed;
    }

    bool operator!=(const WalletOperation &other) const {
        return !operator==(other);
    }

    bool operator<(const WalletOperation &other) const {
        return performed < other.performed;
    }

    bool operator<=(const WalletOperation &other) const {
        return operator<(other) || operator==(other);
    }

    bool operator>(const WalletOperation &other) const {
        return operator!=(other) && !operator<(other);
    }

    bool operator>=(const WalletOperation &other) const {
        return operator>(other) || operator==(other);
    }

    friend std::ostream &operator<<(std::ostream &os, const WalletOperation &dt) {
        os << "Wallet balance is " << dt.units
           << " B after operation made at day " << dt.performed_str << std::endl;
        return os;
    }

private:
    number units;
    time_t performed;
    std::string performed_str;

    const std::string performed_date(time_t time) const {
        std::tm *ptm = std::localtime(&time);
        char buffer[11];
        std::strftime(buffer, 11, "%Y-%m-%d", ptm);
        return std::string(buffer);
    }
};

class Wallet {
private:
    std::vector<WalletOperation> operations;
    number units;

    static number TOTAL_B_UNITS;

    void create_and_add(number n) {
        units = n;
        // TODO check if it is less than the total number of B allowed
        add_operation(n);
    }

    void add_operation(number n) {
        operations.emplace_back(n);
    }

public:
    Wallet() {
        create_and_add(0);
    }

    explicit Wallet(number n) {
        create_and_add(n * UNITS_IN_B);
    }

    Wallet(std::string s) {
        const std::regex reg("\\s*[-+]?[0-9]*[.,]?[0-9]{1,8}([eE][-+]?[0-9]+)?\\s*");
        //TODO find out if assert is a good practice in C++
        assert(std::regex_match(s, reg));
        number n;
        if (s.find(',') != std::string::npos) {
            std::string s_period = s;
            std::replace(s_period.begin(), s_period.end(), ',', '.');
            n = stof(s_period) * UNITS_IN_B;
        } else {
            n = stof(s) * UNITS_IN_B;
        }
        create_and_add(n);
    }

    Wallet(Wallet &&w) {
        operations = std::move(w.operations);
        units = w.units;
        w.units = 0;
        add_operation(units);
    }

    Wallet(Wallet &&a, Wallet &&b) {
        units = a.units + b.units;
        a.units = 0;
        b.units = 0;

        operations = std::move(a.operations);
        operations.reserve(operations.size() + b.operations.size());

        // TODO write mergesort manually
        operations.insert(
            operations.end(),
            std::make_move_iterator(b.operations.begin()),
            std::make_move_iterator(b.operations.end())
        );
        std::sort(operations.begin(), operations.end());
        create_and_add(units);
    }

    ~Wallet() {

    }

    int getUnits() const {
        return units;
    }

    size_t opSize() const {
        return static_cast<int>(operations.size());
    }

    bool operator==(const Wallet &other) const {
        return units == other.units;
    }

    bool operator==(number other) const {
        return units == other * UNITS_IN_B;
    }

    bool operator<(const Wallet &other) const {
        return units < other.units;
    }

    bool operator!=(const Wallet &other) const {
        return !operator==(other);
    }

    bool operator<=(const Wallet &other) const {
        return operator<(other) || operator==(other);
    }

    bool operator>(const Wallet &other) const {
        return operator!=(other) && !operator<(other);
    }

    bool operator>=(const Wallet &other) const {
        return operator>(other) || operator==(other);
    }

    Wallet operator=(Wallet &&rhs) {
        // return value optimization should kick in here
        // TODO ask about that during lab
        return Wallet(std::forward<Wallet>(rhs));
    }

    Wallet &operator+=(Wallet &rhs) {
        // the ordering of these is important!
        // as we have to care not to exceed the Bs global limit
        // TODO remember to check adding history here
        rhs -= rhs.units;
        (*this) += rhs.units;
        return (*this);
    }

    Wallet &operator-=(Wallet &rhs) {
        // the ordering of these is important!
        // TODO remember to check adding history here
        *(this) -= rhs.units;
        rhs += rhs.units;
        return (*this);
    }


    Wallet &operator-=(number rhs) {
        *(this) += (-rhs);
        return (*this);
    }

    Wallet &operator+=(number rhs) {
        this->units += (rhs * UNITS_IN_B);
        this->add_operation(this->units);
        return (*this);
    }

    Wallet &operator+=(Wallet &&rhs) {
        //TODO
        return (*this);
    }


    Wallet &operator*=(number rhs) {
        number to_be_added = (rhs * units - units) / UNITS_IN_B;
        *(this) += to_be_added;
        return (*this);
    }

    const WalletOperation &operator[](size_t x) const {
        // TODO will it really return const reference?
        return operations[x];
    }

    friend std::ostream &operator<<(std::ostream &os, const Wallet &dt) {
        os << "Wallet[" << dt.units << " B]" << std::endl;
        return os;
    }

    static Wallet fromBinary(const std::string &val) {
        return Wallet(stoi(val, nullptr, 2));
    }
};


Wallet operator*(Wallet &lhs, number rhs) {
    rhs *= (lhs.getUnits() / UNITS_IN_B);
    return Wallet(rhs);
}

Wallet operator*(number lhs, Wallet &rhs) {
    lhs *= (rhs.getUnits() / UNITS_IN_B);
    return Wallet(lhs);
}


Wallet operator+(Wallet &&lhs, Wallet &&rhs) {
    return Wallet(std::forward<Wallet>(lhs), std::forward<Wallet>(rhs));
}

Wallet operator+(Wallet &&lhs, number rhs) {
    //TODO
    Wallet w(2);
    return w;
}


Wallet operator+(Wallet &&lhs, Wallet &rhs) {
    number units = (rhs.getUnits() / UNITS_IN_B);
    rhs -= units;
    auto w = Wallet(std::forward<Wallet>(lhs));
    w += units;
    return w;
}

Wallet operator-(Wallet &&lhs, Wallet &&rhs) {
    auto w = Wallet(std::forward<Wallet>(lhs));
    w -= (rhs.getUnits() / UNITS_IN_B);
    return w;
}

Wallet operator-(Wallet &&lhs, Wallet &rhs) {
    number units = rhs.getUnits() / UNITS_IN_B;
    rhs += units;
    auto w = Wallet(std::forward<Wallet>(lhs));
    w -= units;
    return w;
}

bool operator==(number lhs, const Wallet &rhs) {
    return lhs * UNITS_IN_B == rhs.getUnits();
}

bool operator<(number lhs, const Wallet &rhs) {
    return lhs < rhs.getUnits();
}

bool operator!=(number lhs, const Wallet &rhs) {
    return !operator==(lhs, rhs);
}

bool operator<=(number lhs, const Wallet &rhs) {
    return operator<(lhs, rhs) || operator==(lhs, rhs);
}

bool operator>(number lhs, const Wallet &rhs) {
    return operator!=(lhs, rhs) && !operator<(lhs, rhs);
}

bool operator>=(number lhs, const Wallet &rhs) {
    return operator>(lhs, rhs) || operator==(lhs, rhs);
}


const Wallet Empty() {
    // TODO empty wallet should be non-modifiable
    return Wallet();
}

#endif
