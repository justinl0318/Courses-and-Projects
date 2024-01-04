module Control
(
    input wire NoOp_in,
    input wire [6:0] opcode_in,
    output wire RegWrite_out,
    output wire MemtoReg_out,
    output wire MemRead_out,
    output wire MemWrite_out,
    output wire [1:0] ALUOP_out,
    output wire ALUSrc_out,
    output wire Branch_out
);
    assign RegWrite_out = (
        (NoOp_in == 1'b1) ? 1'b0 : 
        (opcode_in == 7'b0110011) ? 1'b1 : //R
        (opcode_in == 7'b0010011) ? 1'b1 : //Immediate
        (opcode_in == 7'b0000011) ? 1'b1 : //load
        (opcode_in == 7'b0100011) ? 1'b0 : //store
        (opcode_in == 7'b1100011) ? 1'b0 : //beq
        1'b0
    );

    assign MemtoReg_out = (
        (NoOp_in == 1'b1) ? 1'b0 : 
        (opcode_in == 7'b0110011) ? 1'b0 : //R
        (opcode_in == 7'b0010011) ? 1'b0 : //Immediate
        (opcode_in == 7'b0000011) ? 1'b1 : //load
        (opcode_in == 7'b0100011) ? 1'b0 : 1'b0  //store, branch
    );

    //only load set MemRead_out to 1
    assign MemRead_out = (
        (NoOp_in == 1'b1) ? 1'b0 : 
        (opcode_in == 7'b0000011) ? 1'b1 : 1'b0 //load
    );

    //only store set MemWrite_out to 1
    assign MemWrite_out = (
        (NoOp_in == 1'b1) ? 1'b0 :
        (opcode_in == 7'b0100011) ? 1'b1 : 1'b0 //store
    );

    //R-type: 10, I-type: 11, Load or Store: 00, Branch: 01
    assign ALUOP_out = (
        (opcode_in == 7'b0110011) ? 2'b10 : //R
        (opcode_in == 7'b0010011) ? 2'b00 : //Immediate
        (opcode_in == 7'b0000011) ? 2'b00 : //lw
        (opcode_in == 7'b0100011) ? 2'b00 : //sw
        (opcode_in == 7'b1100011) ? 2'b01 : //branch
        2'b0
    ); 

    //0 = mux should choose from prev mux, 1 = mux should choose from sign-extend
    assign ALUSrc_out = (
        (opcode_in == 7'b0110011) ? 1'b0 : //R
        (opcode_in == 7'b0010011) ? 1'b1 : //Immediate
        (opcode_in == 7'b0000011) ? 1'b1 : //lw
        (opcode_in == 7'b0100011) ? 1'b1 : //sw
        (opcode_in == 7'b1100011) ? 1'b0 : //branch
        1'b0
    ); 

    //only Branch set branch to 1
    assign Branch_out = (
        (NoOp_in == 1'b1) ? 1'b0 : 
        (opcode_in == 7'b1100011) ? 1'b1 : 1'b0 //branch
    );
endmodule