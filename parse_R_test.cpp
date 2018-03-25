#include <iostream>
#include <fstream>
#include "stdlib.h"
#include <iomanip>
#include <string>
#include <sstream>
#include <bitset>
using namespace std;

struct instWord
{
	string Text;
	unsigned int MachineCode;
	unsigned int rd, rs1, rs2, funct3, funct7, opcode;
	unsigned int I_imm, S_imm, B_imm, U_imm, J_imm;
};

unsigned int getfield(int& after, char beg, char end, string s);
void parse(instWord& inst);

int main() {
	instWord inst;
    // Insert any R instruction here to test
	inst.Text = "add\tx3, x4, x5";

	parse(inst);

	cout << "rd: " << bitset<5>(inst.rd) << endl;
    cout << "rs1: " << bitset<5>(inst.rs1) << endl;
    cout << "rs2: " << bitset<5>(inst.rs2) << endl;
    cout << "funct7: " << bitset<7>(inst.funct7) << endl;
    cout << "funct3: " << bitset<3>(inst.funct3) << endl;
    cout << "opcode: " << bitset<7>(inst.opcode) << endl;
	cout << "Machine Code: " << bitset<32>(inst.MachineCode) << endl;

	return 0;
}

unsigned int getfield(int& after, char beg, char end, string s)
{
    int x, i, k;
    string sub;

    i = s.find(beg, after);
    k = s.find(end, i);
    sub = s.substr(i + 1, k - i - 1);

    stringstream stream(sub);
    stream >> x;
    after = k + 1;
    return x;
}

