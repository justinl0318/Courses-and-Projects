module Control
(
    input wire [6:0] opcode,
    output wire [1:0] ALUOp,
    output wire ALUSrc,
    output wire RegWrite
);
    //00 = R-instruction, 01 = I-instruction
    assign ALUOp = ((opcode == 7'b0110011) ? 2'b00 : 2'b01);
    //0 = mux should choose from register, 1 = mux should choose from sign-extend
    assign ALUSrc = ((opcode == 7'b0110011) ? 1'b0 : 1'b1);
    assign RegWrite = 1'b1; //all operations write to register
endmodule