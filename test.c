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

int g1;
int g2;
int garr[4];

int main() {
    test(0, 0, "0");
    test(30, 30, "30");

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
    test(8, ({ int *x; sizeof(x); }), "({ int *x; sizeof(x); })");
    test(4, ({ int x; sizeof(x + 3); }), "({ int x; sizeof(x + 3); })");
    test(8, ({ int *x; sizeof(x + 3); }), "({ int *x; sizeof(x + 3); })");
    test(4, sizeof(3), "sizeof(3)");
    test(8, ({ int ***ptr; sizeof(ptr); }), "({ int ***ptr; sizeof(ptr); })");
    test(4, ({ int *ptr; sizeof(*ptr); }), "({ int *ptr; sizeof(*ptr); })");
    test(4, ({ int x = 20; int *y = &x; int **z = &y; int ***ptr = &z; sizeof(***ptr); }), "({ int x = 20; int *y = &x; int **z = &y; int ***ptr = &z; sizeof(***ptr); })");
    test(40, ({ int arr[10]; sizeof(arr); }), "({ int arr[10]; sizeof(arr); })");

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

    printf("OK\n");
    return 0;
}
