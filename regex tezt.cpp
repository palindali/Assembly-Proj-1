#include <iostream>
#include <fstream>
#include "stdlib.h"
#include <iomanip>
#include <string>
#include <sstream>
#include <bitset>
#include <regex>

using namespace std;

#define SPACE "\\s+"
#define REG "\\s*x(\\d{1,2})"
#define LABEL "(\\w+:?)"
#define IMM "\\s*([-|+]?\\d+)"
#define OR "|"
#define REG3        LABEL SPACE REG "," REG "," REG
#define REGIMMREG   LABEL SPACE REG "," IMM "\\(" REG "\\)"
#define REG2IMM     LABEL SPACE REG "," REG "," IMM
#define REGIMM      LABEL SPACE REG "," IMM
#define MYEX		REG3 OR REGIMMREG OR REGIMM OR REG2IMM

struct instWord
{
	string Text;
	unsigned int MachineCode;
	unsigned int rd, rs1, rs2, funct3, funct7, opcode;
	unsigned int I_imm, S_imm, B_imm, U_imm, J_imm;
};

void parse(instWord& inst);

int main(){

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


void parse (instWord& inst)
{
    regex ex(LABEL);
    smatch m;
    regex_search(inst.Text, m, ex);
    string ins = m[1];
    regex ex1(".+:");

    if(!(regex_match(ins, ex1)))
    {
        //R
        if (ins == "add")
        {
            regex ex(REG3);
            smatch M;
            regex_search(inst.Text, M, ex);

            inst.rd     = (unsigned int)stoi(M[2]);
            inst.rs1    = (unsigned int)stoi(M[3]);
            inst.rs2    = (unsigned int)stoi(M[4]);
            inst.funct3 = 0b000;
            inst.funct7 = 0b0000000;
            inst.opcode = 0b0110011;
            inst.MachineCode =  inst.MachineCode = inst.opcode | (inst.rd << 7) | (inst.funct3 << 12) |
            (inst.rs1 << 15) | (inst.rs2 << 20) | (inst.funct7 << 25);
        }
        else if (ins == "sub")
        {
            regex ex(REG3);
            smatch M;
            regex_search(inst.Text, M, ex);
            inst.rd     = (unsigned int)stoi(M[2]);
            inst.rs1    = (unsigned int)stoi(M[3]);
            inst.rs2    = (unsigned int)stoi(M[4]);
            inst.funct3 = 0b000;
            inst.funct7 = 0b0100000;
            inst.opcode = 0b0110011;
            inst.MachineCode = inst.opcode | (inst.rd << 7) | (inst.funct3 << 12) |
            (inst.rs1 << 15) | (inst.rs2 << 20) | (inst.funct7 << 25);

        }
        else if (ins == "sll")
        {
            regex ex(REG3);
            smatch M;
            regex_search(inst.Text, M, ex);
            inst.rd     = (unsigned int)stoi(M[2]);
            inst.rs1    = (unsigned int)stoi(M[3]);
            inst.rs2    = (unsigned int)stoi(M[4]);
            inst.funct3 = 0b001;
            inst.funct7 = 0b0000000;
            inst.opcode = 0b0110011;
            inst.MachineCode = inst.opcode | (inst.rd << 7) | (inst.funct3 << 12) |
            (inst.rs1 << 15) | (inst.rs2 << 20) | (inst.funct7 << 25);

        }
        else if (ins == "srl")
        {
            regex ex(REG3);
            smatch M;
            regex_search(inst.Text, M, ex);
            inst.rd     = (unsigned int)stoi(M[2]);
            inst.rs1    = (unsigned int)stoi(M[3]);
            inst.rs2    = (unsigned int)stoi(M[4]);
            inst.funct3 = 0b101;
            inst.funct7 = 0b0000000;
            inst.opcode = 0b0110011;
            inst.MachineCode = inst.opcode | (inst.rd << 7) | (inst.funct3 << 12) |
            (inst.rs1 << 15) | (inst.rs2 << 20) | (inst.funct7 << 25);

        }
        else if (ins == "sra")
        {
            regex ex(REG3);
            smatch M;
            regex_search(inst.Text, M, ex);
            inst.rd     = (unsigned int)stoi(M[2]);
            inst.rs1    = (unsigned int)stoi(M[3]);
            inst.rs2    = (unsigned int)stoi(M[4]);
            inst.funct3 = 0b101;
            inst.funct7 = 0b0100000;
            inst.opcode = 0b0110011;
            inst.MachineCode = inst.opcode | (inst.rd << 7) | (inst.funct3 << 12) |
            (inst.rs1 << 15) | (inst.rs2 << 20) | (inst.funct7 << 25);

        }
        else if (ins == "and")
        {
            regex ex(REG3);
            smatch M;
            regex_search(inst.Text, M, ex);
            inst.rd     = (unsigned int)stoi(M[2]);
            inst.rs1    = (unsigned int)stoi(M[3]);
            inst.rs2    = (unsigned int)stoi(M[4]);
            inst.funct3 = 0b111;
            inst.funct7 = 0b0000000;
            inst.opcode = 0b0110011;
            inst.MachineCode = inst.opcode | (inst.rd << 7) | (inst.funct3 << 12) |
            (inst.rs1 << 15) | (inst.rs2 << 20) | (inst.funct7 << 25);

        }
        else if (ins == "or")
        {
            regex ex(REG3);
            smatch M;
            regex_search(inst.Text, M, ex);
            inst.rd     = (unsigned int)stoi(M[2]);
            inst.rs1    = (unsigned int)stoi(M[3]);
            inst.rs2    = (unsigned int)stoi(M[4]);
            inst.funct3 = 0b110;
            inst.funct7 = 0b0000000;
            inst.opcode = 0b0110011;
            inst.MachineCode = inst.opcode | (inst.rd << 7) | (inst.funct3 << 12) |
            (inst.rs1 << 15) | (inst.rs2 << 20) | (inst.funct7 << 25);

        }
        else if (ins == "xor")
        {
            regex ex(REG3);
            smatch M;
            regex_search(inst.Text, M, ex);
            inst.rd     = (unsigned int)stoi(M[2]);
            inst.rs1    = (unsigned int)stoi(M[3]);
            inst.rs2    = (unsigned int)stoi(M[4]);
            inst.funct3 = 0b100;
            inst.funct7 = 0b0000000;
            inst.opcode = 0b0110011;
            inst.MachineCode = inst.opcode | (inst.rd << 7) | (inst.funct3 << 12) |
            (inst.rs1 << 15) | (inst.rs2 << 20) | (inst.funct7 << 25);

        }
        else if (ins == "slt")
        {
            regex ex(REG3);
            smatch M;
            regex_search(inst.Text, M, ex);
            inst.rd     = (unsigned int)stoi(M[2]);
            inst.rs1    = (unsigned int)stoi(M[3]);
            inst.rs2    = (unsigned int)stoi(M[4]);
            inst.funct3 = 0b010;
            inst.funct7 = 0b0000000;
            inst.opcode = 0b0110011;
            inst.MachineCode = inst.opcode | (inst.rd << 7) | (inst.funct3 << 12) |
            (inst.rs1 << 15) | (inst.rs2 << 20) | (inst.funct7 << 25);

        }
        else if (ins == "sltu")
        {
            regex ex(REG3);
            smatch M;
            regex_search(inst.Text, M, ex);
            inst.rd     = (unsigned int)stoi(M[2]);
            inst.rs1    = (unsigned int)stoi(M[3]);
            inst.rs2    = (unsigned int)stoi(M[4]);
            inst.funct3 = 0b011;
            inst.funct7 = 0b0000000;
            inst.opcode = 0b0110011;
            inst.MachineCode = inst.opcode | (inst.rd << 7) | (inst.funct3 << 12) |
            (inst.rs1 << 15) | (inst.rs2 << 20) | (inst.funct7 << 25);

        }

    }
    else
    {
        // LABEL CODE
        cout <<"LABEL";
    }
}
