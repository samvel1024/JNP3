#include "wallet.h"

#include <vector>
#include <ostream>
#include <chrono>
#include <algorithm>
#include <regex>
#include <exception>
#include <iomanip>


const number UNITS_IN_B = 100000000ll;
const number MAX_TOTAL_B_UNITS = 21000000ll * UNITS_IN_B;
using namespace std::chrono;

WalletOperation::WalletOperation(number u) : units(u) {
    auto time_point = std::chrono::system_clock::now();
    performed = duration_cast<milliseconds>(
            time_point.time_since_epoch()
    );
    date_str = performed_date(
            std::chrono::system_clock::to_time_t(time_point)
    );
}

number WalletOperation::getUnits() const {
    return units;
};

bool WalletOperation::operator==(const WalletOperation &other) const {
    return performed == other.performed;
}

bool WalletOperation::operator<(const WalletOperation &other) const {
    return performed < other.performed;
}

bool WalletOperation::operator!=(const WalletOperation &other) const {
    return !(*this == other);
}

bool WalletOperation::operator<=(const WalletOperation &other) const {
    return (*this < other) || (*this == other);
}

bool WalletOperation::operator>(const WalletOperation &other) const {
    return (*this != other) && !(*this < other);
}

bool WalletOperation::operator>=(const WalletOperation &other) const {
    return (*this > other) || (*this == other);
}

std::ostream &operator<<(std::ostream &os, const WalletOperation &dt) {
    os << "Wallet balance is " << dt.units
       << " B after operation made at day " << dt.date_str;
    return os;
}

const std::string WalletOperation::performed_date(time_t time) const {
    // it would be much easier to use date.h library
    // but the specification disallows any dependencies
    std::tm *ptm = std::localtime(&time);
    const int DATE_LEN = 11;
    char buffer[DATE_LEN];
    std::strftime(buffer, DATE_LEN, "%Y-%m-%d", ptm);
    return std::string(buffer);
}

void Wallet::add_operation(number n) {
    this->operations.emplace_back(n);
}

void Wallet::check_for_overflow(number n) {
    if (MAX_TOTAL_B_UNITS - Wallet::TOTAL_B_UNITS < n) {
        throw std::overflow_error("Total number of B exceeded");
    }
}

void Wallet::check_for_underflow(number balance, number n) {
    if (0 > balance + n) {
        throw std::underflow_error("Wallets cannot have less than 0 units");
    }
}

void Wallet::create_and_add(number n) {
    check_for_underflow(0, n);
    check_for_overflow(n);
    Wallet::TOTAL_B_UNITS += n;
    units = n;
    this->add_operation(n);
}

void Wallet::move_wallet(Wallet &&w) {
    this->operations = std::move(w.operations);
    this->units = w.units;
    w.units = 0;
    this->add_operation(units);
}

Wallet::Wallet() : Wallet(0ll) {}

Wallet::Wallet(int n) : Wallet((number) n) {}

Wallet::Wallet(number n) {
    this->create_and_add(n * UNITS_IN_B);
}

Wallet::Wallet(const std::string &s) {
    static const std::regex reg("\\s*[0-9]*[.,]?[0-9]{1,8}([eE][-+]?[0-9]+)?\\s*");
    if (!std::regex_match(s, reg)) {
        throw std::invalid_argument("Given string is not a valid number");
    }
    number n;
    if (s.find(',') != std::string::npos) {
        std::string s_period = s;
        std::replace(s_period.begin(), s_period.end(), ',', '.');
        n = (number) (stof(s_period) * UNITS_IN_B);
    } else {
        n = (number) (stof(s) * UNITS_IN_B);
    }
    this->create_and_add(n);
}

Wallet::Wallet(const char *s) : Wallet(std::string(s)) {}

Wallet::Wallet(char *s) : Wallet(std::string(s)) {}

Wallet::Wallet(Wallet &&w) {
    this->move_wallet(std::forward<Wallet>(w));
}

Wallet::Wallet(Wallet &&a, Wallet &&b) {
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
    this->create_and_add(units);
}

