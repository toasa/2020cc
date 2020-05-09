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

test 0 "int main() { 0; }"
test 30 "int main() { 30; }"

test 50 "int main() { 25 + 25; }"
test 6 "int main() { 1+2+3; }"

test 20 "int main() { 40-20; }"

test 10 "int main() { 10+40-10-30; }"
test 20 "int main() {   10 + 11- 1 ; }"

test 20 "int main() { 4 * 5; }"
test 23 "int main() { 3 + 4 * 5; }"
test 17 "int main() { 3 * 4 + 5; }"
test 27 "int main() { 3 * (4 + 5); }"
test 77 "int main() { (3 + 4) * (5 + 6); }"

test 8 "int main() { 56 / 7; }"
test 25 "int main() { 20 + 10 / 2; }"
test 4 "int main() { 20 / 10 + 2; }"

test 3 "int main() { -3 + 6; }"
test 13 "int main() { -(3+10) * -1; }"
test 15 "int main() { -(-3*5); }"

test 1 "int main() { 10 == 10; }"
test 0 "int main() { 5 == 30 / 10; }"
test 1 "int main() { 5 != (30/10); }"
test 0 "int main() { (3*4+5) != 17; }"
test 1 "int main() { (1 == 1) != 0; }"

test 1 "int main() { 10 <= 10; }"
test 0 "int main() { 10 < 10; }"
test 0 "int main() { 10 > 10; }"
test 1 "int main() { 10 >= 10; }"

test 3 "int main() { int a; a = 3; a; }"
test 22 "int main() { int b; b = 5 * 6 - 8; b; }"
test 4 "int main() { int a; int b; a = 2; b = a + 30; b / 8; }"
test 40 "int main() { int a; a = 20; int b; b = a + (10 * 2); b; }"
test 30 "int main() { int a; a = 30; int b; b = 11; a; }"
test 30 "int main() { int a; a = 30; int b; b = 11; int c; c = b - 10; int d; d = a * (b - c); d / 10; }"

test 20 "int main() { int abc; abc = 20; abc; }"
test 20 "int main() { int foo; int bar; foo = 30; bar = 20; (foo - bar) * 2; }"

test 20 "int main() { return 20; }"
test 5 "int main() { return 5; return 4; }"

test 20 "int main() { if (1) 20; }"
test 20 "int main() { if (1) return 20; }"
test 20 "int main() { if (0+1) return 20; }"
test 30 "int main() { if (0) return 20; return 30; }"

test 20 "int main() { if (1) return 20; else return 30; }"
test 30 "int main() { if (0) return 20; else return 30; }"

test 20 "int main() { int x; x = 1; if (x) return 20; else return 30; }"
test 30 "int main() { int x; x = 0; if (x) return 20; else return 30; }"

test 11 "int main() { int i; i = 0; while (i <= 10) i = i + 1; i; }"

test 32 "int main() { int i; for (i = 20; i < 30; i = i + 1) i = i + 5; i; }"

test 11 "int main() {
int i;
i = 0;
while (i <= 10)
    i = i + 1;
i;
}"

test 55 "int main() {
int i;
int sum;
i = 0;
sum = 0;
while (i <= 10) {
    sum = sum + i;
    i = i + 1;
}
sum;
}"

test 1 "int main() {
int i;
i = 0;
{
    i = i + 1;
}
i;
}"

test 41 "int main() {
int i;
int a;
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

./main "int main() { foo(); }" > tmp.s
cc tmp.s extern_funcs.o -o tmp
./tmp

test 21 "int main() {
return bar(1, 2, 3, 4, 5, 6);
}"

test 222 "int ret222() {
    return 222;
}

int main() {
    return ret222();
}
"

test 15 "
int main() {
    return add(7, 8);
}

int add(int x, int y) {
    return x + y;
}
"

test 97 "
int sub(int x, int y) {
    return x - y;
}

int main() {
    return sub(100, 3);
}
"

test 55 "
int fib(int n) {
    if (n <= 2) {
        return 1;
    }
    return fib(n - 1) + fib(n - 2);
}

int main() {
    return fib(10);
}
"

test 10 "
int f(int n) {
    return n;
}

int main() {
    return f(10);
}
"

test 12 "
int f(int n1, int n2) {
    return n1 + n2;
}

int main() {
    return f(7, 5);
}
"

test 16 "
int f(int n1, int n2, int n3) {
    return n1 + n2 + n3;
}

int main() {
    return f(7, 5, 4);
}
"

test 18 "
int f(int n1, int n2, int n3, int n4) {
    return n1 + n2 + n3 + n4;
}

int main() {
    return f(7, 5, 4, 2);
}
"

test 19 "
int f(int n1, int n2, int n3, int n4, int n5) {
    return n1 + n2 + n3 + n4 + n5;
}

