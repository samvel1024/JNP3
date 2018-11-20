#!/bin/sh
# first argument $1 - number of test cases in err_test.cc
# second argument $2 - number of test cases in err_runtime.cc

echo "Testing compilation"
for f in $(seq 1 $1); do
    echo "-DERR$f";
    g++ -Wall -Wextra -O2 -std=c++17 -DERR$f wallet.cc wallet.h err_test.cc > /dev/null 2>&1;
    out=$?;
    if [ "$out" -eq "0" ]; then
        echo "Test failed";
    else
        echo "Test OK"
    fi;
done;

echo "Testing runtime"
for f in $(seq 1 $2); do
    echo "-DERR$f";
    g++ -Wall -Wextra -O2 -std=c++17 -DERR$f wallet.cc wallet.h err_runtime.cc -o run > /dev/null 2>&1;
    out=$?;
    if [ "$out" -ne "0" ]; then
        echo "Test failed";
    else
        echo "Compilation OK"
    fi;

    ./run > /dev/null 2>&1
    out=$?;
    if [ "$out" -eq "0" ]; then
        echo "Test failed";
    else
        echo "Test OK"
    fi;
done;