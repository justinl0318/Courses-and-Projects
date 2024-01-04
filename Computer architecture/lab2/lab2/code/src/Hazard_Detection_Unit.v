module Hazard_Detection_Unit
(
    input wire rst_i,
    input wire [4:0] inst19_15_in, //ID.rs1
    input wire [4:0] inst24_20_in, //ID.rs2
    input wire EX_MemRead_in,
    input wire [4:0] EX_inst11_7_in, //EX.rd

    output wire NoOp_out,
    output wire PCWrite_out,
    output wire Stall_out
);

reg NoOp_register;
reg PCWrite_register;
reg Stall_register;
//detects whether the rd in EX stage is the same as rs1 or rs2 in ID stage.
//If so, adding a nop (no operation) to the pipeline to resolve data hazard.
always@(negedge rst_i or inst19_15_in or inst24_20_in or EX_MemRead_in or EX_inst11_7_in) begin
    if (~rst_i) begin
        NoOp_register = 1'b0;
        PCWrite_register = 1'b1;
        Stall_register = 1'b0;
    end
    else begin
        //normal case: PC write should be 1 (cuz fetch next instruction), and Noop & stall should be 0
        NoOp_register = 1'b0;
        PCWrite_register = 1'b1;
        Stall_register = 1'b0;

        //load stall => EX_MemRead_in = 1
        if (EX_MemRead_in && ((EX_inst11_7_in == inst19_15_in) || (EX_inst11_7_in == inst24_20_in)) && (EX_inst11_7_in != 5'b0)) begin
            NoOp_register = 1'b1;
            PCWrite_register = 1'b0;
            Stall_register = 1'b1;
        end
    end
end

assign NoOp_out = NoOp_register;
assign PCWrite_out = PCWrite_register;
assign Stall_out = Stall_register;
endmodule