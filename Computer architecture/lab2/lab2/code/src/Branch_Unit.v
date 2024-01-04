module Branch_Unit
(
    input wire clk_i,
    input wire [31:0] read_data_1_in,
    input wire [31:0] read_data_2_in,
    input wire [31:0] imme32_in,
    input wire [31:0] ID_pc_in,
    input wire Branch_in,

    output wire [31:0] next_pc_out,
    output wire Flush_out
);

assign next_pc_out = ID_pc_in + (imme32_in << 1);
assign Flush_out = (read_data_1_in == read_data_2_in && Branch_in) ? 1'b1 : 1'b0;

// reg flush_register;
// always@(read_data_1_in or read_data_2_in or imme32_in or ID_pc_in or Branch_in) begin
//     flush_register = 1'b0;
//     if (read_data_1_in == read_data_2_in && Branch_in) begin
//         flush_register = 1'b1;
//     end
// end

// assign Flush_out = flush_register;
endmodule