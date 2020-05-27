#!/bin/bash

test() {
    expected=$1
    input=$2

    ./main "$input" > tmp.s
    cc -no-pie tmp.s extern_funcs.o -o tmp
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

test 110 "
int main() {
    return add(100, 8 + 2);
}

int add(int x, int y) {
    return x + y;
}
"

test 30 "
int main() {
    return add(7 + 6/ 2, 8 + 2 + (20 - 10));
}

int add(int x, int y) {
    return x + y;
}
"

test 60 "
int main() {
    // return f(7+(6/2), 1+1+1+1+1+1+1+1+1+1, (30-25)*2, (2 + 3) * (4/2), 10, 60/6);
    //                                                   ~~~~~~~~~~~~~~~~~~~~~~~~~
    //                                                           これだめ？(第四引数以降）
    return f(7+(6/2), 1+1+1+1+1+1+1+1+1+1, (30-25)*2, 10,10, 10);
}

int f(int x1, int x2, int x3, int x4, int x5, int x6) {
    return x1+x2+x3+x4+x5+x6;
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

test 8 "
int main() {
    int ***ptr;
    return sizeof(ptr);
}
"

test 4 "
int main() {
    int *ptr;
    return sizeof(*ptr);
}
"

test 4 "
int main() {
    int x = 20;
    int *y = &x;
    int **z = &y;
    int ***ptr = &z;
    return sizeof(***ptr);
}
"

test 40 "
int main() {
    int arr[10];
    return sizeof(arr);
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

test 5 "
int main() {
    int arr[10];
    int *p1 = arr;
    int *p2 = arr + 5;
    return p2 - p1;
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

test 30 "
int main() {
    int a[3];
    a[0] = 10;
    a[1] = 20;
    a[2] = 30;
    return 2[a];
}
"

test 30 "
int main() {
    int a[3];
    0[a] = 10;
    1[a] = 20;
    2[a] = 30;
    return 2[a];
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

test 11 "int main() { int x; x = 10; ++x; return x; }"
test 9 "int main() { int x; x = 10; --x; return x; }"
test 11 "int main() { int x; int y; x = 10; y = ++x; return y; }"
test 9 "int main() { int x; int y; x = 10; y = --x; return y; }"
test 0 "int main() { int x; int y; x = 10; y = ++x; return x - y; }"

test 11 "int main() { int x; x = 10; x++; return x; }"
test 9 "int main() { int x; x = 10; x--; return x; }"
test 10 "int main() { int x; int y; x = 10; y = x++; return y; }"
test 10 "int main() { int x; int y; x = 10; y = x--; return y; }"
test 1 "int main() { int x; int y; x = 10; y = x++; return x - y; }"

test 40 "
int main() {
    int a[3];
    a[0] = 10;
    a[1] = 20;
    a[2] = 30;

    int *p;
    p = a;
    p++;
    *p = 40;
    return a[1];
}
"

test 10 "int main() { int a = 10; return a; }"
test 11 "int main() { int a = 20; int b = 20; return a - b + 11; }"

test 40 "int main() { int arr[4] = {10, 20, 30, 40}; return arr[3]; }"
test 40 "
int main() {
    int a = 20;
    int arr[3] = {1, 2, 3};
    return a * arr[1];
}
"

test 40 "int main() { int arr[] = {10, 20, 30, 40}; return arr[3]; }"
test 40 "
int main() {
    int a = 20;
    int arr[] = {1, 2, 3};
    return a * arr[1];
}
"

test 60 "
int main() {
    int a = 20;
    int b = 30;
    int arr[] = {a, b, a * b};
    return arr[2] / (arr[1] - arr[0]);
}
"

test 10 "
int main() {
    for (int i = 0; i < 10; i++) {
    }
    return i;
}
"

test 10 "
int main() {
    int i = 0;
    for (; i < 10; i++) {
    }
    return i;
}
"

test 0 "int x; int main() { return x; }"
test 3 "int x; int main() { x=3; return x; }"
test 7 "int x; int y; int main() { x=3; y=4; return x+y; }"
test 3 "int x; int main() { int x = 3; return x; }"
test 0 "int x[4]; int main() { x[0]=0; x[1]=1; x[2]=2; x[3]=3; return x[0]; }"
test 1 "int x[4]; int main() { x[0]=0; x[1]=1; x[2]=2; x[3]=3; return x[1]; }"
test 2 "int x[4]; int main() { x[0]=0; x[1]=1; x[2]=2; x[3]=3; return x[2]; }"
test 3 "int x[4]; int main() { x[0]=0; x[1]=1; x[2]=2; x[3]=3; return x[3]; }"
test 4 "int x; int main() { return sizeof(x); }"
test 16 "int x[4]; int main() { return sizeof(x); }"

test 1 "int main() { char x = 1; return x; }"
test 1 "int main() { char x = 1; char y = 2; return x; }"
test 2 "int main() { char x = 1; char y = 2; return y; }"
test 1 "int main() { char x; return sizeof(x); }"
test 10 "int main() { char x[10]; return sizeof(x); }"
test 100 "int main() { char arr[3] = { 1, 10, 111 }; return arr[2] - arr[1] - arr[0]; }"
test 3 "
int main() {
    char arr[3] = { -1, 2, 111 };
    int y = 4;
    return arr[0] + y;
}"
test 4 "int main() { return sub_char(7, 1, 2); } int sub_char(char a, char b, char c) { return a-b-c; }"

test 97 'int main() { return "abc"[0]; }'
test 98 'int main() { return "abc"[1]; }'
test 99 'int main() { return "abc"[2]; }'
test 0 'int main() { return "abc"[3]; }'
test 4 'int main() { return sizeof("abc"); }'

test 7 'int main() { return "\a"[0]; }'
test 8 'int main() { return "\b"[0]; }'
test 9 'int main() { return "\t"[0]; }'
test 10 'int main() { return "\n"[0]; }'
test 11 'int main() { return "\v"[0]; }'
test 12 'int main() { return "\f"[0]; }'
test 13 'int main() { return "\r"[0]; }'
test 27 'int main() { return "\e"[0]; }'

test 106 'int main() { return "\j"[0]; }'
test 107 'int main() { return "\k"[0]; }'
test 108 'int main() { return "\l"[0]; }'

test 0 'int main() { return ({ 0; }); }'
test 2 'int main() { return ({ 0; 1; 2; }); }'
test 1 'int main() { return ({ 0; return 1; 2; }); return 3; }'
test 30 'int main() { return ({ 10; }) + 20; }'
test 6 'int main() { return ({ 1; }) + ({ 2; }) + ({ 3; }); }'
test 3 'int main() { return ({ int x=3; x; }); }'

test 2 "int main() { /* return 1; */ return 2; }"
test 2 "
int main() {
    // return 1;
    return 2;
}"

test 2 "int main() { int x = 2; { int x = 3; } return x;}"
test 2 "int main() { int x = 2; { int x = 3; } { int y = 4; return x; }}"
test 3 "int main() { int x = 2; { x = 3; } return x;}"

echo "OK"
