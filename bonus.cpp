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
    unsigned long instMachineCode;
    unsigned int rd, rs1, rs2; //5 bits
    unsigned int funct3, funct7;
    unsigned int opcode; //7 bits
    unsigned int I_imm; //12 bits
    unsigned int S_imm, B_imm, U_imm, J_imm;
};

struct mipsWord
{
    unsigned int machineCode;
    unsigned int rd, rs, rt; //5 bits
    unsigned int funct; //6 bits in R
    unsigned int shamt; //5 bits in R
    unsigned int I_imm; //16 bits
    unsigned int J_imm; //26 bits
    unsigned int opcode; //6 bits
};
void parse (instWord& inst);
int concatenate (int a, int b);
void mars (instWord& inst, mipsWord& m, ofstream& ofile);
void marsR (instWord& inst,  mipsWord& m, ofstream& ofile);
void marsI (instWord& inst, mipsWord& m, ofstream& ofile);
void marsS (instWord& inst, mipsWord& m, ofstream& ofile);
void marsSB (instWord& inst, mipsWord& m, ofstream& ofile);
void marsU (instWord& inst, mipsWord& m, ofstream& ofile);
void marsUJ (instWord& inst, mipsWord& m, ofstream& ofile);
void marsEcall (instWord& inst, mipsWord& m, ofstream& ofile);
void Rmachine (mipsWord & m, ofstream& ofile);
void Imachine (mipsWord & m, ofstream& ofile);
void Jmachine (mipsWord& m, ofstream& ofile);

int pc;

int main()
{
    ifstream inFile;
    ofstream outFile;
    instWord w;
    mipsWord m;
    
    outFile.open("output.txt", ios::app);
    if (!outFile.is_open())
        cout<< "can't open output file"<<endl;
    inFile.open("div.txt");
    if(inFile.is_open())
    {
        pc=0;
        string t;
        inFile >> t;
        while (!inFile.eof())
        {
            w.instMachineCode = stol (t,nullptr,16);
            parse(w);
            mars (w, m, outFile);
            inFile >> t;
            pc+=4;
        }
    }
    else cout<<"can't open output file"<<endl;
    return 0;
}

