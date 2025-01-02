/* EightyOne, Windows based Sinclair emulators.
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
 *
 *
 * soundDX.c - DirectSound Output module
 */

#include <dsound.h>
#include <tchar.h>
#include "sound\SoundDX.h"

CDSnd::CDSnd(void)
{
        m_ThreadHandle=NULL;
        m_hWnd=NULL;
}

int CDSnd::Initialise(HWND hWnd, int FPS, int BitsPerSample, int SampleRate, int Channels)
{

        m_BitsPerSample=BitsPerSample;
        m_SampleRate=SampleRate;
        m_Channels=Channels;
        m_FPS=FPS;
        m_hWnd=hWnd;


        // If any essentials haven't been initialised,
        // or the audio thread is already running

        if (!m_hWnd || !m_FPS || !m_BitsPerSample || !m_SampleRate || !m_Channels) return(0);
        if (m_ThreadHandle) return(0);

        // Initialise Variables

	ZeroMemory(&m_WFE, sizeof(m_WFE));
	m_lpDS = NULL;
	m_lpDSB = NULL;
	m_pHEvent[0] = CreateEvent(NULL, FALSE, FALSE, _TEXT("DSound_Buf_Notify_0"));
	m_pHEvent[1] = CreateEvent(NULL, FALSE, FALSE, _TEXT("DSound_Buf_Notify_1"));
        m_lpAudioQueue = NULL;

        LPVOID lpvAudio1 = NULL, lpvAudio2 = NULL;
	DWORD dwBytesAudio1, dwBytesAudio2;

        // Set Sound Output Format - Stereo/22kHz/8Bit

        m_WFE.wFormatTag =  WAVE_FORMAT_PCM;
        m_WFE.nChannels = m_Channels;
        m_WFE.nSamplesPerSec = m_SampleRate;
        m_WFE.wBitsPerSample = m_BitsPerSample;
        m_WFE.nBlockAlign = m_WFE.nChannels;
        m_WFE.nAvgBytesPerSec = m_WFE.nChannels *
                        m_WFE.nSamplesPerSec * m_WFE.wBitsPerSample/8;

        // Calculate Bufferlengths
        // AudioQueue is 2 Seconds long
        // DXbuffer is 2 frames long

        m_DXBufLen = (m_WFE.nBlockAlign * m_WFE.nSamplesPerSec) / FPS;
        m_DXBufLen = (m_DXBufLen & ~1) * 2;
        m_QueueLen = m_DXBufLen * 40;

	//Create DirectSound
        int r = DirectSoundCreate(NULL, &m_lpDS, NULL);
        if (r)
                return(r); // Create DirectSound Failed

	//Set Cooperative Level
        if (m_lpDS->SetCooperativeLevel(m_hWnd, DSSCL_PRIORITY))
                return(-1); // SetCooperativeLevel Failed

	//Create Primary Buffer
	DSBUFFERDESC dsbd;
	ZeroMemory(&dsbd, sizeof(dsbd));
	dsbd.dwSize = sizeof(DSBUFFERDESC);
	dsbd.dwFlags = DSBCAPS_PRIMARYBUFFER;
	dsbd.dwBufferBytes = 0;
	dsbd.lpwfxFormat = NULL;

	LPDIRECTSOUNDBUFFER lpDSB = NULL;
        if (m_lpDS->CreateSoundBuffer(&dsbd, &lpDSB, NULL))
                return -1; //Create Primary Sound Buffer Failed

	//Set Primary Buffer Format
        if (lpDSB->SetFormat(&m_WFE))
               return -1; // Set Primary Format Failed!"));

	//Create Secondary Buffer
	dsbd.dwFlags = DSBCAPS_CTRLPOSITIONNOTIFY | DSBCAPS_GLOBALFOCUS;
	dsbd.dwBufferBytes = m_DXBufLen;
	dsbd.lpwfxFormat = &m_WFE;

        if (m_lpDS->CreateSoundBuffer(&dsbd, &m_lpDSB, NULL))
                return(-1);// Create Second Sound Buffer Failed

	//Query DirectSoundNotify
	LPDIRECTSOUNDNOTIFY lpDSBNotify;

        if (m_lpDSB->QueryInterface(IID_IDirectSoundNotify, (LPVOID *)&lpDSBNotify))
                return -1; // QueryInterface DirectSoundNotify Failed

	//Set Direct Sound Buffer Notify Position
	DSBPOSITIONNOTIFY pPosNotify[2];
	pPosNotify[0].dwOffset = m_DXBufLen / 2;
	pPosNotify[0].hEventNotify = m_pHEvent[0];
	pPosNotify[1].dwOffset = m_DXBufLen - 1;
	pPosNotify[1].hEventNotify = m_pHEvent[1];

        if (lpDSBNotify->SetNotificationPositions(2, pPosNotify))
                return -1; //Set NotificationPosition Failed

	//New audio Queue
	if (m_lpAudioQueue !=NULL)
        {
	       	delete []m_lpAudioQueue;
		m_lpAudioQueue = NULL;
	}

	m_lpAudioQueue = new BYTE[m_QueueLen];

	//Initialise Audio Queue
        m_QueueStart = 0;
        m_QueueSize = 0;

	memset(m_lpAudioQueue, 0x80, m_QueueLen);

        if (m_lpDSB->Lock(0, m_DXBufLen, &lpvAudio1, &dwBytesAudio1, &lpvAudio2, &dwBytesAudio2, 0))
                return -1; // Lock DirectSoundBuffer Failed!

        if (!lpvAudio2)
        {
        	memcpy(lpvAudio1, m_lpAudioQueue, dwBytesAudio1);
        }
        else
        {
	        memcpy(lpvAudio1, m_lpAudioQueue, dwBytesAudio1);
	        memcpy(lpvAudio2, m_lpAudioQueue + dwBytesAudio1, dwBytesAudio2);
        }

        // Unlock DirectSoundBuffer
        m_lpDSB->Unlock(lpvAudio1, dwBytesAudio1, lpvAudio2, dwBytesAudio2);

        // Create the Audio Thread
        m_ThreadHandle = CreateThread(0, 0, &CallThread, this, 0, &m_ThreadID);
        if(!m_ThreadHandle) return -1; // Cannot create Audio thread.

        ResumeThread(m_ThreadHandle);    // Start/Resume the Audio Thread
	m_lpDSB->Play(0, 0, DSBPLAY_LOOPING);

        return 0;
}

