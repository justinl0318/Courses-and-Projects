module ID_EX_Register
(
    input wire rst_i,
    input wire clk_i,

    input wire RegWrite_in,
    input wire MemtoReg_in,
    input wire MemRead_in,
    input wire MemWrite_in,
    input wire [1:0] ALUOp_in,
    input wire ALUSrc_in,
    input wire [31:0] read_data_1_in,
    input wire [31:0] read_data_2_in,
    input wire [31:0] imme_in,
    input wire [6:0] inst31_25_in,
    input wire [2:0] inst14_12_in,
    input wire [4:0] inst19_15_in,
    input wire [4:0] inst24_20_in,
    input wire [4:0] inst11_7_in,

    output wire RegWrite_out,
    output wire MemtoReg_out,
    output wire MemRead_out,
    output wire MemWrite_out,
    output wire [1:0] ALUOp_out,
    output wire ALUSrc_out,
    output wire [31:0] read_data_1_out,
    output wire [31:0] read_data_2_out,
    output wire [31:0] imme_out,
    output wire [6:0] inst31_25_out,
    output wire [2:0] inst14_12_out,
    output wire [4:0] inst19_15_out,
    output wire [4:0] inst24_20_out,
    output wire [4:0] inst11_7_out
);

reg RegWrite_register;
reg MemtoReg_register;
reg MemRead_register;
reg MemWrite_register;
reg [1:0] ALUOP_register;
reg ALUSrc_register;
reg [31:0] read_data_1_register;
reg [31:0] read_data_2_register;
reg [31:0] imme_register;
reg [6:0] inst31_25_register;
reg [2:0] inst14_12_register;
reg [4:0] inst19_15_register;
reg [4:0] inst24_20_register;
reg [4:0] inst11_7_register;

always@(posedge clk_i or negedge rst_i) begin
    if (~rst_i) begin
        RegWrite_register <= 1'b0;
        MemtoReg_register <= 1'b0;
        MemRead_register <= 1'b0;
        MemWrite_register <= 1'b0;
        ALUOP_register <= 2'b0;
        ALUSrc_register <= 1'b0;
        read_data_1_register <= 32'b0;
        read_data_2_register <= 32'b0;
        imme_register <= 32'b0;
        inst31_25_register <= 7'b0;
        inst14_12_register <= 3'b0;
        inst19_15_register <= 5'b0;
        inst24_20_register <= 5'b0;
        inst11_7_register <= 5'b0;
    end
    else begin
        RegWrite_register <= RegWrite_in;
        MemtoReg_register <= MemtoReg_in;
        MemRead_register <= MemRead_in;
        MemWrite_register <= MemWrite_in;
        ALUOP_register <= ALUOp_in;
        ALUSrc_register <= ALUSrc_in;
        read_data_1_register <= read_data_1_in;
        read_data_2_register <= read_data_2_in;
        imme_register <= imme_in;
        inst31_25_register <= inst31_25_in;
        inst14_12_register <= inst14_12_in;
        inst19_15_register <= inst19_15_in;
        inst24_20_register <= inst24_20_in;
        inst11_7_register <= inst11_7_in;
    end
end

assign RegWrite_out = RegWrite_register;
assign MemtoReg_out = MemtoReg_register;
assign MemRead_out = MemRead_register;
assign MemWrite_out = MemWrite_register;
assign ALUOp_out = ALUOP_register;
assign ALUSrc_out = ALUSrc_register;
assign read_data_1_out = read_data_1_register;
assign read_data_2_out = read_data_2_register;
assign imme_out = imme_register;
assign inst31_25_out = inst31_25_register;
assign inst14_12_out = inst14_12_register;
assign inst19_15_out = inst19_15_register;
assign inst24_20_out = inst24_20_register;
assign inst11_7_out = inst11_7_register;

endmodule