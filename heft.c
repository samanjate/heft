#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <float.h>
#include "taskp.c"

int numOftasks; // number of tasks to be schduled
int numOfProcessors; // bounded number of heteroenous processors
double** dag; // adjacency matrix form for the input DAG
double** computationCost; // the cost of processes on each processor table
double* upperRank; // the calculated upper ranks of each process
int* sortedTasks; // the sorted indexes of each processor based on their upper rank
double* AFTs;

typedef struct ProcessorSchedule {
    int size;
    TaskProcessor* tasks;
} ProcessorSchedule;

ProcessorSchedule** processorSchedule;

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

    dag = (double**)malloc(sizeof(double*)*numOftasks);
    for(i = 0; i < numOftasks; ++i) {
        dag[i] = (double*)malloc(sizeof(double)*numOftasks);
    }

    upperRank = (double*)malloc(sizeof(double)*numOftasks);
    for(i = 0; i < numOftasks; ++i) {
        upperRank[i] = -1;
    }

    sortedTasks = (int*)malloc(sizeof(int)*numOftasks);

    processorSchedule = (ProcessorSchedule**)malloc(sizeof(ProcessorSchedule*)*numOfProcessors);
    for(i = 0; i < numOfProcessors; ++i) {
        processorSchedule[i] = (ProcessorSchedule*)malloc(sizeof(ProcessorSchedule)*numOfProcessors);
        processorSchedule[i]->size = 0;
        processorSchedule[i]->tasks = NULL;
    }

    AFTs = (double*)malloc(sizeof(double)*numOftasks);
    for(i = 0; i < numOftasks; ++i) {
        AFTs[i] = -1;
    }

    int j;
    for(i = 0; i < numOftasks; ++i) {
        for(j = 0; j < numOfProcessors; ++j) {
            fscanf(fp, "%lf", &computationCost[i][j]);
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

// A fucntion to compuete the average computation cost across all the 
// processors for the given task
double calculateAvgComputationCosts(int task) {
    int j;
    double avg = 0.0;
    for(j = 0; j < numOfProcessors; ++j) {
        avg += computationCost[task][j];
    }
    return avg/numOfProcessors;;
}

// A function to calculate the upper rank of the provided task
double calculateUpperRank(int task) {
    double max = 0.0;
    int i;
    for(i = 0; i < numOftasks; ++i) {
        if(dag[task][i] != -1) {
            if(upperRank[i] != -1) {
                max = dag[task][i] + upperRank[i];
            } else {
                double cost = dag[task][i] + calculateUpperRank(i);
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

// function that calculates the upper ranks of the input task,
// sorts them in decreasing order and displays it to the user.
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

// A function to determine whether the provided task
// is an entry task or not
bool isEntryTask(int task) {
    int i;
    for(i = 0; i < numOftasks; ++i) {
        if(dag[i][task] != -1) {
            return false;
        }
    }
    return true;
}

// Function to calcuate the EST
void calculateEST(int task, int processor, double* EST) {
    double earliestTime;
    if(processorSchedule[processor]->size == 0) {
        earliestTime = 0.0;
    } else {
        avail(processorSchedule[processor]->tasks, processorSchedule[processor]->size, computationCost[task][processor], &earliestTime);
    }
    double max = DBL_MIN;
    int i;
    for(i = 0; i < numOftasks; ++i) {
        if(dag[i][task] != -1) {
            if(max < AFTs[i]) {
                max = AFTs[i];
            }
        }
    }
    *EST = maxDouble(earliestTime, max);
    return;
}

// HEFT scheduler function
void heft() {
    int i;
    for(i = 0; i < numOftasks; ++i) {
        int task = sortedTasks[i];
        if(isEntryTask(task)) {
            double min = DBL_MAX;
            int processor;
            int j;
            for(j = 0; j < numOfProcessors; ++j) {
                if(computationCost[task][j] < min) {
                    min = computationCost[task][j];
                    processor = j;
                }
            }
            processorSchedule[processor]->tasks = (TaskProcessor*)malloc(sizeof(TaskProcessor));
            processorSchedule[processor]->tasks[processorSchedule[processor]->size].AST = 0.0;
            processorSchedule[processor]->tasks[processorSchedule[processor]->size].AFT = min;
            processorSchedule[processor]->tasks[processorSchedule[processor]->size].process = task;
            processorSchedule[processor]->size++;
            AFTs[task] = min;
        } else {
            double min = DBL_MAX;
            int processor;
            int j;
            for(j = 0; j < numOfProcessors; ++j) {
                double EST;
                calculateEST(task, j, &EST);
                if(EST < min) {
                    min = EST;
                    processor = j;
                }
            }
            processorSchedule[processor]->tasks = (TaskProcessor*)realloc(processorSchedule[processor]->tasks, sizeof(TaskProcessor));
            processorSchedule[processor]->tasks[processorSchedule[processor]->size].AST = min;
            processorSchedule[processor]->tasks[processorSchedule[processor]->size].AFT = min + computationCost[task][processor];
            processorSchedule[processor]->tasks[processorSchedule[processor]->size].process = task;
            processorSchedule[processor]->size++;
        }
    }
}

void displaySchedule() {
    int i;
    puts("HEFT SCHEDULE");
    puts("---------------------------");
    puts("Proc\tTask\tAST\tAFT");
    puts("---------------------------");
    for(i = 0; i < numOfProcessors; ++i) {
        int j;
        for(j = 0; j < processorSchedule[i]->size; ++j) {
            printf("%d\t%d\t%g\t%g\n",
                   i+1,
                   processorSchedule[i]->tasks[j].process+1, 
                   processorSchedule[i]->tasks[j].AST, 
                   processorSchedule[i]->tasks[j].AFT);
        }
    }
}

// Deallocated all memory that was allocated for the scheduler
void freeSpace() {
    int i;

    for(i = 0; i < numOftasks; ++i) {
        free(computationCost[i]);
    }

    for(i = 0; i < numOftasks; ++i) {
        free(dag[i]);
    }

    for(i = 0; i < numOfProcessors; ++i) {
        free(processorSchedule[i]);
    }

    free(computationCost);
    free(dag);
    free(upperRank);
    free(processorSchedule);
    free(AFTs);
    free(sortedTasks);
}

// Entry point of the program
int main() {
    initEnvironment();
    displayInput();
    calculateAndDisplayRanks();
    heft();
    displaySchedule();
    freeSpace();
    return 0;
}