void parse (instWord& inst)
{
    inst.opcode = inst.instMachineCode & 0x7F;
    switch (inst.opcode)
    {
            //Rtype
        case 0x33:
        {
            inst.rd = (inst.instMachineCode & 0xF80) >> 7;
            inst.funct3 = (inst.instMachineCode & 0x7000) >> 10;
            inst.rs1 = (inst.instMachineCode & 0xF8000) >> 15;
            inst.rs2 = (inst.instMachineCode & 0x1F00000) >> 20;
            inst.funct7 = (inst.instMachineCode & 0xFE000000) >> 25;
            
        }
            break;
            //Itype
        case 0x13:
        case 0x3:
        case 0x67:
        {
            inst.rd = (inst.instMachineCode & 0xF80) >> 7;
            inst.funct3 = (inst.instMachineCode & 0x7000) >> 10;
            inst.rs1 = (inst.instMachineCode & 0xF8000) >> 15;
            inst.I_imm = (inst.instMachineCode & 0xFFF00000) >> 20;
        }
            break;
            //Stype
        case 0x23:
        {
            inst.rd = (inst.instMachineCode & 0xF80) >> 7;
            int a = inst.instMachineCode & 0x7000;
            inst.rs1 = inst.instMachineCode & 0xF8000;
            inst.rs2 = inst.instMachineCode & 0x1F00000;
            int b = inst.instMachineCode & 0xFE000000;
            inst.S_imm = concatenate(a, b);
        }
            break;
            //SBtype
        case 0x63:
        {
            int c = inst.instMachineCode & 0x80;
            int a = inst.instMachineCode & 0xF00;
            inst.funct3 = inst.instMachineCode & 0x7000;
            inst.rs1 = inst.instMachineCode & 0xF8000;
            inst.rs2 = inst.instMachineCode & 0x1F00000;
            int b = inst.instMachineCode & 0x7E000000;
            int d = inst.instMachineCode & 0xFE000000;
            a = concatenate(a, b);
            a = concatenate (a, d) << 1;
            inst.B_imm = concatenate (a, c) << 1;
        }
            break;
            //Utype
        case 0x37:
        case 0x17:
        {
            inst.rd = inst.instMachineCode & 0xF80;
            inst.U_imm = (inst.instMachineCode & 0xFFFFF000) << 12;
        }
            break;
            //UJtype
        case 0x6F:
        {
            inst.rd = inst.instMachineCode & 0xF80;
            int a = inst.instMachineCode & 0x7FE00000;
            int b = inst.instMachineCode & 0x100000;
            int c = inst.instMachineCode & 0xFF000;
            int d = inst.instMachineCode & 0x80000000;
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

void mars (instWord& inst,  mipsWord& m, ofstream& ofile)
{
    switch (inst.opcode)
    {
            //Rtype
        case 0x33:
            marsR(inst,m, ofile);
            break;
            //Itype
        case 0x13:
        case 0x3:
        case 0x67:
            marsI(inst,m, ofile);
            break;
            //Stype
        case 0x23:
            marsS(inst,m, ofile);
            break;
            //SBtype
        case 0x63:
            marsSB(inst,m, ofile);
            break;
            //Utype
        case 0x37:
        case 0x17:
            marsU(inst,m, ofile);
            break;
            //UJtype
        case 0x6F:
            marsUJ(inst,m, ofile);
            break;
            //ecall
        case 0x73 :
            marsEcall(inst,m, ofile);
            break;
        default:
        {
            
        }
    }
}

void marsR (instWord& inst, mipsWord& m, ofstream& ofile)
{
    m.opcode=0;
    m.rd = inst.rd;
    m.rs = inst.rs1;
    m.rt = inst.rs2;
    m.I_imm = NULL;
    m.J_imm = NULL;
    
    switch(inst.funct3)
    {
        case 0:
            if(inst.funct7 == 32) //risc: subtract --> mips: sub
            {
                m.funct = 0x22;
                m.shamt = 0;
                Rmachine(m, ofile);
                //ofile << "sub   $" << inst.rd << ",$" << inst.rs1 << ",$" << inst.rs2 << endl;
            }
            else //risc: add --> mips: add
            {
                m.funct = 0x20;
                m.shamt = 0;
                Rmachine(m, ofile);
                //ofile << "add   $" << inst.rd << ",$" << inst.rs1 << ",$" << inst.rs2 << endl;
            }
            break;
        case 1: //risc: sll --> mips: sllv
        {
            m.funct = 6;
            m.shamt = 0;
            Rmachine(m, ofile);
            //ofile << "sllv   $" << inst.rd << ",$" << inst.rs1 << ",$" << inst.rs2 << endl;
        }
            break;
        case 2: //risc: slt --> mips: slt
        {
            m.funct = 0x2a;
            m.shamt = 0;
            Rmachine(m, ofile);
            //ofile << "slt   $" << inst.rd << ",$" << inst.rs1 << ",$" << inst.rs2 << endl;
        }
            break;
        case 3: //risc: sltu --> mips: sltu
        {
            m.funct = 0x2b;
            m.shamt = 0;
            Rmachine(m, ofile);
            //ofile << "sltu   $" << inst.rd << ",$" << inst.rs1 << ",$" << inst.rs2 << endl;
        }
            break;
        case 4: //risc: bitwise XOR --> mips: xor
        {
            m.funct = 0x26;
            m.shamt = 0;
            Rmachine(m, ofile);
            //ofile << "xor   $" << inst.rd << ",$" << inst.rs1 << ",$" << inst.rs2 << endl;
        }
            break;
            ///////////////////////////
        case 5:
            if(inst.funct7 == 32) //risc: sra --> mips: srav
            {
                m.funct = 0x7;
                m.shamt = 0;
                Rmachine(m, ofile);
                // ofile << "srav   $" << inst.rd << ",$" << inst.rs1 << ",$" << inst.rs2 << endl;
            }
            else //risc: srl --> mips: srlv
            {
                m.funct = 0x6;
                m.shamt = 0;
                Rmachine(m, ofile);
                //ofile << "srlv   $" << inst.rd << ",$" << inst.rs1 << ",$" << inst.rs2 << endl;
            }
            break;
            /////////////////////////////
        case 6: //risc: bitwise OR --> mips:or
        {
            m.funct = 0x25;
            m.shamt = 0;
            Rmachine(m, ofile);
            //ofile << "or   $" << inst.rd << ",$" << inst.rs1 << ",$" << inst.rs2 << endl;
        }
            break;
        case 7: //risc: bitwise AND --> mips: and
        {
            m.funct = 0x24;
            m.shamt = 0;
            Rmachine(m, ofile);
            //ofile << "and   $" << inst.rd << ",$" << inst.rs1 << ",$" << inst.rs2 << endl;
        }
            break;
        default:
            ofile << "\tUnknown R Instruction \n";
    }
}

void marsI (instWord& inst, mipsWord& m, ofstream& ofile)
{
    switch (inst.opcode)
    {
        case 0x13:
        {
            switch (inst.funct3)
            {
                    //risc: addi --> mips addi
                case 0:
                {
                    m.opcode = 8;
                    m.rs = inst.rs1;
                    m.rt = inst.rd;
                    m.I_imm = inst.I_imm;
                    m.rd = NULL;
                    m.shamt = NULL;
                    m.J_imm = NULL;
                    m.funct = NULL;
                    Imachine(m, ofile);
                    //ofile << "addi   $" << inst.rd << ",$" << inst.rs1 << "," << inst.I_imm << endl;
                }
                    break;
                    //risc: slli --> mips: sll
                case 1:
                {
                    m.opcode = 0;
                    m.funct = 0;
                    m.rs = inst.rs1;
                    m.rt = inst.rd;
                    m.rd = NULL;
                    m.shamt =  inst.I_imm & 0x1F;
                    m.I_imm = NULL;
                    m.J_imm = NULL;
                    Rmachine(m, ofile);
                    //ofile << "sll   $" << inst.rd << ",$" << inst.rs1 << "," << inst.I_imm << endl;
                }
                    break;
                    //risc:slti --> mips:slti
                case 2:
                {
                    m.opcode = 10;
                    m.funct = NULL;
                    m.rs = inst.rs1;
                    m.rt = inst.rd;
                    m.rd = NULL;
                    m.shamt = NULL;
                    m.I_imm = inst.I_imm;
                    m.J_imm = NULL;
                    Imachine(m, ofile);
                    // ofile << "slti   $" << inst.rd << ",$" << inst.rs1 << "," << inst.I_imm << endl;
                }
                    break;
                    //sltiu
                case 3:
                {
                    m.opcode = 11;
                    m.funct = NULL;
                    m.rs = inst.rs1;
                    m.rt = inst.rd;
                    m.rd = NULL;
                    m.shamt = NULL;
                    m.I_imm = inst.I_imm;
                    m.J_imm = NULL;
                    Imachine(m, ofile);
                    //ofile << "sltiu   $" << inst.rd << ",$" << inst.rs1 << "," << inst.I_imm << endl;
                }
                    break;
                    //xori
                case 4:
                {
                    m.opcode = 14;
                    m.funct = NULL;
                    m.rs = inst.rs1;
                    m.rt = inst.rd;
                    m.rd = NULL;
                    m.shamt = NULL;
                    m.I_imm = inst.I_imm;
                    m.J_imm = NULL;
                    Imachine(m, ofile);
                    //ofile << "xori   $" << inst.rd << ",$" << inst.rs1 << "," << inst.I_imm << endl;
                }
                    break;
                    //sra & srl
                case 5:
                {
                    int hi = inst.I_imm & (0b111111100000); //most signficant 7 bits of i immediate
                    //int lo = inst.I_imm & (0b000000011111); //lower significant 8 bits of i immediate
                    if (hi==0) //risc: srli --> mips: srl
                    {
                        m.opcode = 0;
                        m.funct = 2;
                        m.rs = NULL;
                        m.rt = inst.rs1;
                        m.rd = inst.rd;
                        m.shamt = inst.I_imm;
                        m.I_imm = NULL;
                        m.J_imm = NULL;
                        Rmachine(m, ofile);
                        //ofile << "srl   $" << inst.rd << ",$" << inst.rs1 << "," << inst.I_imm << endl;
                    }
                    else //srai
                    {
                        m.opcode = 0;
                        m.funct = 3;
                        m.rs = NULL;
                        m.rt = inst.rs1;
                        m.rd = inst.rd;
                        m.shamt = inst.I_imm;
                        m.I_imm = NULL;
                        m.J_imm = NULL;
                        Rmachine(m, ofile);
                        //ofile << "sra   $" << inst.rd << ",$" << inst.rs1 << "," << inst.I_imm << endl;
                    }
                }
                    break;
                    //ori
                case 6:
                {
                    m.opcode = 13;
                    m.funct = NULL;
                    m.rs = inst.rs1;
                    m.rt = inst.rd;
                    m.rd = NULL;
                    m.shamt = NULL;
                    m.I_imm = inst.I_imm;
                    m.J_imm = NULL;
                    Imachine(m, ofile);
                    //ofile << "ori   $" << inst.rd << ",$" << inst.rs1 << "," << inst.I_imm << endl;
                }
                    break;
                    //andi
                case 7:
                {
                    m.opcode = 12;
                    m.funct = NULL;
                    m.rs = inst.rs1;
                    m.rt = inst.rd;
                    m.rd = NULL;
                    m.shamt = NULL;
                    m.I_imm = inst.I_imm;
                    m.J_imm = NULL;
                    Imachine(m, ofile);
                    //ofile << "andi   $" << inst.rd << ",$" << inst.rs1 << "," << inst.I_imm << endl;
                }
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
                    //risc:lb --> mips:lb
                case 0:
                {
                    m.opcode = 32;
                    m.funct = NULL;
                    m.rs = inst.rs1;
                    m.rt = inst.rd;
                    m.rd = NULL;
                    m.shamt = NULL;
                    m.I_imm = inst.I_imm;
                    m.J_imm = NULL;
                    Imachine(m, ofile);
                    //ofile << "lb   $" << inst.rd << "," << inst.I_imm << "($" << inst.rs1 << ")" << endl;
                }
                    break;
                    //risc:lh --> mips:lh
                case 1:
                {
                    m.opcode = 33;
                    m.funct = NULL;
                    m.rs = inst.rs1;
                    m.rt = inst.rd;
                    m.rd = NULL;
                    m.shamt = NULL;
                    m.I_imm = inst.I_imm;
                    m.J_imm = NULL;
                    Imachine(m, ofile);
                    //ofile << "lh   $" << inst.rd << "," << inst.I_imm << "($" << inst.rs1 << ")" << endl;
                }
                    break;
                    //lw
                case 2:
                {
                    m.opcode = 34;
                    m.funct = NULL;
                    m.rs = inst.rs1;
                    m.rt = inst.rd;
                    m.rd = NULL;
                    m.shamt = NULL;
                    m.I_imm = inst.I_imm;
                    m.J_imm = NULL;
                    Imachine(m, ofile);
                    //ofile << "lw   $" << inst.rd << "," << inst.I_imm << "($" << inst.rs1 << ")" << endl;
                }
                    break;
                    //lbu
                case 4:
                {
                    m.opcode = 36;
                    m.funct = NULL;
                    m.rs = inst.rs1;
                    m.rt = inst.rd;
                    m.rd = NULL;
                    m.shamt = NULL;
                    m.I_imm = inst.I_imm;
                    m.J_imm = NULL;
                    Imachine(m, ofile);
                    //ofile << "lbu   $" << inst.rd << "," << inst.I_imm << "($" << inst.rs1 << ")" << endl;
                }
                    break;
                    //lhu
                case 5:
                {
                    m.opcode = 37;
                    m.funct = NULL;
                    m.rs = inst.rs1;
                    m.rt = inst.rd;
                    m.rd = NULL;
                    m.shamt = NULL;
                    m.I_imm = inst.I_imm;
                    m.J_imm = NULL;
                    Imachine(m, ofile);
                    //ofile << "lhu   $" << inst.rd << "," << inst.I_imm << "($" << inst.rs1 << ")" << endl;
                }
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
            //jal to any address
            //add $rd, $0, $ra
            //jr back to ra
            //addi $at, $rs, imm
            //jr $$at
            
            m.opcode = 3;
            m.funct = NULL;
            m.rs = NULL;
            m.rt = NULL;
            m.rd = 31;
            m.shamt = NULL;
            m.I_imm = inst.I_imm;
            m.J_imm = NULL;
            Jmachine(m, ofile);
            /////
            
            m.opcode=0;
            m.rd = inst.rd;
            m.rs = 0;
            m.rt = 31;
            m.I_imm = NULL;
            m.J_imm = NULL;
            m.funct = 0x20;
            m.shamt = 0;
            Rmachine(m, ofile);
            ////////
            
            m.opcode=0;
            m.rd = NULL;
            m.rs = 31;
            m.rt = NULL;
            m.I_imm = NULL;
            m.J_imm = NULL;
            m.funct = 8;
            m.shamt = 0;
            Rmachine(m, ofile);
            
            ///////
            
            m.opcode = 8;
            m.rs = inst.rs1;
            m.rt = 1;
            m.I_imm = inst.I_imm;
            m.rd = NULL;
            m.shamt = NULL;
            m.J_imm = NULL;
            m.funct = NULL;
            Imachine(m, ofile);
            
            ///////
            
            m.opcode=0;
            m.rd = NULL;
            m.rs = 1;
            m.rt = NULL;
            m.I_imm = NULL;
            m.J_imm = NULL;
            m.funct = 8;
            m.shamt = 0;
            Rmachine(m, ofile);
            //          ofile << "addi   $" << inst.rs1 << ",$" << inst.rs1  << "," << inst.I_imm << endl;
            //          ofile << "jr   $" << inst.rs1 << endl;
        }
    }
}

void marsS (instWord& inst, mipsWord& m, ofstream& ofile)
{
    switch (inst.funct3)
    {
            //sb
        case 0:
        {
            m.opcode = 36;
            m.funct = NULL;
            m.rs = inst.rs1;
            m.rt = inst.rs2;
            m.rd = NULL;
            m.shamt = NULL;
            m.I_imm = inst.I_imm;
            m.J_imm = NULL;
            Imachine(m, ofile);
            //ofile << "sb   $" << inst.rd << "," << inst.S_imm << "($" << inst.rs1 << ")" << endl;
        }
            break;
            //sh
        case 1:
        {
            m.opcode = 41;
            m.funct = NULL;
            m.rs = inst.rs1;
            m.rt = inst.rs2;
            m.rd = NULL;
            m.shamt = NULL;
            m.I_imm = inst.I_imm;
            m.J_imm = NULL;
            Imachine(m, ofile);
            //ofile << "sh   $" << inst.rd << "," << inst.S_imm << "($" << inst.rs1 << ")" << endl;
        }
            break;
            //sw
        case 2:
        {
            m.opcode = 43;
            m.funct = NULL;
            m.rs = inst.rs1;
            m.rt = inst.rs2;
            m.rd = NULL;
            m.shamt = NULL;
            m.I_imm = inst.I_imm;
            m.J_imm = NULL;
            Imachine(m, ofile);
            //ofile << "sw   $" << inst.rd << "," << inst.S_imm << "($" << inst.rs1 << ")" << endl;
        }
            break;
        default:
            ofile << "\tUnknown S Instruction \n";
    }
}

void marsSB (instWord& inst, mipsWord& m, ofstream& ofile)
{
    switch (inst.funct3)
    {
            //beq
        case 0:
        {
            m.opcode = 4;
            m.funct = NULL;
            m.rs = inst.rs1;
            m.rt = inst.rs2;
            m.rd = NULL;
            m.shamt = NULL;
            m.I_imm = inst.I_imm;
            m.J_imm = NULL;
            Imachine(m, ofile);
            //ofile << "beq   ,$" << inst.rs1 << ",$" << inst.rs2 << "," << inst.B_imm << endl;
        }
            break;
            //bne
        case 1:
        {
            m.opcode = 5;
            m.funct = NULL;
            m.rs = inst.rs1;
            m.rt = inst.rs2;
            m.rd = NULL;
            m.shamt = NULL;
            m.I_imm = inst.I_imm;
            m.J_imm = NULL;
            Imachine(m, ofile);
            //ofile << "bne   ,$" << inst.rs1 << ",$" << inst.rs2 << "," << inst.B_imm <<endl;
        }
            break;
            //blt
        case 4:
        {
            m.opcode=0;
            m.rd = inst.rd;
            m.rs = inst.rs1;
            m.rt = inst.rs2;
            m.I_imm = NULL;
            m.J_imm = NULL;
            m.funct = 0x2a;
            m.shamt = 0;
            Rmachine(m, ofile);
            ///////OUTPUT/////////
            
            m.opcode = 5;
            m.funct = NULL;
            m.rs = inst.rs1;
            m.rt = inst.rs2;
            m.rd = NULL;
            m.shamt = NULL;
            m.I_imm = inst.I_imm;
            m.J_imm = NULL;
            Rmachine(m, ofile);
            ////////OUTPUT///////
            
            //slt $t0, $rt, $rs
            //bne $t0, $zero, LABEL
            //ofile << "slt   ,$t0,$" << inst.rs1 << ",$" << inst.rs2 << endl;
            //ofile << "bne   ,$t0,$0," << inst.B_imm << endl;
        }
            break;
            //bge
        case 5:
        {
            m.opcode=0;
            m.rd = inst.rd;
            m.rs = inst.rs1;
            m.rt = inst.rs2;
            m.I_imm = NULL;
            m.J_imm = NULL;
            m.funct = 0x2a;
            m.shamt = 0;
            Rmachine(m, ofile);
            ///////OUTPUT/////////
            m.opcode = 4;
            m.funct = NULL;
            m.rs = inst.rs1;
            m.rt = inst.rs2;
            m.rd = NULL;
            m.shamt = NULL;
            m.I_imm = inst.I_imm;
            m.J_imm = NULL;
            Imachine(m, ofile);
            ///////OUTPUT/////////
            
            //slt $t0, $rt, $rs
            //beq $t0, $zero, LABEL
            //ofile << "slt   ,$t0,$" << inst.rs1 << ",$" << inst.rs2 << endl;
            //ofile << "beq   ,$t0,$0," << inst.B_imm << endl;
        }
            break;
            //bltu
        case 6:
        {
            m.opcode=0;
            m.rd = inst.rd;
            m.rs = inst.rs1;
            m.rt = inst.rs2;
            m.I_imm = NULL;
            m.J_imm = NULL;
            m.funct = 0x2b;
            m.shamt = 0;
            Rmachine(m, ofile);
            ///////OUTPUT//////
            m.opcode = 5;
            m.funct = NULL;
            m.rs = inst.rs1;
            m.rt = inst.rs2;
            m.rd = NULL;
            m.shamt = NULL;
            m.I_imm = inst.I_imm;
            m.J_imm = NULL;
            Imachine(m, ofile);
            ////////OUTPUT///////
            
            
            // ofile << "sltu   ,$t0,$" << inst.rs1 << ",$" << inst.rs2 << endl;
            // ofile << "bne   ,$t0,$0," << inst.B_imm << endl;
        }
            break;
            //bgeu
        case 7:
        {
            m.opcode=0;
            m.rd = inst.rd;
            m.rs = inst.rs1;
            m.rt = inst.rs2;
            m.I_imm = NULL;
            m.J_imm = NULL;
            m.funct = 0x2b;
            m.shamt = 0;
            Rmachine(m, ofile);
            ///////OUTPUT//////
            m.opcode = 4;
            m.funct = NULL;
            m.rs = inst.rs1;
            m.rt = inst.rs2;
            m.rd = NULL;
            m.shamt = NULL;
            m.I_imm = inst.I_imm;
            m.J_imm = NULL;
            Imachine(m, ofile);
            ///////OUTPUT/////////
            
            //ofile << "sltu   ,$t0,$" << inst.rs1 << ",$" << inst.rs2 << endl;
            //ofile << "beq   ,$t0,$0," << inst.B_imm << endl;
        }
            break;
        default:
            ofile << "\tUnknown R Instruction \n";
    }
}

void marsU (instWord& inst, mipsWord& m, ofstream& ofile)
{
    switch (inst.opcode)
    {
            //lui
        case 0x37:
        {
            m.opcode = 15;
            m.funct = NULL;
            m.rs = NULL;
            m.rt = NULL;
            m.rd = inst.rd;
            m.shamt = NULL;
            m.I_imm = inst.U_imm;
            m.J_imm = NULL;
            Imachine(m, ofile);
            ///////OUTPUT/////////
            
            // ofile << "lui   $" << inst.rd << "," << inst.U_imm << endl;
        }
            break;
            //auipc
        case 0x17:
        {
            //jal
            //move ra to another register
            //jr back
            // add u immediate to pc
            m.opcode = 3;
            m.funct = NULL;
            m.rs = NULL;
            m.rt = NULL;
            m.rd = 31;
            m.shamt = NULL;
            m.I_imm = 0;
            m.J_imm = NULL;
            Jmachine(m, ofile);
            /////
            
            m.opcode=0;
            m.rd = inst.rd;
            m.rs = 0;
            m.rt = 31;
            m.I_imm = NULL;
            m.J_imm = NULL;
            m.funct = 0x20;
            m.shamt = 0;
            Rmachine(m, ofile);
            ////////
            
            m.opcode=0;
            m.rd = NULL;
            m.rs = 31;
            m.rt = NULL;
            m.I_imm = NULL;
            m.J_imm = NULL;
            m.funct = 8;
            m.shamt = 0;
            Rmachine(m, ofile);
            //////
            
            
            m.opcode = 3;
            m.funct = NULL;
            m.rs = NULL;
            m.rt = NULL;
            m.rd = 31;
            m.shamt = NULL;
            m.I_imm = inst.I_imm;
            m.J_imm = NULL;
            Jmachine(m, ofile);
            /////
            m.opcode=0;
            m.rd = inst.rd;
            m.rs = 0;
            m.rt = inst.rs2;
            m.I_imm = NULL;
            m.J_imm = NULL;
            m.funct = 0x20;
            m.shamt = 0;
            Rmachine(m, ofile);
            ////////
            
            m.opcode = 8;
            m.rs = inst.rd;
            m.rt = inst.rd;
            m.I_imm = inst.U_imm; //<-------
            m.rd = NULL;
            m.shamt = NULL;
            m.J_imm = NULL;
            m.funct = NULL;
            Imachine(m, ofile);
            
            ////
            
            
        }
            ///////////////////////HELP///////////
            break;
        default:
            ofile << "\tUnknown R Instruction \n";
    }
}

void marsUJ (instWord& inst, mipsWord& m, ofstream& ofile)
{
    m.opcode = 3;
    m.funct = NULL;
    m.rs = NULL;
    m.rt = NULL;
    m.rd = 31;
    m.shamt = NULL;
    m.I_imm = inst.I_imm;
    m.J_imm = NULL;
    Jmachine(m, ofile);
    ///////OUTPUT/////////
    
    //add $rd, $ra, &0
    m.opcode=0;
    m.rd = inst.rd;
    m.rs = 31;
    m.rt = 0;
    m.I_imm = NULL;
    m.J_imm = NULL;
    m.funct = 0x20;
    m.shamt = 0;
    Rmachine(m, ofile);
    
    // ofile << "jal" << inst.J_imm << endl;
}

void marsEcall (instWord& inst, mipsWord& m, ofstream& ofile)
{
    
    //ofile << "syscall" << endl;
}

void Rmachine (mipsWord & m, ofstream& ofile)
{
    m.machineCode = m.funct | (m.shamt << 6) | (m.rd << 11) |
    (m.rt << 16) | (m.rs << 21) | (m.opcode << 26);
    ofile << hex << m.machineCode << endl;
}

void Imachine (mipsWord & m, ofstream& ofile)
{
    m.machineCode = m.I_imm | (m.rt << 16) | (m.rs << 21) |
    (m.opcode << 26);
    ofile << hex << m.machineCode << endl;
}

void Jmachine (mipsWord& m, ofstream& ofile)
{
    m.machineCode = m.J_imm | (m.opcode << 26);
    ofile << hex << m.machineCode << endl;
}

