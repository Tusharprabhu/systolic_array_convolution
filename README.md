# systolic_array_convolution

This repository contains the design and implementation of a systolic array for convolution operations.

## Contents

* **B1/**: This directory contains the design and testbench files (`design.cpp`, `testbench.cpp`) for a basic systolic array, along with a script to run the simulation (`run.sh`) and a note file (`note.txt`)[cite: 1, 2, 3, 4].
* **B2/**: Similar to B1, this directory contains the design, testbench, run script, and note for another systolic array implementation[cite: 5, 6, 7, 8].
* **F/**:  This directory includes design and testbench files (`design.cpp`, `testbench.cpp`) for a systolic array with an adder module, a run script (`run.sh`), and a note file (`note.txt`)[cite: 9, 10, 11, 12].
* **R1/**: Contains design and testbench files (`design.cpp`, `testbench.cpp`) for a systolic array with registers, along with a run script (`run.sh`) and a note (`note.txt`)[cite: 13, 14].
* **R2/**:  This directory includes design and testbench files (`design.cpp`, `testbench.cpp`) for another systolic array design with registers, a run script (`run.sh`), and a note (`note.txt`). The `design.cpp` file in the main directory (`/design.cpp`) also corresponds to this R2 design.
* **W1/**:  Contains design and testbench files (`design.cpp`, `testbench.cpp`) for a systolic array where inputs are provided every two cycles, along with a run script (`run.sh`) and a note (`note.txt`).
* **W2/**:  Similar to W1, this directory contains design and testbench files (`design.cpp`, `testbench.cpp`) for a systolic array with input delays, a run script (`run.sh`), and a note (`note.txt`).
* **design.cpp**: Contains the implementation of the R2 systolic array design.
* **testbench.cpp**: Contains a testbench for verifying the R2 systolic array design with convolution operations. Note that there is also a `testbhench.cpp` file.

## Recent Activity

The codebase was recently refactored to improve readability and maintainability.

## Getting Started

To use the code, you'll need a SystemC environment. Here's a general outline:

1.  Clone the repository:

    ```bash
    git clone [https://github.com/Tusharprabhu/systolic_array_convolution.git](https://github.com/Tusharprabhu/systolic_array_convolution.git)
    cd systolic_array_convolution
    ```

2.  Navigate to the specific directory (e.g., `B1/result.zip/`) you want to run.

3.  Set up your SystemC environment.  You might need to set environment variables like `PATH`, `LIBRARY_PATH`, `LD_LIBRARY_PATH`, and `CPATH` as shown in the provided `run.sh` scripts[cite: 3, 7, 11].  For example:

    ```bash
    export PATH=/usr/bin:/bin:/tool/pandora64/bin
    export LIBRARY_PATH=:/playground_lib/systemc-2.3.3/lib-linux64
    export LD_LIBRARY_PATH=/playground_lib/systemc-2.3.3/lib-linux64
    export CPATH=:/playground_lib/systemc-2.3.3/include
    ```

4.  Compile the design and testbench files.  You can typically do this with `g++`.  For example:

    ```bash
    g++ -o design design.cpp testbench.cpp -I/playground_lib/systemc-2.3.3/include -L/playground_lib/systemc-2.3.3/lib-linux64 -lsystemc
    ```

5.  Run the executable:

    ```bash
    ./design
    ```

* Note that the compilation command might need adjustments based on your SystemC installation path.  The `run.sh` scripts in each directory provide the exact commands used in the development environment[cite: 3, 7, 11].

## Usage

Each directory (`B1`, `B2`, `F`, `R1`, `R2`, `W1`, `W2`) contains a different systolic array implementation.  The `design.cpp` files implement the systolic array, and the `testbench.cpp` files provide test inputs and check the outputs.  The `note.txt` files may contain specific details about each implementation[cite: 1, 5, 9, 13].

* For example, the `B1` directory implements a basic systolic array with processing elements (PEs) that perform multiplication and pass partial sums[cite: 1, 2, 3, 4].
* The `R2` directory implements a systolic array with registers to hold intermediate values.
* The `W1` and `W2` directories demonstrate systolic arrays with specific input timing considerations.

To understand the specifics of each implementation, refer to the `design.cpp`, `testbench.cpp`, and `note.txt` files within each directory.

## Contributors
Sujal yatin , Sushant Naik , Vijith M ,Sharn L



