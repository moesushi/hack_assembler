#include "constants.h"
#include "splashkit.h"
#include "terminal_user_input.h"
#include <algorithm>
#include <bitset>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

using std::string;

typedef std::unordered_map<string, int> symbol_map;

std::unordered_map<string, uint8_t> jumpTable = {
    {"JGT", 0b001},
    {"JEQ", 0b010},
    {"JGE", 0b011},
    {"JLT", 0b100},
    {"JNE", 0b101},
    {"JLE", 0b110},
    {"JMP", 0b111},
};

std::unordered_map<string, uint8_t> compTable = {
    {"0",   0b0101010},
    {"1",   0b0111111},
    {"-1",  0b0111010},
    {"D",   0b0001100},
    {"A",   0b0110000},
    {"M",   0b1110000},
    {"!D",  0b0001101},
    {"!A",  0b0110001},
    {"!M",  0b1110001},
    {"-D",  0b0001111},
    {"-A",  0b0110011},
    {"-M",  0b1110011},
    {"D+1", 0b0011111},
    {"A+1", 0b0110111},
    {"M+1", 0b1110111},
    {"D-1", 0b0001110},
    {"A-1", 0b0110010},
    {"M-1", 0b1110010},
    {"D+A", 0b0000010},
    {"D+M", 0b1000010},
    {"D-A", 0b0010011},
    {"D-M", 0b1010011},
    {"A-D", 0b0000111},
    {"M-D", 0b1000111},
    {"D&A", 0b0000000},
    {"D&M", 0b1000000},
    {"D|A", 0b0010101},
    {"D|M", 0b1010101},
};


/**
 * Sanitises a line of assembly code by removing comments and whitespace
 * 
 * @param line The line of assembly to sanitise
 * 
 * @returns A sanitised line of assembly, free from whitespace and comments
 */
string sanitiseLine(string line)
{
    // cuts comments
    size_t slIndex = line.find('/');
    if (slIndex != string::npos && line[slIndex + 1] == '/')
        line = line.substr(0, slIndex);

    // deletes all whitespace
    line.erase(remove_if(line.begin(), line.end(), ::isspace), line.end());
    return line;
}

/**
 * Preprocess the assembly code to extract label information
 * 
 * @param fstream Filestream containing the assembly code
 *
 * @returns An unordered_map containing default symbols and any preprocessed labels
 */
symbol_map label_preprocessor(std::ifstream &fstream)
{
    symbol_map symbols = {
        {"KBD", KBD},
        {"SCREEN", SCREEN},
        {"SP", 0},
        {"LCL", 1},
        {"ARG", 2},
        {"THIS", 3},
        {"THAT", 4}
    };

    for (int i = 0; i < 16; i++)
    {
        symbols["R" + std::to_string(i)] = i;
    }

    string line = "";
    int romaddr = 0;
    while(std::getline(fstream, line))
    {
        line = sanitiseLine(line);

        if (line[0] == '(')
        {
            string label = line.substr(1, line.length() - 2);
            symbols[label] = romaddr;
        }
        else if (line != "")
        {
            romaddr++;
        }
    }

    return symbols;
}

/**
 * Parses an A-instruction into its binary form
 * 
 * @param line The A-instruction in symbolic form
 * @param var_counter The variable address counter
 * @param symbols The symbol map
 *
 * @returns The parsed A-instruction in binary form
 */
uint16_t parse_addr_instruction(string line, int &var_counter, symbol_map &symbols)
{
    uint16_t result = -1;

    string saddr = line.substr(1);

    if (is_integer(saddr))
        result = convert_to_integer(saddr);
    else if (symbols.find(saddr) != symbols.end())
        result = symbols[saddr];
    else
    {
        symbols[saddr] = var_counter;
        result = var_counter++;
    };
    return result;
}

/**
 * Parses a C-instruction into its binary form
 * 
 * @param line The C-instruction in symbolic form
 *
 * @returns The parsed C-instruction in binary form
 */
uint16_t parse_compute_instruction(string line)
{
    uint16_t result = 0b111 << 13;

    string expr = line;

    size_t equal_idx = expr.find('=');
    string dest = "";
    if (equal_idx != string::npos)
    {
        dest = expr.substr(0, equal_idx);
        expr = expr.substr(equal_idx + 1);
    }

    size_t semi_idx = expr.find(';');
    string jump = "";
    if (semi_idx != string::npos)
    {
        jump = expr.substr(semi_idx + 1);
        expr = expr.substr(0, semi_idx);
    }

    for (int i = 0; i < dest.size(); i++)
    {
        switch (dest[i])
        {
            case 'A':
                result |= 1 << 5;
                break;
            case 'D':
                result |= 1 << 4;
                break;
            case 'M':
                result |= 1 << 3;
                break;
            default:
                write_line("Unknown destination");
                throw;
        }
    }

    if (jumpTable.find(jump) != jumpTable.end())
        result |= jumpTable[jump];
    else if (jump != "")
    {
        write_line("invalid jump statement");
        throw;
    }

    if (compTable.find(expr) != compTable.end())
        result |= compTable[expr] << 6;
    else if (expr != "")
    {
        write_line("invalid comp statement");
        throw;
    }

    return result;
}

/**
 * Parse assembly code into binary format
 * 
 * @param rom A vector of 16-bit unsigned integers to store instructions into
 * @param fstream Filestream containing the assembly code
 * @param symbols The symbol map
 */
void parse(std::vector<uint16_t> &rom, std::ifstream &fstream, symbol_map &symbols)
{
    string line = "";
    int var_counter = 16;

    while(std::getline(fstream, line))
    {
        uint16_t instr;

        line = sanitiseLine(line);

        if (line == "")
            continue;
        if (line[0] == '(')
            continue;

        if (line[0] == '@')
            instr = parse_addr_instruction(line, var_counter, symbols);
        else 
            instr = parse_compute_instruction(line);

        rom.push_back(instr);
    }
}

int main()
{
    std::vector<uint16_t> rom;

    string filename = read_string("Enter filename to parse: ");

    std::ifstream fstream(filename);

    if (fstream.fail())
    {
        write("An error occured while reading file: ");
        write_line(std::strerror(errno));
        return errno;
    }

    symbol_map symbols = label_preprocessor(fstream);
    fstream.clear();
    fstream.seekg(0, std::ios::beg);
    parse(rom, fstream, symbols);

    string ofname = filename.substr(0, filename.find_last_of('.')) + ".hack";
    std::ofstream ostream;
    ostream.open(ofname);

    if (ostream.fail())
    {
        write("An error occured while writing to file: ");
        write_line(std::strerror(errno));
        return errno;
    }

    for (uint16_t instruction : rom)
        ostream << std::bitset<16>(instruction) << "\n";

    ostream.close();

    write_line("Parsed into " + ofname);

    return 0;
}