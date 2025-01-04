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

#include "IBasicLister.h"
#include "zx81config.h"
#include <vcl4.h>
#include <sstream>
#include <iomanip>
#include <string>

using namespace std;

IBasicLister::IBasicLister() :
        mProgramDisplayRows(0)
{
}

void IBasicLister::PopulateKeywords()
{
        std::string keywords = GetKeywords();

        int keywordIndex = 0;
        int currPos = 0;
        int prevPos = 0;
        int sepPos;

        do
        {
                sepPos = keywords.find("¦", currPos);
                if (sepPos >= 0)
                {
                        currPos = sepPos;
                        mKeyword[keywordIndex] = keywords.substr(prevPos, currPos - prevPos);
                        currPos++;
                        prevPos = currPos;
                        keywordIndex++;
                }
        }
        while (sepPos >= 0);
}

IBasicLister::~IBasicLister()
{
}

int IBasicLister::GetKeywordLength(unsigned char code)
{
        int len;

        if (((mKeyword[code][0] == '%') && (mKeyword[code].length() > 1)) ||
            (mKeyword[code][0] == '\\'))
        {
                len = 1;
        }
        else
        {
                len = mKeyword[code].length();
        }

        return len;
}

void IBasicLister::SetLines(std::vector<LineInfo>* linesInfo)
{
        mLines = linesInfo;
}

void IBasicLister::ExtractProgramDetails()
{
        const int startAddressOfProgram = GetProgramStartAddress();

        mLines->clear();

        mProgramDisplayRows = 0;
        bool lineAvailable;
        int displayRow = 0;

        int address = startAddressOfProgram;

        mEmbeddedNumberSize = GetEmbeddedNumberSize();
        mFloatingPointNumberCode = GetFloatingPointNumberCode();
        mLineEndingCode = GetLineEndingCode();
        mSupportsFloatingPointNumbers = SupportFloatingPointNumbers();
        mSupportEmbeddedControlCodes = SupportEmbeddedControlCodes();

        do
        {
                LineInfo lineInfo;

                lineAvailable = ExtractLineDetails(&address, lineInfo);
                if (lineAvailable)
                {
                        lineInfo.startDisplayRow = displayRow;
                        mLines->push_back(lineInfo);

                        mProgramDisplayRows += lineInfo.displayRows;
                        displayRow += lineInfo.displayRows;
                }
        }
        while (lineAvailable);
}

