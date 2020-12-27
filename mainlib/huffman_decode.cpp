#include "huffman.h"
#include "../param_parser.h"
#include <iostream>
#include <string>
#include <fstream>
#include <iomanip> 

using namespace std;

huffman_code *reconstruct_code_from_header(ifstream &in)
{
    char *magic_num = new char[12 + 1];
    in.get(magic_num, 12 + 1);

    for (int i = 0; i < 12; i++)
    {
        if (!(magic_num[i] == _MAGIC_BYTES[i]))
        {
            delete[] magic_num;
            return 0;
        }
    }
    delete[] magic_num;

    unsigned char data;
    unsigned code = 0;
    unsigned size = 0;

    huffman_code *table = new huffman_code[256];
    for (int i = 0; i < 256; i++)
    {
        data = in.get();
        if (i != 0 && data == 0) //If there isn't a 0xFF in the alphabet
        {
            in.unget();
            break;
        }

        size = in.get();
        for (int j = 0; j < 4; j++)
            code = (code << 8) | in.get();

        table[data].set_code(code);
        table[data].set_size(size);

        code = 0;
        size = 0;
    }

    in.get(); //read the delimiting 0x00

    for (int j = 0; j < 4; j++)
        code = (code << 8) | in.get();
    table[256].set_code(code); //size of the thing
    return table;
}

inline bool pluck_bit(unsigned char buffer)
//this could be a marco function
//or a separate class. IDK what to do
{
    return (buffer & 0x80);
}
inline unsigned pack_bit(unsigned code, bool bit)
{
    return (code << 1) | bit;
}

int naive_decode_with_header(input_param options)
{
    unsigned char buffer;
    unsigned code = 0;
    unsigned code_size = 0;
    ifstream in;
    ofstream out;
    in.open(options.input_file, ios::binary | ios::in);
    out.open(options.output_file, ios::binary | ios::out);

    huffman_code *huff_code = reconstruct_code_from_header(in);
    if (huff_code == 0)
        return 1;
    unsigned file_size_in_bits = huff_code[256].get_code() - 1;

    for (unsigned long i = in.tellg(); i < options.input_file_size; i++)
    {
        buffer = in.get();
       
        for (int j = 0; j < 8; j++)
        {
            
            code = (code << 1) | pluck_bit(buffer);
            code_size++;
            buffer = buffer << 1;
            for (int k = 0; k < 256; k++)
            {
                if (code == huff_code[k].get_code() && code_size == huff_code[k].get_size())
                {
                    out << (unsigned char)k;
                    code = 0;
                    code_size = 0;
                    break;
                }
            }

            if (!file_size_in_bits--)
                break;
        }
        if (options.verbose && !(i % 20480)) //every 20kb
        {
            cout << setw(3) << (int)(((double)i / (double)options.input_file_size) * 100)
                 << "% decompressed\r" << flush;
        }
    }
    cout << "100% decompressed" << endl;

    delete[] huff_code;
    in.close();
    out.close();
} 