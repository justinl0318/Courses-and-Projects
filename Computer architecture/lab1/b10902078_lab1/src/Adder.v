//add: pc = pc + 4
module Adder
(
    input wire [31:0] input_data_1,
    input wire [31:0] input_data_2,
    output wire [31:0] output_data
);
    assign output_data = input_data_1 + input_data_2;
endmodule