#include <iostream>
#include <fstream>
#include "stdlib.h"
#include <iomanip>
#include <string>
#include <sstream>
#include <bitset>
#include <regex>
#include <unordered_map>
using namespace std;


#define SPACE "\\s+"
#define REG "\\s*x(\\d{1,2})"
#define LABEL "(\\w+:?)"
#define JLABEL "^(\\w+):$"
#define IMM "\\s*([-|+]?\\d+)"
#define OR "|"
#define REG1    LABEL SPACE REG "," IMM
#define REG2    LABEL SPACE REG "," REG "," IMM
#define REG3    LABEL SPACE REG "," REG "," REG
#define REGi    LABEL SPACE REG "," IMM "\\(" REG "\\)"
#define JAL1    LABEL SPACE REG ",\\s*" LABEL
#define BRANCH    LABEL SPACE REG "," REG ",\\s*" LABEL
// #define MYEX		REG3 OR REGi OR REG1 OR REG2

struct instWord
{
    string Text;
    unsigned int MachineCode;
    unsigned int rd, rs1, rs2, funct3, funct7, opcode;
    unsigned int I_imm, S_imm, B_imm, U_imm, J_imm;
    string J_label;
    bool known = true, islabel = false;
};
int regs[32]={0}; //register simulation
unsigned int pc = 0x0; //program counter
char memory[8*1024];    // 8KB of memory located at address 0

// Function prototype
bool instAssembleExec (instWord& inst);
void printPrefix (unsigned int instA, unsigned int instCode);
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

    outFile.open("output.txt",ios::app);
    inFile.open("div.s");
    if(inFile.is_open())
    {
        int i = 0;
        string s;
        smatch m;
        regex lbl(JLABEL);
        unordered_map <string, int> map;
        // go through file to find labels
        while(!inFile.eof())
        {
            getline(inFile, s);
            if(regex_search(s, m, lbl))
            {
                pair<string, int> p(m[1], i);
                map.insert(p);
            }
            i++;
        }

        for (auto& x: map)
            cout << x.first << ": " << x.second << endl;
    }
    return 0;
}
