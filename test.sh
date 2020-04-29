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

test 0 "0;"
test 30 "30;"

test 50 "25 + 25;"
test 6 "1+2+3;"

test 20 "40-20;"

test 10 "10+40-10-30;"
test 20 "  10 + 11- 1 ;"

test 20 "4 * 5;"
test 23 "3 + 4 * 5;"
test 17 "3 * 4 + 5;"
test 27 "3 * (4 + 5);"
test 77 "(3 + 4) * (5 + 6);"

test 8 "56 / 7;"
test 25 "20 + 10 / 2;"
test 4 "20 / 10 + 2;"

test 3 "-3 + 6;"
test 13 "-(3+10) * -1;"
test 15 "-(-3*5);"

test 1 "10 == 10;"
test 0 "5 == 30 / 10;"
test 1 "5 != (30/10);"
test 0 "(3*4+5) != 17;"
test 1 "(1 == 1) != 0;"

test 1 "10 <= 10;"
test 0 "10 < 10;"
test 0 "10 > 10;"
test 1 "10 >= 10;"

test 3 "a = 3; a;"
test 22 "b = 5 * 6 - 8; b;"
test 4 "a = 2; b = a + 30; b / 8;"
test 40 "a = 20; b = a + (10 * 2); b;"
test 30 "a = 30; b = 11; a;"
test 30 "a = 30; b = 11; c = b - 10; d = a * (b - c); d / 10;"
test 4 "a = 4;"

test 20 "abc = 20; abc;"
test 20 "foo = 30; bar = 20; (foo - bar) * 2;"

test 20 "return 20;"
test 5 "return 5; return 4;"

test 20 "if (1) 20;"
test 20 "if (1) return 20;"
test 20 "if (0+1) return 20;"
test 30 "if (0) return 20; return 30;"

test 20 "if (1) return 20; else return 30;"
test 30 "if (0) return 20; else return 30;"

test 20 "x = 1; if (x) return 20; else return 30;"
test 30 "x = 0; if (x) return 20; else return 30;"

test 11 "i = 0; while (i <= 10) i = i + 1; i;"

test 32 "for (i = 20; i < 30; i = i + 1) i = i + 5; i;"

test 11 "i = 0;
while (i <= 10)
    i = i + 1;
i;"

test 55 "i = 0;
sum = 0;
while (i <= 10) {
    sum = sum + i;
    i = i + 1;
}
sum;"

test 2 "i = 0;
{
    i = i + 1;
    i = i + 1;
}
i;"

./main "foo();" > tmp.s
cc tmp.s foo.o -o tmp
./tmp

echo "OK"
