/*
 This is just a skeleton. It DOES NOT implement all the requirements.
 It only recognizes the "ADD" and "SUB" instructions and prints
 "Unknown Instruction" for all other instructions.
 
 References:
 (1) The risc-v ISA Manual ver. 2.1 @ https://riscv.org/specifications/
 (2) https://github.com/michaeljclark/riscv-meta/blob/master/meta/opcodes
 */

#include <iostream>
#include <fstream>
#include "stdlib.h"
#include <iomanip>
#include <string>
using namespace std;

struct instWord
{
    string instText;
    unsigned int instMachineCode;
    unsigned int rd, rs1, rs2, funct3, funct7, opcode;
    unsigned int I_imm, S_imm, B_imm, U_imm, J_imm;
};
int regs[32]={0}; //register simulation
unsigned int pc = 0x0; //program counter
char memory[8*1024];    // 8KB of memory located at address 0

//Function prototype
bool instAssembleExec(instWord& inst);
void printPrefix(unsigned int instA, unsigned int instCode);
void emitError(char *s);
void Rtype (instWord& inst);
void Itype(instWord& inst);
void Stype (instWord& inst);
void SBtype (instWord& inst);
void UJtype (instWord& inst);
void Utype (instWord& inst);
bool ecall (instWord& inst);

int main()
{
    ifstream inFile;
    ofstream outFile;
    instWord W;
    
    inFile.open("div.s");
    if(inFile.is_open())
    {
        pc = 0x0;
        while(!inFile.eof())
        {
            getline (inFile, W.instText);
            
            //parse(&W);        //parse instText into its instruction format fields
            //Generate instruction machine code and execute instruction
            
            if (instAssembleExec(W)) //execute instructions. return 0 if code for termination and ecall are detected
            {
                return 0;
            }
            
            //printPrefix(pc, W.instMachineCode);
            //save machine code to an output file
            pc += 4;
        }
        
        inFile.close();
        
        // print out the registers values
        for(int i = 0; i < 32; i++)
            cout << "x" << dec << i << ": \t"<< "0x" << hex << std::setfill('0') << std::setw(8) << regs[i] << "\n";
    }
    else
        emitError("Cannot access input file\n");
}

void emitError(char *s)
{
    cout << s;
    exit(0);
}

void printPrefix(unsigned int instA, unsigned int instCode)
{
    cout << "0x" << hex << std::setfill('0') << std::setw(8) << instA << "\t0x" << hex << std::setw(8) << instCode;
}


//returns true only if x10=10/ code for program termination
bool instAssembleExec(instWord&inst)
{
    //execute instruction
    switch(inst.opcode)
    {
        case 0x33:
        {
            Rtype(inst);
            return false;
        }
            break;
        case 0x13:
        {
            Itype(inst);
            return false;
        }
            break;
        case 0x3:
        {
            Itype(inst);
            return false;
        }
            break;
        case 0x67:
        {
            Itype (inst);
            return false;
        }
            break;
        case 0x23:
        {
            Stype(inst);
            return false;
        }
            break;
        case 0x63:
        {
            SBtype(inst);
            return false;
        }
            break;
        case 0x6F:
        {
            UJtype(inst);
            return false;
        }
            break;
        case 0x37:
        {
            Utype(inst);
            return false;
        }
            break;
        case 0x17:
        {
            Utype(inst);
            return false;
        }
            break;
        case 0: return ecall(inst);
            break;
        default:
        {
            cout<<"unsupported instruction\n";
            return false;
        }
            
    }
    
}

//function to execute S type
void Stype (instWord& inst)
{
    switch (inst.funct3)
    {
            //sb
        case 0:
        {
            int t = inst.rs2 & 0x000000ff;
            memory[inst.rs1 + inst.S_imm] = t;
        }
            break;
            //sh
        case 1:
        {
            int t = inst.rs2 & 0x0000ffff;
            memory[inst.rs1 + inst.S_imm] = t;
        }
            break;
            //sw
        case 2:
        {
            memory[inst.rs1 + inst.S_imm] = inst.rs2;
        }
            break;
        default:
            cout << "\tUnknown S Instruction \n";
    }
}

