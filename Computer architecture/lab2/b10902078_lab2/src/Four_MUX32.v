module Four_MUX32
(
    input wire [31:0] input_data_1,
    input wire [31:0] input_data_2,
    input wire [31:0] input_data_3,
    input wire [31:0] input_data_4,
    input wire [1:0] mux_select,
    output wire [31:0] output_data
);
    assign output_data = (
        (mux_select == 2'b00) ? input_data_1 :
        (mux_select == 2'b01) ? input_data_2 :
        (mux_select == 2'b10) ? input_data_3 : input_data_4
    );
endmodule