module ALU
(
    //inputs and output are 32 bits, can be seen from Registers.v
    input wire [31:0] read_data_1, 
    input wire [31:0] read_data_2,
    input wire [2:0] ALUControl,
    output wire zero,
    output wire [31:0] ALU_result
);
    /*
    if (ALUControl == 3'b000) //and
        assign ALU_result = read_data_1 & read_data_2;
    else if (ALUControl == 3'b001) //xor
        assign ALU_result = read_data_1 ^ read_data_2;
    else if (ALUControl == 3'b010) //sll
        assign ALU_result = read_data_1 << read_data_2;
    else if (ALUControl == 3'b011) //add
        assign ALU_result = read_data_1 + read_data_2;
    else if (ALUControl == 3'b100) //sub
        assign ALU_result = read_data_1 - read_data_2;
    else if (ALUControl == 3'b101) //mul
        assign ALU_result = read_data_1 * read_data_2;
    else if (ALUControl == 3'b110) //addi
        assign ALU_result = read_data_1 + read_data_2;
    else if (ALUControl == 3'b111) //srai
        //use >>> to perform sign extension on MSB
        assign ALU_result = read_data_1 >>> read_data_2[4:0];
    
    assign zero = ((read_data_1 - read_data_1 == 0) ? 1 : 0);
    */
    assign zero = ((read_data_1 - read_data_2 == 0) ? 1 : 0);   
    assign ALU_result = 
    ((ALUControl == 3'b000) ? read_data_1 & read_data_2 : 
        (ALUControl == 3'b001) ? read_data_1 ^ read_data_2 : 
            (ALUControl == 3'b010) ? read_data_1 << read_data_2 : 
                (ALUControl == 3'b011) ? read_data_1 + read_data_2 : 
                    (ALUControl == 3'b100) ? read_data_1 - read_data_2 : 
                        (ALUControl == 3'b101) ? read_data_1 * read_data_2 : 
                            (ALUControl == 3'b110) ? read_data_1 + read_data_2 : read_data_1 >>> read_data_2[4:0]);               
endmodule