//Function to execute I instructions
void Itype(instWord& inst)
{
    switch (inst.opcode)
    {
        case 0x13:
        {
            switch (inst.funct3)
            {
                    //addi
                case 0: regs[inst.rd] = regs[inst.rs1] + (inst.I_imm);
                    break;
                    //slli
                case 1:
                {
                    int lo = inst.I_imm & (0b000000011111); //lower significant 8 bits of i immediate
                    regs[inst.rd] = regs[inst.rs1] << lo;
                }
                    break;
                    //slti
                case 2: regs[inst.rd] = (regs[inst.rs1] < (inst.I_imm));
                    break;
                    //sltiu
                case 3: regs[inst.rd] = ( (unsigned int) regs[inst.rs1] < ((unsigned int)inst.I_imm) );
                    break;
                    //xori
                case 4: regs[inst.rd] = regs[inst.rs1] ^ (inst.I_imm);
                    break;
                    //sra & srl
                case 5:
                {
                    int hi = inst.I_imm & (0b111111100000); //most signficant 7 bits of i immediate
                    int lo = inst.I_imm & (0b000000011111); //lower significant 8 bits of i immediate
                    if (hi==0) //srli
                        regs[inst.rd] = (unsigned int) regs[inst.rs1] >> lo;
                    else //srai
                        regs[inst.rd] = regs[inst.rs1] >> lo;
                }
                    break;
                    //ori
                case 6:  regs[inst.rd] = regs[inst.rs1] | (inst.I_imm);
                    break;
                    //andi
                case 7: regs[inst.rd] = regs[inst.rs1] & (inst.I_imm);
                    break;
                default:
                    cout << "\tUnknown I Instruction \n";
            }
        }
            break;
        case 0x3:
        {
            switch (inst.funct3)
            {
                    //lb
                case 0:
                    regs[inst.rd] = memory[regs[inst.rs1] + inst.I_imm];
                    break;
                    //lh
                case 1:
                {
                    int x = memory[regs[inst.rs1] + inst.I_imm] << 8;
                    int y = (unsigned int) memory[regs[inst.rs1] + inst.I_imm+1];
                    
                    regs[inst.rd] = x + y;
                }
                    break;
                    //lw
                case 2:
                {
                    int x = memory[regs[inst.rs1] + inst.I_imm] << 24;
                    int y = (unsigned int) memory[regs[inst.rs1] + inst.I_imm+1] << 16;
                    int z = (unsigned int) memory[regs[inst.rs1] + inst.I_imm+2] << 8;
                    int t = (unsigned int) memory[regs[inst.rs1] + inst.I_imm+3];
                    regs[inst.rd] = x + y + z + t;
                }
                    break;
                    //lbu
                case 4:
                    regs[inst.rd] = (unsigned int) memory[regs[inst.rs1] + inst.I_imm];
                    break;
                    //lhu
                case 5:
                {
                    int x = (unsigned int) memory[regs[inst.rs1] + inst.I_imm] << 8;
                    int y = (unsigned int) memory[regs[inst.rs1] + inst.I_imm+1];
                    
                    regs[inst.rd] = x + y;
                }
                    break;
                default:
                    cout << "\tUnknown I Instruction \n";
            }
        }
            break;
        case 0x67:
        {
            inst.rd = pc + 4;
            pc = inst.rs1 + inst.I_imm;
            pc = pc & 0xfffffffe;
        }
    }
}


//function to execute R instructions
void Rtype (instWord& inst)
{
    switch(inst.funct3)
    {
        case 0:
            if(inst.funct7 == 32) //subtract
                regs[inst.rd] = regs[inst.rs1] - regs[inst.rs2];
            else //add
                regs[inst.rd] = regs[inst.rs1] + regs[inst.rs2];
            break;
        case 1: //sll
            regs[inst.rd] = regs[inst.rs1] << regs[inst.rs2];
            break;
        case 2: //slt
            if (regs[inst.rs1] < regs[inst.rs2])
                regs[inst.rd] = 1;
            break;
        case 3: //sltu
            if ((unsigned int)regs[inst.rs1] < (unsigned int)regs[inst.rs2])
                regs[inst.rd]=1;
            break;
        case 4: //bitwise XOR
            regs[inst.rd] = regs[inst.rs1] ^ regs[inst.rs2];
            break;
            ///////////////////////////
        case 5:
            if(inst.funct7 == 32) //sra
                regs[inst.rd] = regs[inst.rs1] >> regs[inst.rs2];
            else //srl
                regs[inst.rd] = (unsigned int)regs[inst.rs1] >> regs[inst.rs2];
            break;
            /////////////////////////////
        case 6: //bitwise OR
            regs[inst.rd] = regs[inst.rs1] | regs[inst.rs2];
            break;
        case 7: //bitwise AND
            regs[inst.rd] = regs[inst.rs1] & regs[inst.rs2];
            break;
        default:
            cout << "\tUnknown R Instruction \n";
    }
}


//function to execute SB instructions
void SBtype (instWord& inst)
{
    switch (inst.funct3)
    {
            //beq
        case 0:
        {
            if (regs[inst.rs1] == regs[inst.rs1])
                pc = pc + inst.B_imm;
        }
            break;
            //bne
        case 1:
        {
            if (regs[inst.rs1] != regs[inst.rs1])
                pc = pc + inst.B_imm;
        }
            break;
            //blt
        case 4:
        {
            if (regs[inst.rs1] < regs[inst.rs1])
                pc = pc + inst.B_imm;
        }
            break;
            //bge
        case 5:
        {
            if (regs[inst.rs1] >= regs[inst.rs1])
                pc = pc + inst.B_imm;
        }
            break;
            //bltu
        case 6:
        {
            if ((unsigned int)regs[inst.rs1] < (unsigned int)regs[inst.rs1])
                pc = pc + inst.B_imm;
        }
            break;
            //bgeu
        case 7:
        {
            if ((unsigned int)regs[inst.rs1] >= (unsigned int)regs[inst.rs1])
                pc = pc + inst.B_imm;
        }
            break;
        default:
            cout << "\tUnknown R Instruction \n";
    }
}

//function to execute UJ instructions
void UJtype (instWord& inst)
{
    inst.rd = pc + 4;
    pc = pc + inst.J_imm;
}

//function to execute U instructions
void Utype (instWord& inst)
{
    switch (inst.opcode)
    {
            //lui
        case 0x37: regs[inst.rd] = inst.U_imm;
            break;
            //auipc
        case 0x17: regs[inst.rd] = pc + inst.U_imm;
            break;
    }
}


//function to execute ecall
//returns true only if x10=10/ code for program termination
bool ecall (instWord& inst)
{
    switch (regs[10])
    {
        case 1:
        {
            cout << regs[11];
            return false;
        }
            break;
        case 4:
        {
            cout << regs[11];
            return false;
        }
            break;
        case 5:
        {
            cin >> regs[11];
            return false;
        }
            break;
        case 10:
            return true;
            break;
        default:
        {
            cout<< "Unsupported ecall instruction\n";
            return false;
        }
    }
}
