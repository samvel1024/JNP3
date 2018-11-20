#ifndef JNP3_WALLET_H
#define JNP3_WALLET_H

#include <vector>
#include <ostream>
#include <chrono>
#include <algorithm>
#include <regex>
#include <exception>


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

// PRIVATE CONSTSS
using number = int;
const number UNITS_IN_B = 100000000;
using namespace std::chrono;


class WalletOperation {

public:
    WalletOperation(number u) : units(u) {
        // some assert here?

        auto time_point = std::chrono::system_clock::now();
        performed = duration_cast<milliseconds>(
            time_point.time_since_epoch()
        );
        date_str = performed_date(
            std::chrono::system_clock::to_time_t(time_point)
        );
    }

    int getUnits() const {
        return units;
    };

    bool operator==(const WalletOperation &other) const {
        return performed == other.performed;
    }

    bool operator<(const WalletOperation &other) const {
        return performed < other.performed;
    }

    bool operator!=(const WalletOperation &other) const {
        return !(*this == other);
    }

    bool operator<=(const WalletOperation &other) const {
        return (*this < other) || (*this == other);
    }

    bool operator>(const WalletOperation &other) const {
        return (*this != other) && !(*this < other);
    }

    bool operator>=(const WalletOperation &other) const {
        return (*this > other) || (*this == other);
    }

    friend std::ostream &operator<<(std::ostream &os, const WalletOperation &dt) {
        os << "Wallet balance is " << dt.units
           << " B after operation made at day " << dt.date_str;
        return os;
    }

private:
    number units;
    milliseconds performed;
    std::string date_str;

    const std::string performed_date(time_t time) const {
        // it would be much easier to use date.h library
        // but the specification disallows any dependencies
        std::tm *ptm = std::localtime(&time);
        const int DATE_LEN = 11;
        char buffer[DATE_LEN];
        std::strftime(buffer, DATE_LEN, "%Y-%m-%d", ptm);
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
    template<typename T> Wallet(T n) = delete;

    Wallet() : Wallet(0) {}

    Wallet(number n) {
        create_and_add(n * UNITS_IN_B);
    }

    Wallet(const std::string& s) {
        static const std::regex reg("\\s*[-+]?[0-9]*[.,]?[0-9]{1,8}([eE][-+]?[0-9]+)?\\s*");
        if(!std::regex_match(s, reg)){
            throw std::invalid_argument("Given string is not a valid number");
        }
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

    Wallet(const char* s) : Wallet(std::string(s)) {}

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
        return operations.size();
    }

    Wallet operator=(Wallet &&rhs) {
        return Wallet(std::forward<Wallet>(rhs));
    }

    Wallet &operator-=(Wallet&& rhs) {
        *this -= rhs;
        return *this;
    }

    Wallet &operator+=(Wallet&& rhs) {
        *this += rhs;
        return *this;
    }

    Wallet &operator+=(Wallet &rhs) {
        // the ordering of these is important!
        // as we have to care not to exceed the Bs global limit
        // TODO remember to check adding history here
        number units = rhs.units;
        rhs.units = 0;
        rhs.add_operation(rhs.units);

        this->units += units;
        add_operation(this->units);
        return *this;
    }

    Wallet &operator-=(Wallet &rhs) {
        // the ordering of these is important!
        // TODO remember to check adding history here
        this->units -= rhs.units;
        add_operation(this->units);

        rhs.units += rhs.units;
        rhs.add_operation(rhs.units);
        return *this;
    }

    Wallet& operator*=(number rhs) {
        number to_be_added = (rhs * units - units) / UNITS_IN_B;
        *this += to_be_added;
        return *this;
    }


    const WalletOperation &operator[](size_t x) const {
        return operations[x];
    }

    static Wallet fromBinary(const std::string &val) {
        static const std::regex reg("\\s*(0|1)+\\s*");
        if (!std::regex_match(val, reg)) {
            throw std::invalid_argument("Given string is not valid binary");
        }
        return Wallet(stoi(val, nullptr, 2));
    }
};

std::ostream &operator<<(std::ostream &os, const Wallet &dt) {
    os << "Wallet[" << dt.getUnits() / UNITS_IN_B << " B]";
    return os;
}

Wallet operator*(Wallet &lhs, number rhs) {
    rhs *= (lhs.getUnits() / UNITS_IN_B); // TODO style brackets
    return Wallet(rhs);
}

Wallet operator*(number lhs, Wallet &rhs) {
    lhs *= (rhs.getUnits() / UNITS_IN_B);
    return Wallet(lhs);
}

Wallet operator+(Wallet &&lhs, Wallet &&rhs) {
    return Wallet(std::forward<Wallet>(lhs), std::forward<Wallet>(rhs));
}

Wallet operator+(Wallet &&lhs, Wallet &rhs) {
    number units = (rhs.getUnits() / UNITS_IN_B);
    rhs -= units;
    auto w = Wallet(std::forward<Wallet>(lhs));
    w += units;
    return w;
}

Wallet operator-(Wallet &&lhs, Wallet &&rhs) {
    auto w = Wallet(std::forward<Wallet>(lhs)); // TODO warning - wrong order - to check
    w -= (rhs.getUnits() / UNITS_IN_B);
    return w;
}

Wallet operator-(Wallet &&lhs, Wallet &rhs) {
    number units = rhs.getUnits() / UNITS_IN_B;
    auto w = Wallet(std::forward<Wallet>(lhs));
    w -= units;
    rhs += units;
    return w;
}
bool operator==(const Wallet& lhs, const Wallet &rhs) {
    return lhs.getUnits() == rhs.getUnits();
}

bool operator<(const Wallet& lhs, const Wallet &rhs) {
     return lhs.getUnits() < rhs.getUnits();
}

bool operator!=(const Wallet& lhs, const Wallet &rhs) {
    return !(lhs == rhs);
}

bool operator<=(const Wallet& lhs, const Wallet &rhs) {
    return (lhs < rhs) || (lhs == rhs);
}

bool operator>(const Wallet& lhs, const Wallet &rhs) {
    return (lhs != rhs) && !(lhs < rhs);
}

bool operator>=(const Wallet& lhs, const Wallet &rhs) {
    return operator>(lhs, rhs) || operator==(lhs, rhs);
}

const Wallet &Empty() {
    static const Wallet empty = Wallet();
    return empty;
}

#endif
