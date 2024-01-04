module OR(
    input wire input_data_1,
    input wire input_data_2,
    output wire output_data
);
assign output_data = input_data_1 | input_data_2;
endmodule