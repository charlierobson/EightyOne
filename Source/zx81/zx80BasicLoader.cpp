/* EightyOne  - A Windows ZX80/81/clone emulator.
 * Copyright (C) 2003-2019 Michael D Wynne
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "zx80BasicLoader.h"
#include <fstream>
#include <map>
#include <string>
#include <cctype>
#include <sstream>

using namespace std;

void zx80BasicLoader::OutputStartOfProgramData(AnsiString filename, int& addressOffset)
{
        // Output the system variables
        OutputByte(addressOffset, 0xFF);        //ERR_NR
        OutputByte(addressOffset, 0x80);        //FLAGS
        OutputWord(addressOffset, 0xFFFE);      //PPC
        OutputWord(addressOffset, 0x0000);      //P_PTR
        OutputWord(addressOffset, 0x0000);      //E_PPC
        OutputWord(addressOffset, 0x0000);      //VARS
        OutputWord(addressOffset, 0x0000);      //E_LINE
        OutputWord(addressOffset, 0x0000);      //D_FILE
        OutputWord(addressOffset, 0x0000);      //DF_EA
        OutputWord(addressOffset, 0x0000);      //DF_END
        OutputByte(addressOffset, 0x02);        //DF_SZ
        OutputWord(addressOffset, 0x0000);      //S_TOP
        OutputWord(addressOffset, 0x0000);      //X_PTR
        OutputWord(addressOffset, 0x0000);      //OLDPPC
        OutputByte(addressOffset, 0x00);        //FLAGX
        OutputWord(addressOffset, 0x07B6);      //T_ADDR
        OutputWord(addressOffset, 0x0000);      //SEED
        OutputWord(addressOffset, 0x0000);      //FRAMES
        OutputWord(addressOffset, 0x0000);      //DEST
        OutputWord(addressOffset, 0x0000);      //RESULT
        OutputWord(addressOffset, 0x1721);      //S_POSN
        OutputWord(addressOffset, 0xFFFF);      //CH_ADD
}

void zx80BasicLoader::OutputEndOfProgramData(int& addressOffset)
{
        // Insert the variables area
        const int startOfRam = 16384;
        int varsAddress = startOfRam + addressOffset;

        OutputByte(addressOffset, 0x80);

        int elineAddress = startOfRam + addressOffset;

        // Update system variables
        int pptrOffset = 4;
        ChangeWord(pptrOffset, elineAddress);
        int varsOffset = 8;
        ChangeWord(varsOffset, varsAddress);
        int elineOffset = 10;
        ChangeWord(elineOffset, elineAddress);
        int dfileOffset = 12;
        ChangeWord(dfileOffset, elineAddress);
        int dfeaOffset = 14;
        ChangeWord(dfeaOffset, elineAddress);
        int dfendOffset = 16;
        ChangeWord(dfendOffset, elineAddress);
        int destOffset = 32;
        ChangeWord(destOffset, elineAddress);
}

void zx80BasicLoader::ExtractTokens()
{
        map<unsigned char, string> tokens;

        tokens[213] = " THEN ";
        tokens[214] = " TO ";
        tokens[219] = "NOT ";
        tokens[224] = " AND ";
        tokens[225] = " OR ";
        tokens[226] = "**";
        tokens[230] = " LIST ";
        tokens[231] = " RETURN ";
        tokens[232] = " CLS ";
        tokens[233] = " DIM ";
        tokens[234] = " SAVE ";
        tokens[235] = " FOR ";
        tokens[236] = " GO TO ";
        tokens[237] = " POKE ";
        tokens[238] = " INPUT ";
        tokens[239] = " RANDOMISE ";
        tokens[240] = " LET ";
        tokens[243] = " NEXT ";
        tokens[244] = " PRINT ";
        tokens[246] = " NEW ";
        tokens[247] = " RUN ";
        tokens[248] = " STOP ";
        tokens[249] = " CONTINUE ";
        tokens[250] = " IF ";
        tokens[251] = " GO SUB ";
        tokens[252] = " LOAD ";
        tokens[253] = " CLEAR ";
        tokens[254] = " REM ";

        DoTokenise(tokens);
}

void zx80BasicLoader::OutputLine(int lineNumber, int& addressOffset)
{
        int i = 0;

        OutputByte(addressOffset, lineNumber >> 8);
        OutputByte(addressOffset, lineNumber & 0xFF);

        while (mLineBuffer[i] != '\0')
        {
                if (mLineBufferPopulated[i])
                {
                        unsigned char chr = mLineBufferOutput[i];
                        OutputByte(addressOffset, chr);
                }

                i++;
        }

        OutputByte(addressOffset, Newline);
}

unsigned char zx80BasicLoader::DecodeGraphic(unsigned char chr1, unsigned char chr2)
{
        map<string, unsigned char> graphics;

        graphics["  "] = 0;
        graphics[": "] = 2;
        graphics[".."] = 3;
        graphics["' "] = 4;
        graphics[" '"] = 5;
        graphics[". "] = 6;
        graphics[" ."] = 7;
        graphics[".'"] = 8;
        graphics["##"] = 9;
        graphics[",,"] = 10;
        graphics["~~"] = 11;
        graphics["::"] = 128;
        graphics[" :"] = 130;
        graphics["''"] = 131;
        graphics[".:"] = 132;
        graphics[":."] = 133;
        graphics["':"] = 134;
        graphics[":'"] = 135;
        graphics["'."] = 136;
        graphics["@@"] = 137;
        graphics[";;"] = 138;
        graphics["!!"] = 139;

        string search;
        search += (char)chr1;
        search += (char)chr2;

        map<string, unsigned char>::const_iterator it = graphics.find(search);
        if (it == graphics.end())
        {
                ostringstream code;
                code << chr1;
                code << chr2;
                string msg = "Invalid graphic - Code " + code.str();
                throw out_of_range(msg.c_str());
        }

        return it->second;
}

unsigned char zx80BasicLoader::AsciiToZX(unsigned char ascii)
{
        const unsigned char Number0 = 28;
        const unsigned char LetterA = 38;

        if (isalpha(ascii))
        {
                return (ascii - 'A') + LetterA;
        }
        else if (isdigit(ascii))
        {
                return (ascii - '0') + Number0;
        }

        unsigned char zxChr;

        map<string, unsigned char> chrs;

        switch ((char)ascii)
        {
                case ' ':
                        zxChr = 0;
                        break;
                case '\"':
                        zxChr = 1;
                        break;
                case '�':
                case '#':
                        zxChr = 12;
                        break;
                case '$':
                        zxChr = 13;
                        break;
                case ':':
                        zxChr = 14;
                        break;
                case '?':
                        zxChr = 15;
                        break;
                case '(':
                        zxChr = 218;
                        break;
                case ')':
                        zxChr = 217;
                        break;
                case '-':
                        zxChr = 220;
                        break;
                case '+':
                        zxChr = 221;
                        break;
                case '*':
                        zxChr = 222;
                        break;
                case '/':
                        zxChr = 223;
                        break;
                case '=':
                        zxChr = 227;
                        break;
                case '>':
                        zxChr = 228;
                        break;
                case '<':
                        zxChr = 229;
                        break;
                case ';':
                        zxChr = 215;
                        break;
                case ',':
                        zxChr = 216;
                        break;
                case '.':
                        zxChr = 27;
                        break;
                default:
                        ostringstream code;
                        code << ascii;
                        string msg = "Invalid character - Character " + code.str();
                        throw out_of_range(msg.c_str());
        }

        return zxChr;
}
                                                                                   
void zx80BasicLoader::ExtractInverseCharacters()
{
        char* pPos = mLineBuffer;

        while (*pPos != '\0')
        {
                if (*pPos == '%')
                {
                        *pPos = Blank;
                        pPos++;

                        int index = pPos - mLineBuffer;
                        mLineBufferOutput[index] = 0x80 | AsciiToZX(*pPos);
                        mLineBufferPopulated[index] = true;
                        
                        *pPos = Blank;
                }

                pPos++;
        }
}

unsigned char zx80BasicLoader::GetEscapeCharacter()
{
        return Escape;
}                 

bool zx80BasicLoader::SupportUppercaseOnly()
{
        return true;
}
