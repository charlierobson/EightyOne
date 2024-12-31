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

#include "BasicLoader/IBasicLoader.h"
#include "BasicLoaderOptions_.h"
#include <fstream>
#include <sstream>
#include <cctype>

void IBasicLoader::LoadBasicFile(ZXString filename, bool tokeniseRemContents, bool tokeniseStrings, bool discardSurplusSpaces, bool acceptAlternateKeywordSpelling, bool zxTokenSupport)
{
        string result;

        mProgramLength = 0;
        mEscapeCharacter = GetEscapeCharacter();

        try
        {
                ReadBasicListingFile(filename);
        }
        catch (exception& ex)
        {
                stringstream msg;
                msg << ex.what() << endl;
                msg << endl;
                int index = mLines.size() - 1;
                if (index >= 0)
                {
                        bool truncateLine = (mLines[index].line.length() > 256);
                        int displayLen = truncateLine ? 256: mLines[index].line.length();
                        msg << mLines[index].line.substr(0, displayLen);
                        if (truncateLine) msg << "...";
                }
#if __CODEGEARC__ >= 0x0620
                wchar_t temp[512];
                mbstowcs(temp, msg.str().c_str(),512);
                Application->MessageBox(temp, L"Load BASIC Listing", MB_OK | MB_ICONERROR);
#else
                Application->MessageBox(msg.str().c_str(), "Load BASIC Listing", MB_OK | MB_ICONERROR);
#endif
                return;
        }

        memset(mProgramData, 0, sizeof(mProgramData));

        int addressOffset = 0x0000;
        OutputStartOfProgramData(filename, addressOffset);

        int numberOfLines = mLines.size();

        for (int i = 0; i < numberOfLines; i++)
        {
                try
                {
                        ProcessLine(mLines[i], addressOffset, tokeniseRemContents, tokeniseStrings, discardSurplusSpaces, acceptAlternateKeywordSpelling, zxTokenSupport);
                }
                catch (exception& ex)
                {
                        stringstream msg;
                        msg << "Unable to parse line " << mLines[i].sourceLine << " - " << ex.what() << endl;
                        msg << endl;
                        bool truncateLine = (mLines[i].line.length() > 256);
                        int displayLen = truncateLine ? 256: mLines[i].line.length();
                        msg << mLines[i].line.substr(0, displayLen);
                        if (truncateLine) msg << "...";
#if __CODEGEARC__ >= 0x0620
                        wchar_t temp[512];
                        mbstowcs(temp, msg.str().c_str(),512);
                        Application->MessageBox(temp, L"Load BASIC Listing", MB_OK | MB_ICONERROR);
#else
                        Application->MessageBox(msg.str().c_str(), "Load BASIC Listing", MB_OK | MB_ICONERROR);
#endif
                        return;
                }
        }
        
        OutputEndOfProgramData(addressOffset);

        mProgramLength = addressOffset;
}

void IBasicLoader::ReadBasicListingFile(ZXString filename)
{
        if (!FileExists(filename))
        {
                stringstream msg;
                msg << "File not found:" << endl << endl;
                msg << AnsiString(filename).c_str() << endl;

                throw runtime_error(msg.str());
        }

        ifstream basicFile(AnsiString(filename).c_str());
        if (basicFile.fail())
        {
                stringstream msg;
                msg << "Failed to load file:" << endl << endl;
                msg << AnsiString(filename).c_str() << endl;

                throw runtime_error(msg.str());
        }

        mLines = vector<LineEntry>();

        int lineNumberIncrement = LoadBasicListingOptionsForm->GetAutomaticLineNumberIncrement();

        string line;
        int currentLineNumber = -1;
        LineEntry entry;
        int sourceLine = 0;

        try
        {
                while (ReadLine(basicFile, line, sourceLine))
                {
                        entry.line = line;
                        entry.lineNumberLength = 0;
                        entry.lineNumber = -1;
                        entry.lineLabel = "";
                        entry.sourceLine = sourceLine;

                        if (GetLineNumber(entry))
                        {
                                if (entry.lineNumber <= currentLineNumber)
                                {
                                        throw runtime_error("Line number not greater than previous line number");
                                }
                                currentLineNumber = entry.lineNumber;
                        }
                        else if (GetLineLabel(entry))
                        {
                                entry.lineNumber = currentLineNumber + lineNumberIncrement;
                                if (BasicLineExists(entry))
                                {
                                        currentLineNumber += lineNumberIncrement;
                                }
                        }
                        else
                        {
                                currentLineNumber += lineNumberIncrement;
                                entry.lineNumber = currentLineNumber;
                        }

                        if ((entry.lineNumber < 0) || (entry.lineNumber > 16383))
                        {
                                throw out_of_range("Line number out of range");
                        }
                        
                        mLines.push_back(entry);
                }
        }
        catch (exception& ex)
        {
                mLines.push_back(entry);

                stringstream msg;
                msg << "Error reading line " << mLines[mLines.size()-1].sourceLine << " - " << ex.what();
                throw runtime_error(msg.str().c_str());
        }
}

