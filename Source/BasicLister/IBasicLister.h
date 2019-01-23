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

#ifndef IBASICLISTER
#define IBASICLISTER

#include <Classes.hpp>
#include <vector>
#include <string>

const int PixelsPerCharacterWidth = 8;
const int PixelsPerCharacterHeight = 8;

struct LineInfo
{
        int lineNumber;
        int address;
        int addressContent;
        int lineLength;
        int contentLength;
        int displayLength;
        int displayRows;
        int startDisplayRow;
};

class IBasicLister
{
private:
        static const int DisplayColumns = 32;
        static const int EmbeddedNumberSize = 5;

        std::string mKeywords;
        std::vector<LineInfo>* mLines;
        int mProgramDisplayRows;
        int mEmbeddedNumberSize;
        int mFloatingPointNumberCode;
        int mLineEndingCode;
        bool mSupportsFloatingPointNumbers;
        bool mSupportEmbeddedControlCodes;
        AnsiString mEscapeCharacter;
        
        void RenderLine(HDC hdc, HDC cshdc, int& y, LineInfo& lineInfo);
        void RenderLineNumber(HDC hdc, HDC cshdc, int& x, int& y, int lineNumber);
        void RenderToken(HDC hdc, HDC cshdc, int& address, int& x, int& y, int& lengthRemaining, bool& lastKeywordEndedWithSpace);
        void RenderCharacter(HDC hdc, HDC cshdc, int& x, int& y, unsigned char c);
        bool RenderTokenAsText(int& address, int& lengthRemaining, bool& lastKeywordEndedWithSpace, AnsiString& zxCharacter, bool& outputLineAsControlCodes, bool outputRemTokensAsCharacterCodes, bool outputStringTokensAsCharacterCodes, bool outputNonAsciiAsCharacterCodes, bool outputVariableNamesInLowercase, bool& withinQuotes, bool& withinRem);
        AnsiString FormatLineNumber(int lineNumber, bool outputFullWidthLineNumbers = false);
        COLORREF GetBackgroundColour();
        unsigned char GetEscapeCharacter() { return '\\'; }

public:
        IBasicLister();
        virtual ~IBasicLister();
        void PopulateKeywords();
        virtual void ExtractProgramDetails();
        virtual int GetProgramRows();
        virtual int GetDisplayColumns() { return DisplayColumns; };
        virtual void ClearRenderedListing(HDC hdc, HBITMAP bitmap, RECT rect, bool showLineEnds);
        virtual void RenderListing(HDC hdc, HBITMAP bitmap, RECT rect, bool showLineEnds);
        virtual AnsiString RenderLineAsText(LineInfo& lineInfo, bool outputRemTokensAsCharacterCodes, bool outputStringTokensAsCharacterCodes, bool outputNonAsciiAsCharacterCodes, bool outputVariableNamesInLowercase, bool limitLineLengths, bool outputFullWidthLineNumbers);
        virtual void SetLines(std::vector<LineInfo>* linesInfo);
        virtual COLORREF GetInkColour() { return RGB(0, 0, 0); }
        virtual COLORREF GetPaperColour() { return RGB(255, 255, 255); }
        virtual AnsiString GetMachineName() { return ""; }
        virtual AnsiString GetBasicFileExtension() { return "txt"; }

protected:
        COLORREF mInk;
        COLORREF mPaper;

        virtual std::string GetKeywords() { return std::string(""); }
        virtual int GetProgramStartAddress() { return 65535; }
        virtual int GetProgramEndAddress() { return 65535; }
        virtual inline unsigned char ConvertToZXCode(unsigned char code) { return code; }
        virtual inline bool SupportFloatingPointNumbers() { return true; }
        virtual unsigned char GetFloatingPointNumberCode() { return 0; }
        virtual unsigned char GetLineEndingCode() { return 0; }
        virtual int GetEmbeddedNumberSize() { return EmbeddedNumberSize; }
        virtual bool ExtractLineDetails(int* address, LineInfo& lineInfo);
        virtual inline bool SupportEmbeddedControlCodes() { return false; }
        virtual bool IsEmbeddedControlCode(unsigned char code) { return false; }
        virtual int GetEmbeddedControlCodeSize(unsigned char code) { return 0; }
        virtual void ProcessControlCode(unsigned char code, unsigned char arg1, unsigned char arg2) {}
        virtual void InitialiseColours() { }
        virtual inline bool CustomColoursSupported() { return false; }
        virtual COLORREF GetDefaultPaperColour() { return RGB(255, 255, 255); }
        virtual bool RemContainsMachineCode(int address, int lengthRemaining, bool outputRemTokensAsCharacterCodes) { return false; }

        int GetKeywordLength(unsigned char code);

        std::string mKeyword[256];
};

#endif
