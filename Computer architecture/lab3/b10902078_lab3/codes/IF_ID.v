module IF_ID
(
    input wire rst_i,
    input wire clk_i,

    input wire flush_i,
    input wire Stall_in,
    input wire [31:0] instruction_in,
    input wire [31:0] pc_in,
    
    output wire [31:0] instruction_out,
    output wire [31:0] pc_out
);

reg [31:0] instruction_register;
reg [31:0] pc_register;
always@(posedge clk_i or negedge rst_i) begin
    if (~rst_i) begin
        instruction_register <= 32'b0;
        pc_register <= 32'b0;
    end
    else begin
        if (flush_i) begin
            instruction_register <= 32'b0;
            pc_register <= 32'b0;
        end
        else if (!Stall_in) begin
            instruction_register <= instruction_in;
            pc_register <= pc_in;
        end
    end
end

assign instruction_out = instruction_register;
assign pc_out = pc_register;

endmodule