bool IBasicLoader::GetLineNumber(LineEntry& lineEntry)
{
        char* pEnd;
        const char* pStart = lineEntry.line.c_str();

        lineEntry.lineNumber = strtol(pStart, &pEnd, 10);

        if (pEnd == pStart)
        {
                return false;
        }

        lineEntry.lineNumberLength = pEnd - pStart;

        return true;
}

bool IBasicLoader::GetLineLabel(LineEntry& lineEntry)
{
        if (lineEntry.line[1] != '@')
        {
                return false;
        }

        size_t endLabel = lineEntry.line.find_first_of(":");
        if (endLabel == string::npos)
        {
                return false;
        }

        if (endLabel == 2)
        {
                throw runtime_error("Invalid label");
        }

        lineEntry.lineLabel = lineEntry.line.substr(1, endLabel-1);

        return true;
}

bool IBasicLoader::BasicLineExists(const LineEntry& lineEntry)
{
        size_t endOfLabelIndex = lineEntry.line.find(":");

        size_t linePositionIndex = lineEntry.line.substr(endOfLabelIndex).find_first_not_of(" \t");

        return (linePositionIndex != string::npos);               
}

bool IBasicLoader::ReadLine(ifstream& basicFile, string& line, int& sourceLine)
{
        size_t i;
        string inputLine;

        do
        {
                line = " ";

                do
                {
                        do
                        {
                                sourceLine++;

                                bool lineAvailable = (getline(basicFile, inputLine) != NULL);
                                if (!lineAvailable)
                                {
                                        return false;
                                }

                                // Remove the trailing '\0'
                                if (inputLine[inputLine.length()-1] == '\0')
                                {
                                        inputLine = inputLine.substr(0, inputLine.length()-1);
                                }

                                i = inputLine.find_first_not_of(" \t");
                        }
                        while ((i == string::npos) || (inputLine[i] == '\0'));

                        int len = inputLine.length();
                        if (len > 0 && inputLine[len-1] == mEscapeCharacter)
                        {
                                len--;
                        }
                        line += inputLine.substr(0, len);
                }
                while (inputLine.length() > 0 && inputLine[inputLine.length()-1] == mEscapeCharacter);

                i = line.find_first_not_of(" \t");
        }
        while (line[i] == '#');

        return true;
}

void IBasicLoader::OutputByte(int& addressOffset, unsigned char byte)
{
        if (addressOffset >= maxProgramLength)
        {
                throw length_error("Program size too large");
        }

        mProgramData[addressOffset++] = byte;
}

void IBasicLoader::OutputWord(int& addressOffset, int word)
{
        OutputByte(addressOffset, word & 0xFF);
        OutputByte(addressOffset, word >> 8);
}

void IBasicLoader::ChangeWord(int addressOffset, int word)
{
        OutputWord(addressOffset, word);
}

unsigned char* IBasicLoader::ProgramData()
{
        return mProgramData;
}

int IBasicLoader::ProgramLength()
{
        return mProgramLength;
}

