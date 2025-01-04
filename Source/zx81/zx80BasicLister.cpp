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

#include "zx80BasicLister.h"

using namespace std;

zx80BasicLister::zx80BasicLister(bool zxpandEnabled)
{
        mZxpandEnabled = zxpandEnabled;

        // Details on ZxToken format can be found: http://dskcenter.free.fr/zxtools.html
        mZxTokenChars["\\: "] = "\xBA";
        mZxTokenChars["\\.."] = "\xAE";
        mZxTokenChars["\\' "] = "\xB6";
        mZxTokenChars["\\ '"] = "\xB7";
        mZxTokenChars["\\. "] = "\xB9";
        mZxTokenChars["\\ ."] = "\xB2";
        mZxTokenChars["\\.'"] = "\xBB";
        mZxTokenChars["\\##"] = "\xBD";
        mZxTokenChars["\\,,"] = "\xBE";
        mZxTokenChars["\\~~"] = "\xBF";

        for (int i = 0x40; i <= 0x7F; i++)
        {
                ZXString hex = ZXString::IntToHex(i, 4).UpperCase();
                mZxTokenChars["\\" + hex] = "[HEX:" + hex + "]";
        }

        mZxTokenChars["% "] = "\xAB";
        mZxTokenChars["%\""] = "\x86";
        mZxTokenChars["\\ :"] = "\xB0";
        mZxTokenChars["\\''"] = "\xB8";
        mZxTokenChars["\\.:"] = "\xAC";
        mZxTokenChars["\\:."] = "\xAA";
        mZxTokenChars["\\':"] = "\xAF";
        mZxTokenChars["\\:'"] = "\xBC";
        mZxTokenChars["\\'."] = "\xB1";
        mZxTokenChars["\\@@"] = "\xB3";
        mZxTokenChars["\\;;"] = "\xB4";
        mZxTokenChars["\\!!"] = "\xB5";    
        mZxTokenChars["%£"] = "\x87";
        mZxTokenChars["%$"] = "\x88";
        mZxTokenChars["%:"] = "\x89";
        mZxTokenChars["%?"] = "\x8A";
        mZxTokenChars["%("] = "\x8B";
        mZxTokenChars["%)"] = "\x8C";
        mZxTokenChars["%-"] = "\x91";
        mZxTokenChars["%+"] = "\x90";
        mZxTokenChars["%*"] = "\x92";
        mZxTokenChars["%/"] = "\x93";
        mZxTokenChars["%="] = "\x8F";
        mZxTokenChars["%>"] = "\x8D";
        mZxTokenChars["%<"] = "\x8E";
        mZxTokenChars["%;"] = "\x94";
        mZxTokenChars["%,"] = "\x95";
        mZxTokenChars["%."] = "\x96";
        mZxTokenChars["%0"] = "\x97";
        mZxTokenChars["%1"] = "\x98";
        mZxTokenChars["%2"] = "\x99";
        mZxTokenChars["%3"] = "\x9A";
        mZxTokenChars["%4"] = "\x9B";
        mZxTokenChars["%5"] = "\x9C";
        mZxTokenChars["%6"] = "\x9D";
        mZxTokenChars["%7"] = "\x9E";
        mZxTokenChars["%8"] = "\x9F";
        mZxTokenChars["%9"] = "\xA0";

        for (int i = 'A'; i <= 'Z'; i++)
        {
                ZXString letter = (char)i;
                mZxTokenChars["%" + letter] = letter.LowerCase();
        }

        for (int i = 0xC0; i <= 0xD3; i++)
        {
                ZXString hex = ZXString::IntToHex(i, 4).UpperCase();
                mZxTokenChars["\\" + hex] = "[HEX:" + hex + "]";
        }

        mZxTokenChars["\\F1"] = "[HEX:F1]";
        mZxTokenChars["\\F2"] = "[HEX:F2]";
        mZxTokenChars["\\F5"] = "[HEX:F5]";
        mZxTokenChars["\\FF"] = "[HEX:FF]";
}

