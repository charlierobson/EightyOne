#include "audio.h"
#include "sp0256drv.h"

#include "sp0256_al2.h"	// SP0256-AL2 "Narrator"
#include "sp0256_012.h"	// SP0256-012 "Intellivoice"

SP0256 sp0256_AL2(_AL2);
//SP0256 sp0256_ivoice(_012);

SP0256::SP0256(model_t model)
{
        const int xtal = 3120000;
        const int freq = xtal/2/156;

        m_ThreadHandle=NULL;

        if (model==_012)
	        sp0256_setLabels( sp0256_012::nlabels, sp0256_012::labels );
        else
	        sp0256_setLabels( sp0256_al2::nlabels, sp0256_al2::labels );

	systemClock.setClockSpeed( freq );
	systemClock.setSleeper( &sleeper );
	systemClock.setAutoTurbo( false );
	outWaveInit();
	outWaveSetClockSpeed( freq );
	outWaveReset();

        if (model==_012)
                ivoice_init( sp0256_012::mask );
        else
                ivoice_init( sp0256_al2::mask );

        reset_counter = 0;
        
        // Create the Audio Thread
        m_ThreadHandle = CreateThread(0, 0, &CallThread, this, 0, &m_ThreadID);

        ResumeThread(m_ThreadHandle);    // Start/Resume the Audio Thread
}

SP0256::~SP0256(void)
{
        //Kill the audio thread
        TerminateThread(m_ThreadHandle,0);
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

                // Since the user doesn't have access to a physical reset
                // button, this prevents the SP0256 from getting stuck
                // forever if misprogrammed.
                if (reset_counter)
                {
                        if (!--reset_counter)
                                ivoice_reset();
                }
        }
}

void SP0256::Write(unsigned char Data)
{
        reset_counter = 20000;
        sp0256_sendCommand( uint32_t( Data&0x3F ) );
}

unsigned char SP0256::Busy(void)
{
        unsigned int status = sp0256_getStatus();

        return !status;
}

