int print_func() {
    printf("Howdy? toasa!\n");
}

int add_func(int x1, int x2, int x3, int x4, int x5, int x6) {
    return x1 + x2 + x3 + x4 + x5 + x6;
}

int ret222() {
    return 222;
}

int test(int expected, int actual, char *code) {
    if (expected == actual) {
        printf("%s => %d\n", code, expected);
    } else {
        printf("%s => %d expected, but got %d\n", code, expected, actual);
        exit(1);
    }
}

int id(int n) {
    return n;
}

int add2(int x1, int x2) {
    return x1+x2;
}

int add3(int x1, int x2, int x3) {
    return x1+x2+x3;
}

int add4(int x1, int x2, int x3, int x4) {
    return x1+x2+x3+x4;
}

int add5(int x1, int x2, int x3, int x4, int x5) {
    return x1+x2+x3+x4+x5;
}

int add6(int x1, int x2, int x3, int x4, int x5, int x6) {
    return x1+x2+x3+x4+x5+x6;
}

int sub(int x, int y) {
    return x - y;
}

int fib(int n) {
    if (n <= 2) {
        return 1;
    }
    return fib(n - 1) + fib(n - 2);
}

int sub_char(char a, char b, char c){
    return a-b-c;
}

int sub_short(short a, short b, short c) {
    return a - b - c;
}

int sub_long(long a, long b, long c) {
    return a - b - c;
}

int hehehe();
int hahaha();

int g1;
int g2;
int garr[4];

int *g1_ptr() { return &g1; }
char int_to_char(int x) { return x; }
int char_to_int(char c) { return c; }

int div_long(long a, long b) {
    return a / b;
}

_Bool bool_fn_add(_Bool x) { return x + 1; }
_Bool bool_fn_sub(_Bool x) { return x - 1; }

typedef int MyInt, MyInt2[4];

