module CPU
(
    clk_i, 
    rst_i,
);

// Ports
input               clk_i;
input               rst_i;

//naming convention: wire belong to a module if it is its output

//IF_ID_Register
wire [31:0] ID_instr_out;
wire [31:0] ID_pc_out;

//ID_EX_Register
wire EX_RegWrite_out;
wire EX_MemtoReg_out;
wire EX_MemRead_out;
wire EX_MemWrite_out;
wire [1:0] EX_ALUOp_out;
wire EX_ALUSrc_out;
wire EX_Branch_out;
wire [31:0] EX_Read_data_1_out;
wire [31:0] EX_Read_data_2_out;
wire [31:0] EX_Imme32_out;
wire [6:0] EX_funct7_out;
wire [2:0] EX_funct3_out;
wire [4:0] EX_inst19_15_out;
wire [4:0] EX_inst24_20_out;
wire [4:0] EX_inst11_7_out;
wire [31:0] EX_pc_out;

//EX_MEM_Register
wire MEM_RegWrite_out;
wire MEM_MemtoReg_out;
wire MEM_MemRead_out;
wire MEM_MemWrite_out;
wire [31:0] MEM_ALU_result_out;
wire [31:0] MEM_Forward_B_MUX_out;
wire [4:0] MEM_inst11_7_out;

//MEM_WB_Register
wire WB_RegWrite_out;
wire WB_MemtoReg_out;
wire [31:0] WB_ALU_result_out;
wire [31:0] WB_Data_Memory_out;
wire [4:0] WB_inst11_7_out;

//PC
wire [31:0] pc_out;
wire [31:0] added_pc_out;

//Control
wire RegWrite_out;
wire MemtoReg_out;
wire MemRead_out;
wire MemWrite_out;
wire [1:0] ALUOp_out;
wire ALUSrc_out;
wire Branch_out;

//Instruction Memory
wire [31:0] instr_o;

//Register
wire [31:0] RS1data_o; 
wire [31:0] RS2data_o;

//ALU
wire zero_out;
wire [31:0] ALU_result_out;

//ALU Control
wire [2:0] ALUControl_out;

//Forwarding Unit
wire [1:0] Forward_A_out;
wire [1:0] Forward_B_out;

//Hazard Detection Unit
wire NoOp_out;
wire PCWrite_out;
wire Stall_out;

//branch predictor
wire branch_predictor_predict_o;

//rollback
wire rollback_IF_ID_flush_out;
wire rollback_ID_EX_flush_out;
wire rollback_mux_select_out;
wire [31:0] rollback_next_pc_out;

//Branch Unit
wire [31:0] Branch_unit_next_pc_out;
wire Branch_unit_Flush_out;

//Data Memory
wire [31:0] Data_Memory_out;

//Sign Extend
wire [11:0] Sign_Extend_in;
wire [31:0] Sign_Extend_out;

//LeftMost 2-MUX
wire [31:0] LeftMost_Mux_out;

//New MUX before PC (after LeftMost MUX)
wire [31:0] before_PC_MUX_out;

//Forward A MUX
wire [31:0] Forward_A_MUX_out;

//Forward B MUX
wire [31:0] Forward_B_MUX_out;

//Middle MUX
wire [31:0] Middle_MUX_out;

//RightMost MUX
wire [31:0] RightMost_MUX_out;

//IF_ID_flush
wire OR_flush_out;

//pass in module's argument
Control Control(
    .NoOp_in (NoOp_out),
    .opcode_in (ID_instr_out[6:0]),
    .RegWrite_out (RegWrite_out),
    .MemtoReg_out (MemtoReg_out),
    .MemRead_out (MemRead_out),
    .MemWrite_out (MemWrite_out),
    .ALUOP_out (ALUOp_out),
    .ALUSrc_out (ALUSrc_out),
    .Branch_out (Branch_out)
);

Adder Add_PC(
    .input_data_1 (pc_out),
    .input_data_2 (4),
    .output_data (added_pc_out)
);

PC PC(
    .rst_i (rst_i),
    .clk_i (clk_i),
    .PCWrite_i (PCWrite_out),
    .pc_i (before_PC_MUX_out),
    .pc_o (pc_out)
);

Instruction_Memory Instruction_Memory(
    .addr_i (pc_out),
    .instr_o (instr_o)
);

Registers Registers(
    .rst_i (rst_i),
    .clk_i (clk_i),
    .RS1addr_i (ID_instr_out[19:15]),
    .RS2addr_i (ID_instr_out[24:20]),
    .RDaddr_i (WB_inst11_7_out),
    .RDdata_i (RightMost_MUX_out),
    .RegWrite_i (WB_RegWrite_out),
    .RS1data_o (RS1data_o),
    .RS2data_o (RS2data_o)
);

