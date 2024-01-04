module Sign_Extend
(
    input wire [11:0] instruction20to31,
    output wire [31:0] SignExtend
);
    assign SignExtend[31:12] = {20{instruction20to31[11]}};
    assign SignExtend[11:0] = instruction20to31;
endmodule