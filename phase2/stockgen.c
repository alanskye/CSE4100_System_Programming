#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 1024

int main() {
    FILE* fp = fopen("stock.txt", "w");
    srand(NULL);
    for (int i = 1; i <= N; i++) {
        fprintf(fp, "%d %d %d\n", i, rand() % 20 + 2, i);
    }
    fclose(fp);
    return 0;
}

