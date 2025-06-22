#include <systemc.h>

// Processing Element (PE) module
SC_MODULE(PE) {
    sc_in<bool> clk;
    sc_in<bool> rst;
  
    sc_in<int> x_in;       // Input data
    sc_in<int> w_in;       // Moving weight input
    sc_in<bool> tag_in;    // Input tag bit
  
    sc_out<int> x_out;     // Output x
    sc_out<int> w_out;     // Moving weight output
    sc_out<bool> tag_out;  // Output tag bit
  
    sc_out<int> y_out;     // Output data

    // Internal registers
    int w_reg;             // Register for input weight
    int tag_reg;           // Register for input tag
    int x;
    int y;                 // Output accumulator
    int x_reg;             // Added x_reg
    int mult_result_reg;   // Added mult_result_reg

    // Constructor
    SC_CTOR(PE) {
        SC_METHOD(compute);
        sensitive << clk.pos();
        sensitive << rst.pos();
        
        w_reg = 0;
        tag_reg = 0;
        y = 0;
        x_reg = 0;         // Initialize added registers
        mult_result_reg = 0;
    }
  
    // Compute function implementing the PE logic
    void compute() {
        if (rst.read()) {
            // Reset all registers
            x_reg = 0;
            mult_result_reg = 0;
            w_reg = 0;
            tag_reg = 0;
            y = 0;
            y_out.write(0);
        } else if (clk.read()) {
            if (tag_in.read()){
              // If tag is high, ouput y_out = y, reset PE
              y_out.write(y);
              y = 0;
              w_reg = 0;
              tag_reg = 0;
            } else {
              // If tag is low, ouput y_out = 0
              y_out.write(0);
            }
            
            // Register inputs
            x = x_in.read();
            w_reg = w_in.read();
            tag_reg = tag_in.read();
            y = x*w_reg + y;
            
            // Forward weight and tag signals
            w_out.write(w_reg);
            tag_out.write(tag_reg);
            x_out.write(x);
        }
    }
};

// Systolic Output Logic module with multiplexers
SC_MODULE(OutputLogic) {
    sc_in<bool> clk;
    sc_in<bool> rst;
    
    // Inputs from PEs
    sc_in<int> y_out1;  // From PE1 (leftmost)
    sc_in<int> y_out2;  // From PE2 (middle)
    sc_in<int> y_out3;  // From PE3 (rightmost)
    
    // Final output
    sc_out<int> y_out;
    
    // Three registers for systolic output
    int reg1, reg2, reg3;
    
    // Constructor
    SC_CTOR(OutputLogic) {
        // Initialize registers
        reg1 = 0;
        reg2 = 0;
        reg3 = 0;
        
        // Process sensitive to clock and reset
        SC_METHOD(process_output);
        sensitive << clk.pos();
        sensitive << rst.pos();
    }
    
    // Process output function using multiplexers
    void process_output() {
        if (rst.read()) {
            // Reset all registers
            reg1 = 0;
            reg2 = 0;
            reg3 = 0;
            y_out.write(0);
        } else if (clk.read()) {
            // Update registers using multiplexers
            // For each stage, select PE output if it's non-zero, otherwise select previous register
            
            // First register (can only get input from PE1)
            reg1 = y_out1.read();
            
            // Second register (choose between reg1 and PE2)
            int pe2_val = y_out2.read();
            reg2 = (pe2_val != 0) ? pe2_val : reg1;
            
            // Third register (choose between reg2 and PE3)
            int pe3_val = y_out3.read();
            reg3 = (pe3_val != 0) ? pe3_val : reg2;
            
            // Write final output
            y_out.write(reg3);
        }
    }
};

// Top-level systolic array module
SC_MODULE(R1_SystolicArray) {
    sc_in<bool> clk;
    sc_in<bool> rst;
    
    // Input ports
    sc_in<int> x_in;   // Input data stream
    sc_in<int> w_in;
    sc_in<bool> tag_in;
    
    // Output ports
    sc_out<int> x_out; // Forwarded data
    sc_out<int> w_out;
    sc_out<bool> tag_out;
    sc_out<int> y_out; // Final result
    
    // Processing elements
    PE* pe1;
    PE* pe2;
    PE* pe3;
    
    // Output logic module
    OutputLogic* output_logic;
    
    // Internal signals for connecting PEs
    sc_signal<int> x_sig1, x_sig2;  // x connections between PEs
    sc_signal<int> w_sig1, w_sig2;  // w connections between PEs
    sc_signal<bool> tag_sig1, tag_sig2;  // tag connections between PEs
    
    // Signals for PE outputs
    sc_signal<int> y_out1, y_out2, y_out3;
    
    // Constructor
    SC_CTOR(R1_SystolicArray) {
        // Create the processing elements
        pe1 = new PE("PE1");
        pe2 = new PE("PE2");
        pe3 = new PE("PE3");
        
        // Create output logic module
        output_logic = new OutputLogic("OutputLogic");
        
        // Connect clock and reset to all modules
        pe1->clk(clk);
        pe1->rst(rst);
        pe2->clk(clk);
        pe2->rst(rst);
        pe3->clk(clk);
        pe3->rst(rst);
        output_logic->clk(clk);
        output_logic->rst(rst);
        
        // Connect the input registers and PEs
        pe3->w_in(w_in);       // Input w goes to rightmost PE (PE3)
        pe3->w_out(w_sig2);    // PE3 output to PE2
        pe2->w_in(w_sig2);     // PE2 input from PE3
        pe2->w_out(w_sig1);    // PE2 output to PE1
        pe1->w_in(w_sig1);     // PE1 input from PE2
        pe1->w_out(w_out);     // PE1 output
        
        pe1->x_in(x_in);       // Initial x goes to leftmost PE (PE1)
        pe1->x_out(x_sig1);    // PE1 output to PE2
        pe2->x_in(x_sig1);     // PE2 input from PE1
        pe2->x_out(x_sig2);    // PE2 output to PE3
        pe3->x_in(x_sig2);     // PE3 input from PE2
        pe3->x_out(x_out);     // PE3 output to external port
      
        pe3->tag_in(tag_in);       // Input tag goes to rightmost PE (PE3)
        pe3->tag_out(tag_sig2);    // PE3 output to PE2
        pe2->tag_in(tag_sig2);     // PE2 input from PE3
        pe2->tag_out(tag_sig1);    // PE2 output to PE1
        pe1->tag_in(tag_sig1);     // PE1 input from PE2
        pe1->tag_out(tag_out);     // PE1 output to external port
        
        // Connect PE outputs to output logic module
        pe1->y_out(y_out1);
        pe2->y_out(y_out2);
        pe3->y_out(y_out3);
        output_logic->y_out1(y_out1);
        output_logic->y_out2(y_out2);
        output_logic->y_out3(y_out3);
        output_logic->y_out(y_out);
    }
    
    // Destructor
    ~R1_SystolicArray() {
        delete pe1;
        delete pe2;
        delete pe3;
        delete output_logic;
    }
};