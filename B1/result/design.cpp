#include <systemc.h>
#include <iostream>

// Processing Element (PE) module
SC_MODULE(PE) {
    sc_in<bool> clk;
    sc_in<bool> rst;
    
    sc_in<int> x_in;       // Input data
    sc_in<int> y_in;       // Partial sum input
    sc_out<int> y_out;     // Forward partial sum to next PE

    int weight;            // Fixed weight for this PE

    // Internal registers
    int mult_result_reg;   // Register for multiplication result
    int y_reg;             // Register for output data
  
  	int sum;
  	int x;

    // Constructor
    SC_CTOR(PE) {
        SC_METHOD(compute);
        sensitive << clk.pos();
        sensitive << rst.pos();
        
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
            mult_result_reg = 0;
            y_reg = 0;
            y_out.write(0);
        } else if (clk.read()) {
            
            // Pipeline stage 1: Second input data register (extra delay for x path)
            x = x_in.read();
            // Pipeline stage 3: Perform multiplication and store in register
            mult_result_reg = x * weight;
            
            // Pipeline stage 4: Register output data
          	y_reg = y_in.read();
          
          	// Add multiplication result with output data
            sum = mult_result_reg + y_reg;
            
            // Forward data and partial sum
            y_out.write(sum);
        }
    }
};

// Top-level systolic array module
SC_MODULE(B1_SystolicArray) {
    sc_in<bool> clk;
    sc_in<bool> rst;
    
    // Input ports
    sc_in<int> x_in;   // Input data stream
    sc_in<int> y_in;   // Initial partial sum (usually 0)
    
    // Output ports
    sc_out<int> y_out; // Final result
    
    // Processing elements
    PE* pe1;
    PE* pe2;
    PE* pe3;
    
    // Internal signals for connecting PEs
    sc_signal<int> y_sig1, y_sig2;  // y connections between PEs
    
    // Constructor
    SC_CTOR(B1_SystolicArray) {
        // Create the processing elements
        pe1 = new PE("PE1");
        pe2 = new PE("PE2");
        pe3 = new PE("PE3");
        
        // Set weights for each PE (w1, w2, w3) - note the order is reversed compared to W1
        pe1->set_weight(1); // w3 (leftmost PE)
        pe2->set_weight(2); // w2
        pe3->set_weight(3); // w1 (rightmost PE)
        
        // Connect clock and reset to all PEs
        pe1->clk(clk);
        pe1->rst(rst);
        pe2->clk(clk);
        pe2->rst(rst);
        pe3->clk(clk);
        pe3->rst(rst);
        
        // Connect the PEs in series
        // Both x and y flow from left to right: PE1 -> PE2 -> PE3
        
        // Input data connections (x flows from left to right: PE1->PE2->PE3)
        pe1->x_in(x_in);       // Input x goes to leftmost PE (PE1)
        pe2->x_in(x_in);     // PE2 input from PE1
        pe3->x_in(x_in);     // PE3 input from PE2
        
        // Partial sum connections (y also flows from left to right: PE1->PE2->PE3)
        pe1->y_in(y_in);       // Initial y (usually 0) goes to leftmost PE (PE1)
        pe1->y_out(y_sig1);    // PE1 output to PE2
        pe2->y_in(y_sig1);     // PE2 input from PE1
        pe2->y_out(y_sig2);    // PE2 output to PE3
        pe3->y_in(y_sig2);     // PE3 input from PE2
        pe3->y_out(y_out);     // PE3 output is the final result
    }
    
    // Destructor
    ~B1_SystolicArray() {
        delete pe1;
        delete pe2;
        delete pe3;
    }
};
