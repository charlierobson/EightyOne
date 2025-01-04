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

#include "zx81BasicLister.h"
#include "zx81config.h"

using namespace std;

zx81BasicLister::zx81BasicLister(bool zxpandEnabled)
{
        mZxpandEnabled = zxpandEnabled;

        // Details on ZxToken format can be found: http://dskcenter.free.fr/zxtools.html
        mZxTokenChars["\\' "] = "\xB6";
        mZxTokenChars["\\ '"] = "\xB7";
        mZxTokenChars["\\''"] = "\xB8";
        mZxTokenChars["\\. "] = "\xB9";
        mZxTokenChars["\\: "] = "\xBA";
        mZxTokenChars["\\.'"] = "\xBB";
        mZxTokenChars["\\:'"] = "\xBC";
        mZxTokenChars["\\##"] = "\xBD";
        mZxTokenChars["\\,,"] = "\xBE";
        mZxTokenChars["\\~~"] = "\xBF";

        for (int i = 0x43; i <= 0x7F; i++)
        {
                ZXString hex = ZXString::IntToHex(i, 4).UpperCase();
                mZxTokenChars["\\" + hex] = "[HEX:" + hex + "]";
        }

        mZxTokenChars["% "] = "\xAB";
        mZxTokenChars["\\.:"] = "\xAC";
        mZxTokenChars["\\:."] = "\xAA";
        mZxTokenChars["\\.."] = "\xAE";
        mZxTokenChars["\\':"] = "\xAF";
        mZxTokenChars["\\ :"] = "\xB0";
        mZxTokenChars["\\'."] = "\xB1";
        mZxTokenChars["\\ ."] = "\xB2";
        mZxTokenChars["\\@@"] = "\xB3";
        mZxTokenChars["\\;;"] = "\xB4";
        mZxTokenChars["\\!!"] = "\xB5";
        mZxTokenChars["%\""] = "\x86";
        mZxTokenChars["%£"] = "\x87";
        mZxTokenChars["%$"] = "\x88";
        mZxTokenChars["%:"] = "\x89";
        mZxTokenChars["%?"] = "\x8A";
        mZxTokenChars["%("] = "\x8B";
        mZxTokenChars["%)"] = "\x8C";
        mZxTokenChars["%>"] = "\x8D";
        mZxTokenChars["%<"] = "\x8E";
        mZxTokenChars["%="] = "\x8F";
        mZxTokenChars["%+"] = "\x90";
        mZxTokenChars["%-"] = "\x91";
        mZxTokenChars["%*"] = "\x92";
        mZxTokenChars["%/"] = "\x93";
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

        mZxTokenChars["\"\""] = "\xC0";
        mZxTokenChars["\\C3"] = "[HEX:C3]";
}

