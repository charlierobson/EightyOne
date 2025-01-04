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

#include "specBasicLoader.h"
#include <fstream>
#include <map>
#include <string>
#include <cctype>
#include <sstream>
#include "utils.h"

using namespace std;

specBasicLoader::specBasicLoader(bool spec128, bool if1)
{
        mSpec128 = spec128;
        mIF1 = if1;
}

void specBasicLoader::OutputStartOfProgramData(ZXString filename, int& addressOffset)
{
        // Header block

        OutputWord(addressOffset, 0x0013); //Block length
        OutputByte(addressOffset, 0x00);   //Flag: 00=Header
        OutputByte(addressOffset, 0x00);   //Type: 00=Program

        ZXString name = RemoveExt(RemovePath(filename));
        name += "         ";

        for (int n = 1; n <= 10; n++)
        {
                OutputByte(addressOffset, name[n]);    //Filename
        }

        OutputWord(addressOffset, 0x0000); //Length of data block
        OutputWord(addressOffset, 0x8000); //No auto-run
        OutputWord(addressOffset, 0x0000); //Offset to variables
        OutputByte(addressOffset, 0x00);   //Checksum

        // Data block

        OutputWord(addressOffset, 0x0000); //Block length
        OutputByte(addressOffset, 0xFF);   //Flag: FF=Data
}

void specBasicLoader::OutputEndOfProgramData(int& addressOffset)
{
        const int HeaderLength = 19;
        const int HeaderBlockLength = 2;
        const int DataBlockLength = 2;
        const int DataFlagLength = 1;

        const int HeaderStart = HeaderBlockLength;
        const int DataStart = HeaderBlockLength + HeaderLength + DataBlockLength;

        // Update TAP header block

        int dataBlockLength = addressOffset - HeaderBlockLength - HeaderLength - DataBlockLength - DataFlagLength;
        ChangeWord(14, dataBlockLength);
        ChangeWord(18, dataBlockLength);

        unsigned char checksum = 0;
        for (int b = 0; b < HeaderLength - 1; b++)
        {
                checksum = checksum ^ mProgramData[HeaderStart + b];
        }

        mProgramData[20] = checksum;

        // Update TAP Data block

        checksum = 0;
        for (int b = 0; b < dataBlockLength + 1; b++)
        {
                checksum = checksum ^ mProgramData[DataStart + b];
        }

        OutputByte(addressOffset, checksum);

        ChangeWord(21, dataBlockLength + DataBlockLength);
}