std::string zx80BasicLister::GetKeywords()
{
        std::string keywords;

        if (!mZxpandEnabled)
        {
                // Graphics encoded in accordance with ZXText2P: http://freestuff.grok.co.uk/zxtext2p/index.html
                //Non-printable characters encoded as '\' followed by a 2 digit hexadecimal value representing the character code.
                keywords = " ¦\"¦\\: ¦\\..¦\\' ¦\\ '¦\\. ¦\\ .¦\\.'¦\\##¦\\,,¦\\~~¦£¦$¦:¦?¦(¦)¦-¦+¦*¦/¦=¦>¦<¦;¦,¦.¦0¦1¦2¦3¦4¦5¦6¦7¦8¦9¦A¦B¦C¦D¦E¦F¦G¦H¦I¦J¦K¦L¦M¦N¦O¦P¦Q¦R¦S¦T¦U¦V¦W¦X¦Y¦Z¦"
                           "\\40¦\\41¦\\42¦\\43¦\\44¦\\45¦\\46¦\\47¦\\48¦\\49¦\\4A¦\\4B¦\\4C¦\\4D¦\\4E¦\\4F¦\\50¦\\51¦\\52¦\\53¦\\54¦\\55¦\\56¦\\57¦\\58¦\\59¦\\5A¦\\5B¦\\5C¦\\5D¦\\5E¦\\5F¦"
                           "\\60¦\\61¦\\62¦\\63¦\\64¦\\65¦\\66¦\\67¦\\68¦\\69¦\\6A¦\\6B¦\\6C¦\\6D¦\\6E¦\\6F¦\\70¦\\71¦\\72¦\\73¦\\74¦\\75¦\\76¦\\77¦\\78¦\\79¦\\7A¦\\7B¦\\7C¦\\7D¦\\7E¦\\7F¦"
                           "% ¦%\"¦\\ :¦\\''¦\\.:¦\\:.¦\\':¦\\:'¦\\'.¦\\@@¦\\;;¦\\!!¦%£¦%$¦%:¦%?¦%(¦%)¦%-¦%+¦%*¦%/¦%=¦%>¦%<¦%;¦%,¦%.¦%0¦%1¦%2¦%3¦%4¦%5¦%6¦%7¦%8¦%9¦%A¦%B¦%C¦%D¦%E¦%F¦%G¦%H¦%I¦%J¦%K¦%L¦%M¦%N¦%O¦%P¦%Q¦%R¦%S¦%T¦%U¦%V¦%W¦%X¦%Y¦%Z¦"
                           "\\C0¦\\C1¦\\C2¦\\C3¦\\C4¦\\C5¦\\C6¦\\C7¦\\C8¦\\C9¦\\CA¦\\CB¦\\CC¦\\CD¦\\CE¦\\CF¦\\D0¦\\D1¦\\D2¦\\D3¦"
                           "\"¦ THEN ¦ TO ¦;¦,¦)¦(¦NOT ¦-¦+¦*¦/¦ AND ¦ OR ¦**¦=¦>¦<¦ LIST ¦ RETURN ¦ CLS ¦ DIM ¦ SAVE ¦ FOR ¦ GO TO ¦ POKE ¦ INPUT ¦ RANDOMISE ¦ LET ¦\\F1¦\\F2¦ NEXT ¦ PRINT ¦\\F5¦ NEW ¦ RUN ¦ STOP ¦ CONTINUE ¦ IF ¦ GO SUB ¦ LOAD ¦ CLEAR ¦ REM ¦\\FF¦";
        }
        else
        {
                keywords = " ¦\"¦\\: ¦\\..¦\\' ¦\\ '¦\\. ¦\\ .¦\\.'¦\\##¦\\,,¦\\~~¦£¦$¦:¦?¦(¦)¦-¦+¦*¦/¦=¦>¦<¦;¦,¦.¦0¦1¦2¦3¦4¦5¦6¦7¦8¦9¦A¦B¦C¦D¦E¦F¦G¦H¦I¦J¦K¦L¦M¦N¦O¦P¦Q¦R¦S¦T¦U¦V¦W¦X¦Y¦Z¦"
                           "\\40¦\\41¦\\42¦\\43¦\\44¦\\45¦\\46¦\\47¦\\48¦\\49¦\\4A¦\\4B¦\\4C¦\\4D¦\\4E¦\\4F¦\\50¦\\51¦\\52¦\\53¦\\54¦\\55¦\\56¦\\57¦\\58¦\\59¦\\5A¦\\5B¦\\5C¦\\5D¦\\5E¦\\5F¦"
                           "\\60¦\\61¦\\62¦\\63¦\\64¦\\65¦\\66¦\\67¦\\68¦\\69¦\\6A¦\\6B¦\\6C¦\\6D¦\\6E¦\\6F¦\\70¦\\71¦\\72¦\\73¦\\74¦\\75¦\\76¦\\77¦\\78¦\\79¦\\7A¦\\7B¦\\7C¦\\7D¦\\7E¦\\7F¦"
                           "% ¦%\"¦\\ :¦\\''¦\\.:¦\\:.¦\\':¦\\:'¦\\'.¦\\@@¦\\;;¦\\!!¦%£¦%$¦%:¦%?¦%(¦%)¦%-¦%+¦%*¦%/¦%=¦%>¦%<¦%;¦%,¦%.¦%0¦%1¦%2¦%3¦%4¦%5¦%6¦%7¦%8¦%9¦%A¦%B¦%C¦%D¦%E¦%F¦%G¦%H¦%I¦%J¦%K¦%L¦%M¦%N¦%O¦%P¦%Q¦%R¦%S¦%T¦%U¦%V¦%W¦%X¦%Y¦%Z¦"
                           "\\C0¦\\C1¦\\C2¦\\C3¦\\C4¦\\C5¦\\C6¦\\C7¦\\C8¦\\C9¦\\CA¦\\CB¦\\CC¦\\CD¦\\CE¦\\CF¦\\D0¦\\D1¦\\D2¦\\D3¦"
                           "\"¦ THEN ¦ TO ¦;¦,¦)¦(¦NOT ¦-¦+¦*¦/¦ AND ¦ OR ¦**¦=¦>¦<¦ LIST ¦ RETURN ¦ CLS ¦ DIM ¦ SAVE ¦ FOR ¦ GO TO ¦ POKE ¦ INPUT ¦ RANDOMISE ¦ LET ¦ CONFIG ¦\\F2¦ NEXT ¦ PRINT ¦ DELETE ¦ NEW ¦ RUN ¦ STOP ¦ CONTINUE ¦ IF ¦ GO SUB ¦ LOAD ¦ CLEAR ¦ REM ¦ CAT ¦";
        }

        return keywords;
}