int CDSnd::Play()
{
        // Ensure Sound is initialised before attempting the play sould

        if (!m_hWnd || !m_FPS || !m_BitsPerSample || !m_SampleRate || !m_Channels)
                return(0);

        ResumeThread(m_ThreadHandle);    // Start/Resume the Audio Thread
        m_lpDSB->Play(0, 0, DSBPLAY_LOOPING);
        return 0;
}

int CDSnd::End()
{
	if (m_lpDSB != NULL)
        {
		m_lpDSB->Stop();  //Stop Playing Sound

                //Kill the audio thread
                if (m_ThreadHandle) TerminateThread(m_ThreadHandle,0);
                m_ThreadHandle=NULL;

		//Empty the buffer
		LPVOID lpvAudio1 = NULL;
		DWORD dwBytesAudio1 = 0;

		if (m_lpDSB->Lock(0, 0, &lpvAudio1, &dwBytesAudio1, NULL, NULL, DSBLOCK_ENTIREBUFFER))
                        return -1; // Lock entirebuffer failed! Stop Failed!

		memset(lpvAudio1, 0, dwBytesAudio1);
		m_lpDSB->Unlock(lpvAudio1, dwBytesAudio1, NULL, NULL);

		//Move the current play position to begin
		m_lpDSB->SetCurrentPosition(0);

                m_QueueStart=0;
                m_QueueSize=0;
	}

	if (m_lpAudioQueue != NULL)
        {
		delete []m_lpAudioQueue;
		m_lpAudioQueue = NULL;
	}

        return 0;
}

// Helper function to get around not being able to
// use class functions as windows callback directly
DWORD WINAPI CDSnd::CallThread(LPVOID Param)
{
        CDSnd *instance = (CDSnd *)Param;

        if(!instance) return -1;

        instance->ThreadFN();
        return(0);
}

