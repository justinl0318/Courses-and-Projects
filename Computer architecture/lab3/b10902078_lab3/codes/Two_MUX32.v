module Two_MUX32
(
    input wire [31:0] input_data_1,
    input wire [31:0] input_data_2,
    input wire mux_select,
    output wire [31:0] output_data
);
    assign output_data = (mux_select == 1'b0) ? input_data_1 : input_data_2;
endmodule