void specBasicLoader::ExtractTokens(bool acceptAlternateKeywordSpelling)
{
        map<unsigned char, string> tokens;

        if (mSpec128)
        {
                tokens[163] = " SPECTRUM ";
                tokens[164] = " PLAY ";
        }

        tokens[165] = "RND";
        tokens[166] = "INKEY$";
        tokens[167] = "PI";
        tokens[168] = "FN ";
        tokens[169] = "POINT ";
        tokens[170] = "SCREEN$ ";
        tokens[171] = "ATTR ";
        tokens[172] = "AT ";
        tokens[173] = "TAB ";
        tokens[174] = "VAL$ ";
        tokens[175] = "CODE ";
        tokens[176] = "VAL ";
        tokens[177] = "LEN ";
        tokens[178] = "SIN ";
        tokens[179] = "COS ";
        tokens[180] = "TAN ";
        tokens[181] = "ASN ";
        tokens[182] = "ACS ";
        tokens[183] = "ATN ";
        tokens[184] = "LN ";
        tokens[185] = "EXP ";
        tokens[186] = "INT ";
        tokens[187] = "SQR ";
        tokens[188] = "SGN ";
        tokens[189] = "ABS ";
        tokens[190] = "PEEK ";
        tokens[191] = "IN ";
        tokens[192] = "USR ";
        tokens[193] = "STR$ ";
        tokens[194] = "CHR$ ";
        tokens[195] = "NOT ";
        tokens[196] = "BIN ";
        tokens[197] = " OR ";
        tokens[198] = " AND ";
        tokens[199] = "<=";
        tokens[200] = ">=";
        tokens[201] = "<>";
        tokens[202] = " LINE ";
        tokens[203] = " THEN ";
        tokens[204] = " TO ";
        tokens[205] = " STEP ";
        tokens[206] = " DEF FN ";
        tokens[207] = " CAT ";
        tokens[208] = " FORMAT ";
        tokens[209] = " MOVE ";
        tokens[210] = " ERASE ";
        tokens[211] = " OPEN #";
        tokens[212] = " CLOSE #";
        tokens[213] = " MERGE ";
        tokens[214] = " VERIFY ";
        tokens[215] = " BEEP ";
        tokens[216] = " CIRCLE ";
        tokens[217] = " INK ";
        tokens[218] = " PAPER ";
        tokens[219] = " FLASH ";
        tokens[220] = " BRIGHT ";
        tokens[221] = " INVERSE ";
        tokens[222] = " OVER ";
        tokens[223] = " OUT ";
        tokens[224] = " LPRINT ";
        tokens[225] = " LLIST ";
        tokens[226] = " STOP ";
        tokens[227] = " READ ";
        tokens[228] = " DATA ";
        tokens[229] = " RESTORE ";
        tokens[230] = " NEW ";
        tokens[231] = " BORDER ";
        tokens[232] = " CONTINUE ";
        tokens[233] = " DIM ";
        tokens[234] = " REM ";
        tokens[235] = " FOR ";
        tokens[236] = " GO TO ";
        tokens[237] = " GO SUB ";
        tokens[238] = " INPUT ";
        tokens[239] = " LOAD ";
        tokens[240] = " LIST ";
        tokens[241] = " LET ";
        tokens[242] = " PAUSE ";
        tokens[243] = " NEXT ";
        tokens[244] = " POKE ";
        tokens[245] = " PRINT ";
        tokens[246] = " PLOT ";
        tokens[247] = " RUN ";
        tokens[248] = " SAVE ";
        tokens[249] = " RANDOMIZE ";
        tokens[250] = " IF ";
        tokens[251] = " CLS ";
        tokens[252] = " DRAW ";
        tokens[253] = " CLEAR ";
        tokens[254] = " RETURN ";
        tokens[255] = " COPY ";

        DoTokenise(tokens);

        if (acceptAlternateKeywordSpelling)
        {
                ReplaceTokenEndCharacters(tokens, ' ', '(');
                DoTokenise(tokens);

                ReplaceTokenEndCharacters(tokens, '(', '\"');
                DoTokenise(tokens);

                tokens.clear();

                tokens[206] = " DEFFN ";
                tokens[211] = " OPEN#";
                tokens[212] = " CLOSE#";
                tokens[224] = " LPRINT#";
                tokens[225] = " LLIST#";
                tokens[238] = " INPUT#";
                tokens[240] = " LIST#";
                tokens[245] = " PRINT#";

                tokens[232] = " CONT ";
                tokens[236] = " GOTO ";
                tokens[237] = " GOSUB ";
                tokens[249] = " RAND ";

                tokens[197] = "OR ";
                tokens[198] = "AND ";
                tokens[204] = "TO ";

                DoTokenise(tokens);

                tokens.clear();

                if (mIF1)
                {
                        tokens[213] = " MERGE*";
                        tokens[214] = " VERIFY*";
                        tokens[239] = " LOAD*";
                        tokens[248] = " SAVE*";
                }

                tokens[204] = " TO)";
                tokens[245] = " PRINT'";
                tokens[249] = " RANDOMISE ";

                DoTokenise(tokens);

                tokens.clear();

                tokens[197] = "OR(";
                tokens[198] = "AND(";
                tokens[204] = "TO(";

                DoTokenise(tokens);

                tokens.clear();

                if (mSpec128)
                {
                        tokens[207] = " CAT!";
                        tokens[210] = " ERASE!";
                        tokens[213] = " MERGE!";
                        tokens[214] = " VERIFY!";
                        tokens[239] = " LOAD!";
                        tokens[248] = " SAVE!";
                }

                DoTokenise(tokens);

                tokens.clear();

                if (mIF1)
                {
                        tokens[207] = " CAT#";
                        tokens[209] = " MOVE#";
                        tokens[251] = " CLS#";
                }

                tokens[245] = " PRINT,";

                DoTokenise(tokens);

                tokens.clear();

                tokens[245] = " PRINT;";

                DoTokenise(tokens);
        }
}

