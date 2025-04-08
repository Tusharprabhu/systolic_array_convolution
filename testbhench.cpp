#include <systemc.h>

#include <iostream>

#include "design.cpp"  // This would contain the design code above
SC_MODULE(R2_Testbench) {
    sc_out<bool> clk;
    sc_out<bool> rst;
    sc_out<int> x_in;
    sc_out<int> w_in;
    sc_in<int> x_out;
    sc_in<int> w_out;
    sc_in<int> result1;
    sc_in<int> result2;
    sc_in<int> result3;
    
    SC_CTOR(R2_Testbench) {
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
    // Reset
    rst.write(true);
    x_in.write(0);
    w_in.write(0);
    wait(15, SC_NS);
    rst.write(false);
    
    // Send data in correct order for convolution
    // First sequence: [1,2,3,4,5]
    // Second sequence: [3,2,1]
    
    w_in.write(1);  // w1
    x_in.write(3);  // x1
    wait(10, SC_NS);
    
    w_in.write(2);  // w2
    x_in.write(2);  // x2
    wait(10, SC_NS);
    
    w_in.write(3);  // w3
    x_in.write(1);  // x3
    wait(10, SC_NS);
    
    w_in.write(4);  // w4
    x_in.write(0);
    wait(10, SC_NS);
    
    w_in.write(5);  // w5
    x_in.write(0);
    wait(10, SC_NS);
    
    // Flush pipeline
    w_in.write(0);
    x_in.write(0);
    wait(50, SC_NS);
    
    sc_stop();
}
    
    void monitor() {
        cout << "Time\tw_in\tx_in\tw_out\tx_out\tresult1\tresult2\tresult3" << endl;
        while(true) {
            wait(clk.posedge_event());
            cout << sc_time_stamp() << "\t"
                 << w_in.read() << "\t"
                 << x_in.read() << "\t"
                 << w_out.read() << "\t"
                 << x_out.read() << "\t"
                 << result1.read() << "\t"
                 << result2.read() << "\t"
                 << result3.read() << endl;
        }
    }
};

// Main function
int sc_main(int argc, char* argv[]) {
    // Signals for connecting modules
    sc_signal<bool> clk_sig, rst_sig;
    sc_signal<int> x_in_sig, w_in_sig;
    sc_signal<int> x_out_sig, w_out_sig;
    sc_signal<int> result1_sig, result2_sig, result3_sig;
    
    // Instantiate modules
    R2_SystolicArray systolic_array("R2_SystolicArray");
    R2_Testbench tb("R2_Testbench");
    
    // Connect signals
    systolic_array.clk(clk_sig);
    systolic_array.rst(rst_sig);
    systolic_array.x_in(x_in_sig);
    systolic_array.w_in(w_in_sig);
    systolic_array.x_out(x_out_sig);
    systolic_array.w_out(w_out_sig);
    systolic_array.result1(result1_sig);
    systolic_array.result2(result2_sig);
    systolic_array.result3(result3_sig);
    
    tb.clk(clk_sig);
    tb.rst(rst_sig);
    tb.x_in(x_in_sig);
    tb.w_in(w_in_sig);
    tb.x_out(x_out_sig);
    tb.w_out(w_out_sig);
    tb.result1(result1_sig);
    tb.result2(result2_sig);
    tb.result3(result3_sig);
    
    // Create VCD trace file
    sc_trace_file* tf = sc_create_vcd_trace_file("r2_systolic_array");
    
    // Add signals to trace
    sc_trace(tf, clk_sig, "clk");
    sc_trace(tf, rst_sig, "rst");
    sc_trace(tf, x_in_sig, "x_in");
    sc_trace(tf, w_in_sig, "w_in");
    sc_trace(tf, x_out_sig, "x_out");
    sc_trace(tf, w_out_sig, "w_out");
    sc_trace(tf, result1_sig, "result1");
    sc_trace(tf, result2_sig, "result2");
    sc_trace(tf, result3_sig, "result3");
    sc_signal<int> sum_result_sig;
    sc_trace(tf, sum_result_sig, "sum_result");

    // Process to calculate the sum of results
    SC_THREAD([&]() {
        while (true) {
            sum_result_sig.write(result1_sig.read() + result2_sig.read() + result3_sig.read());
            wait(SC_ZERO_TIME);
        }
    });
    
    // Start simulation
    cout << "Starting R2 systolic array simulation..." << endl;
    sc_start();
    
    // Close trace file
    sc_close_vcd_trace_file(tf);
    
    cout << "Simulation completed." << endl;
    
    return 0;
}