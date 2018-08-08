import os

# Compile our heft program
os.system('gcc heft.c -o heft')

# Run Example 1
os.system('./heft ./examples/1')

# Run Example 2
os.system('./heft ./examples/2')
