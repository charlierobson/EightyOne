#include <vcl4.h>
#include ".\symbolstore.h"

#include <assert.h>

#include <map>

using namespace std;

typedef map<int, ZXString> VAL2SYM;
typedef map<ZXString, int> SYM2VAL;

static VAL2SYM romV2S;
static SYM2VAL romS2V;

static VAL2SYM fileV2S;
static SYM2VAL fileS2V;

void splitnosval(ZXString& inval, ZXString& outval)
{
        ZXString tempVal = inval;
        int wspos = tempVal.LastDelimiter(":");
        if (wspos == 0)
        {
                return;
        }

        ZXString actualVal(tempVal.SubString(wspos + 1, tempVal.Length() - wspos));

        outval = actualVal;
}

static bool z88Splitter(const char* input, ZXString& symOut, ZXString& valOut)
{
        ZXString sym(input);
        sym = sym.Trim();
        if (sym.Length() == 0 || sym[1] == ';')
        {
                return false;
        }

        // 'SYM        = $xxxx ; ......'

        int wspos = sym.LastDelimiter(";");
        if (wspos == 0)
        {
                return false;
        }

        // 'SYM        = $xxxx '
        sym.SetLength(wspos - 1);

        wspos = sym.LastDelimiter("=");
        if (wspos == 0)
        {
                return false;
        }

        if (sym.SubString(1,2) == "__") return false;

        ZXString val(sym.SubString(wspos + 3, sym.Length() - wspos));
        sym.SetLength(wspos - 1);

        // val = 'xxxx '
        // sym = 'SYM        '

        symOut = sym.Trim();
        valOut = val.Trim();
        return true;
}

static bool symSplitter(const char* input, ZXString& symOut, ZXString& valOut)
{
        ZXString sym(input);
        sym = sym.Trim();
        if (sym.Length() == 0 || sym[1] == ';')
        {
                return false;
        }

        // take the last word from the line leaving the remainder in sym
        //
        int wspos = sym.LastDelimiter(",= \t");
        if (wspos == 0)
        {
                return false;
        }

        ZXString addr(sym.SubString(wspos + 1, sym.Length() - wspos));

        // chop any middle words from the line, leaving the symbol name
        //
        wspos = sym.LastDelimiter(",= \t");
        while (wspos != 0)
        {
                sym.SetLength(wspos - 1);
                sym = sym.Trim();
                wspos = sym.LastDelimiter(",= \t");
        }

        symOut = addr;
        valOut = sym;
        return true;
}

void loadFileSymbolsProxy(const char* path)
{
        AnsiString pp = AnsiString(path).LowerCase();
        if (pp.Length() < 3) return;

        AnsiString ppp = pp.SubString(pp.Length() - 1, 2);
        if (ppp != ".p") return;

        symbolstore::loadSymFileSymbols((pp + ".sym").c_str());
        symbolstore::loadZ88FileSymbols((pp + ".map").c_str());
}

static void clearROMSymbols(void)
{
        romV2S.clear();
        romS2V.clear();
}

static void clearFileSymbols(void)
{
        fileV2S.clear();
        fileS2V.clear();
}

void symbolstore::reset(void)
{
        clearROMSymbols();
        clearFileSymbols();
}

static bool loadSymbols(const char* filename, VAL2SYM& v2s, SYM2VAL& s2v,
        symbolstore::SYMBOLSPLITTER splitter)
{
        FILE* symfile = fopen(filename, "r");
        if (!symfile)
        {
                return false;
        }

        char buf[128];
        ZXString sym, val;

        while(fgets(buf, 128, symfile))
        {
                if (!splitter(buf, sym, val))
                {
                        continue;
                }

                sym = UpperCase(sym);

                splitnosval(val, val);

                val = "0x" + val.Trim();
                int v = StrToInt(val);
                v2s[v] = sym;
                s2v[sym] = v;
        }

        fclose(symfile);
        
        return true;
}

bool symbolstore::loadROMSymbols(const char* filename)
{
        clearROMSymbols();
        return loadSymbols(filename, romV2S, romS2V, symSplitter);
}

bool symbolstore::loadSymFileSymbols(const char* filename)
{
        clearFileSymbols();
        return loadSymbols(filename, fileV2S, fileS2V, symSplitter);
}

bool symbolstore::loadZ88FileSymbols(const char* filename)
{
        clearFileSymbols();
        return loadSymbols(filename, fileV2S, fileS2V, z88Splitter);
}

bool symbolstore::addressToSymbol(const int addr, ZXString& result)
{
        if (addr == 0) return false;  // zero is always 0

        VAL2SYM::iterator it = fileV2S.find(addr);
        if (it != fileV2S.end())
        {
                result = it->second;
                return true;
        }

        it = romV2S.find(addr);
        if (it != romV2S.end())
        {
                result = it->second;
                return true;
        }

        return false;
}

ZXString symbolstore::addressToSymbolOrHex(const int addr)
{
        ZXString temp;
        if (!addressToSymbol(addr, temp))
        {
                temp = temp.IntToHex(addr,4);
                int len = _tcslen(temp.c_str());
                int delLen = len < 4 ? 0 : len - 4; 
                temp = "$" + temp.Delete(1, delLen);
        }

        return temp;
}

// NUNS! REVERSE!
//
bool symbolstore::symbolToAddress(const ZXString& sym, int& val)
{
        SYM2VAL::iterator it = fileS2V.find(sym);
        if (it != fileS2V.end())
        {
                val = it->second;
                return true;
        }

        it = romS2V.find(sym);
        if (it != romS2V.end())
        {
                val = it->second;
                return true;
        }

        return false;
}


// NOT THREAD SAFE! actually - none of it is :P
//
static SYM2VAL::iterator nasty;
static char type;

void symbolstore::beginenumeration(void)
{
        nasty = romS2V.begin();
        type = 'r';
}

bool symbolstore::enumerate(ZXString& sym, int& val, char& storetype)
{
        if (nasty == romS2V.end())
        {
                type = 'p';
                nasty = fileS2V.begin();
        }
        if (nasty == fileS2V.end())
        {
                return false;
        }

        sym = nasty->first;
        val = nasty->second;
        storetype = type;

        ++nasty;

        return true;
}

// this is cheesy
//
bool symbolstore::romLoaded(void)
{
        return romV2S.size() != 0;
}

bool symbolstore::fileLoaded(void)
{
        return fileV2S.size() != 0;
}

