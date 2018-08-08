# Generator

A random graph generator has been provided for you. This generates a fairly simply DAG based on the user input. The topological sort will give you a graph with rank 1 at each level and the vertices sorted in increasing order. This mean the graph will be long and thin.

## Getting Started

To get you a copy of the project up and running on your local machine for development and testing purposes, please follow the steps provided.

### Prerequisites

Please make sure your system has `gcc` and `python` (preferably 2x) installed. You can verify by using the following commands.

```
$ gcc --version
```

```
$ python --version
```

## Running

Change your directory to the git repository using

```
$ cd path/to/the/repository/finalproject-cpu-scheduler
```

You can now use the python files provided to compile and run the examples.

```
$ python generate.py
$ python run.py
$ Enter number of tasks: 10
$ Enter number of processors: 3
$ Enter the file name you want to save the graph in: your_file_name
$ Graph generated in /path/to/the/repository/finalproject-cpu-scheduler/examples/your_file_name
```

While the code is designed to work across all platforms, the python files for compiling and running the programs are for linux-based systems. If you are on Windows, please use the appropriate `gcc` commands to generate the repective executable file. Then run `exec N M your_file_name` to run the generator.

### Running the output with HEFT

Simply can add `os.system('./heft ./examples/your_file_name')` at the end of `run.py` file in the main folder. Compile and run. You should see the result of your input.
