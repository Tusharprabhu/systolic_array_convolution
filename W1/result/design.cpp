#include <systemc.h>
#include <iostream>

// Processing Element (PE) module
SC_MODULE(PE) {
    sc_in<bool> clk;
    sc_in<bool> rst;
  
    sc_in<int> x_in;       // Input data
    sc_in<int> y_in;       // Partial sum input
    sc_out<int> x_out;     // Forward data to next PE
    sc_out<int> y_out;     // Forward partial sum to previous PE

    int weight;            // Fixed weight for this PE

    // Internal registers
    int x_reg;             // Register for input data
    int mult_result_reg;   // Register for multiplication result
    int y_reg;             // Register for output data
  
  	int sum;

    // Constructor
    SC_CTOR(PE) {
        SC_METHOD(compute);
        sensitive << clk.pos();
        sensitive << rst.pos();
        
        x_reg = 0;
        mult_result_reg = 0;
        y_reg = 0;
    }

    // Set the weight for this PE
    void set_weight(int w) {
        weight = w;
    }

    // Compute function implementing the PE logic
    void compute() {
        if (rst.read()) {
            // Reset all registers
            x_reg = 0;
            mult_result_reg = 0;
            y_reg = 0;
            x_out.write(0);
            y_out.write(0);
        } else if (clk.read()) {
            // Pipeline stage 1: Register input data
            x_reg = x_in.read();
            
            // Pipeline stage 2: Perform multiplication and store in register
            mult_result_reg = x_reg * weight;
            
            // Pipeline stage 3: Register output data
            y_reg = y_in.read();
          
          	// Add multiplication result with output data
          	sum = mult_result_reg + y_reg;
            
            // Forward data and partial sum
            x_out.write(x_reg);
            y_out.write(sum);
        }
    }
};

// Top-level systolic array module
SC_MODULE(W1_SystolicArray) {
    sc_in<bool> clk;
    sc_in<bool> rst;
    
    // Input ports
    sc_in<int> x_in;   // Input data stream
    sc_in<int> y_in;   // Initial partial sum (usually 0)
    
    // Output ports
    sc_out<int> x_out; // Forwarded data (not used)
    sc_out<int> y_out; // Final result
    
    // Processing elements
    PE* pe1;
    PE* pe2;
    PE* pe3;
    
    // Internal signals for connecting PEs
    sc_signal<int> x_sig1, x_sig2;  // x connections between PEs
    sc_signal<int> y_sig1, y_sig2;  // y connections between PEs
    
    
    // Constructor
    SC_CTOR(W1_SystolicArray) {
        // Create the processing elements
        pe1 = new PE("PE1");
        pe2 = new PE("PE2");
        pe3 = new PE("PE3");
        
        // Set weights for each PE (w1, w2, w3)
        pe1->set_weight(1); // w1
        pe2->set_weight(2); // w2
        pe3->set_weight(3); // w3
        
        // Connect clock and reset to all PEs
        pe1->clk(clk);
        pe1->rst(rst);
        pe2->clk(clk);
        pe2->rst(rst);
        pe3->clk(clk);
        pe3->rst(rst);
        
        // Connect the input registers and PEs
        // PE3 (rightmost) -> PE2 -> PE1 (leftmost) for data flow
        // PE1 (leftmost) -> PE2 -> PE3 (rightmost) for partial sum flow
        
        // Input data connections (x flows from right to left: PE3->PE2->PE1)
        pe3->x_in(x_in);       // Input x goes to rightmost PE (PE3)
        pe3->x_out(x_sig2);    // PE3 output to PE2
        pe2->x_in(x_sig2);     // PE2 input from PE3
        pe2->x_out(x_sig1);    // PE2 output to PE1
        pe1->x_in(x_sig1);     // PE1 input from PE2
        pe1->x_out(x_out);     // PE1 output (typically not used)
        
        // Partial sum connections (y flows from left to right: PE1->PE2->PE3)
        pe1->y_in(y_in);       // Initial y (usually 0) goes to leftmost PE (PE1)
        pe1->y_out(y_sig1);    // PE1 output to PE2
        pe2->y_in(y_sig1);     // PE2 input from PE1
        pe2->y_out(y_sig2);    // PE2 output to PE3
        pe3->y_in(y_sig2);     // PE3 input from PE2
        pe3->y_out(y_out);     // PE3 output is the final result

    }
    
    // Destructor
    ~W1_SystolicArray() {
        delete pe1;
        delete pe2;
        delete pe3;
    }
};
