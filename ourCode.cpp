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
#define REG1    LABEL SPACE REG "," IMM
#define REG2    LABEL SPACE REG "," REG "," IMM
#define REG3    LABEL SPACE REG "," REG "," REG
#define REGi    LABEL SPACE REG "," IMM "\\(" REG "\\)"
// #define MYEX		REG3 OR REGi OR REG1 OR REG2

struct instWord
{
    string Text;
    unsigned int MachineCode;
    unsigned int rd, rs1, rs2, funct3, funct7, opcode;
    unsigned int I_imm, S_imm, B_imm, U_imm, J_imm;
};
int regs[32]={0}; //register simulation
unsigned int pc = 0x0; //program counter
char memory[8*1024];    // 8KB of memory located at address 0

// Function prototype
bool instAssembleExec (instWord& inst);
void printPrefix (unsigned int instA, unsigned int instCode);
void emitError (char *s);
void Rtype (instWord& inst);
void Itype (instWord& inst);
void Stype (instWord& inst);
void SBtype (instWord& inst);
void UJtype (instWord& inst);
void Utype (instWord& inst);
bool ecall (instWord& inst);
void parse (instWord& inst);

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
            getline (inFile, W.Text);

            parse(W);        //parse Text into its instruction format fields
            //Generate instruction machine code and execute instruction

            if (instAssembleExec(W)) //execute instructions. return 0 if code for termination and ecall are detected
            {
                return 0;
            }

            //printPrefix(pc, W.MachineCode);
            //save machine code to an output file
            pc += 4;
        }

        inFile.close();

        // print out the registers values
        for(int i = 0; i < 32; i++)
            cout << "x" << dec << i << ": \t"<< "0x" << hex << setfill('0') << setw(8) << regs[i] << "\n";
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
    cout << "0x" << hex << setfill('0') << setw(8) << instA << "\t0x" << hex << setw(8) << instCode;
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
            ///////////////////////
            //////////////////////
        case 0x73: return ecall(inst);
            break;
        default:
        {
            cout << "unsupported instruction\n";
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
    int B_imm = inst.B_imm << 1;
    switch (inst.funct3)
    {
            //beq
        case 0:
        {
            if (regs[inst.rs1] == regs[inst.rs2])
                pc = pc + B_imm;
        }
            break;
            //bne
        case 1:
        {
            if (regs[inst.rs1] != regs[inst.rs2])
                pc = pc + B_imm;
        }
            break;
            //blt
        case 4:
        {
            if (regs[inst.rs1] < regs[inst.rs2])
                pc = pc + B_imm;
        }
            break;
            //bge
        case 5:
        {
            if (regs[inst.rs1] >= regs[inst.rs2])
                pc = pc + B_imm;
        }
            break;
            //bltu
        case 6:
        {
            if ((unsigned int)regs[inst.rs1] < (unsigned int)regs[inst.rs2])
                pc = pc + B_imm;
        }
            break;
            //bgeu
        case 7:
        {
            if ((unsigned int)regs[inst.rs1] >= (unsigned int)regs[inst.rs2])
                pc = pc + B_imm;
        }
            break;
        default:
            cout << "\tUnknown R Instruction \n";
    }
}

