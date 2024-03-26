static SystemClock systemClock;
static Win32Sleeper sleeper;
const int freq = 3120000/2/156;

void SP0256_Init(void)
{
	sp0256_setLabels( sp0256_al2::nlabels, sp0256_al2::labels );

	systemClock.setClockSpeed( freq );
	systemClock.setSleeper( &sleeper );
	systemClock.setAutoTurbo( false );
	outWaveInit();
	outWaveSetClockSpeed( freq );
	outWaveReset();

        ivoice_init( sp0256_al2::mask );
}

void SP0256_Write(unsigned char Data)
{
}

unsigned char SP0256_Busy(void)
{
}

