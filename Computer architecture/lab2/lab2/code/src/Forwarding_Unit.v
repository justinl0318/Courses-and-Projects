module Forwarding_Unit
(
    input wire MEM_RegWrite_in,
    input wire [4:0] MEM_Rd_in,
    input wire WB_RegWrite_in,
    input wire [4:0] WB_Rd_in,
    input wire [4:0] EX_Rs1_in,
    input wire [4:0] EX_Rs2_in,
    output wire [1:0] Forward_A_out,
    output wire [1:0] Forward_B_out
);

reg [1:0] Forward_A_register;
reg [1:0] Forward_B_register;
always@(MEM_RegWrite_in or MEM_Rd_in or WB_RegWrite_in or WB_Rd_in or EX_Rs1_in or EX_Rs2_in) begin
    Forward_A_register = 2'b00;
    Forward_B_register = 2'b00;
    
    if (MEM_RegWrite_in && (MEM_Rd_in != 0) && (MEM_Rd_in == EX_Rs1_in)) begin
        Forward_A_register = 2'b10;
    end

    if (MEM_RegWrite_in && (MEM_Rd_in != 0) && (MEM_Rd_in == EX_Rs2_in)) begin
        Forward_B_register = 2'b10;
    end

    if (WB_RegWrite_in && (WB_Rd_in != 0) && !(MEM_RegWrite_in && (MEM_Rd_in != 0) &&
        (MEM_Rd_in == EX_Rs1_in)) && (WB_Rd_in == EX_Rs1_in)) begin
        Forward_A_register = 2'b01;
    end

    if (WB_RegWrite_in && (WB_Rd_in != 0) && !(MEM_RegWrite_in && (MEM_Rd_in != 0) &&
        (MEM_Rd_in == EX_Rs2_in)) && (WB_Rd_in == EX_Rs2_in)) begin
        Forward_B_register = 2'b01;
    end
end

assign Forward_A_out = Forward_A_register;
assign Forward_B_out = Forward_B_register;
endmodule