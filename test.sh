#!/bin/bash

test() {
    expected=$1
    input=$2

    clang main.c -o tmp
    ./tmp ${input} > tmp.s
    clang tmp.s -o main
    ./main
    result=$?

    if [ ${expected} -eq ${result} ]
    then
        echo "${input} => ${result}"
    else
        echo "expected ${expected}, but got ${result}"
        exit 1
    fi
}

test 0 0
test 30 30
echo "OK"
