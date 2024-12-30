#ifndef SP0256DRV_H_
#define SP0256DRV_H_

#include "Win32Sleeper.h"
#include "SystemClock.h"
#include "sp0256.h"


enum model_t
{
	_012, _AL2
};


class SP0256
{
public:
        SP0256(model_t model);
        ~SP0256();
        void Write(unsigned char Data);
        unsigned char Busy(void);

private:
        void ThreadFN(void);
        static DWORD WINAPI CallThread(LPVOID Param);

        DWORD m_ThreadID;
        HANDLE m_ThreadHandle;

        SystemClock systemClock;
        Win32Sleeper sleeper;
        int reset_counter;

};


extern SP0256 sp0256_AL2;
extern SP0256 sp0256_ivoice;

#endif


 