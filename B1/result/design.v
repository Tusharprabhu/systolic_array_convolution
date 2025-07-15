module PE #(
    parameter WEIGHT = 1
)(
    input wire clk,
    input wire rst,
    input wire [31:0] x_in,
    input wire [31:0] y_in,
    output reg [31:0] y_out
);
    reg [31:0] mult_result_reg;
    reg [31:0] y_reg;
    reg [31:0] sum;
    reg [31:0] x;

    always @(posedge clk or posedge rst) begin
        if (rst) begin
            mult_result_reg <= 32'd0;
            y_reg <= 32'd0;
            y_out <= 32'd0;
        end else begin
            x <= x_in;
            mult_result_reg <= x * WEIGHT;
            y_reg <= y_in;
            sum <= mult_result_reg + y_reg;
            y_out <= sum;
        end
    end
endmodule

module b1_systolic_array(
    input wire clk,
    input wire rst,
    input wire [31:0] x_in,
    input wire [31:0] y_in,
    output wire [31:0] y_out
);
    wire [31:0] y_sig1, y_sig2;

    // PE1: weight = 1
    PE #(.WEIGHT(1)) pe1 (
        .clk(clk),
        .rst(rst),
        .x_in(x_in),
        .y_in(y_in),
        .y_out(y_sig1)
    );

    // PE2: weight = 2
    PE #(.WEIGHT(2)) pe2 (
        .clk(clk),
        .rst(rst),
        .x_in(x_in),
        .y_in(y_sig1),
        .y_out(y_sig2)
    );

    // PE3: weight = 3
    PE #(.WEIGHT(3)) pe3 (
        .clk(clk),
        .rst(rst),
        .x_in(x_in),
        .y_in(y_sig2),
        .y_out(y_out)
    );
endmodule