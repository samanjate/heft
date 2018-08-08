// A structure to store the scheduled tasks.
typedef struct TaskProcessor {
    int process;
    double AST;
    double AFT;
} TaskProcessor;

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

// A function to find an avaialble slot on a processors which has the tasks schedules as provided.
// The number of tasks already scheduled is equal to the size provided.
// This will be the first slot available
void avail(TaskProcessor* tasks, int size, double computationCost, double* earliestTime) {
    if(size == 0) {
        *earliestTime = 0;
        return;
    }
    qsort((void*) tasks, size, sizeof(TaskProcessor), comparator);
    *earliestTime = tasks[size-1].AFT;
    return;
}
