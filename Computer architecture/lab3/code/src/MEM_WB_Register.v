module MEM_WB_Register
(
    input wire rst_i,
    input wire clk_i,

    input wire RegWrite_in,
    input wire MemtoReg_in,
    input wire [31:0] ALU_result_in,
    input wire [31:0] Read_data_in,
    input wire [4:0] inst11_7_in,

    output wire RegWrite_out,
    output wire MemtoReg_out,
    output wire [31:0] ALU_result_out,
    output wire [31:0] Read_data_out,
    output wire [4:0] inst11_7_out
);

reg RegWrite_register;
reg MemtoReg_register;
reg [31:0] ALU_result_register;
reg [31:0] Read_data_register;
reg [4:0] inst11_7_register;

always@(posedge clk_i or negedge rst_i) begin
    if (~rst_i) begin
        RegWrite_register <= 1'b0;
        MemtoReg_register <= 1'b0;
        ALU_result_register <= 32'b0;
        Read_data_register <= 32'b0;
        inst11_7_register <= 5'b0;
    end
    else begin
        RegWrite_register <= RegWrite_in;
        MemtoReg_register <= MemtoReg_in;
        ALU_result_register <= ALU_result_in;
        Read_data_register <= Read_data_in;
        inst11_7_register <= inst11_7_in;
    end
end

assign RegWrite_out = RegWrite_register;
assign MemtoReg_out = MemtoReg_register;
assign ALU_result_out = ALU_result_register;
assign Read_data_out = Read_data_register;
assign inst11_7_out = inst11_7_register;
endmodule