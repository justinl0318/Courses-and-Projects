`ifndef ADDERS
`define ADDERS
`include "gates.v"

// half adder, gate level modeling
module HA(output C, S, input A, B);
	XOR g0(S, A, B);
	AND g1(C, A, B);
endmodule

// full adder, gate level modeling
module FA(output CO, S, input A, B, CI);
	wire c0, s0, c1, s1;
	HA ha0(c0, s0, A, B);
	HA ha1(c1, s1, s0, CI);
	assign S = s1;
	OR or0(CO, c0, c1);
endmodule

// adder without delay, register-transfer level modeling
module adder_rtl(
	output C3,       // carry output
	output[2:0] S,   // sum
	input[2:0] A, B, // operands
	input C0         // carry input
	);
	assign {C3, S} = A+B+C0;
endmodule

//  ripple-carry adder, gate level modeling
//  Do not modify the input/output of module
module rca_gl(
	output C3,       // carry output
	output[2:0] S,   // sum
	input[2:0] A, B, // operands
	input C0         // carry input
	);

	// TODO:: Implement gate-level RCA
	FA fa_0(C1, S[0], A[0], B[0], C0);
	FA fa_1(C2, S[1], A[1], B[1], C1);
	FA fa_2(C3, S[2], A[2], B[2], C2);
endmodule

// carry-lookahead adder, gate level modeling
// Do not modify the input/output of module
module cla_gl(
	output C3,       // carry output
	output[2:0] S,   // sum
	input[2:0] A, B, // operands
	input C0         // carry input
	);

	// TODO:: Implement gate-level CLA
	wire [2:0] p, g;
	//0: first bit
	AND g_0(g[0], A[0], B[0]);
	OR p_0(p[0], A[0], B[0]);
	AND pc_0(pc0, p[0], C0);
	OR c_1(C1, g[0], pc0);
	FA fa_0(temp0, S[0], A[0], B[0], C0);

	//1: second bit
	AND g_1(g[1], A[1], B[1]);
	OR p_1(p[1], A[1], B[1]);
	AND4 c_0p_0p_1(c0p0p1, C0, p[0], p[1], 1'b1);
	AND g_0p_1(g0p1, g[0], p[1]);
	OR4 c_2(C2, g[1], g0p1, c0p0p1, 1'b0);
	FA fa_1(temp1, S[1], A[1], B[1], C1);

	//2: third bit
	AND g_2(g[2], A[2], B[2]);
	OR p_2(p[2], A[2], B[2]);
	AND4 c_0p_0p_1p_2(c0p0p1p2, C0, p[0], p[1], p[2]);
	AND4 g_0p_1p_2(g0p1p2, g[0], p[1], p[2], 1'b1);
	AND g_1p_2(g1p2, g[1], p[2]);
	OR4 c_3(C3, g[2], g1p2, g0p1p2, c0p0p1p2);
	FA fa_2(temp2, S[2], A[2], B[2], C2);
		
endmodule

`endif
