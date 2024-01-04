module CPU
(
    clk_i, 
    rst_i,
);

// Ports
input               clk_i;
input               rst_i;

wire [31:0] pc_input;
wire [31:0] pc_output;
wire [31:0] instruction;
wire [1:0] ALUOp;
wire ALUSrc;
wire RegWrite;
wire [31:0] read_data_1;
wire [31:0] read_data_2;
wire [2:0] ALU_control;
wire zero;
wire [31:0] ALU_result;
wire [31:0] immediate32;
wire [31:0] mux_output;


//pass in module's argument
Control Control(instruction[6:0], ALUOp, ALUSrc, RegWrite);

Adder Add_PC(pc_output, 4, pc_input);

PC PC(clk_i, rst_i, pc_input, pc_output);

Instruction_Memory Instruction_Memory(pc_output, instruction);


Registers Registers(rst_i, clk_i, instruction[19:15], instruction[24:20], instruction[11:7], 
                    ALU_result, RegWrite, read_data_1, read_data_2);

MUX32 MUX_ALUSrc(read_data_2, immediate32, ALUSrc, mux_output);

Sign_Extend Sign_Extend(instruction[31:20], immediate32);
  
ALU ALU(read_data_1, mux_output, ALU_control, zero, ALU_result);

ALU_Control ALU_Control(instruction[31:25], instruction[14:12], ALUOp, ALU_control);

endmodule

