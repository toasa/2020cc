#!/bin/bash

test() {
    expected=$1
    input=$2

    ./main "$input" > tmp.s
    cc tmp.s extern_funcs.o -o tmp
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

test 0 "main() { 0; }"
test 30 "main() { 30; }"

test 50 "main() { 25 + 25; }"
test 6 "main() { 1+2+3; }"

test 20 "main() { 40-20; }"

test 10 "main() { 10+40-10-30; }"
test 20 "main() {   10 + 11- 1 ; }"

test 20 "main() { 4 * 5; }"
test 23 "main() { 3 + 4 * 5; }"
test 17 "main() { 3 * 4 + 5; }"
test 27 "main() { 3 * (4 + 5); }"
test 77 "main() { (3 + 4) * (5 + 6); }"

test 8 "main() { 56 / 7; }"
test 25 "main() { 20 + 10 / 2; }"
test 4 "main() { 20 / 10 + 2; }"

test 3 "main() { -3 + 6; }"
test 13 "main() { -(3+10) * -1; }"
test 15 "main() { -(-3*5); }"

test 1 "main() { 10 == 10; }"
test 0 "main() { 5 == 30 / 10; }"
test 1 "main() { 5 != (30/10); }"
test 0 "main() { (3*4+5) != 17; }"
test 1 "main() { (1 == 1) != 0; }"

test 1 "main() { 10 <= 10; }"
test 0 "main() { 10 < 10; }"
test 0 "main() { 10 > 10; }"
test 1 "main() { 10 >= 10; }"

test 3 "main() { a = 3; a; }"
test 22 "main() { b = 5 * 6 - 8; b; }"
test 4 "main() { a = 2; b = a + 30; b / 8; }"
test 40 "main() { a = 20; b = a + (10 * 2); b; }"
test 30 "main() { a = 30; b = 11; a; }"
test 30 "main() { a = 30; b = 11; c = b - 10; d = a * (b - c); d / 10; }"

test 20 "main() { abc = 20; abc; }"
test 20 "main() { foo = 30; bar = 20; (foo - bar) * 2; }"

test 20 "main() { return 20; }"
test 5 "main() { return 5; return 4; }"

test 20 "main() { if (1) 20; }"
test 20 "main() { if (1) return 20; }"
test 20 "main() { if (0+1) return 20; }"
test 30 "main() { if (0) return 20; return 30; }"

test 20 "main() { if (1) return 20; else return 30; }"
test 30 "main() { if (0) return 20; else return 30; }"

test 20 "main() { x = 1; if (x) return 20; else return 30; }"
test 30 "main() { x = 0; if (x) return 20; else return 30; }"

test 11 "main() { i = 0; while (i <= 10) i = i + 1; i; }"

test 32 "main() { for (i = 20; i < 30; i = i + 1) i = i + 5; i; }"

test 11 "main() {
i = 0;
while (i <= 10)
    i = i + 1;
i;
}"

test 55 "main() {
i = 0;
sum = 0;
while (i <= 10) {
    sum = sum + i;
    i = i + 1;
}
sum;
}"

test 1 "main() {
i = 0;
{
    i = i + 1;
}
i;
}"

test 41 "main() {
i = 0;
a = 20;
{
    i = i + 1;
    i = i + 1;
    a = a * i;
    a = a + 1;
}
a;
}"

./main "main() { foo(); }" > tmp.s
cc tmp.s extern_funcs.o -o tmp
./tmp

test 21 "main() {
return bar(1, 2, 3, 4, 5, 6);
}"

test 222 "ret222() {
    return 222;
}

main() {
    return ret222();
}
"

test 15 "
main() {
    return add(7, 8);
}

add(x, y) {
    return x + y;
}
"

test 97 "
sub(x, y) {
    return x - y;
}

main() {
    return sub(100, 3);
}
"

test 55 "
fib(n) {
    if (n <= 2) {
        return 1;
    }
    return fib(n - 1) + fib(n - 2);
}

main() {
    return fib(10);
}
"

test 10 "
f(n) {
    return n;
}

main() {
    return f(10);
}
"

test 12 "
f(n1, n2) {
    return n1 + n2;
}

main() {
    return f(7, 5);
}
"

test 16 "
f(n1, n2, n3) {
    return n1 + n2 + n3;
}

main() {
    return f(7, 5, 4);
}
"

test 18 "
f(n1, n2, n3, n4) {
    return n1 + n2 + n3 + n4;
}

main() {
    return f(7, 5, 4, 2);
}
"

test 19 "
f(n1, n2, n3, n4, n5) {
    return n1 + n2 + n3 + n4 + n5;
}

main() {
    return f(7, 5, 4, 2, 1);
}
"

test 25 "
f(n1, n2, n3, n4, n5, n6) {
    return n1 + n2 + n3 + n4 + n5 + n6;
}

main() {
    return f(7, 5, 4, 2, 1, 6);
}
"

echo "OK"