void IBasicLoader::ProcessLine(LineEntry lineEntry, int& addressOffset, bool tokeniseRemContents, bool tokeniseStrings, bool discardSurplusSpaces, bool acceptAlternateKeywordSpelling, bool zxTokenSupport)
{
        memset(mLineBuffer, 0, sizeof(mLineBuffer));
        int offset = lineEntry.lineLabel.length() > 0 ? lineEntry.lineLabel.length() + 2 : 0;
        strcpy((char*)mLineBuffer, lineEntry.line.substr(offset).c_str());

        memset(mLineBufferOutput, 0, sizeof(mLineBufferOutput));
        int i = strlen((char*)mLineBuffer);
        memset(mLineBufferOutput, Blank, i);

        memset(mLineBufferPopulated, (unsigned char)false, sizeof(mLineBufferPopulated));

        BlankLineStart(lineEntry);

        ExtractZxTokenFormatExtensions(zxTokenSupport);
        ExtractInverseCharacters();
        ExtractEscapeCharacters();
        ExtractDoubleQuoteCharacters();
        ExtractZxTokenFormatExtensionByteEncoding(zxTokenSupport);

        memset(mLineBufferTokenised, 0, sizeof(mLineBufferTokenised));
        strcpy((char*)mLineBufferTokenised, (char*)mLineBuffer);

        if (acceptAlternateKeywordSpelling)
        {
                i = 0;
                while (mLineBufferTokenised[i] != '\0')
                {
                        mLineBufferTokenised[i] = toupper(mLineBufferTokenised[i]);
                        i++;
                }
        }
        
        // Append a space at the end of the line to ensure a token without a trailing space is detected
        i = strlen((char*)mLineBufferTokenised);
        mLineBufferTokenised[i] = ' ';

        memset(mLineBufferStrings, 0, sizeof(mLineBufferStrings));
        strcpy((char*)mLineBufferStrings, (char*)mLineBufferTokenised);

        MaskOutStrings(mLineBufferStrings);
        MaskOutRemContents(mLineBufferStrings);

        if (!tokeniseStrings)
        {
                MaskOutStrings(mLineBufferTokenised);
        }

        if (!tokeniseRemContents)
        {
                MaskOutRemContents(mLineBufferTokenised);
        }

        ExtractTokens(acceptAlternateKeywordSpelling);

        ExtractSingleCharacters(discardSurplusSpaces);

        bool lineExists;

        if (lineEntry.lineLabel != "")
        {
                size_t pos = lineEntry.line.find_first_of(":");
                lineExists = (lineEntry.line.find_first_not_of(Blank, pos + 1) != string::npos);
        }
        else
        {
                lineExists = (lineEntry.line.find_first_not_of(Blank, lineEntry.lineNumberLength) != string::npos);
        }

        if (lineExists)
        {
                OutputLine(lineEntry.lineNumber, addressOffset);
        }
}

void IBasicLoader::BlankLineStart(LineEntry lineEntry)
{
        if (lineEntry.lineNumberLength > 0)
        {
                //Blank out line number characters
                memset(mLineBuffer, Blank, lineEntry.lineNumberLength);
        }
        else
        {
                // Blank out leading spaces when there is no line number
                int i = 0;
                while ((mLineBuffer[i] == ' ') || (mLineBuffer[i] == '\t'))
                {
                        mLineBuffer[i] = Blank;
                        i++;
                }

                mLineBuffer[--i] = ' ';
        }
}

void IBasicLoader::ExtractSingleCharacters(bool discardSurplusSpaces)
{
        int i = 1;

        while (mLineBuffer[i] != '\0')
        {
                if (mLineBuffer[i] != Blank)
                {
                        if (!discardSurplusSpaces || (discardSurplusSpaces && (mLineBuffer[i] != ' ' || mLineBufferStrings[i] != ' ')))
                        {
                                unsigned char c = mLineBuffer[i];

                                if (SupportUppercaseOnly())
                                {
                                        c = toupper(c);
                                }

                                mLineBufferOutput[i] = AsciiToZX(c);
                                mLineBufferPopulated[i] = true;
                        }
                        else
                        {
                                mLineBuffer[i] = Blank;
                        }
                }
                
                i++;
        }
}

void IBasicLoader::MaskOutStrings(unsigned char* buffer)
{
        const char* const quote = "\"";
        char* pQuote = strstr((char*)buffer, quote);
        if (pQuote == NULL)
        {
                return;
        }

        const char* const rem = " REM ";
        char* pRem = strstr((char*)buffer, rem);
        if (pRem != NULL && pRem < pQuote)
        {
                return;
        }

        bool withinQuote = false;

        buffer = (unsigned char *)pQuote;

        while (*buffer != '\0')
        {
                if (*buffer == '\"')
                {
                        withinQuote = !withinQuote;
                }
                else if (withinQuote)
                {
                        *buffer = Blank;
                }
                else
                {
                        pQuote = strstr((char*)buffer+1, quote);
                        if (pQuote == NULL)
                        {
                                return;
                        }

                        pRem = strstr((char*)buffer+1, rem);
                        if (pRem != NULL && pRem < pQuote)
                        {
                                return;
                        }
                }

                buffer++;
        }
}

