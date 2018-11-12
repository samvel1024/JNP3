


//Sandbox test

class MyClass {
    int val;


public:
    MyClass(int val) : val(val) {
    }

    MyClass() {

    }

    MyClass &operator=(const MyClass &rhs) {

        // Only do assignment if RHS is a different object from this.

        if (this != &rhs) {
            this->val = rhs.val;
        }

        return *this;
    }
};

int main() {
    MyClass a1(1);
    MyClass a3(3);
     a3 = a1;
}