void specBasicLoader::OutputLine(int lineNumber, int& addressOffset)
{
        int i = 0;

        OutputByte(addressOffset, (unsigned char)(lineNumber >> 8));
        OutputByte(addressOffset, (unsigned char)(lineNumber & 0xFF));

        int lineLengthOffset = addressOffset;
        addressOffset += 2;
        int commandOffset = addressOffset;

        bool withinQuotes = false;
        bool withinRem = false;
        bool withinDefFn = false;
        bool lastTokenWasBin = false;

        while (mLineBuffer[i] != '\0')
        {
                if (mLineBufferPopulated[i])
                {
                        unsigned char chr = mLineBufferOutput[i];
                        
                        if (!withinRem && !withinDefFn && (chr == '\"'))
                        {
                                withinQuotes = !withinQuotes;
                        }
                        else if (!withinQuotes && !withinDefFn && (chr == Rem))
                        {
                                withinRem = true;
                        }
                        else if (!withinQuotes && !withinRem && (chr == DefFn))
                        {
                                withinDefFn = true;
                        }
                        else if (!withinQuotes && !withinRem && withinDefFn && (chr == '='))
                        {
                                withinDefFn = false;
                        }

                        if (withinDefFn && ((chr == ',') || (chr == ')')))
                        {
                                OutputByte(addressOffset, Number);
                                OutputByte(addressOffset, 0x00);
                                OutputByte(addressOffset, 0x00);
                                OutputByte(addressOffset, 0x00);
                                OutputByte(addressOffset, 0x00);
                                OutputByte(addressOffset, 0x00);
                                OutputByte(addressOffset, chr);
                        }
                        else if (!withinDefFn && !withinQuotes && !withinRem && StartOfNumber(i))
                        {
                                OutputEmbeddedNumber(i, addressOffset, lastTokenWasBin);
                        }
                        else
                        {
                                OutputByte(addressOffset, chr);
                        }

                        if (chr == Bin)
                        {
                                lastTokenWasBin = true;
                        }
                        else if (chr != ' ')
                        {
                                lastTokenWasBin = false;
                        }
                }

                i++;
        }

        OutputByte(addressOffset, Return);

        int lineLength = addressOffset - commandOffset;
        ChangeWord(lineLengthOffset, lineLength);
}

void specBasicLoader::OutputFloatingPointEncoding(double value, int& addressOffset)
{                             
        int exponent;
        unsigned long mantissa;

        double absValue = (value < 0) ? -value : value;

        if (value >= -32768 && value <= 32767 && (absValue - (int)absValue < DBL_EPSILON))
        {
                //+ve = 00 00 nn mm 00  =>  mmnn
                //-ve = 00 FF nn mm 00  =>  mmnn

                OutputByte(addressOffset, 0x00);
                OutputByte(addressOffset, 0x00);
                OutputByte(addressOffset, (unsigned char)((int)absValue & 0xFF));
                OutputByte(addressOffset, (unsigned char)((int)absValue >> 8));
                OutputByte(addressOffset, 0x00);
        }
        else
        {
                exponent = (int)floor(DBL_EPSILON + (log(value) / log(2.0)));
                if (exponent < -129 || exponent > 126)
                {
                        throw out_of_range("Number out of range");
                }

                double mantissaVal = (value / pow(2.0, exponent)) - 1;
                mantissaVal *= 0x80000000;
                mantissa = (unsigned long)floor(mantissaVal);

                exponent += 129;

                // Numbers in BASIC are always positive (a leading negative sign is an operator and
                // not part of the actual number) so set the msb to 0 to signify a positive value

                OutputByte(addressOffset, (unsigned char)exponent);
                OutputByte(addressOffset, (unsigned char)(mantissa >> 24));
                OutputByte(addressOffset, (unsigned char)(mantissa >> 16));
                OutputByte(addressOffset, (unsigned char)(mantissa >> 8));
                OutputByte(addressOffset, (unsigned char)(mantissa & 0xFF));
        }
}