//function to execute UJ instructions
void UJtype (instWord& inst)
{
    int J_imm = inst.J_imm << 1;
    inst.rd = pc + 4;
    pc = pc + J_imm;
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

unsigned int getfield(int& after, char beg, char end, string s)
{
    int x, i, k;
    string sub;

    i = s.find(beg, after);
    k = s.find(end, i);
    sub = s.substr(i + 1, k - i - 1);

    stringstream stream(sub);
    stream >> x;
    after = k+1;
    return x;
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
            inst.MachineCode = inst.opcode | (inst.rd << 7) | (inst.funct3 << 12) |
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
        //I
        else if (ins == "addi")
        {
            regex ex(REG2);
            smatch M;
            regex_search(inst.Text, M, ex);
            inst.rd     = (unsigned int)stoi(M[2]);
            inst.rs1    = (unsigned int)stoi(M[3]);
            inst.I_imm  = (unsigned int)stoi(M[4]);
            inst.funct3 = 0b000;
            inst.opcode = 0b0010011;
            inst.MachineCode = inst.opcode | (inst.rd << 7) | (inst.funct3 << 12) |
                               (inst.rs1 << 15) | (inst.I_imm << 20);
        }
        else if (ins == "slli")
        {
            regex ex(REG2);
            smatch M;
            regex_search(inst.Text, M, ex);
            inst.rd     = (unsigned int)stoi(M[2]);
            inst.rs1    = (unsigned int)stoi(M[3]);
            inst.I_imm  = (unsigned int)stoi(M[4]);
            inst.funct3 = 0b001;
            inst.opcode = 0b0010011;
            inst.MachineCode = inst.opcode | (inst.rd << 7) | (inst.funct3 << 12) |
                               (inst.rs1 << 15) | (inst.I_imm << 20);
        }
        else if (ins == "srli")
        {
            regex ex(REG2);
            smatch M;
            regex_search(inst.Text, M, ex);
            inst.rd     = (unsigned int)stoi(M[2]);
            inst.rs1    = (unsigned int)stoi(M[3]);
            inst.I_imm  = (unsigned int)stoi(M[4]);
            inst.funct3 = 0b101;
            inst.opcode = 0b0010011;
            inst.MachineCode = inst.opcode | (inst.rd << 7) | (inst.funct3 << 12) |
                               (inst.rs1 << 15) | (inst.I_imm << 20);
        }
        else if (ins == "srai")
        {
            regex ex(REG2);
            smatch M;
            regex_search(inst.Text, M, ex);
            inst.rd     = (unsigned int)stoi(M[2]);
            inst.rs1    = (unsigned int)stoi(M[3]);
            inst.I_imm  = ((0b010000000000) | (unsigned int)stoi(M[4]));
            inst.funct3 = 0b101;
            inst.opcode = 0b0010011;
            inst.MachineCode = inst.opcode | (inst.rd << 7) | (inst.funct3 << 12) |
                               (inst.rs1 << 15) | (inst.I_imm << 20);
        }
        else if (ins == "andi")
        {
            regex ex(REG2);
            smatch M;
            regex_search(inst.Text, M, ex);
            inst.rd     = (unsigned int)stoi(M[2]);
            inst.rs1    = (unsigned int)stoi(M[3]);
            inst.I_imm  = (unsigned int)stoi(M[4]);
            inst.funct3 = 0b111;
            inst.opcode = 0b0010011;
            inst.MachineCode = inst.opcode | (inst.rd << 7) | (inst.funct3 << 12) |
                               (inst.rs1 << 15) | (inst.I_imm << 20);
        }
        else if (ins == "ori")
        {
            regex ex(REG2);
            smatch M;
            regex_search(inst.Text, M, ex);
            inst.rd     = (unsigned int)stoi(M[2]);
            inst.rs1    = (unsigned int)stoi(M[3]);
            inst.I_imm  = (unsigned int)stoi(M[4]);
            inst.funct3 = 0b110;
            inst.opcode = 0b0010011;
            inst.MachineCode = inst.opcode | (inst.rd << 7) | (inst.funct3 << 12) |
                               (inst.rs1 << 15) | (inst.I_imm << 20);
        }
        else if (ins == "xori")
        {
            regex ex(REG2);
            smatch M;
            regex_search(inst.Text, M, ex);
            inst.rd     = (unsigned int)stoi(M[2]);
            inst.rs1    = (unsigned int)stoi(M[3]);
            inst.I_imm  = (unsigned int)stoi(M[4]);
            inst.funct3 = 0b100;
            inst.opcode = 0b0010011;
            inst.MachineCode = inst.opcode | (inst.rd << 7) | (inst.funct3 << 12) |
                               (inst.rs1 << 15) | (inst.I_imm << 20);
        }
        else if (ins == "slti")
        {
            regex ex(REG2);
            smatch M;
            regex_search(inst.Text, M, ex);
            inst.rd     = (unsigned int)stoi(M[2]);
            inst.rs1    = (unsigned int)stoi(M[3]);
            inst.I_imm  = (unsigned int)stoi(M[4]);
            inst.funct3 = 0b010;
            inst.opcode = 0b0010011;
            inst.MachineCode = inst.opcode | (inst.rd << 7) | (inst.funct3 << 12) |
                               (inst.rs1 << 15) | (inst.I_imm << 20);
        }
        else if (ins == "sltiu")
        {
            regex ex(REG2);
            smatch M;
            regex_search(inst.Text, M, ex);
            inst.rd     = (unsigned int)stoi(M[2]);
            inst.rs1    = (unsigned int)stoi(M[3]);
            inst.I_imm  = (unsigned int)stoi(M[4]);
            inst.funct3 = 0b011;
            inst.opcode = 0b0010011;
            inst.MachineCode = inst.opcode | (inst.rd << 7) | (inst.funct3 << 12) |
                               (inst.rs1 << 15) | (inst.I_imm << 20);
        }
        //U
        else if (ins == "lui")
        {
            regex ex(REG1);
            smatch M;
            regex_search(inst.Text, M, ex);
            inst.rd     = (unsigned int)stoi(M[2]);
            inst.U_imm  = (unsigned int)stoi(M[3]);
            inst.opcode = 0b0110111;
            inst.MachineCode = inst.opcode | (inst.rd << 7) | (inst.U_imm << 12);
        }
        else if (ins == "auipc")
        {
            regex ex(REG1);
            smatch M;
            regex_search(inst.Text, M, ex);
            inst.rd     = (unsigned int)stoi(M[2]);
            inst.U_imm  = (unsigned int)stoi(M[3]);
            inst.opcode = 0b0010111;
            inst.MachineCode = inst.opcode | (inst.rd << 7) | (inst.U_imm << 12);
        }
        //I
        else if (ins == "lb")
        {
            regex ex(REGi);
            smatch M;
            regex_search(inst.Text, M, ex);
            inst.rd     = (unsigned int)stoi(M[2]);
            inst.rs1    = (unsigned int)stoi(M[4]);
            inst.I_imm    = (unsigned int)stoi(M[3]);
            inst.funct3 = 0b000;
            inst.opcode = 0b0000011;
            inst.MachineCode = inst.opcode | (inst.rd << 7) | (inst.funct3 << 12) |
                               (inst.rs1 << 15) | (inst.I_imm << 20);

        }
        else if (ins == "lbu")
        {
            regex ex(REGi);
            smatch M;
            regex_search(inst.Text, M, ex);
            inst.rd     = (unsigned int)stoi(M[2]);
            inst.rs1    = (unsigned int)stoi(M[4]);
            inst.I_imm    = (unsigned int)stoi(M[3]);
            inst.funct3 = 0b100;
            inst.opcode = 0b0000011;
            inst.MachineCode = inst.opcode | (inst.rd << 7) | (inst.funct3 << 12) |
                               (inst.rs1 << 15) | (inst.I_imm << 20);

        }
        else if (ins == "lh")
        {
            regex ex(REG3);
            smatch M;
            regex_search(inst.Text, M, ex);
            inst.rd     = (unsigned int)stoi(M[]);
            inst.rs1    = (unsigned int)stoi(M[]);
            inst.I_imm    = (unsigned int)stoi(M[]);
            inst.funct3 = 0b001;
            inst.opcode = 0b0000011;
            inst.MachineCode = inst.opcode | (inst.rd << 7) | (inst.funct3 << 12) |
                               (inst.rs1 << 15) | (inst.I_imm << 20);

        }
        else if (ins == "lhu")
        {
            regex ex(REG3);
            smatch M;
            regex_search(inst.Text, M, ex);
            inst.rd     = (unsigned int)stoi(M[]);
            inst.rs1    = (unsigned int)stoi(M[]);
            inst.I_imm    = (unsigned int)stoi(M[]);
            inst.funct3 = 0b101;
            inst.funct7 = NULL;
            inst.opcode = 0b0000011;
            inst.rs2  = NULL;
            inst.S_imm  = NULL;
            inst.B_imm  = NULL;
            inst.U_imm  = NULL;
            inst.J_imm  = NULL;
            inst.MachineCode = inst.opcode | (inst.rd << 7) | (inst.funct3 << 12) |
                               (inst.rs1 << 15) | (inst.I_imm << 20);

        }
        else if (ins == "lw")
        {
            regex ex(REG3);
            smatch M;
            regex_search(inst.Text, M, ex);
            inst.rd     = (unsigned int)stoi(M[]);
            inst.rs1    = (unsigned int)stoi(M[]);
            inst.I_imm    = (unsigned int)stoi(M[]);
            inst.funct3 = 0b010;
            inst.funct7 = NULL;
            inst.opcode = 0b0000011;
            inst.rs2  = NULL;
            inst.S_imm  = NULL;
            inst.B_imm  = NULL;
            inst.U_imm  = NULL;
            inst.J_imm  = NULL;
            inst.MachineCode = inst.opcode | (inst.rd << 7) | (inst.funct3 << 12) |
                               (inst.rs1 << 15) | (inst.I_imm << 20);
        }
        //S
        else if (ins == "sb")
        {
            regex ex(REG3);
            smatch M;
            regex_search(inst.Text, M, ex);


        }
        else if (ins == "sh")
        {
            regex ex(REG3);
            smatch M;
            regex_search(inst.Text, M, ex);


        }
        else if (ins == "sw")
        {
            regex ex(REG3);
            smatch M;
            regex_search(inst.Text, M, ex);


        }
        //SB
        else if (ins == "beq")
        {
            regex ex(REG3);
            smatch M;
            regex_search(inst.Text, M, ex);


        }
        else if (ins == "bne")
        {
            regex ex(REG3);
            smatch M;
            regex_search(inst.Text, M, ex);


        }
        else if (ins == "blt")
        {
            regex ex(REG3);
            smatch M;
            regex_search(inst.Text, M, ex);


        }
        else if (ins == "bltu")
        {
            regex ex(REG3);
            smatch M;
            regex_search(inst.Text, M, ex);


        }
        else if (ins == "bge")
        {
            regex ex(REG3);
            smatch M;
            regex_search(inst.Text, M, ex);


        }
        else if (ins == "bgeu")
        {
            regex ex(REG3);
            smatch M;
            regex_search(inst.Text, M, ex);


        }
        //UJ
        else if (ins == "jal")
        {
            regex ex(REG3);
            smatch M;
            regex_search(inst.Text, M, ex);


        }
        //I
        else if (ins == "jalr")
        {
            regex ex(REG3);
            smatch M;
            regex_search(inst.Text, M, ex);


        }
        else
        {
            //UNKOWN
        }
    }
    else
    {
        // LABEL CODE
        cout <<"LABEL";
    }
}
