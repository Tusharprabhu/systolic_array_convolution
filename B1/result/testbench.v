`timescale 1ns/1ps

module testbench;

reg clk;
reg rst;
reg [31:0] x_in;
reg [31:0] y_in;
wire [31:0] y_out;

// Instantiate the DUT (Design Under Test)
b1_systolic_array dut (
    .clk(clk),
    .rst(rst),
    .x_in(x_in),
    .y_in(y_in),
    .y_out(y_out)
);

// Clock generation: 10ns period (100MHz)
initial begin
    clk = 0;
    forever #5 clk = ~clk;
end

// Stimulus process
initial begin
    // Initialize inputs
    rst = 1;
    x_in = 0;
    y_in = 0;
    #15;
    rst = 0;

    // Cycle 1: Send x1
    x_in = 1;
    #10;

    // Cycle 2: Send x2
    x_in = 2;
    #10;

    // Cycle 3: Send x3
    x_in = 3;
    #10;

    // Cycle 4: Send x4
    x_in = 4;
    #10;

    // Cycle 5: Send x5
    x_in = 5;
    #10;

    // Flush pipeline
    x_in = 0;
    #70;

    // End simulation
    $finish;
end

// Monitor process
initial begin
    $display("Time\tx_in\ty_out");
    $monitor("%0t\t%d\t%d", $time, x_in, y_out);
end

// Optional: VCD waveform dump
initial begin
    $dumpfile("b1_systolic_array.vcd");
    $dumpvars(0, testbench);
end

endmodule