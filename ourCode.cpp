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
    switch (inst.funct3)
    {
            //beq
        case 0:
        {
            if (regs[inst.rs1] == regs[inst.rs2])
                pc = pc + inst.B_imm;
        }
            break;
            //bne
        case 1:
        {
            if (regs[inst.rs1] != regs[inst.rs2])
                pc = pc + inst.B_imm;
        }
            break;
            //blt
        case 4:
        {
            if (regs[inst.rs1] < regs[inst.rs2])
                pc = pc + inst.B_imm;
        }
            break;
            //bge
        case 5:
        {
            if (regs[inst.rs1] >= regs[inst.rs2])
                pc = pc + inst.B_imm;
        }
            break;
            //bltu
        case 6:
        {
            if ((unsigned int)regs[inst.rs1] < (unsigned int)regs[inst.rs2])
                pc = pc + inst.B_imm;
        }
            break;
            //bgeu
        case 7:
        {
            if ((unsigned int)regs[inst.rs1] >= (unsigned int)regs[inst.rs2])
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
    int pos = inst.Text.find('\t');
    string part = inst.Text.substr(0, pos++);

    //R
    if (part == "add")
    {
        inst.rd     = getfield(pos, 'x', ',', inst.Text);
        inst.rs1    = getfield(pos, 'x', ',', inst.Text);
        inst.rs2    = getfield(pos, 'x', '\n', inst.Text);
        inst.funct3 = 0b000;
        inst.funct7 = 0b0000000;
        inst.opcode = 0b0110011;
        inst.I_imm  = NULL;
        inst.S_imm  = NULL;
        inst.B_imm  = NULL;
        inst.U_imm  = NULL;
        inst.J_imm  = NULL;
        inst.MachineCode =  inst.MachineCode = inst.opcode | (inst.rd << 7) | (inst.funct3 << 12) |
                    (inst.rs1 << 15) | (inst.rs2 << 20) | (inst.funct7 << 25);inst.opcode | (inst.rd << 7) | (inst.funct3 << 12) |
                    (inst.rs1 << 15) | (inst.rs2 << 20) | (inst.funct7 << 25);
    }
    else if (part == "sub")
    {
        inst.rd     = getfield(pos, 'x', ',', inst.Text);
        inst.rs1    = getfield(pos, 'x', ',', inst.Text);
        inst.rs2    = getfield(pos, 'x', '\n', inst.Text);
        inst.funct3 = 0b000;
        inst.funct7 = 0b0100000;
        inst.opcode = 0b0110011;
        inst.I_imm  = NULL;
        inst.S_imm  = NULL;
        inst.B_imm  = NULL;
        inst.U_imm  = NULL;
        inst.J_imm  = NULL;
        inst.MachineCode = inst.opcode | (inst.rd << 7) | (inst.funct3 << 12) |
                    (inst.rs1 << 15) | (inst.rs2 << 20) | (inst.funct7 << 25);

    }
    else if (part == "sll")
    {
        inst.rd     = getfield(pos, 'x', ',', inst.Text);
        inst.rs1    = getfield(pos, 'x', ',', inst.Text);
        inst.rs2    = getfield(pos, 'x', '\n', inst.Text);
        inst.funct3 = 0b001;
        inst.funct7 = 0b0000000;
        inst.opcode = 0b0110011;
        inst.I_imm  = NULL;
        inst.S_imm  = NULL;
        inst.B_imm  = NULL;
        inst.U_imm  = NULL;
        inst.J_imm  = NULL;
        inst.MachineCode = inst.opcode | (inst.rd << 7) | (inst.funct3 << 12) |
                    (inst.rs1 << 15) | (inst.rs2 << 20) | (inst.funct7 << 25);

    }
    else if (part == "srl")
    {
        inst.rd     = getfield(pos, 'x', ',', inst.Text);
        inst.rs1    = getfield(pos, 'x', ',', inst.Text);
        inst.rs2    = getfield(pos, 'x', '\n', inst.Text);
        inst.funct3 = 0b101;
        inst.funct7 = 0b0000000;
        inst.opcode = 0b0110011;
        inst.I_imm  = NULL;
        inst.S_imm  = NULL;
        inst.B_imm  = NULL;
        inst.U_imm  = NULL;
        inst.J_imm  = NULL;
        inst.MachineCode = inst.opcode | (inst.rd << 7) | (inst.funct3 << 12) |
                    (inst.rs1 << 15) | (inst.rs2 << 20) | (inst.funct7 << 25);

    }
    else if (part == "sra")
    {
        inst.rd     = getfield(pos, 'x', ',', inst.Text);
        inst.rs1    = getfield(pos, 'x', ',', inst.Text);
        inst.rs2    = getfield(pos, 'x', '\n', inst.Text);
        inst.funct3 = 0b101;
        inst.funct7 = 0b0100000;
        inst.opcode = 0b0110011;
        inst.I_imm  = NULL;
        inst.S_imm  = NULL;
        inst.B_imm  = NULL;
        inst.U_imm  = NULL;
        inst.J_imm  = NULL;
        inst.MachineCode = inst.opcode | (inst.rd << 7) | (inst.funct3 << 12) |
                    (inst.rs1 << 15) | (inst.rs2 << 20) | (inst.funct7 << 25);

    }
    else if (part == "and")
    {
        inst.rd     = getfield(pos, 'x', ',', inst.Text);
        inst.rs1    = getfield(pos, 'x', ',', inst.Text);
        inst.rs2    = getfield(pos, 'x', '\n', inst.Text);
        inst.funct3 = 0b111;
        inst.funct7 = 0b0000000;
        inst.opcode = 0b0110011;
        inst.I_imm  = NULL;
        inst.S_imm  = NULL;
        inst.B_imm  = NULL;
        inst.U_imm  = NULL;
        inst.J_imm  = NULL;
        inst.MachineCode = inst.opcode | (inst.rd << 7) | (inst.funct3 << 12) |
                    (inst.rs1 << 15) | (inst.rs2 << 20) | (inst.funct7 << 25);

    }
    else if (part == "or")
    {
        inst.rd     = getfield(pos, 'x', ',', inst.Text);
        inst.rs1    = getfield(pos, 'x', ',', inst.Text);
        inst.rs2    = getfield(pos, 'x', '\n', inst.Text);
        inst.funct3 = 0b110;
        inst.funct7 = 0b0000000;
        inst.opcode = 0b0110011;
        inst.I_imm  = NULL;
        inst.S_imm  = NULL;
        inst.B_imm  = NULL;
        inst.U_imm  = NULL;
        inst.J_imm  = NULL;
        inst.MachineCode = inst.opcode | (inst.rd << 7) | (inst.funct3 << 12) |
                    (inst.rs1 << 15) | (inst.rs2 << 20) | (inst.funct7 << 25);

    }
    else if (part == "xor")
    {
        inst.rd     = getfield(pos, 'x', ',', inst.Text);
        inst.rs1    = getfield(pos, 'x', ',', inst.Text);
        inst.rs2    = getfield(pos, 'x', '\n', inst.Text);
        inst.funct3 = 0b100;
        inst.funct7 = 0b0000000;
        inst.opcode = 0b0110011;
        inst.I_imm  = NULL;
        inst.S_imm  = NULL;
        inst.B_imm  = NULL;
        inst.U_imm  = NULL;
        inst.J_imm  = NULL;
        inst.MachineCode = inst.opcode | (inst.rd << 7) | (inst.funct3 << 12) |
                    (inst.rs1 << 15) | (inst.rs2 << 20) | (inst.funct7 << 25);

    }
    else if (part == "slt")
    {
        inst.rd     = getfield(pos, 'x', ',', inst.Text);
        inst.rs1    = getfield(pos, 'x', ',', inst.Text);
        inst.rs2    = getfield(pos, 'x', '\n', inst.Text);
        inst.funct3 = 0b010;
        inst.funct7 = 0b0000000;
        inst.opcode = 0b0110011;
        inst.I_imm  = NULL;
        inst.S_imm  = NULL;
        inst.B_imm  = NULL;
        inst.U_imm  = NULL;
        inst.J_imm  = NULL;
        inst.MachineCode = inst.opcode | (inst.rd << 7) | (inst.funct3 << 12) |
                    (inst.rs1 << 15) | (inst.rs2 << 20) | (inst.funct7 << 25);

    }
    else if (part == "sltu")
    {
        inst.rd     = getfield(pos, 'x', ',', inst.Text);
        inst.rs1    = getfield(pos, 'x', ',', inst.Text);
        inst.rs2    = getfield(pos, 'x', '\n', inst.Text);
        inst.funct3 = 0b011;
        inst.funct7 = 0b0000000;
        inst.opcode = 0b0110011;
        inst.I_imm  = NULL;
        inst.S_imm  = NULL;
        inst.B_imm  = NULL;
        inst.U_imm  = NULL;
        inst.J_imm  = NULL;
        inst.MachineCode = inst.opcode | (inst.rd << 7) | (inst.funct3 << 12) |
                    (inst.rs1 << 15) | (inst.rs2 << 20) | (inst.funct7 << 25);

    }

    //I
    else if (part == "addi")
    {



    }
    else if (part == "slli")
    {



    }
    else if (part == "srli")
    {



    }
    else if (part == "srai")
    {



    }
    else if (part == "andi")
    {



    }
    else if (part == "ori")
    {



    }
    else if (part == "xori")
    {



    }
    else if (part == "slti")
    {



    }
    else if (part == "sltiu")
    {



    }
    //U
    else if (part == "lui")
    {



    }
    else if (part == "auipc")
    {



    }
    //I
    else if (part == "lb")
    {



    }
    else if (part == "lbu")
    {



    }
    else if (part == "lh")
    {



    }
    else if (part == "lhu")
    {



    }
    else if (part == "lw")
    {



    }
    //S
    else if (part == "sb")
    {



    }
    else if (part == "sh")
    {



    }
    else if (part == "sw")
    {



    }
    //SB
    else if (part == "beq")
    {



    }
    else if (part == "bne")
    {



    }
    else if (part == "blt")
    {



    }
    else if (part == "bltu")
    {



    }
    else if (part == "bge")
    {



    }
    else if (part == "bgeu")
    {



    }
    //UJ
    else if (part == "jal")
    {



    }
    //I
    else if (part == "jalr")
    {



    }
}