bool IBasicLister::ExtractLineDetails(int* address, LineInfo& lineInfo)
{
        int endAddress = GetProgramEndAddress();
        if ((*address) >= endAddress)
        {
                return false;
        }

        lineInfo.address = *address;
        const int lineHeaderLength = 4;
        lineInfo.addressContent = lineHeaderLength + lineInfo.address;

        const int EndOfBasicMarker = 0x40;
        int lineNumber = getbyte((*address)++);
        if (lineNumber >= EndOfBasicMarker)
        {
                return false;
        }

        lineNumber = (lineNumber << 8) + getbyte((*address)++);
        lineInfo.lineNumber = lineNumber;

        int length = getbyte((*address)++) + (getbyte((*address)++) << 8);
        if (length < 1)
        {
                return false;
        }

        lineInfo.contentLength = length;
        lineInfo.lineLength = lineHeaderLength + length;

        const int LineNumberLength = 4;
        lineInfo.displayLength = LineNumberLength;

        bool lastKeywordEndedWithSpace = false;

        for (int c = 0; c < length; c++)
        {
                bool endOfLine = (c == (length - 1));

                unsigned char b = (unsigned char)getbyte((*address)++);

                if (mSupportsFloatingPointNumbers && (b == mFloatingPointNumberCode) && ((length - c) >= mEmbeddedNumberSize))
                {
                        c += mEmbeddedNumberSize;
                        (*address) += mEmbeddedNumberSize;
                        
                        lastKeywordEndedWithSpace = false;
                        continue;
                }

                if (mSupportEmbeddedControlCodes && IsEmbeddedControlCode(b) && ((length - c) >= GetEmbeddedControlCodeSize(b)))
                {
                        int size = GetEmbeddedControlCodeSize(b);
                        c += size;
                        (*address) += size;

                        lastKeywordEndedWithSpace = false;
                        continue;
                }

                if (!endOfLine)
                {
                        int keywordLength = GetKeywordLength(b);
                        lineInfo.displayLength += keywordLength;

                        bool keywordStartsWithSpace = (keywordLength > 1) && (mKeyword[b][0] == ' ');
                        bool keywordEndsWithSpace = (keywordLength > 1) && (mKeyword[b][keywordLength-1] == ' ');

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
        }

        lineInfo.displayRows = (lineInfo.displayLength + (GetDisplayColumns() - 1)) / GetDisplayColumns();

        return true;
}

int IBasicLister::GetProgramRows()
{
        return mProgramDisplayRows;
}

void IBasicLister::ClearRenderedListing(HDC hdc, HBITMAP bitmap, RECT rect, bool showLineEnds)
{
        COLORREF canvasColour = GetSysColor(COLOR_BTNFACE);
        if (mLines->size() > 0)
        {
                canvasColour = showLineEnds ? GetBackgroundColour() : GetDefaultPaperColour();
        }
        HBRUSH hBrush = CreateSolidBrush(canvasColour);
        FillRect(hdc, &rect, hBrush);
        DeleteObject(hBrush);
}

void IBasicLister::RenderListing(HDC hdc, HBITMAP bitmap, RECT rect, bool showLineEnds, int scaling)
{
        mScaling = scaling;

        int yOffset = 0;

        InitialiseColours();

        HDC cshdc = CreateCompatibleDC(hdc);

        HGDIOBJ oldBitmap = SelectObject(cshdc, (HGDIOBJ)((Graphics::TBitmap*)machine.cset)->Handle);

        ClearRenderedListing(hdc, bitmap, rect, showLineEnds);              

        for (std::vector<LineInfo>::iterator it = mLines->begin(); it != mLines->end(); it++)
        {
                LineInfo lineInfo = *it;
                RenderLine(hdc, cshdc, yOffset, lineInfo);
        }

        SelectObject(cshdc, oldBitmap);
        DeleteDC(cshdc);
}

COLORREF IBasicLister::GetBackgroundColour()
{
        return RGB(132, 130, 132);
}

void IBasicLister::RenderLine(HDC hdc, HDC cshdc, int& y, LineInfo& lineInfo)
{
        int x = 0;

        int lineNumber = lineInfo.lineNumber;

        RenderLineNumber(hdc, cshdc, x, y, lineNumber);
        bool requiresInitialSpace = RequiresInitialSpace();
        if (requiresInitialSpace)
        {
                RenderCharacter(hdc, cshdc, x, y, ConvertToZXCode(' '));
        }

        int address = lineInfo.addressContent;
        int lengthRemaining = lineInfo.contentLength;
        bool lastKeywordEndedWithSpace = requiresInitialSpace;

        do
        {
                RenderToken(hdc, cshdc, address, x, y, lengthRemaining, lastKeywordEndedWithSpace);
        }
        while (lengthRemaining > 0);

        if (x > 0)
        {
                x = 0;
                y++;
        }
}

void IBasicLister::RenderLineNumber(HDC hdc, HDC cshdc, int& x, int& y, int lineNumber)
{
        ZXString formattedLineNumber = FormatLineNumber(lineNumber);

        for (signed int i = 2; i <= formattedLineNumber.Length(); i++)
        {
                unsigned char c = ConvertToZXCode(formattedLineNumber[i]);
                RenderCharacter(hdc, cshdc, x, y, c);
        }
}

void IBasicLister::RenderToken(HDC hdc, HDC cshdc, int& address, int& x, int& y, int& lengthRemaining, bool& lastKeywordEndedWithSpace)
{
        unsigned char c = (unsigned char)getbyte(address);
        address++;
        lengthRemaining--;
        bool endOfLine = (lengthRemaining <= 0);

        if (endOfLine && (c == mLineEndingCode))
        {
                return;
        }

        if (mSupportsFloatingPointNumbers && (c == mFloatingPointNumberCode))
        {
                address += mEmbeddedNumberSize;
                lengthRemaining -= mEmbeddedNumberSize;
                
                return;
        }

        if (mSupportEmbeddedControlCodes && IsEmbeddedControlCode(c))
        {
                unsigned char arg1 = (unsigned char)getbyte(address);
                address++;
                lengthRemaining--;

                unsigned char arg2;
                if (GetEmbeddedControlCodeSize(c) == 2)
                {
                        arg2 = (unsigned char)getbyte(address);
                        address++;
                        lengthRemaining--;
                }

                ProcessControlCode(c, arg1, arg2);

                return;
        }

        int length = GetKeywordLength(c);
        if (length > 1)
        {
                std::string kw = mKeyword[c];

                bool keywordStartsWithSpace = (mKeyword[c][0] == ' ');
                bool keywordEndsWithSpace = (mKeyword[c][length-1] == ' ');
                int startIndex = 0;

                if (lastKeywordEndedWithSpace && keywordStartsWithSpace)
                {
                        startIndex = 1;
                }

                for (int i = startIndex; i < length; i++)
                {
                        unsigned char code = mKeyword[c].at(i);
                        unsigned char zxCode = ConvertToZXCode(code);
                        RenderCharacter(hdc, cshdc, x, y, zxCode);
                }

                lastKeywordEndedWithSpace = keywordEndsWithSpace;
        }
        else
        {
                RenderCharacter(hdc, cshdc, x, y, c);

                bool characterIsSpace = (mKeyword[c] == " ");

                if (!characterIsSpace)
                {
                        lastKeywordEndedWithSpace = false;
                }
        }
}

void IBasicLister::RenderCharacter(HDC hdc, HDC cshdc, int& x, int& y, unsigned char c)
{
        int charX = (c % 32) << 3;
        int charY = (c / 32) << 3;
        int xpos = (x << 3) * mScaling;
        int ypos = (y << 3) * mScaling;
        const int srcW = 8;
        const int srcH = 8;
        int destW = 8 * mScaling;
        int destH = 8 * mScaling;
        StretchBlt(hdc, xpos, ypos, destW, destH, cshdc, charX, charY, srcW, srcH, SRCCOPY);

        if (CustomColoursSupported())
        {
                COLORREF backgroundColour = RGB(255, 255, 255);
                COLORREF foregroundColour = RGB(0, 0, 0);
                COLORREF paperColour = GetPaperColour();
                COLORREF inkColour = GetInkColour();
                bool differentPaperColour = (paperColour != backgroundColour);
                bool differentInkColour = (inkColour != foregroundColour);

                for (int y = ypos; y < ypos + (8 * mScaling); y += mScaling)
                {
                        for (int x = xpos; x < xpos + (8 * mScaling); x += mScaling)
                        {
                                COLORREF pixelColor = GetPixel(hdc, x, y);
                                if (pixelColor == backgroundColour)
                                {
                                        if (differentPaperColour)
                                        {
                                                SetPixelV(hdc, x, y, paperColour);
                                                if (mScaling == 2)
                                                {
                                                        SetPixelV(hdc, x+1, y, paperColour);
                                                        SetPixelV(hdc, x, y+1, paperColour);
                                                        SetPixelV(hdc, x+1, y+1, paperColour);
                                                }
                                        }
                                }
                                else
                                {
                                        if (differentInkColour)
                                        {
                                                SetPixelV(hdc, x, y, inkColour);
                                                if (mScaling == 2)
                                                {
                                                        SetPixelV(hdc, x+1, y, inkColour);
                                                        SetPixelV(hdc, x, y+1, inkColour);
                                                        SetPixelV(hdc, x+1, y+1, inkColour);
                                                }
                                        }
                                }
                        }
                }
        }

        x++;
        if (x == GetDisplayColumns())
        {
                x = 0;
                y++;
        }
}

ZXString IBasicLister::RenderLineAsText(LineInfo& lineInfo, bool outputRemTokensAsCharacterCodes, bool outputStringTokensAsCharacterCodes, bool outputNonAsciiAsCharacterCodes, bool outputVariableNamesInLowercase, bool outputInZxTokenFormat, bool limitLineLengths, bool outputFullWidthLineNumbers)
{
        ZXString lineText = "";

        ZXString lineNumber = FormatLineNumber(lineInfo.lineNumber, outputFullWidthLineNumbers);
        int startPos = (lineNumber[1] != ' ') ? 1 : 2;
        int length = _tcsclen(lineNumber.c_str()) - (startPos - 1);
        lineText += lineNumber.SubString(startPos, length);
        bool requiresInitialSpace = RequiresInitialSpace();
        if (requiresInitialSpace)
        {
                lineText += " ";
        }

        int address = lineInfo.addressContent;
        int lengthRemaining = lineInfo.contentLength;
        bool lastKeywordEndedWithSpace = requiresInitialSpace;
        ZXString zxCharacter;

        mEscapeCharacter = AnsiChar(GetEscapeCharacter());

        bool outputLineAsControlCodes = false;
        bool withinRem = false;
        bool withinQuotes = false;
        int lineLength = lineText.Length();

        const int WrapLineThreshold = 120;
        
        do
        {
                if (RenderTokenAsText(address, lengthRemaining, lastKeywordEndedWithSpace, zxCharacter, outputLineAsControlCodes, outputRemTokensAsCharacterCodes, outputStringTokensAsCharacterCodes, outputNonAsciiAsCharacterCodes, outputVariableNamesInLowercase, outputInZxTokenFormat, withinQuotes, withinRem))
                {
                        lineText += zxCharacter;
                        lineLength += zxCharacter.Length();

                        if (limitLineLengths && (lineLength > WrapLineThreshold))
                        {
                                lineText += mEscapeCharacter + '\n';
                                lineLength = 0;
                        }
                }
        }
        while (lengthRemaining > 0);

        if (outputInZxTokenFormat)
        {
                TReplaceFlags flags = TReplaceFlags() << rfReplaceAll;
                lineText = StringReplace(lineText, "][HEX:", ",", flags);
                lineText = StringReplace(lineText, "]\\\n[HEX:", ",\\\n", flags);
        }

        return lineText;
}

bool IBasicLister::RenderTokenAsText(int& address, int& lengthRemaining, bool& lastKeywordEndedWithSpace, ZXString& zxCharacter, bool& outputLineAsControlCodes, bool outputRemTokensAsCharacterCodes, bool outputStringTokensAsCharacterCodes, bool outputNonAsciiAsCharacterCodes, bool outputVariableNamesInLowercase, bool outputInZxTokenFormat, bool& withinQuotes, bool& withinRem)
{
        unsigned char c = (unsigned char)getbyte(address);
        address++;
        lengthRemaining--;
        bool endOfLine = (lengthRemaining <= 0);

        if (endOfLine && (c == mLineEndingCode))
        {
                return false;
        }

        bool remToken = (mKeyword[c] == " REM ");

        if (!withinRem && mKeyword[c] == "\"")
        {
                withinQuotes = !withinQuotes;
        }
        else if (!withinQuotes && remToken)
        {
                withinRem = true;
        }

        if (!withinRem && mSupportsFloatingPointNumbers && (c == mFloatingPointNumberCode))
        {
                address += mEmbeddedNumberSize;
                lengthRemaining -= mEmbeddedNumberSize;

                lastKeywordEndedWithSpace = false;
                return false;
        }

        int length = GetKeywordLength(c);
        bool token = (length > 1);

        if ((outputStringTokensAsCharacterCodes && withinQuotes && token) || outputLineAsControlCodes || (((mKeyword[c] == "£") || (mKeyword[c] == "©")) && outputNonAsciiAsCharacterCodes))
        {
                if (!outputInZxTokenFormat)
                {
                        zxCharacter = mEscapeCharacter;
                        zxCharacter += UpperCase(IntToHex(c, 2));
                }
                else
                {
                        zxCharacter = "[HEX:";
                        zxCharacter += UpperCase(IntToHex(c, 2));
                        zxCharacter += "]";
                }

                lastKeywordEndedWithSpace = false;
        }
        else if (length > 1)
        {
                if (!withinQuotes && withinRem)
                {
                        outputLineAsControlCodes = RemContainsMachineCode(address, lengthRemaining, outputRemTokensAsCharacterCodes);
                }

                bool keywordStartsWithSpace = (mKeyword[c][0] == ' ');
                bool keywordEndsWithSpace = (mKeyword[c][length-1] == ' ');
                int startIndex = 0;

                if (lastKeywordEndedWithSpace && keywordStartsWithSpace)
                {
                        startIndex = 1;
                }

                zxCharacter = "";

                for (int i = startIndex; i < length; i++)
                {
                        zxCharacter += mKeyword[c].at(i);
                }

                lastKeywordEndedWithSpace = keywordEndsWithSpace;
        }
        else
        {
                if (!withinQuotes && !withinRem && outputVariableNamesInLowercase && (mKeyword[c].length() == 1))
                {
                        char chr = (char)tolower(mKeyword[c][0]);
                        zxCharacter = chr;
                }
                else
                {
                        zxCharacter = mKeyword[c].c_str();

                        if (outputInZxTokenFormat)
                        {
                                zxCharacter = TranslateToZxToken(zxCharacter);
                        }
                }

                lastKeywordEndedWithSpace = false;

                if (zxCharacter == mEscapeCharacter)
                {
                        zxCharacter += mEscapeCharacter;
/*
                        unsigned char nc = (unsigned char)getbyte(address);
                        string nextChr = mKeyword[nc].substr(0, 1);

                        string validEscapeChars;
                        validEscapeChars = "\\_1234567890ABCDEFabcdef\n '.#~@!;:";

                        size_t pos = nextChr.find_first_of(validEscapeChars);
                        if (pos != string::npos)
                        {
                                zxCharacter += mEscapeCharacter;
                        }
*/
                }
        }

        return true;
}

ZXString IBasicLister::FormatLineNumber(int lineNumber, bool outputFullWidthLineNumbers)
{
        std::ostringstream ss;

        ss << std::setfill(' ');
        ss << std::setw(5);
        ss << lineNumber;
        ZXString lineNum = ss.str().c_str();

        if (!outputFullWidthLineNumbers && (lineNum[1] != ' '))
        {
                lineNum[2] = (char)('A' + (lineNum[2] - '0'));
                lineNum[1] = ' ';
        }

        return lineNum;
}

