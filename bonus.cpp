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
    string Text;
    unsigned int MachineCode;
    unsigned int rd, rs1, rs2, funct3, funct7, opcode;
    unsigned int I_imm, S_imm, B_imm, U_imm, J_imm;
};

void parse (instWord& inst);
int concatenate (int a, int b);
void mars (instWord& inst, ofstream& ofile);
void marsR (instWord& inst, ofstream& ofile);
void marsI (instWord& inst, ofstream& ofile);
void marsS (instWord& inst, ofstream& ofile);
void marsSB (instWord& inst, ofstream& ofile);
void marsU (instWord& inst, ofstream& ofile);
void marsUJ (instWord& inst, ofstream& ofile);
void marsEcall (instWord& inst, ofstream& ofile);

int main()
{
    ifstream inFile;
    ofstream outFile;
    instWord w;

    inFile.open("div.s");
    if(inFile.is_open())
    {
        inFile >> w.MachineCode;
        parse(w);
        mars (w, outFile);
    }
    return 0;
}

void parse (instWord& inst)
{
    inst.opcode = inst.MachineCode & 0x7F;
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
            a = concatenate (a, d) << 1;
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

void mars (instWord& inst, ofstream& ofile)
{
    switch (inst.opcode)
    {
            //Rtype
        case 0x33:
            marsR(inst, ofile);
            break;
            //Itype
        case 0x13:
        case 0x3:
        case 0x67:
            marsI(inst, ofile);
            break;
            //Stype
        case 0x23:
            marsS(inst, ofile);
            break;
            //SBtype
        case 0x63:
            marsSB(inst, ofile);
            break;
            //Utype
        case 0x37:
        case 0x17:
            marsU(inst, ofile);
            break;
            //UJtype
        case 0x6F:
            marsUJ(inst, ofile);
            break;
            //ecall
        case 0x73 :
            marsEcall(inst, ofile);
            break;
        default:
        {

        }
    }
}

void marsR (instWord& inst, ofstream& ofile)
{
    switch(inst.funct3)
    {
        case 0:
            if(inst.funct7 == 32) //subtract
                ofile << "sub   $" << inst.rd << ",$" << inst.rs1 << ",$" << inst.rs2 << endl;
            else //add
                ofile << "add   $" << inst.rd << ",$" << inst.rs1 << ",$" << inst.rs2 << endl;
            break;
        case 1: //sll
            ofile << "sllv   $" << inst.rd << ",$" << inst.rs1 << ",$" << inst.rs2 << endl;
            break;
        case 2: //slt
            ofile << "slt   $" << inst.rd << ",$" << inst.rs1 << ",$" << inst.rs2 << endl;
            break;
        case 3: //sltu
            ofile << "sltu   $" << inst.rd << ",$" << inst.rs1 << ",$" << inst.rs2 << endl;
            break;
        case 4: //bitwise XOR
            ofile << "xor   $" << inst.rd << ",$" << inst.rs1 << ",$" << inst.rs2 << endl;
            break;
            ///////////////////////////
        case 5:
            if(inst.funct7 == 32) //sra
                ofile << "srav   $" << inst.rd << ",$" << inst.rs1 << ",$" << inst.rs2 << endl;
            else //srl
                ofile << "srlv   $" << inst.rd << ",$" << inst.rs1 << ",$" << inst.rs2 << endl;
            break;
            /////////////////////////////
        case 6: //bitwise OR
            ofile << "or   $" << inst.rd << ",$" << inst.rs1 << ",$" << inst.rs2 << endl;
            break;
        case 7: //bitwise AND
            ofile << "and   $" << inst.rd << ",$" << inst.rs1 << ",$" << inst.rs2 << endl;
            break;
        default:
            ofile << "\tUnknown R Instruction \n";
    }
}

void marsI (instWord& inst, ofstream& ofile)
{
    switch (inst.opcode)
    {
        case 0x13:
        {
            switch (inst.funct3)
            {
                    //addi
                case 0:
                    ofile << "addi   $" << inst.rd << ",$" << inst.rs1 << "," << inst.I_imm << endl;
                    break;
                    //slli
                case 1:
                    ofile << "sll   $" << inst.rd << ",$" << inst.rs1 << "," << inst.I_imm << endl;
                    break;
                    //slti
                case 2:
                    ofile << "slti   $" << inst.rd << ",$" << inst.rs1 << "," << inst.I_imm << endl;
                    break;
                    //sltiu
                case 3:
                    ofile << "sltiu   $" << inst.rd << ",$" << inst.rs1 << "," << inst.I_imm << endl;
                    break;
                    //xori
                case 4:
                    ofile << "xori   $" << inst.rd << ",$" << inst.rs1 << "," << inst.I_imm << endl;
                    break;
                    //sra & srl
                case 5:
                {
                    int hi = inst.I_imm & (0b111111100000); //most signficant 7 bits of i immediate
                    //int lo = inst.I_imm & (0b000000011111); //lower significant 8 bits of i immediate
                    if (hi==0) //srli
                        ofile << "srl   $" << inst.rd << ",$" << inst.rs1 << "," << inst.I_imm << endl;
                    else //srai
                        ofile << "sra   $" << inst.rd << ",$" << inst.rs1 << "," << inst.I_imm << endl;
                }
                    break;
                    //ori
                case 6:
                    ofile << "ori   $" << inst.rd << ",$" << inst.rs1 << "," << inst.I_imm << endl;
                    break;
                    //andi
                case 7:
                    ofile << "andi   $" << inst.rd << ",$" << inst.rs1 << "," << inst.I_imm << endl;
                    break;
                default:
                    ofile << "\tUnknown I Instruction \n";
            }
        }
            break;
        case 0x3:
        {
            switch (inst.funct3)
            {
                    //lb
                case 0:
                    ofile << "lb   $" << inst.rd << "," << inst.I_imm << "($" << inst.rs1 << ")" << endl;
                    break;
                    //lh
                case 1:
                    ofile << "lh   $" << inst.rd << "," << inst.I_imm << "($" << inst.rs1 << ")" << endl;
                    break;
                    //lw
                case 2:
                    ofile << "lw   $" << inst.rd << "," << inst.I_imm << "($" << inst.rs1 << ")" << endl;
                    break;
                    //lbu
                case 4:
                    ofile << "lbu   $" << inst.rd << "," << inst.I_imm << "($" << inst.rs1 << ")" << endl;
                    break;
                    //lhu
                case 5:
                    ofile << "lhu   $" << inst.rd << "," << inst.I_imm << "($" << inst.rs1 << ")" << endl;
                    break;
                default:
                    ofile << "\tUnknown I Instruction \n";
            }
        }
            break;
            //jalr
            ////////////need help////////////////////
        case 0x67:
        {

            ofile << "addi   $" << inst.rs1 << ",$" << inst.rs1  << "," << inst.I_imm << endl;
            ofile << "jr   $" << inst.rs1 << endl;
        }
    }
}

void marsS (instWord& inst, ofstream& ofile)
{
    switch (inst.funct3)
    {
            //sb
        case 0:
            ofile << "sb   $" << inst.rd << "," << inst.S_imm << "($" << inst.rs1 << ")" << endl;
            break;
            //sh
        case 1:
            ofile << "sh   $" << inst.rd << "," << inst.S_imm << "($" << inst.rs1 << ")" << endl;
            break;
            //sw
        case 2:
            ofile << "sw   $" << inst.rd << "," << inst.S_imm << "($" << inst.rs1 << ")" << endl;
            break;
        default:
            ofile << "\tUnknown S Instruction \n";
    }
}

void marsSB (instWord& inst, ofstream& ofile)
{
    switch (inst.funct3)
    {
            //beq
        case 0:
            ofile << "beq   ,$" << inst.rs1 << ",$" << inst.rs2 << "," << inst.B_imm << endl;
            break;
            //bne
        case 1:
            ofile << "bne   ,$" << inst.rs1 << ",$" << inst.rs2 << "," << inst.B_imm <<endl;
            break;
            //blt
        case 4:
        {
            //slt $t0, $rt, $rs
            //bne $t0, $zero, LABEL
            ofile << "slt   ,$t0,$" << inst.rs1 << ",$" << inst.rs2 << endl;
            ofile << "bne   ,$t0,$0," << inst.B_imm << endl;
        }
            break;
            //bge
        case 5:
        {
            //slt $t0, $rt, $rs
            //beq $t0, $zero, LABEL
            ofile << "slt   ,$t0,$" << inst.rs1 << ",$" << inst.rs2 << endl;
            ofile << "beq   ,$t0,$0," << inst.B_imm << endl;
        }
            break;
            //bltu
        case 6:
        {
            ofile << "sltu   ,$t0,$" << inst.rs1 << ",$" << inst.rs2 << endl;
            ofile << "bne   ,$t0,$0," << inst.B_imm << endl;
        }
            break;
            //bgeu
        case 7:
        {
            ofile << "sltu   ,$t0,$" << inst.rs1 << ",$" << inst.rs2 << endl;
            ofile << "beq   ,$t0,$0," << inst.B_imm << endl;
        }
            break;
        default:
            ofile << "\tUnknown R Instruction \n";
    }
}

void marsU (instWord& inst, ofstream& ofile)
{
    switch (inst.opcode)
    {
            //lui
        case 0x37:
            ofile << "lui   $" << inst.rd << "," << inst.U_imm << endl;
            break;
            //auipc
        case 0x17:
            ///////////////////////HELP///////////
            break;
        default:
            ofile << "\tUnknown R Instruction \n";
    }
}

void marsUJ (instWord& inst, ofstream& ofile)
{
    ofile << "jal" << inst.J_imm << endl;
}

void marsEcall (instWord& inst, ofstream& ofile)
{
    ofile << "syscall" << endl;
}
