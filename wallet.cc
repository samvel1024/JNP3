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

const number UNITS_IN_B = 100000000;
using number = unsigned int;


class WalletOperation {
    using namespace std::chrono;

    public:
        WalletOperation(number u) : units(u) {
            // some assert here?
            performed = duration_cast<miliseconds>(
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
            return !(this == other);
        }

        bool operator<(const WalletOperation &other) const {
            return performed < other.performed;
        }

        bool operator<=(const WalletOperation &other) const {
            return (this < other) || !(this == other);
        }

        bool operator>(const WalletOperation &other) const {
            return (this != other) && !(this < other);
        }

        bool operator>=(const WalletOperation &other) const {
            return (this > other) || (this == other);
        }

        friend std::ostream &operator<<(std::ostream &os, const WalletOperation &dt);

    private:
        number units;
        miliseconds performed;
};

class Wallet {
    private:
        std::vector<WalletOperation> operations;
        number units;

        void create_and_add(number n) {
            // TODO check if it is less than the total number of B allowed
            operations = new std::vector<WalletOperation>();
            add_operation(n);
        }

        void add_operation(number n) {
            operations.push_back(new WalletOperation(n));
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
                std::make_move_iterator(b.operations.end()),
            );
            std::sort(operations.begin(), operations.end());
        }

        int getUnits() const {
            return units;
        }

        int opSize() const {
            return operations.size();
        }


        bool operator==(const Wallet &other) const {
            return units == other.units;
        }

        bool operator!=(const Wallet &other) const {
            return !(this == other);
        }

        bool operator<(const Wallet &other) const {
            return units < other.units;
        }

        bool operator<=(const Wallet &other) const {
            return (this < other) || (this == other);
        }

        bool operator>(const Wallet &other) const {
            return !(this < other) && (this != other);
        }

        bool operator>=(const Wallet &other) const {
            return (this > other) || (this == other);
        }

        Wallet &operator=(Wallet &&rhs) {
            // return value optimization should kick in here
            // TODO ask about that during lab
            return Wallet(rhs);
        }

        Wallet &operator+=(const Wallet &rhs);

        explicit Wallet &operator+=(number rhs);

        explicit Wallet &operator*=(number rhs);

        const Wallet operator+(const Wallet &other) const;

        const Wallet operator-(const Wallet &other) const;

        explicit const Wallet operator*(number rhs) const;

        const WalletOperation &operator[](size_t x) const {
            // TODO will it really return const reference?
            return operations[k];
        }

        friend std::ostream &operator<<(std::ostream &os, const Wallet &dt);

        friend bool operator==(const int &lhs, const Wallet &rhs);

        static const Wallet &fromBinary(const std::string &val) {
            create_and_add(stoi(val, nullptr, 2));
        }
};


const Wallet &Empty();

#endif