ZXString zx80BasicLister::TranslateToZxToken(ZXString chr)
{
        ZXString translatedChr;

        map<ZXString, ZXString>::const_iterator it = mZxTokenChars.find(chr);

        if (it != mZxTokenChars.end())
        {
                translatedChr = it->second;

                if (mZxpandEnabled)
                {
                        if (translatedChr == "[HEX:F1]")
                                translatedChr = " DELETE ";
                        else if (translatedChr == "[HEX:F5]")
                                translatedChr = " CONFIG ";
                        else if (translatedChr == "[HEX:FF]")
                                translatedChr = " CAT ";
                }
        }
        else
        {
                translatedChr = chr;
        }

        return translatedChr;
}

int zx80BasicLister::GetProgramStartAddress()
{
        return 16424;
}

int zx80BasicLister::GetProgramEndAddress()
{
        const int vars = 16392;
        return getbyte(vars) + (getbyte(vars + 1) << 8);
}

bool zx80BasicLister::ExtractLineDetails(int* address, LineInfo& lineInfo)
{
        int endAddress = GetProgramEndAddress();
        if ((*address) >= endAddress)
        {
                return false;
        }

        lineInfo.address = *address;
        const int lineHeaderLength = 2;
        lineInfo.addressContent = lineHeaderLength + lineInfo.address;

        const int EndOfBasicMarker = 0x40;
        int lineNumber = getbyte((*address)++);
        if (lineNumber >= EndOfBasicMarker)
        {
                return false;
        }

        lineNumber = (lineNumber << 8) + getbyte((*address)++);
        lineInfo.lineNumber = lineNumber;

        int length = 0;    
        bool lastKeywordEndedWithSpace = false;
        unsigned char b;
        bool endOfLine;

        lineInfo.displayLength = 4;     // Line number length

        do
        {
                b = getbyte((*address)++);
                
                endOfLine = (b == Newline);
                
                if (!endOfLine)
                {
                        int keywordLength = GetKeywordLength(b);
                        lineInfo.displayLength += keywordLength;

                        bool keywordStartsWithSpace = (keywordLength > 1) && (mKeyword[b][0] == ' ');
                        bool keywordEndsWithSpace = (keywordLength > 1) && (mKeyword[b].substr(keywordLength - 1, 1) == " ");

                        if (lastKeywordEndedWithSpace && keywordStartsWithSpace)
                        {
                                lineInfo.displayLength--;
                        }

                        bool characterIsSpace = (mKeyword[b] == " ");

                        if (!characterIsSpace)
                        {
                                lastKeywordEndedWithSpace = keywordEndsWithSpace;
                        }
                }
                
                length++;
        }
        while (!endOfLine);

        lineInfo.lineLength = lineHeaderLength + length;
        lineInfo.contentLength = length;
        lineInfo.displayRows = (lineInfo.displayLength + (GetDisplayColumns() - 1)) / GetDisplayColumns();

        return true;
}