unsigned char specBasicLoader::DecodeGraphic(unsigned char chr1, unsigned char chr2)
{
        map<string, unsigned char> graphics;

        graphics["  "] = 0x80;
        graphics[" '"] = 0x81;
        graphics["' "] = 0x82;
        graphics["''"] = 0x83;
        graphics[" ."] = 0x84;
        graphics[" :"] = 0x85;
        graphics["'."] = 0x86;
        graphics["':"] = 0x87;
        graphics[". "] = 0x88;
        graphics[".'"] = 0x89;
        graphics[": "] = 0x8A;
        graphics[":'"] = 0x8B;
        graphics[".."] = 0x8C;
        graphics[".:"] = 0x8D;
        graphics[":."] = 0x8E;
        graphics["::"] = 0x8F;
        graphics["_A"] = 0x90;
        graphics["_B"] = 0x91;
        graphics["_C"] = 0x92;
        graphics["_D"] = 0x93;
        graphics["_E"] = 0x94;
        graphics["_F"] = 0x95;
        graphics["_G"] = 0x96;
        graphics["_H"] = 0x97;
        graphics["_I"] = 0x98;
        graphics["_J"] = 0x99;
        graphics["_K"] = 0x9A;
        graphics["_L"] = 0x9B;
        graphics["_M"] = 0x9C;
        graphics["_N"] = 0x9D;
        graphics["_O"] = 0x9E;
        graphics["_P"] = 0x9F;
        graphics["_Q"] = 0xA0;
        graphics["_R"] = 0xA1;
        graphics["_S"] = 0xA2;
        graphics["_T"] = 0xA3;
        graphics["_U"] = 0xA4;

        string search;
        search += (char)chr1;
        search += (char)chr2;

        map<string, unsigned char>::const_iterator it = graphics.find(search);
        if (it == graphics.end())
        {
                ostringstream code;
                code << chr1;
                code << chr2;
                string msg = "Invalid graphic Code: " + code.str();
                throw invalid_argument(msg.c_str());
        }

        return it->second;
}

bool specBasicLoader::SupportUppercaseOnly()
{
        return false;
}

unsigned char specBasicLoader::GetEmbbededNumberMark()
{
        return Number;
}

unsigned char specBasicLoader::AsciiToZX(unsigned char ascii)
{
        unsigned char zxChr;

        switch ((char)ascii)
        {
                case '£':
                        zxChr = Pound;
                        break;
                case '©':
                        zxChr = Copyright;
                        break;
                default:
                        zxChr = ascii;
                        break;
        }

        return zxChr;
}

bool specBasicLoader::TokenSupportsLineNumber(unsigned char chr)
{
        return (chr == Goto || chr == Gosub || chr == Run || chr == List || chr == LList || chr == Restore || chr == Line);
}

bool specBasicLoader::SingleEscapeSequence(unsigned char chr, unsigned char& zxChr)
{
        if (chr == '\\')
        {
                zxChr = '\\';
                return true;
        }

        return false;
}

bool specBasicLoader::NoEscapeSequence(unsigned char chr)
{
        string escapeSequenceChars = "\\_\n1234567890ABCDEFabcdef'.:;@#~! ";

        string zxChr;
        zxChr = chr;
        
        size_t pos = zxChr.find_first_of(escapeSequenceChars);

        return (pos == string::npos);
}
