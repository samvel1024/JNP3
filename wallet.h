#ifndef JNP3_WALLET_H
#define JNP3_WALLET_H

#include <vector>
#include <ostream>
#include <chrono>
#include <algorithm>
/*
 * Side notes for implementation
 *    Check for self-assignment in assignment operations
 *    Define your binary (+) arithmetic operators using your compound assignment operators (+=)
 *    Double check if there is no copy constructor/copy assignment and delete if there is
 */


using namespace std::chrono;
using number = unsigned int;
const number UNITS_IN_B = 100000000;


class WalletOperation {

public:
    WalletOperation(number u) : units(u) {
        // some assert here?
        performed = duration_cast<milliseconds>(
            system_clock::now().time_since_epoch()
        );
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
           << " B after operation made at day "; // TODO some way to convert millis to
        return os;
    }

private:
    number units;
    milliseconds performed;
};

class Wallet {
private:
    std::vector<WalletOperation> operations;
    number units;

    void create_and_add(number n) {
        // TODO check if it is less than the total number of B allowed
        add_operation(n);
    }

    void add_operation(number n) {
        operations.emplace_back(n);
    }

public:
    Wallet() : units(0) {
        create_and_add(0);
    }

    explicit Wallet(number n) : units(n) {
        create_and_add(n * UNITS_IN_B);
    }

    Wallet(std::string s) {
        // TODO assert that it has max 8 places after comma
        // TODO assert that it is a correct number (eg not "abcd")
        // TODO can we modify string s? then it would be much less complicated
        number n;
        if (s.find(',') != std::string::npos) {
            std::string s_period = s;
            std::replace(s_period.begin(), s_period.end(), ',', '.');
            n = stof(s_period);
        } else {
            n = stof(s);
        }

        create_and_add(n * UNITS_IN_B);
    }

    Wallet(Wallet &w){
        Wallet();//TODO Impl copy constructor
    }

    Wallet(Wallet &&w) {
        operations = std::move(w.operations);
        units = w.units;
        w.units = 0;

        number X = 0; //TODO what should be there?
        add_operation(X);
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
    }

    int getUnits() const {
        return units;
    }

    int opSize() const {
        return static_cast<int>(operations.size());
    }


    bool operator==(const Wallet &other) const {
        return units == other.units;
    }

    bool operator==(const number num) const {
        return units == num;
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

    Wallet &operator=(Wallet &&rhs) {
        // return value optimization should kick in here
        // TODO ask about that during lab
//        return Wallet(rhs);
        return *this; // Just to compile
    }

    Wallet &operator+=(Wallet &rhs) {
        *(this) += rhs.units;
        rhs -= rhs.units;
        return *this;
    }

    Wallet &operator-=(Wallet &rhs) {
        *(this) -= rhs.units;
        //TODO not exactly what we need
        return *this;
    }

    Wallet &operator-=(number rhs) {
        *(this) += (-rhs);
        return *(this);
    }

    Wallet &operator+=(number rhs) {
        this->units += rhs;
        this->add_operation(rhs);
        return *this;
    }

    Wallet &operator*=(number rhs) {
        number to_be_added = rhs * units - units;
        *(this) += rhs;
        return *this;
    }

    Wallet &operator+(Wallet &other) {
        *(this) += other;
        return *(this);
    }


    Wallet &operator+(Wallet &&other) {
        //TODO probably wrong, needed for supporting Wallet(1) + Wallet(2)
        *(this) += other;
        return *(this);
    }


    Wallet &operator-(Wallet &other) {
        //TODO Odejmowanie, analogicznie jak dodawanie, ale po odejmowaniu w w2 jest dwa razy więcej jednostek, niż było w w2 przed odejmowaniem.
        *(this) -= other;
        return *(this);
    }

    //TODO why explicit ? It failed to compile xD
    const Wallet &operator*(number rhs) {
        *(this) *= rhs;
        return *this;
    };

    const WalletOperation &operator[](size_t x) const {
        // TODO will it really return const reference?
        return operations[x];
    }

    friend std::ostream &operator<<(std::ostream &os, const Wallet &dt) {
        os << "Wallet[" << dt.units << " B]" << std::endl;
        return os;
    }

    friend bool operator==(const int &lhs, const Wallet &rhs) {
        return lhs == rhs.units;
    }

    static const Wallet fromBinary(const std::string &val) {
        return Wallet(val);
    }
};


const Wallet Empty() {
    return Wallet();
}

#endif
