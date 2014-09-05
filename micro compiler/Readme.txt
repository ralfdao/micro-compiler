This is Yu Liu's compiler project (written in C++, used flex & bison)

To compile the project, open a terminal, cd to the directory where "Makefile" locates. 

use "make" command to generate the "Micro" compiler, which locates at directory "./build."

You can also use "make clean" command to clean the generated files.

Two testcases are included in directory "./testcases", which calculate the factorial and 
fibonacci series according to your input. A 4-register tiny simulator "tiny4R" is included.

To run the compiler, copy "Micro" from directory "./build" to "./testcases". use cd to 
make "./testcases" the current directory. 

use command "Micro factorial.micro > fact.out" to compile file "factorial.micro" into "fact.out" 
which contains tiny codes (you can open the file to see) that run on the simulator "tiny4R"

To try it out, use command "tiny4R fact.out." An example is given below:

Please enter an integer number: 6
720

STATISTICS _____________________________
   #Instructions:225
    (move-ops mem:0, reglit:150)
    ( int-ops mem:12, reglit:63)
    (  fp-ops mem:0, reglit:0)
   Memory Usage (mem:12,reg:183)
      register-use[86,39,44,14]
   Total Cycles = 284
OTHER STATSvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
    (int-ops:25, fp-ops:0)
    (branches:26)
      peephole-ops:0

Have fun!