int main() {
    test(0, 0, "0");
    test(30, 30, "30");
    test(-1, -1, "-1");
    test(-4294967296, -4294967296, "-4294967296");
    test(4294967295, 4294967295, "4294967295");

    test(50, 25+25, "25 + 25");
    test(6, 1+2+3, "1+2+3");
    test(20, 40-20, "40-20");

    test(10, 10+40-10-30, "10+40-10-30");
    test(20, 10 + 11- 1, "10 + 11- 1");
    test(20, 4 * 5, "4 * 5");
    test(23, 3 + 4 * 5, "3 + 4 * 5");
    test(17, 3 * 4 + 5, "3 * 4 + 5");
    test(27, 3 * (4 + 5), "3 * (4 + 5)");
    test(77, (3 + 4) * (5 + 6), "(3 + 4) * (5 + 6)");

    test(8, 56 / 7, "56 / 7");
    test(25, 20 + 10 / 2, "20 + 10 / 2");
    test(4, 20 / 10 + 2, "20 / 10 + 2");

    test(3, -3 + 6, "-3 + 6");
    test(13, -(3+10) * -1, "-(3+10) * -1");
    test(15, -(-3*5), "-(-3*5)");

    test(1, 10 == 10, "10 == 10");
    test(0, 5 == 30 / 10, "5 == 30 / 10");
    test(1, 5 != (30/10), "5 != (30/10)");
    test(0, (3*4+5) != 17, "(3*4+5) != 17");
    test(1, (1 == 1) != 0, "(1 == 1) != 0");

    test(1, 10 <= 10, "10 <= 10");
    test(0, 10 < 10, "10 < 10");
    test(0, 10 > 10, "10 > 10");
    test(1, 10 >= 10, "10 >= 10");

    test(3, ({ int a; a = 3; a; }), "({ int a; a = 3; a; })");
    test(22, ({ int b; b = 5 * 6 - 8; b; }), "({ int b; b = 5 * 6 - 8; b; })");
    test(4, ({ int a; int b; a = 2; b = a + 30; b / 8; }), "({ int a; int b; a = 2; b = a + 30; b / 8; })");
    test(40, ({ int a; a = 20; int b; b = a + (10 * 2); b; }), "({ int a; a = 20; int b; b = a + (10 * 2); b; })");
    test(30, ({ int a; a = 30; int b; b = 11; a; }), "({ int a; a = 30; int b; b = 11; a; })");
    test(30, ({ int a; a = 30; int b; b = 11; int c; c = b - 10; int d; d = a * (b - c); d / 10; }), "({ int a; a = 30; int b; b = 11; int c; c = b - 10; int d; d = a * (b - c); d / 10; })");
    test(20, ({ int abc; abc = 20; abc; }), "({ int abc; abc = 20; abc; })");
    test(20, ({ int foo; int bar; foo = 30; bar = 20; (foo - bar) * 2; }), "({ int foo; int bar; foo = 30; bar = 20; (foo - bar) * 2; })");

    test(8, ({ int x, y; x=3; y=5; x+y; }), "({ int x, y; x=3; y=5; x+y; })");

    test(20, ({ if (1) 20; }), "({ if (1) 20; })");
    test(20, ({ if (0+1) 20; }), "({ if (0+1) 20; })");
    test(30, ({ if (0) 20; 30; }), "({ if (0) 20; 30; })");
    test(20, ({ if (1) 20; else 30; }), "({ if (1) 20; else 30; })");
    test(30, ({ if (0) 20; else 30; }), "({ if (0) 20; else 30; })");
    test(20, ({ int x; x = 1; if (x) 20; else 30; }), "({ int x; x = 1; if (x) 20; else 30; })");
    test(30, ({ int x; x = 0; if (x) 20; else 30; }), "({ int x; x = 0; if (x) 20; else 30; })");

    test(11, ({ int i; i = 0; while (i <= 10) i = i + 1; i; }), "({ int i; i = 0; while (i <= 10) i = i + 1; i; })");
    test(55, ({ int i; int sum; i = 0; sum = 0; while (i <= 10) { sum = sum + i; i = i + 1; } sum; }), "{ int i; int sum; i = 0; sum = 0; while (i <= 10) { sum = sum + i; i = i + 1; } sum; }");

    test(32, ({ int i; for (i = 20; i < 30; i = i + 1) i = i + 5; i; }), "{ int i; for (i = 20; i < 30; i = i + 1) i = i + 5; i; }");
    test(10, ({ for (int i = 0; i < 10; i++) {} i; }), "({ for (int i = 0; i < 10; i++) {} i; })");
    test(10, ({ int i = 0; for (; i < 10; i++) {} i; }), "({ int i = 0; for (; i < 10; i++) {} i; })");

    test(1, ({ int i; i = 0; { i = i + 1; } i; }), "({ int i; i = 0; { i = i + 1; } i; })");
    test(41, ({ int i; int a; i = 0; a = 20; { i = i + 1; i = i + 1; a = a * i; a = a + 1; } a; }), "{ int i; int a; i = 0; a = 20; { i = i + 1; i = i + 1; a = a * i; a = a + 1; } a; }");

    print_func();
    test(222, ret222(), "ret222()");
    test(15, add2(7, 8), "add2(7, 8)");
    test(16, add3(7, 5, 4), "add3(7, 5, 4)");
    test(18, add4(7, 5, 4, 2), "add4(7, 5, 4, 2)");
    test(19, add5(7, 5, 4, 2, 1), "add5(7, 5, 4, 2, 1)");
    test(21, add6(1, 2, 3, 4, 5, 6), "add6(1, 2, 3, 4, 5, 6)");
    test(97, sub(100, 3), "sub(100, 3)");
    test(55, fib(10), "fib(10)");
    test(10, id(10), "id(10)");
    test(10, id(id(id(id(10)))), "id(id(id(id(10))))");

    test(3, ({ int x; int *y; x = 3; y = &x; *y; }), "({ int x; int *y; x = 3; y = &x; *y; })");
    test(5, ({ int x; int *y; x = 3; y = &x; *y = 5; x; }), "({ int x; int *y; x = 3; y = &x; *y = 5; x; })");
    test(3, ({ int x; int y; x = 3; y = 5; *(&y + 1); }), "({ int x; int y; x = 3; y = 5; *(&y + 1); })");
    test(5, ({ int x; int y; x = 3; y = 5; *(&x - 1); }), "({ int x; int y; x = 3; y = 5; *(&x - 1); })");
    test(3, ({ int x; int y; int *z; x = 3; y = 5; z = &y; *(z + 1); }), "({ int x; int y; int *z; x = 3; y = 5; z = &y; *(z + 1); })");
    test(3, ({ int x; int y; int *z; x = 3; y = 5; z = &y + 1; *z; }), "({ int x; int y; int *z; x = 3; y = 5; z = &y + 1; *z; })");
    test(222, ({ int x; int *y; y = &x; *y = 222; x; }), "({ int x; int *y; y = &x; *y = 222; x; })");
    test(7, ({ int x; int y; x = 3; y = 5; *(&x - 1) = 7; y; }), "({ int x; int y; x = 3; y = 5; *(&x - 1) = 7; y; })");
    test(7, ({ int x; int y; x = 3; y = 5; *(&y + 1) = 7; x; }), "({ int x; int y; x = 3; y = 5; *(&y + 1) = 7; x; })");

    test(4, ({ int x; sizeof(x); }), "({ int x; sizeof(x); })");
    test(4, ({ int x; sizeof x ; }), "({ int x; sizeof x ; })");
    test(8, ({ int *x; sizeof(x); }), "({ int *x; sizeof(x); })");
    test(4, ({ int x; sizeof(x + 3); }), "({ int x; sizeof(x + 3); })");
    test(8, ({ int *x; sizeof(x + 3); }), "({ int *x; sizeof(x + 3); })");
    test(4, sizeof(3), "sizeof(3)");
    test(8, ({ int ***ptr; sizeof(ptr); }), "({ int ***ptr; sizeof(ptr); })");
    test(4, ({ int *ptr; sizeof(*ptr); }), "({ int *ptr; sizeof(*ptr); })");
    test(4, ({ int x = 20; int *y = &x; int **z = &y; int ***ptr = &z; sizeof(***ptr); }), "({ int x = 20; int *y = &x; int **z = &y; int ***ptr = &z; sizeof(***ptr); })");
    test(40, ({ int arr[10]; sizeof(arr); }), "({ int arr[10]; sizeof(arr); })");
    test(120, ({ int arr[10][3]; sizeof(arr); }), "({ int arr[10][3]; sizeof(arr); })");
    test(12, ({ int arr[10][3]; sizeof(*arr); }), "({ int arr[10][3]; sizeof(*arr); })");
    test(4, ({ int arr[10][3]; sizeof(**arr); }), "({ int arr[10][3]; sizeof(**arr); })");
    test(5, ({ int arr[10][3]; sizeof(**arr) + 1; }), "({ int arr[10][3]; sizeof(**arr) + 1; })");
    test(5, ({ int arr[10][3]; sizeof **arr  + 1; }), "({ int arr[10][3]; sizeof **arr  + 1; })");
    test(4, ({ int arr[10][3]; sizeof(**arr + 1); }), "({ int arr[10][3]; sizeof(**arr + 1); })");

    test(10, ({ int a[3]; *(a+0) = 10; *(a+1) = 20; *(a+2) = 30; *(a+0); }), "({ int a[3]; *(a+0) = 10; *(a+1) = 20; *(a+2) = 30; *(a+0); })");
    test(20, ({ int a[3]; *(a+0) = 10; *(a+1) = 20; *(a+2) = 30; *(a+1); }), "({ int a[3]; *(a+0) = 10; *(a+1) = 20; *(a+2) = 30; *(a+1); })");
    test(30, ({ int a[3]; *(a+0) = 10; *(a+1) = 20; *(a+2) = 30; *(a+2); }), "({ int a[3]; *(a+0) = 10; *(a+1) = 20; *(a+2) = 30; *(a+2); })");
    test(12, ({ int a[4]; *(a+0) = 10; *(a+1) = 20; *(a+2) = 30; *(a+3) = 40; *(a+3) / *(a+1) + *a; }), "({ int a[4]; *(a+0) = 10; *(a+1) = 20; *(a+2) = 30; *(a+3) = 40; *(a+3) / *(a+1) + *a; })");
    test(3, ({ int a[2]; *a = 1; *(a + 1) = 2; int *p; p = a; *p + *(p + 1); }), "({ int a[2]; *a = 1; *(a + 1) = 2; int *p; p = a; *p + *(p + 1); })");

    test(31, ({ int a; int b; int arr[2]; int c; a = 1; *arr = 10; *(arr + 1) = 20; b = a + *arr; c = b + *(arr + 1); c; }), "({ int a; int b; int arr[2]; int c; a = 1; *arr = 10; *(arr + 1) = 20; b = a + *arr; c = b + *(arr + 1); c; })");
    test(30, ({ int a[3]; int i; i = 2; *(a+0) = 10; *(a+1) = 20; *(a+2) = 30; *(a+i); }), "({ int a[3]; int i; i = 2; *(a+0) = 10; *(a+1) = 20; *(a+2) = 30; *(a+i); })");
    test(5, ({ int arr[10]; int *p1 = arr; int *p2 = arr + 5; p2 - p1; }), "({ int arr[10]; int *p1 = arr; int *p2 = arr + 5; p2 - p1; })");

    test(10, ({ int a[3]; a[0] = 10; a[1] = 20; a[2] = 30; a[0]; }), "({ int a[3]; a[0] = 10; a[1] = 20; a[2] = 30; a[0]; })");
    test(20, ({ int a[3]; a[0] = 10; a[1] = 20; a[2] = 30; a[1]; }), "({ int a[3]; a[0] = 10; a[1] = 20; a[2] = 30; a[1]; })");
    test(30, ({ int a[3]; a[0] = 10; a[1] = 20; a[2] = 30; a[2]; }), "({ int a[3]; a[0] = 10; a[1] = 20; a[2] = 30; a[2]; })");

    test(8, ({ int a[3]; a[0] = 4; a[1] = 20; a[2] = 3; a[2] + a[1] / a[0]; }), "({ int a[3]; a[0] = 4; a[1] = 20; a[2] = 3; a[2] + a[1] / a[0]; })");
    test(20, ({ int a[3]; a[0] = 10; a[1] = 20; a[2] = 30; int i; i = 1; a[i]; }), "({ int a[3]; a[0] = 10; a[1] = 20; a[2] = 30; int i; i = 1; a[i]; })");

    test(30, ({ int a[3]; a[0] = 10; a[1] = 20; a[2] = 30; 2[a]; }), "({ int a[3]; a[0] = 10; a[1] = 20; a[2] = 30; 2[a]; })");
    test(30, ({ int a[3]; 0[a] = 10; 1[a] = 20; 2[a] = 30; 2[a]; }), "({ int a[3]; 0[a] = 10; 1[a] = 20; 2[a] = 30; 2[a]; })");

    test(0, ({ int x[2][3]; int *y=x; *y=0; **x; }), "({ int x[2][3]; int *y=x; *y=0; **x; })");
    test(1, ({ int x[2][3]; int *y=x; *(y+1)=1; *(*x+1); }), "({ int x[2][3]; int *y=x; *(y+1)=1; *(*x+1); })");
    test(2, ({ int x[2][3]; int *y=x; *(y+2)=2; *(*x+2); }), "({ int x[2][3]; int *y=x; *(y+2)=2; *(*x+2); })");
    test(3, ({ int x[2][3]; int *y=x; *(y+3)=3; **(x+1); }), "({ int x[2][3]; int *y=x; *(y+3)=3; **(x+1); })");
    test(4, ({ int x[2][3]; int *y=x; *(y+4)=4; *(*(x+1)+1); }), "({ int x[2][3]; int *y=x; *(y+4)=4; *(*(x+1)+1); })");
    test(5, ({ int x[2][3]; int *y=x; *(y+5)=5; *(*(x+1)+2); }), "({ int x[2][3]; int *y=x; *(y+5)=5; *(*(x+1)+2); })");
    test(6, ({ int x[2][3]; int *y=x; *(y+6)=6; **(x+2); }), "({ int x[2][3]; int *y=x; *(y+6)=6; **(x+2); })");

    test(0, ({ int x[2][3]; int *y=x; y[0]=0; x[0][0]; }), "({ int x[2][3]; int *y=x; y[0]=0; x[0][0]; })");
    test(1, ({ int x[2][3]; int *y=x; y[1]=1; x[0][1]; }), "({ int x[2][3]; int *y=x; y[1]=1; x[0][1]; })");
    test(2, ({ int x[2][3]; int *y=x; y[2]=2; x[0][2]; }), "({ int x[2][3]; int *y=x; y[2]=2; x[0][2]; })");
    test(3, ({ int x[2][3]; int *y=x; y[3]=3; x[1][0]; }), "({ int x[2][3]; int *y=x; y[3]=3; x[1][0]; })");
    test(4, ({ int x[2][3]; int *y=x; y[4]=4; x[1][1]; }), "({ int x[2][3]; int *y=x; y[4]=4; x[1][1]; })");
    test(5, ({ int x[2][3]; int *y=x; y[5]=5; x[1][2]; }), "({ int x[2][3]; int *y=x; y[5]=5; x[1][2]; })");
    test(6, ({ int x[2][3]; int *y=x; y[6]=6; x[2][0]; }), "({ int x[2][3]; int *y=x; y[6]=6; x[2][0]; })");

    test(2, 100 % 7, "100 % 7");

    test(128, 1 << 7, "1 << 7");
    test(2, 128 >> 6, "128 >> 6");

    test(64, ({ int x; x = 32; x += 32; x; }), "({ int x; x = 32; x += 32; x; })");
    test(30, ({ int x; x = 32; x -= 2; x; }), "({ int x; x = 32; x -= 2; x; })");
    test(64, ({ int x; x = 32; x *= 2; x; }), "({ int x; x = 32; x *= 2; x; })");
    test(8, ({ int x; x = 32; x /= 4; x; }), "({ int x; x = 32; x /= 4; x; })");
    test(4, ({ int x; x = 32; x %= 7; x; }), "({ int x; x = 32; x %= 7; x; })");
    test(16, ({ int x; x = 32; x >>= 1; x; }), "({ int x; x = 32; x >>= 1; x; })");
    test(64, ({ int x; x = 32; x <<= 1; x; }), "({ int x; x = 32; x <<= 1; x; })");

    test(11, ({ int x; x = 10; ++x; x; }), "({ int x; x = 10; ++x; x; })");
    test(9, ({ int x; x = 10; --x; x; }), "({ int x; x = 10; --x; x; })");
    test(11, ({ int x; int y; x = 10; y = ++x; y; }), "({ int x; int y; x = 10; y = ++x; y; })");
    test(9, ({ int x; int y; x = 10; y = --x; y; }), "({ int x; int y; x = 10; y = --x; y; })");
    test(0, ({ int x; int y; x = 10; y = ++x; x - y; }), "({ int x; int y; x = 10; y = ++x; x - y; })");
    test(11, ({ int x; x = 10; x++; x; }), "({ int x; x = 10; x++; x; })");
    test(9, ({ int x; x = 10; x--; x; }), "({ int x; x = 10; x--; x; })");
    test(10, ({ int x; int y; x = 10; y = x++; y; }), "({ int x; int y; x = 10; y = x++; y; })");
    test(10, ({ int x; int y; x = 10; y = x--; y; }), "({ int x; int y; x = 10; y = x--; y; })");
    test(1, ({ int x; int y; x = 10; y = x++; x - y; }), "({ int x; int y; x = 10; y = x++; x - y; })");

    test(40, ({ int a[3]; a[0] = 10; a[1] = 20; a[2] = 30; int *p; p = a; p++; *p = 40; a[1]; }), "({ int a[3]; a[0] = 10; a[1] = 20; a[2] = 30; int *p; p = a; p++; *p = 40; a[1]; })");

    test(10, ({ int a = 10; a; }), "({ int a = 10; a; })");
    test(11, ({ int a = 20; int b = 20; a - b + 11; }), "({ int a = 20; int b = 20; a - b + 11; })");

    test(10, ({ int arr[4] = {10, 20, 30, 40}; arr[0]; }), "({ int arr[4] = {10, 20, 30, 40}; arr[0]; })");
    test(20, ({ int arr[4] = {10, 20, 30, 40}; arr[1]; }), "({ int arr[4] = {10, 20, 30, 40}; arr[1]; })");
    test(30, ({ int arr[4] = {10, 20, 30, 40}; arr[2]; }), "({ int arr[4] = {10, 20, 30, 40}; arr[2]; })");
    test(40, ({ int arr[4] = {10, 20, 30, 40}; arr[3]; }), "({ int arr[4] = {10, 20, 30, 40}; arr[3]; })");
    test(40, ({ int a = 20; int arr[3] = {1, 2, 3}; a * arr[1]; }), "({ int a = 20; int arr[3] = {1, 2, 3}; a * arr[1]; })");

    test(10, ({ int arr[] = {10, 20, 30, 40}; arr[0]; }), "({ int arr[] = {10, 20, 30, 40}; arr[0]; })");
    test(20, ({ int arr[] = {10, 20, 30, 40}; arr[1]; }), "({ int arr[] = {10, 20, 30, 40}; arr[1]; })");
    test(30, ({ int arr[] = {10, 20, 30, 40}; arr[2]; }), "({ int arr[] = {10, 20, 30, 40}; arr[2]; })");
    test(40, ({ int arr[] = {10, 20, 30, 40}; arr[3]; }), "({ int arr[] = {10, 20, 30, 40}; arr[3]; })");
    test(40, ({ int a = 20; int arr[] = {1, 2, 3}; a * arr[1]; }), "({ int a = 20; int arr[] = {1, 2, 3}; a * arr[1]; })");
    test(60, ({ int a = 20; int b = 30; int arr[] = {a, b, a * b}; arr[2] / (arr[1] - arr[0]); }), "({ int a = 20; int b = 30; int arr[] = {a, b, a * b}; arr[2] / (arr[1] - arr[0]); })");

    test(0, g1, "g1;");
    test(3, ({ g1=3; g1; }), "({ g1=3; g1; })");
    test(7, ({ g1=3; g2=4; g1+g2; }), "({ g1=3; g2=4; g1+g2; })");
    test(3, ({ int g1 = 3; g1; }), "({ int g1 = 3; g1; })");
    test(0, ({ garr[0]=0; garr[1]=1; garr[2]=2; garr[3]=3; garr[0]; }), "({ garr[0]=0; garr[1]=1; garr[2]=2; garr[3]=3; garr[0]; })");
    test(1, ({ garr[0]=0; garr[1]=1; garr[2]=2; garr[3]=3; garr[1]; }), "({ garr[0]=0; garr[1]=1; garr[2]=2; garr[3]=3; garr[1]; })");
    test(2, ({ garr[0]=0; garr[1]=1; garr[2]=2; garr[3]=3; garr[2]; }), "({ garr[0]=0; garr[1]=1; garr[2]=2; garr[3]=3; garr[2]; })");
    test(3, ({ garr[0]=0; garr[1]=1; garr[2]=2; garr[3]=3; garr[3]; }), "({ garr[0]=0; garr[1]=1; garr[2]=2; garr[3]=3; garr[3]; })");
    test(4, sizeof(g1), "sizeof(g1)");
    test(16, sizeof(garr), "sizeof(garr)");

    test(1, ({ char x = 1; x; }), "({ char x = 1; x; })");
    test(127, ({ char x = 127; x; }), "({ char x = 127; x; })");
    test(-128, ({ char x = 128; x; }), "({ char x = 128; x; })");
    test(-1, ({ char x = 255; x; }), "({ char x = 255; x; })");
    test(0, ({ char x = 256; x; }), "({ char x = 256; x; })");
    test(1, ({ char x = 1; char y = 2; x; }), "({ char x = 1; char y = 2; x; })");
    test(2, ({ char x = 1; char y = 2; y; }), "({ char x = 1; char y = 2; y; })");
    test(3, ({ char x = 1; char y = 2; x+y; }), "({ char x = 1; char y = 2; x+y; })");
    test(1, ({ char x; sizeof(x); }), "({ char x; sizeof(x); })");
    test(10, ({ char x[10]; sizeof(x); }), "({ char x[10]; sizeof(x); })");
    test(1, ({ char arr[1] = { 1 }; arr[0]; }), "({ char arr[1] = { 1 }; arr[0]; })");
    test(-1, ({ char arr[1] = { 255 }; arr[0]; }), "({ char arr[1] = { 255 }; arr[0]; })");
    test(1, ({ char arr[2] = { 1, 10 }; arr[0]; }), "({ char arr[2] = { 1, 10 }; arr[0]; })");
    test(10, ({ char arr[2] = { 1, 10 }; arr[1]; }), "({ char arr[2] = { 1, 10 }; arr[0]; })");
    test(11, ({ char arr[2] = { 1, 10 }; arr[1] + arr[0]; }), "({ char arr[2] = { 1, 10 }; arr[1] + arr[0]; })");
    test(9, ({ char arr[2] = { 1, 10 }; arr[1] - arr[0]; }), "({ char arr[2] = { 1, 10 }; arr[1] - arr[0]; })");
    test(100, ({ char arr[3] = { 1, 10, 111 }; arr[2] - arr[1] - arr[0]; }), "({ char arr[3] = { 1, 10, 111 }; arr[2] - arr[1] - arr[0]; })");
    test(3, ({ char arr[3] = { -1, 2, 111 }; int y = 4; arr[0] + y; }), "({ char arr[3] = { -1, 2, 111 }; int y = 4; arr[0] + y; })");
    test(4, sub_char(7, 1, 2), "sub_char(7, 1, 2)");

    test(97, "abc"[0], "abc[0]");
    test(98, "abc"[1], "abc[1]");
    test(99, "abc"[2], "abc[2]");
    test(0, "abc"[3], "abc[3]");
    test(4, sizeof("abc"), "sizeof(\"abc\")");

    test(7, "\a"[0], "\\a[0]");
    test(8, "\b"[0], "\\b[0]");
    test(9, "\t"[0], "\\t[0]");
    test(10, "\n"[0], "\\n[0]");
    test(11, "\v"[0], "\\v[0]");
    test(12, "\f"[0], "\\f[0]");
    test(13, "\r"[0], "\\r[0]");
    test(27, "\e"[0], "\\e[0]");
    test(106, "\j"[0], "\\j[0]");
    test(107, "\k"[0], "\\k[0]");
    test(108, "\l"[0], "\\l[0]");

    test(0, ({ 0; }), "({ 0; })");
    test(2, ({ 0; 1; 2; }), "({ 0; 1; 2; })");
    test(30, ({ ({ 10; }) + 20; }), "({ ({ 10; }) + 20; })");
    test(6, ({ ({ 1; }) + ({ 2; }) + ({ 3; }); }), "({ ({ 1; }) + ({ 2; }) + ({ 3; }); })");
    test(3, ({ int x=3; x; }), "({ int x=3; x; })");

    test(2, ({ /* return 1; */ 2; }), "({ /* return 1; */ 2; })");
    test(2,
           ({
                // return 1;
                2;
            }),
          "({
                // return 1;
                2;
            })");

    test(2, ({ int x = 2; { int x = 3; } x;}), "({ int x = 2; { int x = 3; } x;})");
    test(2, ({ int x = 2; { int x = 3; } { int y = 4; x; } }), "({ int x = 2; { int x = 3; } { int y = 4; x; } })");
    test(3, ({ int x = 2; { x = 3; } x; }), "({ int x = 2; { x = 3; } x; })");
    test(30, ({ int x = ({ int a; a = 30; int b; b = 11; int c; c = b - 10; int d; d = a * (b - c); d / 10; }); x; }), "({ int x = ({ int a; a = 30; int b; b = 11; int c; c = b - 10; int d; d = a * (b - c); d / 10; }); x; })");
    test(20, ({ int x = ({ int abc; abc = 20; abc; }); x; }), "({ int x = ({ int abc; abc = 20; abc; }); x; })");

    test(30,
        ({
            int x = ({ int a; a = 30; int b; b = 11; int c; c = b - 10; int d; d = a * (b - c); d / 10; });
            int y = ({ int abc = 20; });
            x;
        }),
        "({
            int x = ({ int a; a = 30; int b; b = 11; int c; c = b - 10; int d; d = a * (b - c); d / 10; });
            int y = ({ int abc = 20; });
            x;
        })");
    test(20,
        ({
            int x = ({ int a; a = 30; int b; b = 11; int c; c = b - 10; int d; d = a * (b - c); d / 10; });
            int y = ({ int abc; abc = 20; abc; });
            y;
        }),
        "({
            int x = ({ int a; a = 30; int b; b = 11; int c; c = b - 10; int d; d = a * (b - c); d / 10; });
            int y = ({ int abc; abc = 20; abc; });
            y;
        })");
    test(50,
        ({
            int x = ({ int a; a = 30; int b; b = 11; int c; c = b - 10; int d; d = a * (b - c); d / 10; });
            int y = ({ int abc; abc = 20; abc; });
            x+y;
        }),
        "({
            int x = ({ int a; a = 30; int b; b = 11; int c; c = b - 10; int d; d = a * (b - c); d / 10; });
            int y = ({ int abc; abc = 20; abc; });
            x+y;
        })");

    printf("%s\n", "He said \"Howdy? Toasa!\".");

    test(2, ({ int x[5]; int *y=x+2; y-x; }), "({ int x[5]; int *y=x+2; y-x; })");

    test(1, ({ struct {int a; int b;} x; x.a=1; x.b=2; x.a; }), "({ struct {int a; int b;} x; x.a=1; x.b=2; x.a; })");
    test(2, ({ struct {int a; int b;} x; x.a=1; x.b=2; x.b; }), "({ struct {int a; int b;} x; x.a=1; x.b=2; x.b; })");
    test(1, ({ struct {char a; int b; char c;} x; x.a=1; x.b=2; x.c=3; x.a; }), "({ struct {char a; int b; char c;} x; x.a=1; x.b=2; x.c=3; x.a; })");
    test(2, ({ struct {char a; int b; char c;} x; x.b=1; x.b=2; x.c=3; x.b; }), "({ struct {char a; int b; char c;} x; x.b=1; x.b=2; x.c=3; x.b; })");
    test(3, ({ struct {char a; int b; char c;} x; x.a=1; x.b=2; x.c=3; x.c; }), "({ struct {char a; int b; char c;} x; x.a=1; x.b=2; x.c=3; x.c; })");

    test(0, ({ struct {int a; int b;} x[3]; int *p=x; p[0]=0; x[0].a; }), "({ struct {int a; int b;} x[3]; int *p=x; p[0]=0; x[0].a; })");
    test(1, ({ struct {int a; int b;} x[3]; int *p=x; p[1]=1; x[0].b; }), "({ struct {int a; int b;} x[3]; int *p=x; p[1]=1; x[0].b; })");
    test(2, ({ struct {int a; int b;} x[3]; int *p=x; p[2]=2; x[1].a; }), "({ struct {int a; int b;} x[3]; int *p=x; p[2]=2; x[1].a; })");
    test(3, ({ struct {int a; int b;} x[3]; int *p=x; p[3]=3; x[1].b; }), "({ struct {int a; int b;} x[3]; int *p=x; p[3]=3; x[1].b; })");

    test(6, ({ struct {int a[3]; int b[5];} x; int *p=&x; x.a[0]=6; p[0]; }), "({ struct {int a[3]; int b[5];} x; int *p=&x; x.a[0]=6; p[0]; })");
    test(7, ({ struct {int a[3]; int b[5];} x; int *p=&x; x.b[0]=7; p[3]; }), "({ struct {int a[3]; int b[5];} x; int *p=&x; x.b[0]=7; p[3]; })");

    test(6, ({ struct { struct { int b; } a; } x; x.a.b=6; x.a.b; }), "({ struct { struct { int b; } a; } x; x.a.b=6; x.a.b; })");

    test(4, ({ struct {int a;} x; sizeof(x); }), "({ struct {int a;} x; sizeof(x); })");
    test(8, ({ struct {int a; int b;} x; sizeof(x); }), "({ struct {int a; int b;} x; sizeof(x); })");
    test(8, ({ struct {int a, b;} x; sizeof(x); }), "({ struct {int a, b;} x; sizeof(x); })");
    test(12, ({ struct {int a[3];} x; sizeof(x); }), "({ struct {int a[3];} x; sizeof(x); })");
    test(16, ({ struct {int a;} x[4]; sizeof(x); }), "({ struct {int a;} x[4]; sizeof(x); })");
    test(24, ({ struct {int a[3];} x[2]; sizeof(x); }), "({ struct {int a[3];} x[2]; sizeof(x); })");
    test(2, ({ struct {char a; char b;} x; sizeof(x); }), "({ struct {char a; char b;} x; sizeof(x); })");
    test(8, ({ struct {char a; int b;} x; sizeof(x); }), "({ struct {char a; int b;} x; sizeof(x); })");

    test(8, ({ struct t {int a; int b;} x; struct t y; sizeof(y); }), "({ struct t {int a; int b;} x; struct t y; sizeof(y); })");
    test(8, ({ struct t {int a; int b;}; struct t y; sizeof(y); }), "({ struct t {int a; int b;}; struct t y; sizeof(y); })");
    test(2, ({ struct t {char a[2];}; { struct t {char a[4];}; } struct t y; sizeof(y); }), "({ struct t {char a[2];}; { struct t {char a[4];}; } struct t y; sizeof(y); })");
    test(3, ({ struct t {int x;}; int t=1; struct t y; y.x=2; t+y.x; }), "({ struct t {int x;}; int t=1; struct t y; y.x=2; t+y.x; })");

    test(3, ({ struct t {char a;} x; struct t *y = &x; x.a=3; y->a; }), "({ struct t {char a;} x; struct t *y = &x; x.a=3; y->a; })");
    test(3, ({ struct t {char a;} x; struct t *y = &x; y->a=3; x.a; }), "({ struct t {char a;} x; struct t *y = &x; y->a=3; x.a; })");

    test(8, ({ union { int a; char b[6]; } x; sizeof(x); }), "({ union { int a; char b[6]; } x; sizeof(x); })");
    test(3, ({ union { int a; char b[4]; } x; x.a = 515; x.b[0]; }), "({ union { int a; char b[4]; } x; x.a = 515; x.b[0]; })");
    test(2, ({ union { int a; char b[4]; } x; x.a = 515; x.b[1]; }), "({ union { int a; char b[4]; } x; x.a = 515; x.b[1]; })");
    test(0, ({ union { int a; char b[4]; } x; x.a = 515; x.b[2]; }), "({ union { int a; char b[4]; } x; x.a = 515; x.b[2]; })");
    test(0, ({ union { int a; char b[4]; } x; x.a = 515; x.b[3]; }), "({ union { int a; char b[4]; } x; x.a = 515; x.b[3]; })");

    test(10, ({ int a = 20, b, c = 10, d, e; c; }), "({ int a = 20, b, c = 10, d, e; c; })");
    test(20, ({ int a = 20, *p = &a; *p; }), "({ int a = 20, *p = &a; *p; })");
    test(60, ({ int a = 20, arr[4] = { 1,2,3,4 }; arr[2] * a; }), "({ int a = 20, arr[4] = { 1,2,3,4 }; arr[2] * a; })");
    test(7, ({ struct t {int a,b;}; struct t x; x.a=7; struct t y, *p=&x, *q=&y; *q=*p; y.a; }), "({ struct t {int a,b;}; struct t x; x.a=7; struct t y, *p=&x, *q=&y; *q=*p; y.a; })");

    test(3, ({ struct {int a,b;} x,y; x.a=3; y=x; y.a; }), "({ struct {int a,b;} x,y; x.a=3; y=x; y.a; })");
    test(5, ({ struct t {int a,b;}; struct t x; x.a=5; struct t y=x; y.a; }), "({ struct t {int a,b;}; struct t x; x.a=5; struct t y=x; y.a; })");
    test(7, ({ struct t {int a,b;}; struct t x; x.a=7; struct t y; struct t *z=&y; *z=x; y.a; }), "({ struct t {int a,b;}; struct t x; x.a=7; struct t y; struct t *z=&y; *z=x; y.a; })");

    test(2, ({ short x; sizeof(x); }), "({ short x; sizeof(x); })");
    test(4, ({ struct {char a; short b;} x; sizeof(x); }), "({ struct {char a; short b;} x; sizeof(x); })");

    test(8, ({ long x; sizeof(x); }), "({ long x; sizeof(x); })");
    test(16, ({ struct {char a; long b;} x; sizeof(x); }), "({ struct {char a; long b;} x; sizeof(x); })");

    test(1, sub_short(7, 3, 3), "sub_short(7, 3, 3)");
    test(1, sub_long(7, 3, 3), "sub_long(7, 3, 3)");

    test(24, ({ int *x[3]; sizeof(x); }), "({ int *x[3]; sizeof(x); })");
    test(8, ({ int (*x)[3]; sizeof(x); }), "({ int (*x)[3]; sizeof(x); })");
    test(3, ({ int *x[3]; int y; x[0]=&y; y=3; x[0][0]; }), "({ int *x[3]; int y; x[0]=&y; y=3; x[0][0]; })");
    test(4, ({ int x[3]; int (*y)[3]=x; y[0][0]=4; y[0][0]; }), "({ int x[3]; int (*y)[3]=x; y[0][0]=4; y[0][0]; })");

    { void *x; }

    test(1, ({ char x; sizeof(x); }), "({ char x; sizeof(x); })");
    test(2, ({ short int x; sizeof(x); }), "({ short int x; sizeof(x); })");
    test(2, ({ int short x; sizeof(x); }), "({ int short x; sizeof(x); })");
    test(4, ({ int x; sizeof(x); }), "({ int x; sizeof(x); })");
    test(8, ({ long int x; sizeof(x); }), "({ long int x; sizeof(x); })");
    test(8, ({ int long x; sizeof(x); }), "({ int long x; sizeof(x); })");
    test(8, ({ long long x; sizeof(x); }), "({ long long x; sizeof(x); })");
    test(8, ({ long long int x; sizeof(x); }), "({ long long int x; sizeof(x); })");

    test(1, ({ typedef int t; t x=1; x; }), "({ typedef int t; t x=1; x; })");
    test(1, ({ typedef struct {int a;} t; t x; x.a=1; x.a; }), "({ typedef struct {int a;} t; t x; x.a=1; x.a; })");
    test(1, ({ typedef int t; t t=1; t; }), "({ typedef int t; t t=1; t; })");
    test(2, ({ typedef struct {int a;} t; { typedef int t; } t x; x.a=2; x.a; }), "({ typedef struct {int a;} t; { typedef int t; } t x; x.a=2; x.a; })");
    test(4, ({ typedef t; t x; sizeof(x); }), "({ typedef t; t x; sizeof(x); })");
    test(4, ({ typedef typedef t; t x; sizeof(x); }), "({ typedef typedef t; t x; sizeof(x); })");
    test(3, ({ MyInt x=3; x; }), "({ MyInt x=3; x; })");
    test(16, ({ MyInt2 x; sizeof(x); }), "({ MyInt2 x; sizeof(x); })");

    test(1, sizeof(char), "sizeof(char)");
    test(2, sizeof(short), "sizeof(short)");
    test(2, sizeof(short int), "sizeof(short int)");
    test(2, sizeof(int short), "sizeof(int short)");
    test(4, sizeof(int), "sizeof(int)");
    test(8, sizeof(long), "sizeof(long)");
    test(8, sizeof(long int), "sizeof(long int)");
    test(8, sizeof(long int), "sizeof(long int)");
    test(8, sizeof(char *), "sizeof(char *)");
    test(8, sizeof(int *), "sizeof(int *)");
    test(8, sizeof(long *), "sizeof(long *)");
    test(8, sizeof(int **), "sizeof(int **)");
    test(8, sizeof(int(*)[4]), "sizeof(int(*)[4])");
    test(32, sizeof(int*[4]), "sizeof(int*[4])");
    test(16, sizeof(int[4]), "sizeof(int[4])");
    test(48, sizeof(int[3][4]), "sizeof(int[3][4])");
    test(8, sizeof(struct {int a; int b;}), "sizeof(struct {int a; int b;})");

    test(131585, (int)8590066177, "(int)8590066177");
    test(513, (short)8590066177, "(short)8590066177");
    test(1, (char)8590066177, "(char)8590066177");
    test(1, (long)1, "(long)1");
    test(0, (long)&*(int *)0, "(long)&*(int *)0");
    test(513, ({ int x=512; *(char *)&x=1; x; }), "({ int x=512; *(char *)&x=1; x; })");
    test(5, ({ int x=5; long y=(long)&x; *(int*)y; }), "({ int x=5; long y=(long)&x; *(int*)y; })");

    (void)1;

    test(4, sizeof(-10 + 5), "sizeof(-10 + 5)");
    test(4, sizeof(-10 - 5), "sizeof(-10 - 5)");
    test(4, sizeof(-10 * 5), "sizeof(-10 * 5)");
    test(4, sizeof(-10 / 5), "sizeof(-10 / 5)");

    test(8, sizeof(-10 + (long)5), "sizeof(-10 + (long)5)");
    test(8, sizeof(-10 - (long)5), "sizeof(-10 - (long)5)");
    test(8, sizeof(-10 * (long)5), "sizeof(-10 * (long)5)");
    test(8, sizeof(-10 / (long)5), "sizeof(-10 / (long)5)");
    test(8, sizeof((long)-10 + 5), "sizeof((long)-10 + 5)");
    test(8, sizeof((long)-10 - 5), "sizeof((long)-10 - 5)");
    test(8, sizeof((long)-10 * 5), "sizeof((long)-10 * 5)");
    test(8, sizeof((long)-10 / 5), "sizeof((long)-10 / 5)");

    test((long)-5, -10 + (long)5, "-10 + (long)5");
    test((long)-15, -10 - (long)5, "-10 - (long)5");
    test((long)-50, -10 * (long)5, "-10 * (long)5");
    test((long)-2, -10 / (long)5, "-10 / (long)5");

    test(1, -2 < (long)-1, "-2 < (long)-1");
    test(1, -2 <= (long)-1, "-2 <= (long)-1");
    test(0, -2 > (long)-1, "-2 > (long)-1");
    test(0, -2 >= (long)-1, "-2 >= (long)-1");

    test(1, (long)-2 < -1, "(long)-2 < -1");
    test(1, (long)-2 <= -1, "(long)-2 <= -1");
    test(0, (long)-2 > -1, "(long)-2 > -1");
    test(0, (long)-2 >= -1, "(long)-2 >= -1");

    test(0, 2147483647 + 2147483647 + 2, "2147483647 + 2147483647 + 2");
    test((long)-1, ({ long x; x=-1; x; }), "({ long x; x=-1; x; })");

    test(1, ({ char x[3]; x[0]=0; x[1]=1; x[2]=2; char *y=x+1; y[0]; }), "({ char x[3]; x[0]=0; x[1]=1; x[2]=2; char *y=x+1; y[0]; })");
    test(0, ({ char x[3]; x[0]=0; x[1]=1; x[2]=2; char *y=x+1; y[-1]; }), "({ char x[3]; x[0]=0; x[1]=1; x[2]=2; char *y=x+1; y[-1]; })");
    test(5, ({ struct t {char a;} x, y; x.a=5; y=x; y.a; }), "({ struct t {char a;} x, y; x.a=5; y=x; y.a; })");

    test(3, *g1_ptr(), "*g1_ptr()");
    test(5, int_to_char(261), "int_to_char(261)");
    test(0, char_to_int(256), "char_to_int(256)");

    test(-5, div_long(-10, 2), "div_long(-10, 2)");

    test(0, ({ _Bool x=0; x; }), "({ _Bool x=0; x; })");
    test(1, ({ _Bool x=1; x; }), "({ _Bool x=1; x; })");
    test(1, ({ _Bool x=2; x; }), "({ _Bool x=2; x; })");
    test(1, (_Bool)1, "(_Bool)1");
    test(1, (_Bool)2, "(_Bool)2");
    test(0, (_Bool)(char)256, "(_Bool)(char)256");
    test(1, bool_fn_add(3), "bool_fn_add(3)");
    test(0, bool_fn_sub(3), "bool_fn_sub(3)");
    test(1, bool_fn_add(-3), "bool_fn_add(-3)");
    test(0, bool_fn_sub(-3), "bool_fn_sub(-3)");
    test(1, bool_fn_add(0), "bool_fn_add(0)");
    test(1, bool_fn_sub(0), "bool_fn_sub(0)");

    printf("OK\n");
    return 0;
}
