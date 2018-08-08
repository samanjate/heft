int numOftasks; // number of tasks to be schduled
int numOfProcessors; // bounded number of heteroenous processors
double** dag; // adjacency matrix form for the input DAG
double** computationCost; // the cost of processes on each processor table
double* upperRank; // the calculated upper ranks of each process
int* sortedTasks; // the sorted indexes of each processor based on their upper rank
double* AFTs; // to store the actual finish times of the task (represented by the index)
int* proc; // the processor that the task is scheduled on (represented by the index)

// A structure to store the scheduled tasks.
typedef struct TaskProcessor {
    int process;
    double AST;
    double AFT;
} TaskProcessor;

// A structure that keeps track of the tasks scheduled processor wise.
typedef struct ProcessorSchedule {
    int size;
    TaskProcessor* tasks;
} ProcessorSchedule;

ProcessorSchedule** processorSchedule;

// This a comparator used by the qsort function to sort the tasks
// based on their actual start times.
int comparator(const void* v1, const void* v2) {
    TaskProcessor* p1 = (TaskProcessor*) v1;
    TaskProcessor* p2 = (TaskProcessor*) v2;
    return (p1 -> AST) - (p2 -> AST);
}

// a funciton to the returns the greater of the two provided 
// integer values
int max(int n, int m) {
    return n > m ? n : m;
}

// a funciton to the returns the greater of the two provided 
// double values
double maxDouble(double n, double m) {
    return n > m ? n : m;
}
