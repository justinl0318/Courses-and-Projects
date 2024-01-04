module ALU_Control
(
    input wire [6:0] funct7,
    input wire [2:0] funct3,
    input wire [1:0] ALUOp,
    output wire [2:0] ALUControl
);
    /*
    //opcode = 0110011
    if (ALUOp == 2'b00) begin
        if (funct7 == 7'b0000000 && funct3 == 3'b111)
            assign ALUControl = 3'b000; //and
        else if (funct7 == 7'b0000000 && funct3 == 3'b100)
            assign ALUControl = 3'b001; //xor
        else if (funct7 == 7'b0000000 && funct3 == 3'b001)
            assign ALUControl = 3'b010; //sll
        else if (funct7 == 7'b0000000 && funct3 == 3'b000)
            assign ALUControl = 3'b011; //add
        else if (funct7 == 7'b0100000 && funct3 == 3'b000)
            assign ALUControl = 3'b100; //sub
        else if (funct7 == 7'b0000001 && funct3 == 3'b000)
            assign ALUControl = 3'b101; //mul
    end
    
    //opcode = 0010011
    else if (ALUOp == 2'b01) begin
        if (funct3 == 3'b000)
            assign ALUControl = 3'b110; //addi
        else if (funct3 == 3'b101)
            assign ALUControl = 3'b111; //srai
    end
    */
    
    assign ALUControl = ((ALUOp == 2'b00) ? 
        ((funct7 == 7'b0000000 && funct3 == 3'b111) ? 3'b000 : 
            (funct7 == 7'b0000000 && funct3 == 3'b100) ? 3'b001 : 
                (funct7 == 7'b0000000 && funct3 == 3'b001) ? 3'b010 : 
                    (funct7 == 7'b0000000 && funct3 == 3'b000) ? 3'b011 : 
                        (funct7 == 7'b0100000 && funct3 == 3'b000) ? 3'b100 : 3'b101) : 
        ((funct3 == 3'b000) ? 3'b110 : 3'b111)); //ALUOp != 2'b00

endmodule