void IBasicLoader::MaskOutRemContents(unsigned char* buffer)
{
        char* pPos = (char*)buffer;
        bool remFound = false;

        char* pRem;
        const char* const rem = " REM ";

        do
        {
                pRem = strstr(pPos, rem);
                if (pRem == NULL)
                {
                        return;
                }

                const char* const quote = "\"";
                char* pQuote = strstr(pPos, quote);
                if (pQuote != NULL && pQuote < pRem)
                {
                        pQuote = strstr(pQuote+1, quote);
                        if (pQuote == NULL)
                        {
                                return;
                        }

                        pPos = pQuote + 1;
                }
                else
                {
                        remFound = true;
                }
        }
        while (!remFound);    
        
        char* pContents = pRem + strlen(rem);
        memset(pContents, Blank, strlen(pContents));
}

unsigned char* IBasicLoader::ExtractLineNumber(int& lineNumber)
{
        unsigned char* pCommand;

        lineNumber = strtol((const char*)mLineBuffer, (char **)&pCommand, 10);

        if (pCommand == mLineBuffer)
        {
                throw runtime_error("Line number missing");
        }

        if ((lineNumber < 0) || (lineNumber > 16383))
        {
                throw out_of_range("Line number too high");
        }

        return pCommand;
}

bool IBasicLoader::StartOfNumber(int index)
{
        if ((mLineBuffer[index] != '.') && !isdigit(mLineBuffer[index]))
        {
                return false;
        }

        while (mLineBufferTokenised[index-1] == ' ')
        {
                index--;
        }

        return (!isalpha(mLineBuffer[index-1]) && !isdigit(mLineBuffer[index-1]));
}

void IBasicLoader::OutputEmbeddedNumber(int& index, int& addressOffset, bool binaryFormatFlag)
{
        unsigned char* pBufferStart = mLineBuffer + index;
        unsigned char* pTokenisedBufferStart = mLineBufferTokenised + index;

        int maxNumberLength = strlen((char*)pBufferStart);
        unsigned char* pLineBufferWithoutSpaces = new unsigned char[maxNumberLength + 1];

        int withoutSpacesIndex = 0;

        for (int withSpacesIndex = 0; withSpacesIndex < maxNumberLength; withSpacesIndex++)
        {
                if (pTokenisedBufferStart[withSpacesIndex] != ' ')
                {
                        pLineBufferWithoutSpaces[withoutSpacesIndex] = pBufferStart[withSpacesIndex];
                        withoutSpacesIndex++;
                }
        }
        pLineBufferWithoutSpaces[withoutSpacesIndex] = 0;

        char* pWithoutSpacesNumberEnd;

        double value;
        if (binaryFormatFlag)
        {
                const int base = 2;
                value = (double)strtol((char*)pLineBufferWithoutSpaces, &pWithoutSpacesNumberEnd, base);
        }
        else
        {
                value = strtod((char*)pLineBufferWithoutSpaces, &pWithoutSpacesNumberEnd);
        }

        int withoutSpacesNumberLength = pWithoutSpacesNumberEnd - (char *)pLineBufferWithoutSpaces;
             
        int withSpacesIndex = 0;

        for (int lengthCount = 0; lengthCount < withoutSpacesNumberLength; lengthCount++)
        {
                while (pBufferStart[withSpacesIndex] == ' ' || pBufferStart[withSpacesIndex] == Blank)
                {
                        withSpacesIndex++;
                }

                withSpacesIndex++;   
        }

        char* pEnd = (char *)pBufferStart + withSpacesIndex;

        delete[] pLineBufferWithoutSpaces;
        
        while ((char *)(mLineBuffer + index) < pEnd)
        {
                unsigned char chr = mLineBufferOutput[index];
                if (chr != Blank)
                {
                        OutputByte(addressOffset, chr);
                }
                index++;
        }

        while (mLineBuffer[index] == ' ')
        {
                OutputByte(addressOffset, AsciiToZX(' '));
                index++;
        }

        index--;

        OutputByte(addressOffset, GetEmbbededNumberMark());

        OutputFloatingPointEncoding(value, addressOffset);
}