Two_MUX32 MUX_ALUSrc(
    .input_data_1 (Forward_B_MUX_out),
    .input_data_2 (EX_Imme32_out), 
    .mux_select (EX_ALUSrc_out),
    .output_data (Middle_MUX_out)
);

assign Sign_Extend_in = (
    (ID_instr_out[6:0] == 7'b0100011) ? {ID_instr_out[31:25], ID_instr_out[11:7]} : //sw
    (ID_instr_out[6:0] == 7'b1100011) ? {ID_instr_out[31], ID_instr_out[7], ID_instr_out[30:25], ID_instr_out[11:8]} : //branch
    ID_instr_out[31:20] //lw, I-type, R-type
);
Sign_Extend Sign_Extend(
    .instruction20to31 (Sign_Extend_in), 
    .SignExtend (Sign_Extend_out)
);

ALU ALU(
    .read_data_1_in (Forward_A_MUX_out),
    .read_data_2_in (Middle_MUX_out),
    .ALUControl_in (ALUControl_out),
    .zero_out (zero_out),
    .data_o (ALU_result_out)
);

ALU_Control ALU_Control(
    .funct7_in (EX_funct7_out),
    .funct3_in (EX_funct3_out),
    .ALUOp_in (EX_ALUOp_out),
    .ALUControl_out (ALUControl_out)
);

Data_Memory Data_Memory(
    .clk_i (clk_i),
    .addr_i (MEM_ALU_result_out),
    .MemRead_i (MEM_MemRead_out),
    .MemWrite_i (MEM_MemWrite_out),
    .data_i (MEM_Forward_B_MUX_out),
    .data_o (Data_Memory_out)
);

Forwarding_Unit Forwarding_Unit(
    .MEM_RegWrite_in (MEM_RegWrite_out),
    .MEM_Rd_in (MEM_inst11_7_out),
    .WB_RegWrite_in (WB_RegWrite_out),
    .WB_Rd_in (WB_inst11_7_out),
    .EX_Rs1_in (EX_inst19_15_out),
    .EX_Rs2_in (EX_inst24_20_out),
    .Forward_A_out (Forward_A_out),
    .Forward_B_out (Forward_B_out)
);

Hazard_Detection_Unit Hazard_Detection_Unit(
    .rst_i (rst_i),
    .inst19_15_in (ID_instr_out[19:15]),
    .inst24_20_in (ID_instr_out[24:20]),
    .EX_MemRead_in (EX_MemRead_out),
    .EX_inst11_7_in (EX_inst11_7_out),
    .NoOp_out (NoOp_out),
    .PCWrite_out (PCWrite_out),
    .Stall_out (Stall_out)
);

Branch_Unit Branch_Unit(
    .imme32_in (Sign_Extend_out),
    .ID_pc_in (ID_pc_out),
    .branch_prediction_in (branch_predictor_predict_o),
    .Branch_in (Branch_out),
    .next_pc_out (Branch_unit_next_pc_out),
    .Flush_out (Branch_unit_Flush_out)
);

rollback rollback(
    .rst_i (rst_i),
    .clk_i (clk_i),
    .branch_in (EX_Branch_out),
    .zero_in (zero_out),
    .branch_prediction_in (EX_branch_prediction_out),
    .pc_in (EX_pc_out),
    .imme32_in (EX_Imme32_out),
    .IF_ID_flush_out (rollback_IF_ID_flush_out),
    .ID_EX_flush_out (rollback_ID_EX_flush_out),
    .mux_select_out (rollback_mux_select_out),
    .next_pc_out (rollback_next_pc_out)
);

branch_predictor branch_predictor(
    .rst_i (rst_i),
    .clk_i (clk_i),
    .zero_in (zero_out),
    .branch_in (EX_Branch_out),
    .predict_o (branch_predictor_predict_o)
);

IF_ID IF_ID(
    .rst_i (rst_i),
    .clk_i (clk_i),
    .flush_i (OR_flush_out),
    .Stall_in (Stall_out),
    .instruction_in (instr_o),
    .pc_in (pc_out),
    .instruction_out (ID_instr_out),
    .pc_out (ID_pc_out)
);

ID_EX ID_EX(
    .rst_i (rst_i),
    .clk_i (clk_i),
    .flush_in (rollback_ID_EX_flush_out),
    .RegWrite_in (RegWrite_out),
    .MemtoReg_in (MemtoReg_out),
    .MemRead_in (MemRead_out),
    .MemWrite_in (MemWrite_out),
    .ALUOp_in (ALUOp_out),
    .ALUSrc_in (ALUSrc_out),
    .Branch_in (Branch_out),
    .read_data_1_in (RS1data_o),
    .read_data_2_in (RS2data_o),
    .imme_in (Sign_Extend_out),
    .inst31_25_in (ID_instr_out[31:25]),
    .inst14_12_in (ID_instr_out[14:12]),
    .inst19_15_in (ID_instr_out[19:15]),
    .inst24_20_in (ID_instr_out[24:20]),
    .inst11_7_in (ID_instr_out[11:7]),
    .pc_in (ID_pc_out),
    .prediction_in (branch_predictor_predict_o),
    .RegWrite_out (EX_RegWrite_out),
    .MemtoReg_out (EX_MemtoReg_out),
    .MemRead_out (EX_MemRead_out),
    .MemWrite_out (EX_MemWrite_out),
    .ALUOp_out (EX_ALUOp_out),
    .ALUSrc_out (EX_ALUSrc_out),
    .Branch_o (EX_Branch_out),
    .read_data_1_out (EX_Read_data_1_out),
    .read_data_2_out (EX_Read_data_2_out),
    .imme_out (EX_Imme32_out),
    .inst31_25_out (EX_funct7_out),
    .inst14_12_out (EX_funct3_out),
    .inst19_15_out (EX_inst19_15_out),
    .inst24_20_out (EX_inst24_20_out),
    .inst11_7_out (EX_inst11_7_out),
    .pc_out (EX_pc_out),
    .prediction_out (EX_branch_prediction_out)
);

EX_MEM_Register EX_MEM_Register(
    .rst_i (rst_i),
    .clk_i (clk_i),
    .RegWrite_in (EX_RegWrite_out),
    .MemtoReg_in (EX_MemtoReg_out),
    .MemRead_in (EX_MemRead_out),
    .MemWrite_in (EX_MemWrite_out),
    .ALU_result_in (ALU_result_out),
    .MUX_in (Forward_B_MUX_out),
    .inst11_7_in (EX_inst11_7_out),
    .RegWrite_out (MEM_RegWrite_out),
    .MemtoReg_out (MEM_MemtoReg_out),
    .MemRead_out (MEM_MemRead_out),
    .MemWrite_out (MEM_MemWrite_out),
    .ALU_result_out (MEM_ALU_result_out),
    .MUX_out (MEM_Forward_B_MUX_out),
    .inst11_7_out (MEM_inst11_7_out)
);

MEM_WB_Register MEM_WB_Register(
    .rst_i (rst_i),
    .clk_i (clk_i),
    .RegWrite_in (MEM_RegWrite_out),
    .MemtoReg_in (MEM_MemtoReg_out),
    .ALU_result_in (MEM_ALU_result_out),
    .Read_data_in (Data_Memory_out),
    .inst11_7_in (MEM_inst11_7_out),
    .RegWrite_out (WB_RegWrite_out),
    .MemtoReg_out (WB_MemtoReg_out),
    .ALU_result_out (WB_ALU_result_out),
    .Read_data_out (WB_Data_Memory_out),
    .inst11_7_out (WB_inst11_7_out)
);

Four_MUX32 Forward_A_MUX(
    .input_data_1 (EX_Read_data_1_out),
    .input_data_2 (RightMost_MUX_out),
    .input_data_3 (MEM_ALU_result_out),
    .input_data_4 (32'd0),
    .mux_select (Forward_A_out),
    .output_data (Forward_A_MUX_out)
);

Four_MUX32 Forward_B_MUX(
    .input_data_1 (EX_Read_data_2_out),
    .input_data_2 (RightMost_MUX_out),
    .input_data_3 (MEM_ALU_result_out),
    .input_data_4 (32'd0),
    .mux_select (Forward_B_out),
    .output_data (Forward_B_MUX_out)
);

Two_MUX32 RightMost_MUX(
    .input_data_1 (WB_ALU_result_out),
    .input_data_2 (WB_Data_Memory_out),
    .mux_select (WB_MemtoReg_out),
    .output_data (RightMost_MUX_out)
);

Two_MUX32 LeftMost_MUX(
    .input_data_1 (added_pc_out),
    .input_data_2 (Branch_unit_next_pc_out),
    .mux_select (Branch_unit_Flush_out),
    .output_data (LeftMost_Mux_out)
);

Two_MUX32 before_PC_MUX(
    .input_data_1 (LeftMost_Mux_out),
    .input_data_2 (rollback_next_pc_out),
    .mux_select (rollback_mux_select_out),
    .output_data (before_PC_MUX_out)
);

OR OR(
    .input_data_1 (Branch_unit_Flush_out),
    .input_data_2 (rollback_IF_ID_flush_out),
    .output_data (OR_flush_out)
);

endmodule

