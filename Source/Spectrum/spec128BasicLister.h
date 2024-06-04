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
#include "BasicLister\IBasicLister.h"
#include <Classes.hpp>

class spec128BasicLister : public IBasicLister
{
public:
        spec128BasicLister();
        virtual COLORREF GetPaperColour();
        virtual COLORREF GetInkColour();
        virtual ZXString GetMachineName();
        virtual ZXString GetBasicFileExtension();

private:
        static const unsigned char Return = 13;
        static const unsigned char Number = 14;
        static const unsigned char Ink = 16;
        static const unsigned char Paper = 17;
        static const unsigned char Flash = 18;
        static const unsigned char Bright = 19;
        static const unsigned char Inverse = 20;
        static const unsigned char Over = 21;
        static const unsigned char At = 22;
        static const unsigned char Tab = 23;

        int mInkValue;
        int mPaperValue;
        int mInverseValue;
        int mBrightValue;
        COLORREF mColours[16];

        virtual int GetProgramStartAddress();
        virtual int GetProgramEndAddress();
        virtual unsigned char GetFloatingPointNumberCode();
        virtual inline unsigned char GetLineEndingCode();
        virtual inline bool SupportEmbeddedControlCodes();
        virtual inline bool IsEmbeddedControlCode(unsigned char code);
        virtual inline int GetEmbeddedControlCodeSize(unsigned char code);
        virtual std::string GetKeywords();
        virtual void ProcessControlCode(unsigned char code, unsigned char arg1, unsigned char arg2);
        virtual void InitialiseColours();
        virtual bool CustomColoursSupported();
        virtual COLORREF GetDefaultPaperColour();
        virtual bool RequiresInitialSpace();
};

