#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define COMP_RANGE 100 // Computation cost range
#define COM_RANGE 100 // Communication cost range
#define EDGE_PROB 70 // Edge probability 

int main(int argc, char** argv)
{
    int N = atoi(argv[1]);
    int M = atoi(argv[2]);
    char* fileName = argv[3];
    FILE *fp;
    fp = fopen(fileName, "w+");
    int i, j = 0;
    fprintf(fp, "%d %d\n", N, M);
    for(i = 0; i < N; ++i) {
        for(j = 0; j < M; ++j) {
            fprintf(fp, "%d ", rand()%COMP_RANGE);
        }
        fprintf(fp, "\n");
    }
    srand(time(NULL));
    for(i = 0; i < N; ++i) {
        for(j = 0; j < N; ++j) {
            if(i < j && rand()%EDGE_PROB <= 70) {
                fprintf(fp, "%d ", rand()%COM_RANGE);
            } else {
                fprintf(fp, "-1 ");
            }
        }
        fprintf(fp, "\n");
    }
    fclose(fp);
    return 0;
}