// Audio thread function - called every frame
// copy a frame size chunk of audio data from the audio
// queue into the dx sound portion of buffer that isn't
// being used
void CDSnd::ThreadFN()
{
	LPVOID lpvAudio1 = NULL, lpvAudio2 = NULL;
	DWORD dwBytesAudio1, dwBytesAudio2;
	
        while(1)
        {
                int i=WaitForMultipleObjects(2, m_pHEvent, FALSE, -1) - WAIT_OBJECT_0;

                switch(i)
                {
                case 0:
                // Lock DirectSoundBuffer Second Part
                if (m_lpDSB->Lock(m_DXBufLen/2, m_DXBufLen/2,
                                &lpvAudio1, &dwBytesAudio1, &lpvAudio2, &dwBytesAudio2, 0))
                        return;  // Lock Buffer Failed

                        break;
                case 1:
                        // Lock DirectSoundBuffer First Part
		        if (m_lpDSB->Lock(0, m_DXBufLen/2, &lpvAudio1, &dwBytesAudio1, &lpvAudio2, &dwBytesAudio2, 0))
                                return; // Lock Buffer Failed

                        break;

                default:  // Timed out so just run the frame without sound
                        break;
                }

                if (i < 2)
                {
	                // Copy AudioQueue to DirectSoundBuffer
                        // Should check for Queue wrap-around, but can't be bothered for now
	                if (lpvAudio2 == NULL)
                        {
                                if (m_QueueStart+dwBytesAudio1 > m_QueueLen)
                                {
                                        // Queue wraps around end queueso copy both parts
                                        int P1Len = m_QueueLen-m_QueueStart;
                                        int P2Len = m_QueueSize-P1Len;
                                        char *P1 = (char *) lpvAudio1;
                                        char *P2 = (char *) lpvAudio1 + P1Len;

                                        memcpy(P1, m_lpAudioQueue + m_QueueStart, P1Len);
                                        memcpy(P2, m_lpAudioQueue, P2Len);

                                }
                                else
                                        memcpy(lpvAudio1, m_lpAudioQueue + m_QueueStart, dwBytesAudio1);
	                }
	                else
                        {       // BUG - doesn't chach for queue wrap around.  However, the
                                // chances of the queue wrapping around at the same time as
                                // receiving a split DX Sound frame seem unlikely, so we'll
                                // live with it for the time being.

		                memcpy(lpvAudio1, m_lpAudioQueue + m_QueueStart, dwBytesAudio1);
		                memcpy(lpvAudio2, m_lpAudioQueue + dwBytesAudio1 + m_QueueStart, dwBytesAudio2);
	                }


                        int delta;

                        delta= m_QueueSize;
                        if (m_QueueSize < dwBytesAudio1) delta = m_QueueSize;
                        else

                        m_QueueSize -= delta;
                        m_QueueStart += delta;
                        if (m_QueueStart >= m_QueueLen)
                                m_QueueStart -= m_QueueLen;    // BUG //

	                // Unlock DirectSoundBuffer
	                m_lpDSB->Unlock(lpvAudio1, dwBytesAudio1, lpvAudio2, dwBytesAudio2);

                }

                ResetEvent(m_pHEvent[0]);
                ResetEvent(m_pHEvent[1]);

                SetLastError(0);
                SendMessage( m_hWnd, WM_USER, NULL, NULL);
                if (GetLastError())
                {
                        _TCHAR buf[256];
                        FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                                NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                                buf, sizeof(buf), NULL);

                        MessageBox(NULL, buf,_TEXT("Message Sending Error"),2);
                }
        }
}

void CDSnd::Frame( unsigned char *data, unsigned int len )
{
// Copy a sound frame from the emulator into the Sound Queue

        DWORD Pos = m_QueueStart + m_QueueSize;
        if (Pos > m_QueueLen) Pos -= m_QueueLen;
        m_QueueSize += len;
        if (m_QueueSize > m_QueueLen) m_QueueSize = m_QueueLen;


        while(len--)
        {
                m_lpAudioQueue[Pos++] = *data++;
                if( Pos >= m_QueueLen ) Pos -= m_QueueLen;
        }
}

