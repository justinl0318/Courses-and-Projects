module ALU
(
    //inputs and output are 32 bits, can be seen from Registers.v
    input signed [31:0] read_data_1_in, 
    input signed [31:0] read_data_2_in,
    input wire [2:0] ALUControl_in,
    output wire zero_out,
    output wire [31:0] data_o
);
    assign data_o = (
        (ALUControl_in == 3'b000) ? read_data_1_in & read_data_2_in : 
        (ALUControl_in == 3'b001) ? read_data_1_in ^ read_data_2_in :
        (ALUControl_in == 3'b010) ? read_data_1_in << read_data_2_in :
        (ALUControl_in == 3'b011) ? read_data_1_in - read_data_2_in : 
        (ALUControl_in == 3'b100) ? read_data_1_in * read_data_2_in :
        (ALUControl_in == 3'b101) ? read_data_1_in + read_data_2_in :
        (ALUControl_in == 3'b110) ? read_data_1_in >>> read_data_2_in[4:0] :
        32'bx
    );

    // if data1 == data2 and ALUControl = subtract (branch instruction)
    assign zero_out = (
        (read_data_1_in == read_data_2_in) && ALUControl_in == 3'b011) ? 1'b1 : 1'b0;

endmodule