void IBasicLoader::ExtractZxTokenFormatExtensions(bool zxTokenSupport)
{
        if (zxTokenSupport)
        {
                ExtractZxTokenNumericBlocks();
                ExtractZxTokenCharacterCodes();
        }
}

void IBasicLoader::ExtractZxTokenFormatExtensionByteEncoding(bool zxTokenSupport)
{
        if (!zxTokenSupport)
        {
                return;
        }

        unsigned char* pPos = mLineBuffer;

        while (*pPos != '\0')
        {
                if (*pPos == '@')
                {
                        *pPos = Blank;

                        pPos++;
                        unsigned char chr1 = *pPos;
                        if (chr1 == '\0')
                        {
                                throw runtime_error("Escape sequence incomplete before line ending");
                        }

                        *pPos = Blank;

                        unsigned char zxChr;

                        pPos++;
                        unsigned char chr2 = *pPos;
                        if (chr2 == '\0')
                        {
                                throw runtime_error("Escape sequence incomplete before line ending");
                        }

                        *pPos = Blank;

                        if (isxdigit(chr1))
                        {
                                zxChr = ConvertFromHexChars(chr1, chr2);
                        }
                        else
                        {
                                ostringstream code;
                                code << chr1;
                                code << chr2;
                                string msg = "Invalid character Code: " + code.str();
                                throw invalid_argument(msg.c_str());
                        }
                        
                        int index = pPos - mLineBuffer;
                        mLineBufferOutput[index] = zxChr;
                        mLineBufferPopulated[index] = true;
                }

                pPos++;
        }
}

void IBasicLoader::ExtractZxTokenNumericBlocks()
{
        unsigned char* pPos = mLineBuffer;

        bool withinBrackets = false;
        int base;

        while (*pPos != '\0')
        {
                if (*pPos == '[')
                {
                        if (withinBrackets)
                        {
                                throw runtime_error("Nested numeric blocks not supported");
                        }

                        withinBrackets = true;

                        *pPos = Blank;
                        pPos++;

                        base = ExtractNumericBlockBase(&pPos);
                }
                else if (withinBrackets && *pPos != ']')
                {
                        if (*pPos == ' ' || *pPos == ',' || *pPos == ';' || *pPos == '-' || *pPos == ':' || *pPos == '/')
                        {
                                *pPos = Blank;
                        }
                        else
                        {
                                unsigned char zxChr = ExtractByteValue(&pPos, base);

                                int index = pPos - mLineBuffer;
                                mLineBufferOutput[index] = zxChr;
                                mLineBufferPopulated[index] = true;
                        }

                }
                else if (*pPos == ']')
                {
                        if (!withinBrackets)
                        {
                                throw runtime_error("Closing numeric block marker found without opening marker");
                        }

                        *pPos = Blank;

                        withinBrackets = false;
                }

                pPos++;
        }

        if (withinBrackets)
        {
                throw runtime_error("Invalid numberic block");
        }
}

int IBasicLoader::ExtractNumericBlockBase(unsigned char** ppPos)
{
        int base;

        string numericBlockBaseText = ExtractText(*ppPos, ':', "numeric block");

        if (numericBlockBaseText == "DEC")
        {
                base = 10;
        }
        else if (numericBlockBaseText == "HEX")
        {
                base = 16;
        }
        else if (numericBlockBaseText == "BIN")
        {
                base = 2;
        }
        else
        {
                throw runtime_error("Unsupported numeric block base");
        }

        for (unsigned int c = 0; c < numericBlockBaseText.length(); c++)
        {
                **ppPos = Blank;
                (*ppPos)++;
        }

        **ppPos = Blank;

        return base;
}

unsigned char IBasicLoader::ExtractByteValue(unsigned char** ppPos, int base)
{
        unsigned char byteValue = 0;

        if (base == 16 || base == 2 || base == 10)
        {
                char* pEnd;
                int value = strtol((char*)*ppPos, &pEnd, base);
                int numberChars = pEnd - (char *)*ppPos;
                if (numberChars == 0)
                {
                        throw runtime_error("Invalid value in numeric block");
                }

                byteValue = (unsigned char)value;

                for (int c = 0; c < numberChars - 1; c++)
                {
                        **ppPos = Blank;
                        (*ppPos)++;
                }

                **ppPos = Blank;           
        }

        return byteValue;
}

