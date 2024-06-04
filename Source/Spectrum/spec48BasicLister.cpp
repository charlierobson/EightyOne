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

#include "spec48BasicLister.h"
#include "zx81config.h"

// The following embedded codes are not supported:
// AT
// TAB
// OVER
// LEFT
// COMMA

spec48BasicLister::spec48BasicLister()
{
        InitialiseColours();
        
        mColours[0] = RGB(0, 0, 0);
        mColours[1] = RGB(0, 0, 206);
        mColours[2] = RGB(206, 0, 0);
        mColours[3] = RGB(206, 0, 206);
        mColours[4] = RGB(0, 206, 0);
        mColours[5] = RGB(0, 206, 206);
        mColours[6] = RGB(206, 206, 0);
        mColours[7] = RGB(206, 206, 206);
        mColours[8] = RGB(0, 0, 0);
        mColours[9] = RGB(0, 0, 255);
        mColours[10] = RGB(255, 0, 0);
        mColours[11] = RGB(255, 0, 255);
        mColours[12] = RGB(0, 255, 0);
        mColours[13] = RGB(0, 255, 255);
        mColours[14] = RGB(255, 255, 0);
        mColours[15] = RGB(255, 255, 255);
}

std::string spec48BasicLister::GetKeywords()
{
        // Graphics encoding based upon the scheme used by ZXText2P: http://freestuff.grok.co.uk/zxtext2p/index.html
        // UDGs are prefixed with '\_' followed by the letter. Embedded control codes are prefixed by '\' followed by
        // the value of control code as a two digit hexadecimal number.
        std::string keywords = "\\00¦\\01¦\\02¦\\03¦\\04¦\\05¦\\06¦\\07¦\\08¦\\09¦\\0A¦\\0B¦\\0C¦\\0D¦\\0E¦\\0F¦\\10¦\\11¦\\12¦\\13¦\\14¦\\15¦\\16¦\\17¦\\18¦\\19¦\\1A¦\\1B¦\\1C¦\\1D¦\\1E¦\\1F¦"
                               " ¦!¦\"¦#¦$¦%¦&¦'¦(¦)¦*¦+¦,¦-¦.¦/¦0¦1¦2¦3¦4¦5¦6¦7¦8¦9¦:¦;¦<¦=¦>¦?¦"
                               "@¦A¦B¦C¦D¦E¦F¦G¦H¦I¦J¦K¦L¦M¦N¦O¦P¦Q¦R¦S¦T¦U¦V¦W¦X¦Y¦Z¦[¦\\¦]¦^¦_¦"
                               "£¦a¦b¦c¦d¦e¦f¦g¦h¦i¦j¦k¦l¦m¦n¦o¦p¦q¦r¦s¦t¦u¦v¦w¦x¦y¦z¦{¦|¦}¦~¦\xA9¦"
                               "\\  ¦\\ '¦\\' ¦\\''¦\\ .¦\\ :¦\\'.¦\\':¦\\. ¦\\.'¦\\: ¦\\:'¦\\..¦\\.:¦\\:.¦\\::¦\\_A¦\\_B¦\\_C¦\\_D¦\\_E¦\\_F¦\\_G¦\\_H¦\\_I¦\\_J¦\\_K¦\\_L¦\\_M¦\\_N¦\\_O¦\\_P¦\\_Q¦\\_R¦\\_S¦\\_T¦\\_U¦"
                               "RND¦INKEY$¦PI¦FN ¦POINT ¦SCREEN$ ¦ATTR ¦AT ¦TAB ¦VAL$ ¦CODE ¦VAL ¦LEN ¦SIN ¦COS ¦TAN ¦ASN ¦ACS ¦ATN ¦LN ¦EXP ¦INT ¦"
                               "SQR ¦SGN ¦ABS ¦PEEK ¦IN ¦USR ¦STR$ ¦CHR$ ¦NOT ¦BIN ¦ OR ¦ AND ¦<=¦>=¦<>¦ LINE ¦ THEN ¦ TO ¦ STEP ¦ DEF FN ¦ CAT ¦"
                               " FORMAT ¦ MOVE ¦ ERASE ¦ OPEN #¦ CLOSE #¦ MERGE ¦ VERIFY ¦ BEEP ¦ CIRCLE ¦ INK ¦ PAPER ¦ FLASH ¦ BRIGHT ¦ INVERSE ¦ OVER ¦ OUT ¦"
                               " LPRINT ¦ LLIST ¦ STOP ¦ READ ¦ DATA ¦ RESTORE ¦ NEW ¦ BORDER ¦ CONTINUE ¦ DIM ¦ REM ¦ FOR ¦ GO TO ¦ GO SUB ¦ INPUT ¦ LOAD ¦"
                               " LIST ¦ LET ¦ PAUSE ¦ NEXT ¦ POKE ¦ PRINT ¦ PLOT ¦ RUN ¦ SAVE ¦ RANDOMIZE ¦ IF ¦ CLS ¦ DRAW ¦ CLEAR ¦ RETURN ¦ COPY ¦";

        return keywords;
}