void parse(instWord& inst)
{
    int pos = inst.Text.find('\t');
    string part = inst.Text.substr(0, pos++);

    //R
    if (part == "add")
    {
        inst.rd = getfield(pos, 'x', ',', inst.Text);
        inst.rs1 = getfield(pos, 'x', ',', inst.Text);
        inst.rs2 = getfield(pos, 'x', '\n', inst.Text);
        inst.funct3 = 0b000;
        inst.funct7 = 0b0000000;
        inst.opcode = 0b0110011;
        inst.I_imm = NULL;
        inst.S_imm = NULL;
        inst.B_imm = NULL;
        inst.U_imm = NULL;
        inst.J_imm = NULL;
        inst.MachineCode = inst.MachineCode = inst.opcode | (inst.rd << 7) | (inst.funct3 << 12) |
        (inst.rs1 << 15) | (inst.rs2 << 20) | (inst.funct7 << 25); inst.opcode | (inst.rd << 7) | (inst.funct3 << 12) |
        (inst.rs1 << 15) | (inst.rs2 << 20) | (inst.funct7 << 25);
    }
    else if (part == "sub")
    {
        inst.rd = getfield(pos, 'x', ',', inst.Text);
        inst.rs1 = getfield(pos, 'x', ',', inst.Text);
        inst.rs2 = getfield(pos, 'x', '\n', inst.Text);
        inst.funct3 = 0b000;
        inst.funct7 = 0b0100000;
        inst.opcode = 0b0110011;
        inst.I_imm = NULL;
        inst.S_imm = NULL;
        inst.B_imm = NULL;
        inst.U_imm = NULL;
        inst.J_imm = NULL;
        inst.MachineCode = inst.opcode | (inst.rd << 7) | (inst.funct3 << 12) |
        (inst.rs1 << 15) | (inst.rs2 << 20) | (inst.funct7 << 25);

    }
    else if (part == "sll")
    {
        inst.rd = getfield(pos, 'x', ',', inst.Text);
        inst.rs1 = getfield(pos, 'x', ',', inst.Text);
        inst.rs2 = getfield(pos, 'x', '\n', inst.Text);
        inst.funct3 = 0b001;
        inst.funct7 = 0b0000000;
        inst.opcode = 0b0110011;
        inst.I_imm = NULL;
        inst.S_imm = NULL;
        inst.B_imm = NULL;
        inst.U_imm = NULL;
        inst.J_imm = NULL;
        inst.MachineCode = inst.opcode | (inst.rd << 7) | (inst.funct3 << 12) |
        (inst.rs1 << 15) | (inst.rs2 << 20) | (inst.funct7 << 25);

    }
    else if (part == "srl")
    {
        inst.rd = getfield(pos, 'x', ',', inst.Text);
        inst.rs1 = getfield(pos, 'x', ',', inst.Text);
        inst.rs2 = getfield(pos, 'x', '\n', inst.Text);
        inst.funct3 = 0b101;
        inst.funct7 = 0b0000000;
        inst.opcode = 0b0110011;
        inst.I_imm = NULL;
        inst.S_imm = NULL;
        inst.B_imm = NULL;
        inst.U_imm = NULL;
        inst.J_imm = NULL;
        inst.MachineCode = inst.opcode | (inst.rd << 7) | (inst.funct3 << 12) |
        (inst.rs1 << 15) | (inst.rs2 << 20) | (inst.funct7 << 25);

    }
    else if (part == "sra")
    {
        inst.rd = getfield(pos, 'x', ',', inst.Text);
        inst.rs1 = getfield(pos, 'x', ',', inst.Text);
        inst.rs2 = getfield(pos, 'x', '\n', inst.Text);
        inst.funct3 = 0b101;
        inst.funct7 = 0b0100000;
        inst.opcode = 0b0110011;
        inst.I_imm = NULL;
        inst.S_imm = NULL;
        inst.B_imm = NULL;
        inst.U_imm = NULL;
        inst.J_imm = NULL;
        inst.MachineCode = inst.opcode | (inst.rd << 7) | (inst.funct3 << 12) |
        (inst.rs1 << 15) | (inst.rs2 << 20) | (inst.funct7 << 25);

    }
    else if (part == "and")
    {
        inst.rd = getfield(pos, 'x', ',', inst.Text);
        inst.rs1 = getfield(pos, 'x', ',', inst.Text);
        inst.rs2 = getfield(pos, 'x', '\n', inst.Text);
        inst.funct3 = 0b111;
        inst.funct7 = 0b0000000;
        inst.opcode = 0b0110011;
        inst.I_imm = NULL;
        inst.S_imm = NULL;
        inst.B_imm = NULL;
        inst.U_imm = NULL;
        inst.J_imm = NULL;
        inst.MachineCode = inst.opcode | (inst.rd << 7) | (inst.funct3 << 12) |
        (inst.rs1 << 15) | (inst.rs2 << 20) | (inst.funct7 << 25);

    }
    else if (part == "or")
    {
        inst.rd = getfield(pos, 'x', ',', inst.Text);
        inst.rs1 = getfield(pos, 'x', ',', inst.Text);
        inst.rs2 = getfield(pos, 'x', '\n', inst.Text);
        inst.funct3 = 0b110;
        inst.funct7 = 0b0000000;
        inst.opcode = 0b0110011;
        inst.I_imm = NULL;
        inst.S_imm = NULL;
        inst.B_imm = NULL;
        inst.U_imm = NULL;
        inst.J_imm = NULL;
        inst.MachineCode = inst.opcode | (inst.rd << 7) | (inst.funct3 << 12) |
        (inst.rs1 << 15) | (inst.rs2 << 20) | (inst.funct7 << 25);

    }
    else if (part == "xor")
    {
        inst.rd = getfield(pos, 'x', ',', inst.Text);
        inst.rs1 = getfield(pos, 'x', ',', inst.Text);
        inst.rs2 = getfield(pos, 'x', '\n', inst.Text);
        inst.funct3 = 0b100;
        inst.funct7 = 0b0000000;
        inst.opcode = 0b0110011;
        inst.I_imm = NULL;
        inst.S_imm = NULL;
        inst.B_imm = NULL;
        inst.U_imm = NULL;
        inst.J_imm = NULL;
        inst.MachineCode = inst.opcode | (inst.rd << 7) | (inst.funct3 << 12) |
        (inst.rs1 << 15) | (inst.rs2 << 20) | (inst.funct7 << 25);

    }
    else if (part == "slt")
    {
        inst.rd = getfield(pos, 'x', ',', inst.Text);
        inst.rs1 = getfield(pos, 'x', ',', inst.Text);
        inst.rs2 = getfield(pos, 'x', '\n', inst.Text);
        inst.funct3 = 0b010;
        inst.funct7 = 0b0000000;
        inst.opcode = 0b0110011;
        inst.I_imm = NULL;
        inst.S_imm = NULL;
        inst.B_imm = NULL;
        inst.U_imm = NULL;
        inst.J_imm = NULL;
        inst.MachineCode = inst.opcode | (inst.rd << 7) | (inst.funct3 << 12) |
        (inst.rs1 << 15) | (inst.rs2 << 20) | (inst.funct7 << 25);

    }
    else if (part == "sltu")
    {
        inst.rd = getfield(pos, 'x', ',', inst.Text);
        inst.rs1 = getfield(pos, 'x', ',', inst.Text);
        inst.rs2 = getfield(pos, 'x', '\n', inst.Text);
        inst.funct3 = 0b011;
        inst.funct7 = 0b0000000;
        inst.opcode = 0b0110011;
        inst.I_imm = NULL;
        inst.S_imm = NULL;
        inst.B_imm = NULL;
        inst.U_imm = NULL;
        inst.J_imm = NULL;
        inst.MachineCode = inst.opcode | (inst.rd << 7) | (inst.funct3 << 12) |
        (inst.rs1 << 15) | (inst.rs2 << 20) | (inst.funct7 << 25);

    }
}
