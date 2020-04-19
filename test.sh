#!/bin/bash

test() {
    expected=$1
    input=$2

    ./main "$input" > tmp.s
    cc tmp.s -o tmp
    ./tmp
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
test 50 "25 + 25"
test 6 "1+2+3"
test 20 "40-20"
test 10 "10+40-10-30"
test 3 "2 + 1"
test 20 "  10 + 11- 1 "
test 20 "4 * 5"
test 23 "3 + 4 * 5"
test 17 "3 * 4 + 5"
test 27 "3 * (4 + 5)"
test 77 "(3 + 4) * (5 + 6)"
test 8 "56 / 7"
test 25 "20 + 10 / 2"
test 4 "20 / 10 + 2"
test 3 "-3 + 6"
test 13 "-(3+10) * -1"
test 15 "-(-3*5)"

echo "OK"
