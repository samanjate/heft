#include <stdio.h>
#include <stdlib.h>

int numOftasks; // number of tasks to be schduled
int numOfProcessors; // bounded number of heteroenous processors
double** dag; // adjacency matrix form for the input DAG
double** computationCost; // the cost of processes on each processor table
double** dataTransferRate; // the edge values of the DAG i.e. the transfer cost from one process to another
double* upperRank; // the calculated upper ranks of each process
int* sortedTasks; // the sorted indexes of each processor based on their upper rank

// A structure to store the scheduled tasks.
typedef struct TaskProcessor {
    int processor;
    double AST;
    double AFT;
} TaskProcessor;

TaskProcessor* schedule;

// This function intilizes the enovironment for the scheduler
// it gets the value from the 'environment' text file based on a particular format
// It also alloactes memory for the different data structures used by the scheduler
void initEnvironment() {
    FILE *fp;
    fp = fopen("environment.txt", "r+");
    fscanf(fp, "%d%d", &numOftasks, &numOfProcessors);

    int i;
    computationCost = (double**)malloc(sizeof(double*)*numOftasks);
    for(i = 0; i < numOftasks; ++i) {
        computationCost[i] = (double*)malloc(sizeof(double)*numOfProcessors);
    }

    dataTransferRate = (double**)malloc(sizeof(double*)*numOfProcessors);
    for(i = 0; i < numOfProcessors; ++i) {
        dataTransferRate[i] = (double*)malloc(sizeof(double)*numOfProcessors);
    }

    dag = (double**)malloc(sizeof(double*)*numOftasks);
    for(i = 0; i < numOftasks; ++i) {
        dag[i] = (double*)malloc(sizeof(double)*numOftasks);
    }

    upperRank = (double*)malloc(sizeof(double)*numOftasks);
    for(i = 0; i < numOftasks; ++i) {
        upperRank[i] = -1;
    }

    sortedTasks = (int*)malloc(sizeof(int));
    schedule = (TaskProcessor*)malloc(sizeof(TaskProcessor)*numOftasks);
    for(i = 0; i < numOftasks; ++i) {
        schedule[i].processor = -1;
    }

    int j;
    for(i = 0; i < numOftasks; ++i) {
        for(j = 0; j < numOfProcessors; ++j) {
            fscanf(fp, "%lf", &computationCost[i][j]);
        }
    }

    for(i = 0; i < numOfProcessors; ++i) {
        for(j = 0; j < numOfProcessors; ++j) {
            fscanf(fp, "%lf", &dataTransferRate[i][j]);
        }
    }

    for(i = 0; i < numOftasks; ++i) {
        for(j = 0; j < numOftasks; ++j) {
            fscanf(fp, "%lf", &dag[i][j]);
        }
    }
}

// Display the input of the environment on which the algorithm is run
void displayInput() {
    int i, j;
    puts("\nENVIRONMENT CONFIGURATION");
    puts("---------------------------");
    printf("Number of Tasks : %d\n", numOftasks);
    printf("Number of Processors : %d\n", numOfProcessors);
    puts("---------------------------\n");
    puts("COMPUTATION COSTS");
    puts("---------------------------");
    printf("Task");
    for(i = 0; i < numOfProcessors; ++i) {
        printf("\tP%d", i + 1);
    }
    puts("\n---------------------------");
    for(i = 0; i < numOftasks; ++i) {
        printf("%d", i + 1);
        for(j = 0; j < numOfProcessors; ++j) {
            printf("\t%g", computationCost[i][j]);
        }
        printf("\n");
    }
    puts("---------------------------\n");
    puts("DAG");
    puts("---------------------------");
    for(i = 0; i < numOftasks; ++i) {
        for(j = 0; j < numOftasks; ++j) {
            if(dag[i][j] != -1) {
                printf("%d---%g--->%d\n", i+1, dag[i][j], j+1);
            }
        }
    }
    puts("---------------------------\n");
}

double calculateAvgComputationCosts(int task) {
    int j;
    double avg = 0.0;
    for(j = 0; j < numOfProcessors; ++j) {
        avg += computationCost[task][j];
    }
    return avg/numOfProcessors;;
}

double avgCommunicationCost(int source, int destination)
{
    int i, j;
    double avg = 0.0;
    for(i = 0; i < numOfProcessors; ++i) {
        for(j = 0; j < numOfProcessors; ++j)
        {
            if(dataTransferRate[i][j] != 0) {
                avg += (dag[source][destination]/dataTransferRate[i][j]);
            }
        }
    }
    return avg/((numOfProcessors*numOfProcessors) - numOfProcessors);
}

double calculateUpperRank(int task) {
    double max = 0.0;
    int i;
    for(i = 0; i < numOftasks; ++i) {
        if(dag[task][i] != -1) {
            if(upperRank[i] != -1) {
                max = avgCommunicationCost(task, i) + upperRank[i];
            } else {
                double cost = avgCommunicationCost(task, i) + calculateUpperRank(i);
                if(cost > max) {
                    max = cost;
                }
            }
        }
    }
    upperRank[task] = calculateAvgComputationCosts(task) + max;
    return upperRank[task];
}

// Structure used to sort indexes
typedef struct Sort {
    int index;
    double val;
} Sort;

// Sorts in decreasing order based on the upper ranks of the tasks.
// this is equivalent to a topological sort of a DAG
void sortIndexes() {
    Sort* arr = (Sort*)malloc(sizeof(Sort)*numOftasks);
    int i, j;
    for(i = 0; i < numOftasks; ++i) {
        arr[i].index = i;
        arr[i].val = upperRank[i];
    }
    for(i = 0; i < numOftasks; ++i) {
        for(j = i + 1; j < numOftasks; ++j) {
            if(arr[j].val > arr[i].val) {
                int tempIndex = arr[i].index;
                double tempVal = arr[i].val;
                arr[i].val = arr[j].val;
                arr[i].index = arr[j].index;
                arr[j].val = tempVal;
                arr[j].index = tempIndex;
            }
        }
    }
    for(i = 0; i < numOftasks; ++i) {
        sortedTasks[i] = arr[i].index;
    }
    free(arr);
}

void calculateAndDisplayRanks() {
    puts("UPPER RANKS");
    puts("---------------------------");
    puts("Task\tRank");
    puts("---------------------------");
    int i;
    for(i = 0; i < numOftasks; ++i) {
        calculateUpperRank(i);
    }
    sortIndexes();
    for(i = 0; i < numOftasks; ++i) {
        printf("%d\t%g\t\t|\n", sortedTasks[i]+1, upperRank[sortedTasks[i]]);
    }
    printf("\t\t\t˅\n");
    printf("\t\tdecreasing order\n");
    puts("---------------------------\n");
}

// Deallocated all memory that was allocated for the scheduler
void freeSpace() {
    int i;

    for(i = 0; i < numOftasks; ++i) {
        free(computationCost[i]);
    }

    for(i = 0; i < numOfProcessors; ++i) {
        free(dataTransferRate[i]);
    }

    for(i = 0; i < numOftasks; i++) {
        free(dag[i]);
    }

    free(computationCost);
    free(dataTransferRate);
    free(dag);
    free(upperRank);
    free(sortedTasks);
    free(schedule);
}

int main() {
    initEnvironment();
    displayInput();
    calculateAndDisplayRanks();
    freeSpace();
	return 0;
}