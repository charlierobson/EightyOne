#include "Win32Sleeper.h"
#include "SystemClock.h"
#include "audio.h"

#include "sp0256.h"

#include "sp0256_al2.h"	// SP0256-AL2 "Narrator"

class SP0256
{
public:
        SP0256();
	int Init(void);
        int End(void);

private:
        void ThreadFN(void);
        static DWORD WINAPI CallThread(LPVOID Param);

        DWORD m_ThreadID;
        HANDLE m_ThreadHandle;

};

static SystemClock systemClock;
static Win32Sleeper sleeper;
const int freq = 3120000/2/156;

SP0256::SP0256(void)
{
        m_ThreadHandle=NULL;
}

int SP0256::Init(void)
{
        if (m_ThreadHandle) return(0);

        // Create the Audio Thread
        m_ThreadHandle = CreateThread(0, 0, &CallThread, this, 0, &m_ThreadID);
        if(!m_ThreadHandle) return -1; // Cannot create Audio thread.

        ResumeThread(m_ThreadHandle);    // Start/Resume the Audio Thread

        return 0;
}

int SP0256::End(void)
{
        //Kill the audio thread
        if (m_ThreadHandle) TerminateThread(m_ThreadHandle,0);
        m_ThreadHandle=NULL;

        return 0;
}

// Helper function to get around not being able to
// use class functions as windows callback directly
DWORD WINAPI SP0256::CallThread(LPVOID Param)
{
        SP0256 *instance = (SP0256 *)Param;

        if(!instance) return -1;

        instance->ThreadFN();
        return(0);
}

void SP0256::ThreadFN()
{
        while(1)
        {
      		int sample = sp0256_getNextSample();
       		sample >>= 8;
		sample += 0x80;
		sample &= 0xFF;
		outWave( uchar( sample ), uchar( sample ) );
		systemClock.runCycles( 1000 );
		outWaveCycles( 1 );
        }
}

SP0256 sp0256;

void SP0256_Init(void)
{
        sp0256.Init();
	sp0256_setLabels( sp0256_al2::nlabels, sp0256_al2::labels );

	systemClock.setClockSpeed( freq );
	systemClock.setSleeper( &sleeper );
	systemClock.setAutoTurbo( false );
	outWaveInit();
	outWaveSetClockSpeed( freq );
	outWaveReset();

        ivoice_init( sp0256_al2::mask );
}

void SP0256_End(void)
{
        sp0256.End();
}

void SP0256_Write(unsigned char Data)
{
        if (sp0256_halted())
        {
                ivoice_reset();
        }

        sp0256_sendCommand( uint32_t( Data&0x3F ) );
}

unsigned char SP0256_Busy(void)
{
        unsigned int status = sp0256_getStatus();

        return !status;
}

