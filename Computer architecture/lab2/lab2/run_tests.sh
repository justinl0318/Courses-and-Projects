#!/bin/bash

# Assuming you are in the directory where your script is located
# Change the paths accordingly if you are in a different directory

# Run the test cases and compare outputs
for ((i=0; i<10; i++))
do
    # Copy the input file
    cp my_testcases/my_instructions_${i}.txt instruction.txt

    # Compile Verilog code
    iverilog -o cpu code/src/*.v code/supplied/*.v

    # Run the simulation
    vvp cpu > simulation_output.txt

    # Compare the output with the expected output
    diff output.txt my_out/output_${i}.txt > /dev/null

    # Check the result of the diff command
    if [ $? -eq 0 ]; then
        echo "Testcase $i: Same"
    else
        echo "Testcase $i: Wrong"
    fi
done