std::string zx81BasicLister::GetKeywords()
{
        std::string keywords;

        if (!mZxpandEnabled)
        {
                // Graphics encoded in accordance with ZXText2P: http://freestuff.grok.co.uk/zxtext2p/index.html
                keywords = " ¦\\' ¦\\ '¦\\''¦\\. ¦\\: ¦\\.'¦\\:'¦\\##¦\\,,¦\\~~¦\"¦£¦$¦:¦?¦(¦)¦>¦<¦=¦+¦-¦*¦/¦;¦,¦.¦0¦1¦2¦3¦4¦5¦6¦7¦8¦9¦"
                           "A¦B¦C¦D¦E¦F¦G¦H¦I¦J¦K¦L¦M¦N¦O¦P¦Q¦R¦S¦T¦U¦V¦W¦X¦Y¦Z¦RND¦INKEY$¦PI¦"
                           "\\43¦\\44¦\\45¦\\46¦\\47¦\\48¦\\49¦\\4A¦\\4B¦\\4C¦\\4D¦\\4E¦\\4F¦\\50¦\\51¦\\52¦\\53¦\\54¦\\55¦\\56¦\\57¦\\58¦\\59¦\\5A¦\\5B¦\\5C¦\\5D¦\\5E¦\\5F¦"
                           "\\60¦\\61¦\\62¦\\63¦\\64¦\\65¦\\66¦\\67¦\\68¦\\69¦\\6A¦\\6B¦\\6C¦\\6D¦\\6E¦\\6F¦\\70¦\\71¦\\72¦\\73¦\\74¦\\75¦\\76¦\\77¦\\78¦\\79¦\\7A¦\\7B¦\\7C¦\\7D¦\\7E¦\\7F¦"
                           "% ¦\\.:¦\\:.¦\\..¦\\':¦\\ :¦\\'.¦\\ .¦\\@@¦\\;;¦\\!!¦%\"¦%£¦%$¦%:¦%?¦%(¦%)¦%>¦%<¦%=¦%+¦%-¦%*¦%/¦%;¦%,¦%.¦%0¦%1¦%2¦%3¦%4¦%5¦%6¦%7¦%8¦%9¦%A¦%B¦%C¦%D¦%E¦%F¦%G¦%H¦%I¦%J¦%K¦%L¦%M¦%N¦%O¦%P¦%Q¦%R¦%S¦%T¦%U¦%V¦%W¦%X¦%Y¦%Z¦"
                           "\"\"¦AT ¦TAB ¦\\C3¦CODE ¦VAL ¦LEN ¦SIN ¦COS ¦TAN ¦ASN ¦ACS ¦ATN ¦LN ¦EXP ¦INT ¦SQR ¦SGN ¦ABS ¦PEEK ¦USR ¦STR$ ¦CHR$ ¦NOT ¦**¦ OR ¦ AND ¦<=¦>=¦<>¦ THEN ¦ TO ¦ STEP ¦ LPRINT ¦ LLIST ¦ STOP ¦ SLOW ¦ FAST ¦ NEW ¦ SCROLL ¦ CONT ¦ DIM ¦ REM ¦ FOR ¦ GOTO ¦ GOSUB ¦ INPUT ¦ LOAD ¦ LIST ¦ LET ¦ PAUSE ¦ NEXT ¦ POKE ¦ PRINT ¦ PLOT ¦ RUN ¦ SAVE ¦ RAND ¦ IF ¦ CLS ¦ UNPLOT ¦ CLEAR ¦ RETURN ¦ COPY ¦";
        }
        else
        {
                keywords = " ¦\\' ¦\\ '¦\\''¦\\. ¦\\: ¦\\.'¦\\:'¦\\##¦\\,,¦\\~~¦\"¦£¦$¦:¦?¦(¦)¦>¦<¦=¦+¦-¦*¦/¦;¦,¦.¦0¦1¦2¦3¦4¦5¦6¦7¦8¦9¦"
                           "A¦B¦C¦D¦E¦F¦G¦H¦I¦J¦K¦L¦M¦N¦O¦P¦Q¦R¦S¦T¦U¦V¦W¦X¦Y¦Z¦RND¦INKEY$¦PI¦"
                           "\\43¦\\44¦\\45¦\\46¦\\47¦\\48¦\\49¦\\4A¦\\4B¦\\4C¦\\4D¦\\4E¦\\4F¦\\50¦\\51¦\\52¦\\53¦\\54¦\\55¦\\56¦\\57¦\\58¦\\59¦\\5A¦\\5B¦\\5C¦\\5D¦\\5E¦\\5F¦"
                           "\\60¦\\61¦\\62¦\\63¦\\64¦\\65¦\\66¦\\67¦\\68¦\\69¦\\6A¦\\6B¦\\6C¦\\6D¦\\6E¦\\6F¦\\70¦\\71¦\\72¦\\73¦\\74¦\\75¦\\76¦\\77¦\\78¦\\79¦\\7A¦\\7B¦\\7C¦\\7D¦\\7E¦\\7F¦"
                           "% ¦\\.:¦\\:.¦\\..¦\\':¦\\ :¦\\'.¦\\ .¦\\@@¦\\;;¦\\!!¦%\"¦%£¦%$¦%:¦%?¦%(¦%)¦%>¦%<¦%=¦%+¦%-¦%*¦%/¦%;¦%,¦%.¦%0¦%1¦%2¦%3¦%4¦%5¦%6¦%7¦%8¦%9¦%A¦%B¦%C¦%D¦%E¦%F¦%G¦%H¦%I¦%J¦%K¦%L¦%M¦%N¦%O¦%P¦%Q¦%R¦%S¦%T¦%U¦%V¦%W¦%X¦%Y¦%Z¦"
                           "\"\"¦AT ¦TAB ¦\\C3¦CODE ¦VAL ¦LEN ¦SIN ¦COS ¦TAN ¦ASN ¦ACS ¦ATN ¦LN ¦EXP ¦INT ¦SQR ¦SGN ¦ABS ¦PEEK ¦USR ¦STR$ ¦CHR$ ¦NOT ¦**¦ OR ¦ AND ¦<=¦>=¦<>¦ THEN ¦ TO ¦ STEP ¦ ZXPAND ¦ CONFIG ¦ STOP ¦ SLOW ¦ FAST ¦ NEW ¦ SCROLL ¦ CONT ¦ DIM ¦ REM ¦ FOR ¦ GOTO ¦ GOSUB ¦ INPUT ¦ LOAD ¦ LIST ¦ LET ¦ PAUSE ¦ NEXT ¦ POKE ¦ PRINT ¦ PLOT ¦ RUN ¦ SAVE ¦ RAND ¦ IF ¦ CLS ¦ UNPLOT ¦ CLEAR ¦ RETURN ¦ CAT ¦";
        }
        
        return keywords;
}