Wallet::~Wallet() {
    Wallet::TOTAL_B_UNITS -= this->units;
}

number Wallet::getUnits() const {
    return this->units;
}

size_t Wallet::opSize() const {
    return this->operations.size();
}

Wallet &Wallet::operator=(Wallet &&rhs) {
    this->move_wallet(std::forward<Wallet>(rhs));
    return *this;
}

Wallet &Wallet::operator-=(Wallet &&rhs) {
    *this -= rhs;
    return *this;
}

Wallet &Wallet::operator+=(Wallet &&rhs) {
    *this += rhs;
    return *this;
}

Wallet &Wallet::operator-=(number rhs) {
    *this += (-rhs);
    return *this;
}

Wallet &Wallet::operator+=(number rhs) {
    number units = rhs * UNITS_IN_B;
    check_for_underflow(this->units, units);
    check_for_overflow(units);
    this->units += units;
    this->add_operation(this->units);
    return *this;
}

Wallet &Wallet::operator+=(Wallet &rhs) {
    // the ordering of these is important!
    // as we have to care not to exceed the Bs global limit
    number bs = rhs.getUnits() / UNITS_IN_B;
    rhs -= bs;
    *this += bs;
    return *this;
}

Wallet &Wallet::operator-=(Wallet &rhs) {
    // the ordering of these is important!
    // as we have to care not to exceed the Bs global limit
    // if rhs > *this, there will be exception thrown
    number bs = rhs.getUnits() / UNITS_IN_B;
    *this -= bs;
    rhs += bs;
    return *this;
}

Wallet &Wallet::operator*=(number rhs) {
    number to_be_added = (rhs * units - units) / UNITS_IN_B;
    *this += to_be_added;
    return *this;
}

const WalletOperation &Wallet::operator[](size_t x) const {
    return operations[x];
}

Wallet Wallet::fromBinary(const std::string &val) {
    static const std::regex reg("\\s*(0|1)+\\s*");
    if (!std::regex_match(val, reg)) {
        throw std::invalid_argument("Given string is not valid binary");
    }
    return Wallet(stoi(val, nullptr, 2));
}

number Wallet::TOTAL_B_UNITS = 0;

std::ostream &operator<<(std::ostream &os, const Wallet &dt) {
    os << "Wallet[" << dt.getUnits() / UNITS_IN_B << " B]";
    return os;
}

Wallet operator*(Wallet &lhs, number rhs) {
    rhs *= lhs.getUnits() / UNITS_IN_B;
    return Wallet(rhs);
}

Wallet operator*(number lhs, Wallet &rhs) {
    lhs *= rhs.getUnits() / UNITS_IN_B;
    return Wallet(lhs);
}

Wallet operator+(Wallet &&lhs, Wallet &&rhs) {
    return Wallet(std::forward<Wallet>(lhs), std::forward<Wallet>(rhs));
}

Wallet operator+(Wallet &&lhs, Wallet &rhs) {
    number units = rhs.getUnits() / UNITS_IN_B;
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
    auto w = Wallet(std::forward<Wallet>(lhs));
    w -= units;
    rhs += units;
    return w;
}

bool operator==(const Wallet &lhs, const Wallet &rhs) {
    return lhs.getUnits() == rhs.getUnits();
}

bool operator<(const Wallet &lhs, const Wallet &rhs) {
    return lhs.getUnits() < rhs.getUnits();
}

bool operator!=(const Wallet &lhs, const Wallet &rhs) {
    return !(lhs == rhs);
}

bool operator<=(const Wallet &lhs, const Wallet &rhs) {
    return (lhs < rhs) || (lhs == rhs);
}

bool operator>(const Wallet &lhs, const Wallet &rhs) {
    return (lhs != rhs) && !(lhs < rhs);
}

bool operator>=(const Wallet &lhs, const Wallet &rhs) {
    return (lhs > rhs) || (lhs == rhs);
}

const Wallet &Empty() {
    static const Wallet empty = Wallet();
    return empty;
}