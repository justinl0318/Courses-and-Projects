for ((i=0; i<10; i++))
do
    # Copy the input file
    cp my_testcases/my_instructions_${i}.txt instruction.txt

    # Compile Verilog code
    iverilog -o cpu code/src/*.v code/supplied/*.v

    # Run the simulation
    vvp cpu > my_out/output_${i}.txt
done