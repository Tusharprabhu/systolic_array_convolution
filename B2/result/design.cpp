#include <systemc.h>

// Processing Element (PE) module
SC_MODULE(PE) {
    sc_in<bool> clk;
    sc_in<bool> rst;
    
    sc_in<int> x_in;       // Input data
    sc_in<int> w_in;       // Moving weight input
    sc_in<bool> tag_in;    // Input tag bit
  
    sc_out<int> w_out;     // Moving weight output
    sc_out<bool> tag_out;  // Output tag bit
  
    sc_out<int> y_out;     // Output data

    // Internal registers
    int w_reg;             // Register for input weight
    int tag_reg;           // Register for input tag
    int x;
    int y;                 // Output accumulator

    // Constructor
    SC_CTOR(PE) {
        SC_METHOD(compute);
        sensitive << clk.pos() << rst.pos();
        
        w_reg = 0;
        tag_reg = 0;
        y = 0;
    }

    // Compute function implementing the PE logic
    void compute() {
        if (rst.read()) {
            // Reset all registers
            w_reg = 0;
            tag_reg = 0;
            y = 0;
            y_out.write(0);
        } else if (clk.event() && clk.read() == true) {
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
        }
    }
};

// Combinational multiplexer module for selecting one of the three y signals
SC_MODULE(YMux) {
    sc_in<int> y1;
    sc_in<int> y2;
    sc_in<int> y3;
    sc_out<int> y_out;

    void do_mux() {
        int out_val = 0;
        if (y1.read() != 0)
            out_val = y1.read();
        else if (y2.read() != 0)
            out_val = y2.read();
        else if (y3.read() != 0)
            out_val = y3.read();
        else
            out_val = 0;
        
        y_out.write(out_val);
    }

    SC_CTOR(YMux) {
        SC_METHOD(do_mux);
        sensitive << y1 << y2 << y3;
    }
};

// Top-level systolic array module
SC_MODULE(B2_SystolicArray) {
    sc_in<bool> clk;
    sc_in<bool> rst;
    
    // Input port for data stream
    sc_in<int> x_in;
    
    // Final output port
    sc_out<int> y_out;
    
    // Processing elements
    PE* pe1;
    PE* pe2;
    PE* pe3;
    
    // Internal signals for connecting PEs
    sc_signal<int> w_sig1, w_sig2, w_sig3;    // Weight signals between PEs
    sc_signal<bool> tag_sig1, tag_sig2, tag_sig3;  // Tag signals between PEs
    sc_signal<int> y_sig1, y_sig2, y_sig3;      // Internal y signals from each PE
  
    // Multiplexer instance to combine the y signals
    YMux* ymux;
    
    // Constructor
    SC_CTOR(B2_SystolicArray) {
        // Instantiate processing elements
        pe1 = new PE("PE1");
        pe2 = new PE("PE2");
        pe3 = new PE("PE3");
        
        // Connect clock and reset to all PEs
        pe1->clk(clk);
      	pe1->rst(rst);
        pe2->clk(clk);
      	pe2->rst(rst);
        pe3->clk(clk);
        pe3->rst(rst);
        
        // Connect input data to all PEs
        pe1->x_in(x_in);
        pe2->x_in(x_in);
        pe3->x_in(x_in);
      
        // Weight connections (ring structure)
        pe1->w_in(w_sig3);
        pe1->w_out(w_sig1);
        pe2->w_in(w_sig1);
        pe2->w_out(w_sig2);
        pe3->w_in(w_sig2);
        pe3->w_out(w_sig3);
      
        // Tag connections (ring structure)
        pe1->tag_in(tag_sig3);
        pe1->tag_out(tag_sig1);
        pe2->tag_in(tag_sig1);
        pe2->tag_out(tag_sig2);
        pe3->tag_in(tag_sig2);
        pe3->tag_out(tag_sig3);
      
        // Each PE drives its own internal y signal
        pe1->y_out(y_sig1);
        pe2->y_out(y_sig2);
        pe3->y_out(y_sig3);
      
        // Instantiate the combinational multiplexer module
        ymux = new YMux("YMux");
        ymux->y1(y_sig1);
        ymux->y2(y_sig2);
        ymux->y3(y_sig3);
        ymux->y_out(y_out);
      
        // Initialize weight and tag signals
        w_sig3.write(1);
        w_sig1.write(3);
        w_sig2.write(2);
      
        tag_sig3.write(1);
        tag_sig1.write(0);
        tag_sig2.write(0);
    }
    
    // Destructor
    ~B2_SystolicArray() {
        delete pe1;
        delete pe2;
        delete pe3;
        delete ymux;
    }
};