void IBasicLoader::ExtractEscapeCharacters()
{
        unsigned char* pPos = mLineBuffer;

        while (*pPos != '\0')
        {
                if (*pPos == mEscapeCharacter)
                {
                        *pPos = Blank;

                        pPos++;
                        unsigned char chr1 = *pPos;
                        if (chr1 == '\0')
                        {
                                throw runtime_error("Escape sequence incomplete before line ending");
                        }

                        *pPos = Blank;

                        unsigned char zxChr;

                        if (NoEscapeSequence(chr1))
                        {
                                int index = (pPos - 1) - mLineBuffer;
                                mLineBufferOutput[index] = mEscapeCharacter;
                                mLineBufferPopulated[index] = true;
                                
                                zxChr = chr1;
                        }
                        else if (!SingleEscapeSequence(chr1, zxChr))
                        {
                                pPos++;
                                unsigned char chr2 = *pPos;
                                if (chr2 == '\0')
                                {
                                        throw runtime_error("Escape sequence incomplete before line ending");
                                }

                                *pPos = Blank;

                                if (isxdigit(chr1))
                                {
                                        zxChr = ConvertFromHexChars(chr1, chr2);
                                }
                                else
                                {
                                        zxChr = DecodeGraphic(chr1, chr2);
                                }
                        }
                        
                        int index = pPos - mLineBuffer;
                        mLineBufferOutput[index] = zxChr;
                        mLineBufferPopulated[index] = true;
                }

                pPos++;
        }
}

unsigned char IBasicLoader::ConvertFromHexChars(unsigned char chr1, unsigned char chr2)
{
        if (!isxdigit(chr1) || !isxdigit(chr2))
        {
                ostringstream code;
                code << chr1;
                code << chr2;
                string msg = "Invalid character Code: " + code.str();
                throw invalid_argument(msg.c_str());
        }
        
        int d1 = ConvertFromHexChar(chr1);
        int d2 = ConvertFromHexChar(chr2);
        return (d1 << 4) + d2;
}

unsigned char IBasicLoader::ConvertFromHexChar(unsigned char chr)
{
        unsigned char val = isdigit(chr) ? chr - '0' : (chr & 0xDF) - 'A' + 10;
        return val;
}

void IBasicLoader::ReplaceTokenEndCharacters(map<unsigned char, string>& tokens, const unsigned char oldChar, const unsigned char newChar)
{
        map<unsigned char, string>::iterator it;

        for (it = tokens.begin(); it != tokens.end(); it++)
        {
                const char* pToken = (it->second).c_str();
                int lenToken = strlen(pToken);
                const unsigned char endChar = pToken[lenToken-1];

                if (endChar == oldChar)
                {
                        it->second[lenToken-1] = newChar;
                }
        }
}

void IBasicLoader::DoTokenise(map<unsigned char, string> tokens)
{
        map<unsigned char, string>::reverse_iterator it;

        // Iterate through in reverse numerical order
        for (it = tokens.rbegin(); it != tokens.rend(); it++)
        {
                unsigned char tokenCode = it->first;
                const char* pToken = (it->second).c_str();
                int lenToken = strlen(pToken);

                unsigned char* pMatch;
                bool tokenFound;
                int lenAdjustment = 0;

                const unsigned char startChar = pToken[0];
                const unsigned char endChar = pToken[lenToken-1];

                bool tokenBeginsWithSpace = (startChar == ' ');
                bool tokenBeginsWithAlpha = isalpha(startChar);
                bool tokenEndsWithSpace = (endChar == ' ');
                bool tokenEndsWithAlpha = isalpha(endChar);

                if (endChar == '(' || endChar == ')' || endChar == '!' || endChar == '\"' || (endChar == '#' && pToken[lenToken-2] != ' ') || (endChar == '*' &&  it->second != "**") || endChar == '\'' || endChar == ',' || endChar == ';')
                {
                        lenToken--;
                }

                if (endChar == '#' && (it->second == " OPEN#" || it->second == " CLOSE#"))
                {
                        lenAdjustment = 1;
                }
                
                do
                {
                        pMatch = (unsigned char *)strstr((char*)mLineBufferTokenised, (char*)pToken);
                        bool matchFound = (pMatch != NULL);

                        bool startOk = matchFound && (tokenBeginsWithSpace || !tokenBeginsWithAlpha || (!tokenBeginsWithSpace && !isalnum(pMatch[-1])));
                        bool endOk = matchFound && (tokenEndsWithSpace || !tokenEndsWithAlpha || (!tokenEndsWithSpace && !isalnum(pMatch[lenToken])));

                        tokenFound = matchFound && startOk && endOk;

                        if (tokenFound)
                        {
                                int startOffset = tokenBeginsWithSpace? 1 : 0;
                                int endOffset = tokenEndsWithSpace ? lenToken - 1 : lenToken + lenAdjustment;

                                unsigned char* pStartToken = pMatch + startOffset + 1;
                                unsigned char* pAfterToken = pMatch + lenToken;

                                // Blank the token apart from the leading and/or trailing spaces,
                                // which allows supporting suppressed spaces between tokens
                                for (int b = startOffset; b < endOffset; b++)
                                {
                                        *(pMatch + b) = Blank;
                                }

                                int offset = pMatch - mLineBufferTokenised;

                                for (int b = 0; b < lenToken + lenAdjustment; b++)
                                {
                                        mLineBuffer[offset + b] = Blank;
                                }

                                int outputIndex = offset + startOffset;

                                mLineBufferOutput[outputIndex] = tokenCode;
                                mLineBufferPopulated[outputIndex] = true;

                                if (TokenSupportsLineNumber(tokenCode))
                                {
                                        HandleTokenLineNumber(pStartToken, pAfterToken, outputIndex);
                                }
                        }
                }
                while (tokenFound);
        }
}

