module rollback
(
    input wire rst_i,
    input wire clk_i,

    input wire branch_in,
    input wire zero_in,
    input wire branch_prediction_in,
    
    input signed [31:0] pc_in,
    input signed [31:0] imme32_in,

    output wire IF_ID_flush_out,
    output wire ID_EX_flush_out,
    output wire mux_select_out,
    output wire [31:0] next_pc_out
);

assign IF_ID_flush_out = (branch_in && (branch_prediction_in != zero_in)) ? 1'b1 : 1'b0;
assign ID_EX_flush_out = (branch_in && (branch_prediction_in != zero_in)) ? 1'b1 : 1'b0;
assign mux_select_out = (branch_in && (branch_prediction_in != zero_in)) ? 1'b1 : 1'b0;
assign next_pc_out = (branch_in && zero_in && !branch_prediction_in) ? pc_in + (imme32_in << 1) : 
                     (branch_in && !zero_in && branch_prediction_in) ? pc_in + 32'd4 :
                     pc_in;

endmodule