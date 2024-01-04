module branch_predictor
(
    input wire rst_i,
    input wire clk_i,
    input wire zero_in,
    input wire branch_in,

    output wire predict_o
);

reg [1:0] branch_state;

assign predict_o = (branch_state == 2'b11 || branch_state == 2'b10) ? 1'b1 : 1'b0;

//reset state
always@(negedge rst_i) begin
    if (~rst_i) begin
        branch_state <= 2'b11;
    end
end

//update state
always@(posedge clk_i) begin
    if (branch_in) begin
        if (zero_in) begin //taken
            if (branch_state != 2'b11) begin
                branch_state <= branch_state + 1;
            end
        end
        else if (!zero_in) begin //not taken
            if (branch_state != 2'b00) begin
                branch_state <= branch_state - 1;
            end
        end
    end
end

endmodule