void IBasicLoader::HandleTokenLineNumber(unsigned char* pStartToken, unsigned char* pLabelSearch, int outputIndex)
{
        int spaceCount = 0;

        while (*pLabelSearch == ' ')
        {
                pLabelSearch++;
                spaceCount++;
        }

        if (*pLabelSearch != '@')
        {
                return;
        }

        string label = ExtractLabel(pLabelSearch);
        int labelLength = label.length();

        int lineEntryIndex = FindLabelDetails(label);

        ostringstream lineNum;
        int lineNumber = mLines[lineEntryIndex].lineNumber;
        lineNum << lineNumber;
        string ln = lineNum.str();
        const char* pLineNumber = ln.c_str();

        int tokenOffset = pStartToken - mLineBufferTokenised;
        int labelOffset = pLabelSearch - mLineBufferTokenised;

        unsigned char* pLineBuffer = mLineBuffer + tokenOffset;
        bool* pBufferPopulated = mLineBufferPopulated + tokenOffset;

        int lineNumberLength = strlen(pLineNumber);
        for (int n = 0; n < lineNumberLength; n++)
        {
                unsigned char lineNumberDigit = pLineNumber[n];
                pLineBuffer[n] = lineNumberDigit;
        }

        pLineBuffer += lineNumberLength;
        memset(pLineBuffer, ' ', spaceCount);

        memset(pBufferPopulated, (unsigned char)true, lineNumberLength + spaceCount);

        int blankLength = (pLabelSearch + spaceCount + labelLength) - pStartToken;
        memset(mLineBufferTokenised + tokenOffset, Blank, blankLength);

        memset(mLineBufferStrings + labelOffset, Blank, labelLength);

        blankLength = (pLabelSearch + labelLength - lineNumberLength) - pStartToken;
        memset(pLineBuffer + spaceCount, Blank, blankLength);
}

string IBasicLoader::ExtractLabel(unsigned char* pLabelSearch)
{
        return ExtractText(pLabelSearch, ' ', "label");
}

string IBasicLoader::ExtractText(unsigned char* pTextSearch, unsigned char terminator, string errorText)
{
        bool endOfText = false;
        string text;

        text += *pTextSearch;

        do
        {
                pTextSearch++;

                unsigned char nextChr = *pTextSearch;

                if ((nextChr == '\0') || (nextChr == terminator))
                {
                        endOfText = true;
                }
                else
                {
                        text += nextChr;
                }
        }
        while (!endOfText);

        if (text.length() == 1)
        {
                throw runtime_error("Invalid " + errorText);
        }

        while (*pTextSearch == ' ')
        {
                pTextSearch++;
        }

        return text;
}

int IBasicLoader::FindLabelDetails(string& label)
{
        int numberOfLines = mLines.size();

        for (int i = 0; i < numberOfLines; i++)
        {
                if (mLines[i].lineLabel == label)
                {
                        return i;
                }
        }

        throw runtime_error("Unknown label");
}