void spec48BasicLister::InitialiseColours()
{
        mInkValue = 0;
        mPaperValue = 7;
        mInverseValue = 0;
        mBrightValue = 0;
}

int spec48BasicLister::GetProgramStartAddress()
{
        return getbyte(23635) + (getbyte(23636) << 8);
}

int spec48BasicLister::GetProgramEndAddress()
{
        const int vars = 23627;
        return getbyte(vars) + (getbyte(vars + 1) << 8);
}

unsigned char spec48BasicLister::GetFloatingPointNumberCode()
{
        return Number;
}

unsigned char spec48BasicLister::GetLineEndingCode()
{
        return Return;
}

bool spec48BasicLister::SupportEmbeddedControlCodes()
{
        return true;
}

bool spec48BasicLister::IsEmbeddedControlCode(unsigned char code)
{
        bool controlCode = false;

        switch (code)
        {
                case Ink:
                case Paper:
                case Flash:
                case Bright:
                case Inverse:
                case Over:
                case At:
                case Tab:
                        controlCode = true;
                        break;
        }

        return controlCode;
}

int spec48BasicLister::GetEmbeddedControlCodeSize(unsigned char code)
{
        return (code == At) ? 2 : 1;
}

void spec48BasicLister::ProcessControlCode(unsigned char code, unsigned char arg1, unsigned char arg2)
{
        switch (code)
        {
                case Ink:
                        if (arg1 <= 7)
                        {
                                mInkValue = arg1;
                        }
                        break;

                case Paper:
                        if (arg1 <= 7)
                        {
                                mPaperValue = arg1;
                        }
                        break;

                case Bright:
                        mBrightValue = arg1;
                        break;

                case Inverse:
                        mInverseValue = arg1;
                        break;
        }
}

COLORREF spec48BasicLister::GetInkColour()
{
        int bright = (mBrightValue != 0) ? 8 : 0;
        COLORREF ink = mInverseValue ? mColours[mPaperValue + bright] : mColours[mInkValue + bright];
        return ink;
}

COLORREF spec48BasicLister::GetPaperColour()
{
        int bright = (mBrightValue != 0) ? 8 : 0;
        COLORREF paper = mInverseValue ? mColours[mInkValue + bright] : mColours[mPaperValue + bright];
        return paper;
}

bool spec48BasicLister::CustomColoursSupported()
{
        return true;
}

COLORREF spec48BasicLister::GetDefaultPaperColour()
{
        return mColours[7];
}

ZXString spec48BasicLister::GetMachineName()
{
        return "Spectrum";
}

ZXString spec48BasicLister::GetBasicFileExtension()
{
        return "b82";
}

bool spec48BasicLister::RequiresInitialSpace()
{
        return false;
}

