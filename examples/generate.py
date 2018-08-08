import os

# Compile our heft program
os.system('gcc generator.c -o generator')

# Get the number of tasks from user
print "Enter number of tasks:",
tasks = raw_input()

# Get the number of processors from user
print "Enter number of processors:",
processors = raw_input()

# Get the file name from the user
print "Enter the file name you want to save the graph in:",
filename = raw_input()

# Generate the DAG
os.system('./generator ' + tasks + ' ' + processors + ' ' + filename)

print "Graph generated in " + os.getcwd() + "/" + filename
