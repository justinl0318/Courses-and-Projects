module Branch_Unit
(
    input wire [31:0] imme32_in,
    input wire [31:0] ID_pc_in,
    input wire branch_prediction_in, //prediction from "Predict for Branch Unit"
    input wire Branch_in, //if this instruction is a branch instruction or no

    output wire [31:0] next_pc_out,
    output wire Flush_out
);

assign next_pc_out = ID_pc_in + (imme32_in << 1);

reg flush_register;
always@(imme32_in or ID_pc_in or branch_prediction_in or Branch_in) begin
    flush_register = 1'b0;
    if (branch_prediction_in && Branch_in) begin
        flush_register = 1'b1;
    end
end

assign Flush_out = flush_register;
endmodule