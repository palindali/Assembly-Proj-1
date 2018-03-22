#include <iostream>
#include <fstream>
#include "stdlib.h"
#include <iomanip>
#include <string>
#include <sstream>
#include <string>

using namespace std;

struct instWord
{
    string instText;
    unsigned int instMachineCode;
    unsigned int rd, rs1, rs2, funct3, funct7, opcode;
    unsigned int I_imm, S_imm, B_imm, U_imm, J_imm;
};

void parse (instWord& inst);
int concatenate (int a, int b);

int main()
{
    ifstream inFile;
    ofstream outFile;
    instWord w;
    
    inFile.open("div.s");
    if(inFile.is_open())
    {
        inFile >> w.instMachineCode;
        parse(w);
    }
}

void parse (instWord& inst)
{
    inst.opcode = inst.instMachineCode & 0x7F;
    switch (inst.opcode)
    {
            //Rtype
        case 0x33:
        {
            inst.rd = inst.opcode & 0xF80;
            inst.funct3 = inst.opcode & 0x7000;
            inst.rs1 = inst.opcode & 0xF8000;
            inst.rs2 = inst.opcode & 0x1F00000;
            inst.funct7 = inst.opcode & 0xFE000000;
        }
            break;
            //Itype
        case 0x13:
        case 0x3:
        case 0x67:
        {
            inst.rd = inst.opcode & 0xF80;
            inst.funct3 = inst.opcode & 0x7000;
            inst.rs1 = inst.opcode & 0xF8000;
            inst.I_imm = inst.opcode & 0xFFF00000;
        }
            break;
            //Stype
        case 0x23:
        {
            inst.rd = inst.opcode & 0xF80;
            int a = inst.opcode & 0x7000;
            inst.rs1 = inst.opcode & 0xF8000;
            inst.rs2 = inst.opcode & 0x1F00000;
            int b = inst.opcode & 0xFE000000;
            inst.S_imm = concatenate(a, b);
        }
            break;
            //SBtype
         case 0x63:
        {
            int c = inst.opcode & 0x80;
            int a = inst.opcode & 0xF00;
            inst.funct3 = inst.opcode & 0x7000;
            inst.rs1 = inst.opcode & 0xF8000;
            inst.rs2 = inst.opcode & 0x1F00000;
            int b = inst.opcode & 0x7E000000;
            int d = inst.opcode & 0xFE000000;
            a = concatenate(a, b);
            inst.B_imm = concatenate (a, c) << 1;
        }
            break;
            //Utype
        case 0x37:
        case 0x17:
        {
            inst.rd = inst.opcode & 0xF80;
            inst.U_imm = (inst.opcode & 0xFFFFF000) << 12;
        }
            break;
            //UJtype
        case 0x6F:
        {
            inst.rd = inst.opcode & 0xF80;
            int a = inst.opcode & 0x7FE00000;
            int b = inst.opcode & 0x100000;
            int c = inst.opcode & 0xFF000;
            int d = inst.opcode & 0x80000000;
            a = concatenate (a, b);
            a = concatenate (a, c);
            inst.J_imm = concatenate (a,d) << 1;
        }
            break;
            // & ecall
        default:
        {
            
        }
    }
}


int concatenate (int a, int b)
{
    ostringstream oss;
    oss << a << b;
    istringstream iss(oss.str());
    int c;
    iss >> c;
    return c;
}
