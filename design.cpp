
#include <systemc.h>
#include <iostream>

// Processing Element (PE) module for R2 design
SC_MODULE(PE_R2) {
    sc_in<bool> clk;
    sc_in<bool> rst;
    sc_in<int> x_in;
    sc_in<int> w_in;
    sc_out<int> x_out;
    sc_out<int> w_out;
    sc_out<int> result;

    int y;
    
    // Constructor
    SC_CTOR(PE_R2) {
        SC_METHOD(compute);
        sensitive << clk.pos();
        sensitive << rst.pos();
        
        y = 0; 
    }
    
    // Set the fixed y value for this PE
    void set_y(int value) {
        y = value;
    }
    void compute() {
        if (rst.read()) {
            x_out.write(0);
            w_out.write(0);
            result.write(0);
        } else if (clk.read()) {
            // Remove y addition for proper convolution
            result.write(w_in.read() * x_in.read());
            x_out.write(x_in.read());
            w_out.write(w_in.read());
        }
    }
};
// Top-level R2 systolic array module
SC_MODULE(R2_SystolicArray) {
    sc_in<bool> clk;
    sc_in<bool> rst;
    
    // Input ports
    sc_in<int> x_in;  
    sc_in<int> w_in;   
    
    // Output ports
    sc_out<int> x_out;  
    sc_out<int> w_out;  
    
    // Result outputs from each PE
    sc_out<int> result1;
    sc_out<int> result2;
    sc_out<int> result3;
    
    // Processing elements
    PE_R2* pe1;
    PE_R2* pe2;
    PE_R2* pe3;
    
    // Internal signals for connecting PEs
    sc_signal<int> x_sig1, x_sig2;  
    sc_signal<int> w_sig1, w_sig2;  
    
    // Constructor
    SC_CTOR(R2_SystolicArray) {
        // Create the processing elements
        pe1 = new PE_R2("PE1");
        pe2 = new PE_R2("PE2");
        pe3 = new PE_R2("PE3");
        
        // Set fixed y values for each PE (y1, y2, y3)
        pe1->set_y(0);
        pe2->set_y(0);
        pe3->set_y(0);
        
        // Connect clock and reset to all PEs
        pe1->clk(clk);
        pe1->rst(rst);
        pe2->clk(clk);
        pe2->rst(rst);
        pe3->clk(clk);
        pe3->rst(rst);
        
        // Connect the PEs in series for data flow (left to right)
        pe1->x_in(x_in);
        pe1->x_out(x_sig1);
        pe2->x_in(x_sig1);
        pe2->x_out(x_sig2);
        pe3->x_in(x_sig2);
        pe3->x_out(x_out);
    
        // Weight flow (w values: 1,2,3,4,5)
        pe1->w_in(w_in);
        pe1->w_out(w_sig1);
        pe2->w_in(w_sig1);
        pe2->w_out(w_sig2);
        pe3->w_in(w_sig2);
        pe3->w_out(w_out);
    
        // Connect result outputs
        pe1->result(result1);
        pe2->result(result2);
        pe3->result(result3);
    }
    
    // Destructor
    ~R2_SystolicArray() {
        delete pe1;
        delete pe2;
        delete pe3;
    }
};