// Code your design here
#include <systemc.h>

// Processing Element (PE) module
SC_MODULE(PE) {
    sc_in<bool> clk;
    sc_in<bool> rst;
    
    sc_in<int> x_in;       // Input data
    sc_out<int> x_out;     // Forward data to next PE
    sc_out<int> z_out;     // Multiplication result output
    
    int weight;            // Fixed weight for this PE
    int x_reg;             // Register for input data
    
    // Constructor
    SC_CTOR(PE) {
        SC_METHOD(compute);
        sensitive << clk.pos();
        sensitive << rst.pos();
        
        x_reg = 0;
    }
    
    // Set the weight for this PE
    void set_weight(int w) {
        weight = w;
    }
    
    // Compute function implementing the PE logic
    void compute() {
        if (rst.read()) {
            x_reg = 0;
            x_out.write(0);
            z_out.write(0);
        } else {
            // Store input data
            x_reg = x_in.read();
            
            // Multiply by weight and output result
            z_out.write(x_reg * weight);
            
            // Forward x to next PE
            x_out.write(x_reg);
        }
    }
};

// Adder module to combine results from all PEs
SC_MODULE(Adder) {
    sc_in<bool> clk;
    sc_in<bool> rst;
    
    sc_in<int> in1;  // Input from PE1
    sc_in<int> in2;  // Input from PE2
    sc_in<int> in3;  // Input from PE3
    
    sc_out<int> sum_out;  // Final sum output
    
    SC_CTOR(Adder) {
        SC_METHOD(compute);
        sensitive << clk.pos();
    }
    
    void compute() {
        if (rst.read()) {
            sum_out.write(0);
        } else {
            // Sum all inputs
            sum_out.write(in1.read() + in2.read() + in3.read());
        }
    }
  
};

// Top-level systolic array module
SC_MODULE(F_SystolicArray) {
    sc_in<bool> clk;
    sc_in<bool> rst;
    
    // Input ports
    sc_in<int> x_in;   // Input data stream
    
    // Output ports
    sc_out<int> x_out; // Output from last PE (added for completeness)
    sc_out<int> y_out; // Final result
    
    // Processing elements
    PE* pe1;
    PE* pe2;
    PE* pe3;
    
    // Adder module
    Adder* adder;
    
    // Internal signals
    sc_signal<int> x_sig1, x_sig2;         // x connections between PEs
    sc_signal<int> z_out1, z_out2, z_out3;  // Multiplication results from PEs
    
    // Constructor
    SC_CTOR(F_SystolicArray) {
        // Create the processing elements
        pe1 = new PE("PE1");
        pe2 = new PE("PE2");
        pe3 = new PE("PE3");
        
        // Create the adder
        adder = new Adder("Adder");
        
        // Set weights for each PE (w1, w2, w3)
        pe1->set_weight(1); // w1
        pe2->set_weight(2); // w2
        pe3->set_weight(3); // w3
        
        // Connect clock and reset to all modules
        pe1->clk(clk);
        pe1->rst(rst);
        pe2->clk(clk);
        pe2->rst(rst);
        pe3->clk(clk);
        pe3->rst(rst);
        adder->clk(clk);
        adder->rst(rst);
        
        // Connect input data (x flows through the system: x_in -> PE3 -> PE2 -> PE1)
        pe3->x_in(x_in);
        pe3->x_out(x_sig2);
        pe2->x_in(x_sig2);
        pe2->x_out(x_sig1);
        pe1->x_in(x_sig1);
        pe1->x_out(x_out); // Connect to top-level output
        
        // Connect multiplication results to adder
        pe1->z_out(z_out1);
        pe2->z_out(z_out2);
        pe3->z_out(z_out3);
        
        // Connect adder inputs and output
        adder->in1(z_out1);
        adder->in2(z_out2);
        adder->in3(z_out3);
        adder->sum_out(y_out);
    }
    
    // Destructor
    ~F_SystolicArray() {
        delete pe1;
        delete pe2;
        delete pe3;
        delete adder;
    }
};