ZXString zx81BasicLister::TranslateToZxToken(ZXString chr)
{
        ZXString translatedChr;

        map<ZXString, ZXString>::const_iterator it = mZxTokenChars.find(chr);

        if (it != mZxTokenChars.end())
        {
                translatedChr = it->second;
        }
        else
        {
                translatedChr = chr;
        }

        return translatedChr;
}

int zx81BasicLister::GetProgramStartAddress()
{
        return 16509;
}

int zx81BasicLister::GetProgramEndAddress()
{
        const int vars = 16400;
        return getbyte(vars) + (getbyte(vars + 1) << 8);
}

unsigned char zx81BasicLister::ConvertToZXCode(unsigned char code)
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

                case '>':
                        c = GreaterThan;
                        break;

                case '<':
                        c = LessThan;
                        break;

                case '=':
                        c = Equal;
                        break;

                case '\"':
                        c = Quote;
                        break;

                case '$':
                        c = Dollar;
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

unsigned char zx81BasicLister::GetFloatingPointNumberCode()
{
        return Number;
}

unsigned char zx81BasicLister::GetLineEndingCode()
{
        return Newline;
}

ZXString zx81BasicLister::GetMachineName()
{
        return "ZX81";
}

ZXString zx81BasicLister::GetBasicFileExtension()
{
        return "b81";
}                           

bool zx81BasicLister::ZxTokenSupported()
{
        return true;
}

bool zx81BasicLister::RemContainsMachineCode(int address, int lengthRemaining, bool outputRemTokensAsCharacterCodes)
{
        bool containsMachineCode = false;

        bool endOfLine = (lengthRemaining <= 0);

        while (!endOfLine)
        {
                int c = getbyte(address);
                address++;
                lengthRemaining--;
                endOfLine = (lengthRemaining <= 0);

                if ((c >= 0x43 && c < 0x80 && !(c == Newline && endOfLine)) || (c == 0xC3))
                {
                        containsMachineCode = true;
                        break;
                }
                else if (outputRemTokensAsCharacterCodes && ((c >= 0x40 && c <= 0x42) || (c >= 0xC0 && c != 0xC3)))
                {
                        containsMachineCode = true;
                        break;
                }
        }

        return containsMachineCode;
}

