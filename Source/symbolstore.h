#ifndef _symbolstore_h_
#define _symbolstore_h_
#include "zx81config.h"

namespace symbolstore
{
        // Split a line of symbol file into symbol and address
        // return false if line can't/shouldn't be parsed
        //
        typedef bool (*SYMBOLSPLITTER)(const char*, ZXString& symbol, ZXString& address);

        // loading a new symbol file clears old symbols,
        // but when resetting a machine file symbols could be left behind.
        // so call this :)
        //
        void reset(void);

        // rom symbols are associated with a ROM. file symbols are
        // associated with a particular P file.
        // returns true if file loaded ok
        // if you want to do special processing on each symbol/value pair as
        // they're parsed, pass in a munger.
        //
		bool loadROMSymbols(const char* filename);

		bool loadSymFileSymbols(const char* filename);
		bool loadZ88FileSymbols(const char* filename);

        // check if symbols are loaded
        //
        bool romLoaded(void);
        bool fileLoaded(void);

        // searches file set first, then ROM set.
        // returns true if the symbol was resolved into result
        //
        bool addressToSymbol(const int addr, ZXString& result);

        // returns either the resolved symbol or the hex equivalent
        //
        ZXString addressToSymbolOrHex(const int addr);

        // given a symbol, do a reverse look-up and return its address
        // returns true if val was resolved
        //
        bool symbolToAddress(const ZXString& sym, int& addr);

        // query the content
        //
        void beginenumeration(void);
		bool enumerate(ZXString& sym, int& addr, char& type);
};

#endif // _symbolstore_h_
