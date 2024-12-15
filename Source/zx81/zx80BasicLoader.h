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

#ifndef ZX80BASICLOADER_H
#define ZX80BASICLOADER_H

#include "BasicLoader/IBasicLoader.h"
#include <vcl.h>
#include <string>
#include "zx81config.h"

using namespace::std;

class zx80BasicLoader : public IBasicLoader
{
public:
        zx80BasicLoader(bool zxpandEnabled);

private:
        static const unsigned char Newline = 0x76;
        static const unsigned char List = 0xE6;
        static const unsigned char Goto = 0xEC;
        static const unsigned char Run = 0xF7;
        static const unsigned char Gosub = 0xFB;
        static const unsigned char Escape = '\\';

        bool mZxpandEnabled;

        void OutputSystemVariables(int& addressOffset);
        virtual unsigned char DecodeGraphic(unsigned char chr1, unsigned char chr2);
        virtual unsigned char AsciiToZX(unsigned char ascii);
        virtual void ExtractTokens(bool acceptAlternateKeywordSpelling);
        virtual void OutputLine(int lineNumber, int& addressOffset);
        virtual void OutputStartOfProgramData(ZXString filename, int& addressOffset);
        virtual void OutputEndOfProgramData(int& addressOffset);
        virtual void ExtractInverseCharacters();
        virtual bool SupportUppercaseOnly();
        virtual bool TokenSupportsLineNumber(unsigned char chr);
        virtual void ExtractZxTokenCharacterCodes();
};

#endif
