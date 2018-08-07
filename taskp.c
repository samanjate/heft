
// A structure to store the scheduled tasks.
typedef struct TaskProcessor {
    int process;
    double AST;
    double AFT;
} TaskProcessor;

/*
* Merge Intervals C solution by
* https://leetcode.com/problems/merge-intervals/discuss/153162/C-solution-beats-100
*/
int comparator(const void* v1, const void* v2) {
    TaskProcessor* p1 = (TaskProcessor*) v1;
    TaskProcessor* p2 = (TaskProcessor*) v2;
    return (p1 -> AST) - (p2 -> AST);
}

int max(int n, int m) {
    return n > m ? n : m;
}

double maxDouble(double n, double m) {
    return n > m ? n : m;
}

void avail(TaskProcessor* tasks, int size, double computationCost, double* earliestTime) {
    if(size == 0) {
        *earliestTime = 0;
        return;
    }
    qsort((void*) tasks, size, sizeof(TaskProcessor), comparator);
    int curr = 0;
    for(int i = 1; i < size; ++i) {
        if(tasks[i].AST - tasks[curr].AFT >= computationCost) {
            *earliestTime = tasks[curr].AFT;
            return;
        } else {
            curr++;
            tasks[curr] = tasks[i];
        }
    }
    *earliestTime = tasks[curr].AFT;
    return;
}