unsigned char zx80BasicLister::ConvertToZXCode(unsigned char code)
{
        unsigned char c;

        switch ((char)code)
        {
                case ' ':
                        c = Space;
                        break;

                case '*':
                        c = Asterisk;
                        break;

                default:
                        if (code >= '0' && code <= '9')
                        {
                                c = (unsigned char)(Number0 + (code - '0'));
                        }
                        else
                        {
                                c = (unsigned char)(LetterA + (code - 'A'));
                        }
                        break;
        }

        return c;
}

bool zx80BasicLister::SupportFloatingPointNumbers()
{
        return false;
}

unsigned char zx80BasicLister::GetLineEndingCode()
{
        return Newline;
}

ZXString zx80BasicLister::GetMachineName()
{
        return _TEXT("ZX80");
}

ZXString zx80BasicLister::GetBasicFileExtension()
{
        return _TEXT("b80");
}

bool zx80BasicLister::ZxTokenSupported()
{
        return true;
}

bool zx80BasicLister::RemContainsMachineCode(int address, int lengthRemaining, bool outputRemTokensAsCharacterCodes)
{
        bool containsMachineCode = false;

        bool endOfLine = (lengthRemaining <= 0);

        while (!endOfLine)
        {
                int c = getbyte(address);
                address++;
                lengthRemaining--;
                endOfLine = (lengthRemaining <= 0);

                if ((c >= 0x40 && c < 0x80 && !(c == 0x76 && endOfLine)) || (c >= 0xC0 && c <= 0xD3) ||
                     c == 0xF1 || c == 0xF2 || c == 0xF5 || c == 0xFF || (c >= 0x10 && c <= 0x1A))
                {
                        if (!mZxpandEnabled || (mZxpandEnabled && c != 0xF1 && c != 0xF5 && c != 0xFF))
                        {
                                containsMachineCode = true;
                        }
                        break;
                }
                else if (outputRemTokensAsCharacterCodes && ( c == 0xD5 || c == 0xD6 || c == 0xDB || (c >= 0xE0 && c <= 0xE2) || c >= 0xE6))
                {
                        containsMachineCode = true;
                        break;
                }
        }

        return containsMachineCode;
}

