#include <stdio.h>

int max1(int a, int b) {
    return (a > b) ? a : b;
}

int max2(int a, int b) {
    int isaGTB = a > b;
    int max;
    if (isaGTB) {
        max = a;
    } else {
        max = b;
    }
    return max;
}

int testMax() {
    int result = max1(5, 10);
    return result;
}

int main() {
    int result = 0;
    for (int i = 0; i < 1000000000; i++) {  // Call max1 a billion(10^9) times
        result = max1(5, 10);
    }
    printf("Result: %d\n", result);
    return 0;
}