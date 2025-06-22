#include <systemc.h>
#include "design.cpp"

// Testbench for the R1 Systolic Array
SC_MODULE(Testbench) {
    sc_out<bool> clk;
    sc_out<bool> rst;
    
    // Input ports
    sc_out<int> x_in;   // Input data stream
    sc_out<int> w_in;
    sc_out<bool> tag_in;
    
    // Output ports
    sc_in<int> x_out; // Forwarded data
    sc_in<int> w_out;
    sc_in<bool> tag_out;
    sc_in<int> y_out; // Final result
    
    SC_CTOR(Testbench) {
        SC_THREAD(clock_gen);
        SC_THREAD(stimulus);
        SC_THREAD(monitor);
    }
    
    void clock_gen() {
        clk.write(false);
        while(1) {
            wait(5, SC_NS);
            clk.write(!clk.read());
        }
    }
    
    void stimulus() {
        // Reset the system
        rst.write(true);
        x_in.write(0);  // Initialize x input to 0
      	tag_in.write(0);
      	w_in.write(0);
        wait(15, SC_NS);
        rst.write(false);
        
        // Test sequence - providing inputs every cycle (not every 2 cycles like W1)
        // We'll send x1, x2, x3, x4, x5 values
        
        // Cycle 1:
        x_in.write(0);  // x1 = 0
      	w_in.write(2);
      	tag_in.write(0);
        wait(10, SC_NS); // Wait 1 clock cycle
      
        // Cycle 2:
        x_in.write(0);  // x1 = 0
      	w_in.write(3);
      	tag_in.write(0);
        wait(10, SC_NS); // Wait 1 clock cycle
        
        // Cycle 3: Send x1
        x_in.write(1);  // x1 = 1
      	w_in.write(1);
      	tag_in.write(1);
        wait(10, SC_NS); // Wait 1 clock cycle
      
        // Cycle 4: 
        x_in.write(2);  // x1 = 0
      	w_in.write(2);
      	tag_in.write(0);
        wait(10, SC_NS); // Wait 1 clock cycle
        
        // Cycle 5: Send x2
        x_in.write(3);  // x2 = 2
      	w_in.write(3);
      	tag_in.write(0);
        wait(10, SC_NS); // Wait 1 clock cycle
      
        // Cycle 6: Stall
        x_in.write(4);  // x1 = 0
      	w_in.write(1);
      	tag_in.write(1);
        wait(10, SC_NS); // Wait 1 clock cycle
        
        // Cycle 7: Send x3
        x_in.write(5);  // x3 = 3
      	w_in.write(2);
      	tag_in.write(0);
        wait(10, SC_NS); // Wait 1 clock cycle
      
        // Cycle 8: Stall
        x_in.write(0);  // x1 = 0
      	w_in.write(3);
      	tag_in.write(0);
        wait(10, SC_NS); // Wait 1 clock cycle
        
        // Cycle 9: Send x4
        x_in.write(0);  // x4 = 4
      	w_in.write(1);
      	tag_in.write(1);
        wait(10, SC_NS); // Wait 1 clock cycle
      
//         // Cycle 10: Stall
//         x_in.write(0);  // x1 = 0
//       	w_in.write(2);
//       	tag_in.write(0);
//         wait(10, SC_NS); // Wait 1 clock cycle
        
//         // Cycle 11: Send x5
//         x_in.write(0);  // x5 = 5
//       	w_in.write(3);
//       	tag_in.write(0);
//         wait(10, SC_NS); // Wait 1 clock cycle
      
//         // Cycle 12: Stall
//         x_in.write(0);  // x1 = 0
//       	w_in.write(1);
//       	tag_in.write(1);
//         wait(10, SC_NS); // Wait 1 clock cycle
      
//         // Cycle 13: Stall
//         x_in.write(0);  // x = 0
//       	w_in.write(2);
//       	tag_in.write(0);
//         wait(10, SC_NS); // Wait 1 clock cycle
      
//         // Cycle 14: Stall
//         x_in.write(0);  // x1 = 0
//       	w_in.write(3);
//       	tag_in.write(0);
//         wait(10, SC_NS); // Wait 1 clock cycle
      
//         // Cycle 15: Stall
//         x_in.write(0);  // x = 0
//       	w_in.write(1);
//       	tag_in.write(1);
//         wait(10, SC_NS); // Wait 1 clock cycle
        
        // Send a few more inputs to flush the pipeline
        x_in.write(0);
      	w_in.write(0);
      	tag_in.write(0);
        wait(100, SC_NS);
        
        // End simulation
        sc_stop();
    }
    
    void monitor() {
        cout << "Time\tx_in\ty_out" << endl;
        while(true) {
            wait(clk.posedge_event());
            cout << sc_time_stamp() << "\t" 
                 << x_in.read() << "\t" 
                 << y_out.read() << endl;
        }
    }
};

// Main function
int sc_main(int argc, char* argv[]) {
    // Signals for connecting modules

    sc_signal<bool> clk_sig, rst_sig, tag_in_sig, tag_out_sig;
    sc_signal<int> x_in_sig, w_in_sig, x_out_sig, w_out_sig, y_out_sig;
    
    // Instantiate modules
    R2_SystolicArray systolic_array("R2_SystolicArray");
    Testbench tb("Testbench");
    
    // Connect signals to the R1_SystolicArray
    systolic_array.clk(clk_sig);
    systolic_array.rst(rst_sig);
    systolic_array.x_in(x_in_sig);
    systolic_array.w_in(w_in_sig);
    systolic_array.tag_in(tag_in_sig);
    systolic_array.x_out(x_out_sig);
    systolic_array.w_out(w_out_sig);
    systolic_array.tag_out(tag_out_sig);
    systolic_array.y_out(y_out_sig);
    
    // Connect signals to the Testbench
    tb.clk(clk_sig);
    tb.rst(rst_sig);
    tb.x_in(x_in_sig);
    tb.w_in(w_in_sig);
    tb.tag_in(tag_in_sig);
    tb.x_out(x_out_sig);
    tb.w_out(w_out_sig);
    tb.tag_out(tag_out_sig);
    tb.y_out(y_out_sig);
    
    // Create VCD trace file
    sc_trace_file* tf = sc_create_vcd_trace_file("r2_systolic_array");
    
    // Add signals to trace
    sc_trace(tf, clk_sig, "clk");
    sc_trace(tf, rst_sig, "rst");
    sc_trace(tf, x_in_sig, "x_in");
    sc_trace(tf, w_in_sig, "w_in");
    sc_trace(tf, tag_in_sig, "tag_in");
    sc_trace(tf, y_out_sig, "y_out");
    sc_trace(tf, x_out_sig, "x_out");
    sc_trace(tf, w_out_sig, "w_out");
    sc_trace(tf, tag_out_sig, "tag_out");
    
    // Start simulation
    cout << "Starting R2 systolic array simulation..." << endl;
    sc_start();
    
    // Close trace file
    sc_close_vcd_trace_file(tf);
    
    cout << "Simulation completed." << endl;
    
    return 0;
}