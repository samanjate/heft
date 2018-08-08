#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <float.h>
#include "task.c"

int numOftasks; // number of tasks to be schduled
int numOfProcessors; // bounded number of heteroenous processors
double** dag; // adjacency matrix form for the input DAG
double** computationCost; // the cost of processes on each processor table
double* upperRank; // the calculated upper ranks of each process
int* sortedTasks; // the sorted indexes of each processor based on their upper rank
double* AFTs; // to store the actual finish times of the task (represented by the index)
int* proc; // the processor that the task is scheduled on (represented by the index)

// A structure that keeps track of the tasks scheduled processor wise.
typedef struct ProcessorSchedule {
    int size;
    TaskProcessor* tasks;
} ProcessorSchedule;
ProcessorSchedule** processorSchedule;

// This function intilizes the enovironment for the scheduler
// it gets the value from the 'environment' text file based on a particular format
// It also alloactes memory for the different data structures used by the scheduler
void initEnvironment(char* input) {
    FILE *fp;
    printf("Running: %s\n", input);
    puts("---------------------------");
    fp = fopen(input, "r+");
    fscanf(fp, "%d%d", &numOftasks, &numOfProcessors);
    int i, j;

    // A 2D array that stores the computation costs of every task on each processor
    computationCost = (double**)malloc(sizeof(double*)*numOftasks);
    for(i = 0; i < numOftasks; ++i) {
        computationCost[i] = (double*)malloc(sizeof(double)*numOfProcessors);
    }

    // A 2D array to store the communication costs of the switching from one task to another
    // This only applies if the processor is changed between the scheduling of the two tasks
    dag = (double**)malloc(sizeof(double*)*numOftasks);
    for(i = 0; i < numOftasks; ++i) {
        dag[i] = (double*)malloc(sizeof(double)*numOftasks);
    }

    // A data structure to store the computed upper ranks of each task (represented by the index)
    upperRank = (double*)malloc(sizeof(double)*numOftasks);
    for(i = 0; i < numOftasks; ++i) {
        upperRank[i] = -1;
    }

    // An array that stores the sorted tasks in decreasing order as per the upper ranks
    // this is equivalent to a topological sort of a DAG
    sortedTasks = (int*)malloc(sizeof(int)*numOftasks);

    // Initializing the process schduler data structure used to store the scheduling information
    processorSchedule = (ProcessorSchedule**)malloc(sizeof(ProcessorSchedule*)*numOfProcessors);
    for(i = 0; i < numOfProcessors; ++i) {
        processorSchedule[i] = (ProcessorSchedule*)malloc(sizeof(ProcessorSchedule)*numOfProcessors);
        processorSchedule[i]->size = 0;
        processorSchedule[i]->tasks = NULL;
    }

    // initializing the AFT array
    AFTs = (double*)malloc(sizeof(double)*numOftasks);
    for(i = 0; i < numOftasks; ++i) {
        AFTs[i] = -1;
    }

    // Initializing the processor array
    proc = (int*)malloc(sizeof(int)*numOftasks);
    for(i = 0; i <numOftasks; ++i) {
        proc[i] = -1;
    }

    // getting the input fot the computation costs from the input file
    for(i = 0; i < numOftasks; ++i) {
        for(j = 0; j < numOfProcessors; ++j) {
            fscanf(fp, "%lf", &computationCost[i][j]);
        }
    }

    // getting the input fot the communication costs from the input file
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
    if(upperRank[task] != -1) {
        return upperRank[task];
    }
    double max = 0.0;
    int i;
    for(i = 0; i < numOftasks; ++i) {
        if(dag[task][i] != -1) {
            double cost;
            if(upperRank[i] != -1) {
                cost = dag[task][i] + upperRank[i];
            } else {
                cost = dag[task][i] + calculateUpperRank(i);
            }
            if(cost > max) {
                max = cost;
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
        avail(processorSchedule[processor]->tasks, 
              processorSchedule[processor]->size, 
              computationCost[task][processor], &earliestTime);
    }
    //printf("Earliest Available %g\n", earliestTime);
    double max = DBL_MIN;
    int i;
    for(i = 0; i < numOftasks; ++i) {
        if(dag[i][task] != -1) {
            if(proc[i] == processor) {
                if(max < AFTs[i]) {
                    max = AFTs[i];
                }  
            } else {
                if(max < AFTs[i] + dag[i][task]) {
                    max = AFTs[i] + dag[i][task];
                }  
            }
            
        }
    }
    *EST = maxDouble(earliestTime, max);
    //printf("EST on %d is %g\n\n", processor+1, *EST);
    return;
}

// HEFT scheduler function
void heft() {
    int i;
    for(i = 0; i < numOftasks; ++i) {
        int task = sortedTasks[i];
        //printf("Schdeuling %d\n", task+1);
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
            proc[task] = processor;
            //printf("Scheduled on %d with AST %g and AFT %g\n", processor+1, 0.0, min);
        } else {
            double minEFT = DBL_MAX;
            double selectedEST = -1;
            int processor;
            int j;
            for(j = 0; j < numOfProcessors; ++j) {
                double EST;
                calculateEST(task, j, &EST);
                if(EST + computationCost[task][j] < minEFT) {
                    minEFT = EST + computationCost[task][j];
                    selectedEST = EST;
                    processor = j;
                }
            }
            processorSchedule[processor]->tasks 
                            = (TaskProcessor*)realloc(processorSchedule[processor]->tasks, 
                                                      sizeof(TaskProcessor)*((processorSchedule[processor]->size) + 1));
            processorSchedule[processor]->tasks[processorSchedule[processor]->size].AST = selectedEST;
            processorSchedule[processor]->tasks[processorSchedule[processor]->size].AFT = minEFT;
            processorSchedule[processor]->tasks[processorSchedule[processor]->size].process = task;
            processorSchedule[processor]->size++;
            AFTs[task] = minEFT;
            proc[task] = processor;
            //printf("Scheduled on %d with AST %g and AFT %g\n", processor+1, selectedEST, minEFT);
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
        TaskProcessor* tasks = processorSchedule[i]->tasks;
        int j;
        for(j = 0; j < processorSchedule[i]->size; ++j) {
            printf("%d\t%d\t%g\t%g\n",
                   i+1,
                   tasks[j].process+1, 
                   tasks[j].AST, 
                   tasks[j].AFT);
        }
    }
    puts("---------------------------");
    double makespan = DBL_MIN;
    for(i = 0; i < numOftasks; ++i) {
        if(makespan < AFTs[i]) {
            makespan = AFTs[i];
        }
    }
    printf("Makespan = %g\n", makespan);
     puts("---------------------------");
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
    free(proc);
    free(sortedTasks);
}

// Entry point of the program
int main(int argc, char** argv) {
    if(argc <= 1 || argc > 2) {
        puts("Please provide just one file in the specified format.");
    }
    puts("START");
    puts("---------------------------");
    initEnvironment(argv[1]);
    displayInput();
    calculateAndDisplayRanks();
    heft();
    displaySchedule();
    freeSpace();
    puts("END");
    puts("---------------------------");
    return 0;
}