int main() {
    return f(7, 5, 4, 2, 1);
}
"

test 25 "
int f(int n1, int n2, int n3, int n4, int n5, int n6) {
    return n1 + n2 + n3 + n4 + n5 + n6;
}

int main() {
    return f(7, 5, 4, 2, 1, 6);
}
"

test 3 "
int main() {
    int x;
    int *y;
    x = 3;
    y = &x;
    return *y;
}
"

test 5 "
int main() {
    int x;
    int *y;
    x = 3;
    y = &x;
    *y = 5;
    return x;
}
"

test 3 "
int main() {
    int x;
    int y;
    x = 3;
    y = 5;
    return *(&y + 1);
}"

test 5 "
int main() {
    int x;
    int y;
    x = 3;
    y = 5;
    return *(&x - 1);
}"

test 3 "
int main() {
    int x;
    int y;
    int *z;
    x = 3;
    y = 5;
    z = &y;
    return *(z + 1);
}
"

test 3 "
int main() {
    int x;
    int y;
    int *z;
    x = 3;
    y = 5;
    z = &y + 1;
    return *z;
}
"

test 222 "
int main() {
    int x;
    int *y;
    y = &x;
    *y = 222;
    return x;
}
"

test 7 "
int main() {
    int x;
    int y;
    x = 3;
    y = 5;
    *(&x - 1) = 7;
    return y;
}"

test 7 "
int main() {
    int x;
    int y;
    x = 3;
    y = 5;
    *(&y + 1) = 7;
    return x;
}"

test 4 "
int main() {
    int x;
    return sizeof(x);
}
"

test 8 "
int main() {
    int *x;
    return sizeof(x);
}
"

test 4 "
int main() {
    int x;
    return sizeof(x + 3);
}
"

test 8 "
int main() {
    int *x;
    return sizeof(x + 3);
}
"

test 4 "
int main() {
    return sizeof(3);
}
"

test 4 "
int main() {
    return sizeof(3);
}
"

test 10 "
int main() {
    int a[3];
    *(a+0) = 10;
    *(a+1) = 20;
    *(a+2) = 30;
    return *(a+0);
}
"

test 20 "
int main() {
    int a[3];
    *(a+0) = 10;
    *(a+1) = 20;
    *(a+2) = 30;
    return *(a+1);
}
"

test 30 "
int main() {
    int a[3];
    *(a+0) = 10;
    *(a+1) = 20;
    *(a+2) = 30;
    return *(a+2);
}
"

test 12 "
int main() {
    int a[4];
    *(a+0) = 10;
    *(a+1) = 20;
    *(a+2) = 30;
    *(a+3) = 40;
    return *(a+3) / *(a+1) + *a;
}
"

test 3 "
int main() {
    int a[2];
    *a = 1;
    *(a + 1) = 2;
    int *p;
    p = a;
    return *p + *(p + 1);
}
"

test 31 "
int main() {
    int a;
    int b;
    int arr[2];
    int c;
    a = 1;
    *arr = 10;
    *(arr + 1) = 20;
    b = a + *arr;
    c = b + *(arr + 1);
    return c;
}
"

test 30 "
int main() {
    int a[3];
    int i;
    i = 2;
    *(a+0) = 10;
    *(a+1) = 20;
    *(a+2) = 30;
    return *(a+i);
}
"

test 10 "
int main() {
    int a[3];
    a[0] = 10;
    a[1] = 20;
    a[2] = 30;
    return a[0];
}
"

test 20 "
int main() {
    int a[3];
    a[0] = 10;
    a[1] = 20;
    a[2] = 30;
    return a[1];
}
"

test 30 "
int main() {
    int a[3];
    a[0] = 10;
    a[1] = 20;
    a[2] = 30;
    return a[2];
}
"

test 8 "
int main() {
    int a[3];
    a[0] = 4;
    a[1] = 20;
    a[2] = 3;
    return a[2] + a[1] / a[0];
}
"

test 20 "
int main() {
    int a[3];
    a[0] = 10;
    a[1] = 20;
    a[2] = 30;
    int i;
    i = 1;
    return a[i];
}
"

test 2 "int main() { return 100 % 7; }"

test 128 "int main() { return 1 << 7; }"
test 2 "int main() { return 128 >> 6; }"

test 64 "int main() { int x; x = 32; x += 32; return x; }"
test 30 "int main() { int x; x = 32; x -= 2; return x; }"
test 64 "int main() { int x; x = 32; x *= 2; return x; }"
test 8 "int main() { int x; x = 32; x /= 4; return x; }"
test 4 "int main() { int x; x = 32; x %= 7; return x; }"
test 16 "int main() { int x; x = 32; x >>= 1; return x; }"
test 64 "int main() { int x; x = 32; x <<= 1; return x; }"

echo "OK"
