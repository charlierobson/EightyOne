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

#include <string>
#include <vector>
#include <map>
#include "BasicLister\IBasicLister.h"
#include <Classes.hpp>
#include "zx81config.h"

class zx81BasicLister : public IBasicLister
{
public:
        zx81BasicLister(bool zxpandEnabled);
        virtual ZXString GetMachineName();
        virtual ZXString GetBasicFileExtension();
        virtual bool ZxTokenSupported();

private:
        static const unsigned char Space = 0;
        static const unsigned char Quote = 11;
        static const unsigned char Dollar = 13;
        static const unsigned char GreaterThan = 18;
        static const unsigned char LessThan = 19;
        static const unsigned char Equal = 20;
        static const unsigned char Asterisk = 23;
        static const unsigned char Number0 = 28;
        static const unsigned char LetterA = 38;
        static const unsigned char Newline = 118;
        static const unsigned char Number = 126;

        bool mZxpandEnabled;
        std::map<ZXString, ZXString> mZxTokenChars;

        virtual int GetProgramStartAddress();
        virtual int GetProgramEndAddress();
        virtual unsigned char ConvertToZXCode(unsigned char code);
        virtual inline unsigned char GetFloatingPointNumberCode();
        virtual inline unsigned char GetLineEndingCode();
        virtual std::string GetKeywords();
        virtual bool RemContainsMachineCode(int address, int lengthRemaining, bool outputRemTokensAsCharacterCodes);
        virtual ZXString TranslateToZxToken(ZXString chr);
};

