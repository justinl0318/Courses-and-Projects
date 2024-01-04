module ALU_Control
(
    input wire [6:0] funct7_in,
    input wire [2:0] funct3_in,
    input wire [1:0] ALUOp_in,
    output wire [2:0] ALUControl_out
);
    assign ALUControl_out = (
        (ALUOp_in == 2'b10 && funct7_in == 7'b0000000 && funct3_in == 3'b111) ? 3'b000 : //and
        (ALUOp_in == 2'b10 && funct7_in == 7'b0000000 && funct3_in == 3'b100) ? 3'b001 : //xor
        (ALUOp_in == 2'b10 && funct7_in == 7'b0000000 && funct3_in == 3'b001) ? 3'b010 : //sll
        (ALUOp_in == 2'b10 && funct7_in == 7'b0000000 && funct3_in == 3'b000) ? 3'b101 : //add
        (ALUOp_in == 2'b10 && funct7_in == 7'b0100000 && funct3_in == 3'b000) ? 3'b011 : //sub
        (ALUOp_in == 2'b10 && funct7_in == 7'b0000001 && funct3_in == 3'b000) ? 3'b100 : //mul
        (ALUOp_in == 2'b00 && funct3_in == 3'b000) ? 3'b101 : //addi
        (ALUOp_in == 2'b00 && funct3_in == 3'b101) ? 3'b110 : //srai
        (ALUOp_in == 2'b00 && funct3_in == 3'b010) ? 3'b101 : //lw and sw: add
        3'b011 //branch: sub
    );

endmodule