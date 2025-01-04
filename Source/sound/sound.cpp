/* z81/xz81, Linux console and X ZX81/ZX80 emulators.
 * Copyright (C) 1994 Ian Collier. z81 changes (C) 1995-2001 Russell Marks.
 * Modifications/Changes (C) 2021 Michael D Wynne, EightyOne
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
 * sound.c - sound support, based on the beeper/AY code I
 *           wrote for Fuse.
 */

/* ZX81 sound routines seem to be based on using VSYNC; in Basic
 * you might do this by alternating FAST/SLOW mode quickly,
 * for example. So that's what's emulated here. Obviously this
 * won't be perfect, not least as it won't be as noisy... :-)
 */

/* some AY details (volume levels, white noise RNG algorithm) based on
 * info from MAME's ay8910.c - MAME's licence explicitly permits free
 * use of info (even encourages it).
 */

/* NB: I know some of this stuff looks fairly CPU-hogging.
 * For example, the AY code tracks changes with sub-frame timing
 * in a rather hairy way, and there's subsampling here and there.
 * But if you measure the CPU use, it doesn't actually seem
 * very high at all. And I speak as a Cyrix owner. :-)
 *
 * (I based that on testing in Fuse, but I doubt it's that much
 * worse in z81. Though in both, the AY code does cause cache oddities
 * on my machine, so I get the bizarre situation of z81 jumping
 * between <=2% CPU use and *30*% CPU use pretty much at random...)
 */

#pragma warn -8071

#include "sound\sound.h"
#include "sound\soundop.h"
#include "sound\midi.h"
#include "zx81config.h"

CSound Sound;

// Initialise - msy br called several time with different data from different
// sections of the progrm.  eg, first time it's called we provide the window
// handle, next time we're called we get the fps of the emulated machine
// third time we get the sample rate requested.
// Each time we're called we store the provided information, and only when everything
// needed has been provided do we actually do anything.

int CSound::Initialise(HWND hWnd, int FPS, int BitsPerSample, int SampleRate, int Channels)
{
        // If a parameter is not NULL, store the data

        if (hWnd) m_hWnd=hWnd;
        if (FPS) m_FPS=FPS;
        if (BitsPerSample) m_BitsPerSample=BitsPerSample;
        if (SampleRate) m_SampleRate=SampleRate;
        if (Channels) m_Channels=Channels;

        // If anything is still missing, return

        if (!m_hWnd || !m_FPS || !m_BitsPerSample || !m_SampleRate || !m_Channels) return(0);

        // Otherwise, we're good to go, so configure the sound.

        EnvHeld=0;
        EnvAlternating=0;
        BeeperLastSubpos=0;

        // Start by initialsing DirectSound

        int r = DXSound.Initialise(m_hWnd, m_FPS, m_BitsPerSample, m_SampleRate, m_Channels);
        if (r)
        {
                return(r);
        }

        // get a 1 frame size buffer

        FrameSize=m_SampleRate/m_FPS;

        Buffer = new BYTE[FrameSize*m_Channels];

        if(Buffer==NULL)
        {
                DXSound.End();
                return(-1); // Oh dear, malloc failed
        }

        OldVal=OldValOrig=128;
        OldPos=-1;
        FillPos=0;
        SoundPtr=Buffer;

        BeeperTick=0;
        BeeperTickIncr=(1<<24)/m_SampleRate;

        AYInit();  // initialise the AY Chip

        DXSound.Play();
        return(0);
}

// Reinitialise gets called if something changes... we lose the window handle,
// The user changes the sample rats - that kind of thing.

int CSound::ReInitialise(HWND hWnd, int FPS, int BitsPerSample, int SampleRate, int Channels)
{
        if (!hWnd) hWnd = m_hWnd;
        if (!FPS) FPS = m_FPS;
        if (!BitsPerSample) BitsPerSample = m_BitsPerSample;
        if (!SampleRate) SampleRate = m_SampleRate;
        if (!Channels) Channels = m_Channels;

        End();
        return Initialise(hWnd,FPS,BitsPerSample,SampleRate,Channels);
}

// End() - free the sound buffer and tell DirectSould it's all over.
void CSound::End(void)
{
        DXSound.End();
        if(Buffer) delete []Buffer;
}


// Initialise the AY Sound chip
void CSound::AYInit(void)
{
        int f,clock;
        double v;

        for(f=0;f<4;f++) VolumeLevel[f]=AMPL_AY_TONE;

        // logarithmic volume levels, 3dB per step
        v=AMPL_AY_TONE;
        for(f=15;f>0;f--)
        {
                AYToneLevels[f]=(unsigned char)(v+0.5);
                // 10^3/20 = 3dB
                v/=1.4125375446;
        }
        AYToneLevels[0]=0;

        AYNoiseTick=AYNoisePeriod=0;
        AYEnvTick=AYEnvPeriod=0;

        for(f=0;f<3;f++)
                AYToneTick[f]=AYTonePeriod[f]=0;

        switch(machine.aytype)
        {
        case AY_TYPE_QUICKSILVA: clock=AY_CLOCK_QUICKSILVA; break;
        case AY_TYPE_ZONX: clock=AY_CLOCK_ZONX; break;
        case AY_TYPE_FULLER: clock=AY_CLOCK_FULLER; break;
        case AY_TYPE_ACE: clock=AY_CLOCK_ACE; break;
        case AY_TYPE_SINCLAIR: clock=AY_CLOCK_SINCLAIR; break;
        case AY_TYPE_TIMEX: clock=AY_CLOCK_TIMEX; break;
        case AY_TYPE_BOLDFIELD: clock=AY_CLOCK_BOLDFIELD; break;
        default:
                //fprintf(stderr,"AY type not specified - can't happen!\n");
                //sound_ay=0;
                return;
        }

        AYTickIncr=(int)(65536.*clock/m_SampleRate);
        AYChangeCount=0;

}

// not great having this as a macro to inline it, but it's only
// a fairly short routine, and it saves messing about.
// (XXX ummm, possibly not so true any more :-))

#define AY_GET_SUBVAL(tick,period) \
  (level*2*(tick-period)/AYTickIncr)

#define AY_OVERLAY_TONE(ptr,chan,level) \
  was_high=0;								\
  if(level)								\
    {									\
    if(AYToneTick[chan]>=AYTonePeriod[chan])			\
      { (*(ptr))+=(level); was_high=1; }					\
    else								\
      (*(ptr))-=(level);						\
    }									\
  									\
  AYToneTick[chan]+=AYTickIncr;					\
  if(level && !was_high && AYToneTick[chan]>=AYTonePeriod[chan])	\
    (*(ptr))+=AY_GET_SUBVAL(AYToneTick[chan],AYTonePeriod[chan]);	\
  									\
  if(AYToneTick[chan]>=AYTonePeriod[chan]*2)			\
    {									\
    AYToneTick[chan]-=AYTonePeriod[chan]*2;				\
    /* sanity check needed to avoid making samples sound terrible */ 	\
    if(level && AYToneTick[chan]<AYTonePeriod[chan]) 		\
      (*(ptr))-=AY_GET_SUBVAL(AYToneTick[chan],0);			\
    }


void CSound::AYOverlay(void)
{

        static int rng=1;
        static int noise_toggle=1;
        static int env_level=0;
        int tone_level[3];
        int mixer,envshape;
        int f,g,level;
        int v=0;
        unsigned char *ptr;
        struct AYChangeTag *change_ptr;
        int changes_left;
        int reg,r;
        int was_high;

        change_ptr=AYChange;
        changes_left=AYChangeCount;

        // If no AY chip, don't produce any AY sound (!)
        //if(!sound_ay) return;

        // convert change times to sample offsets
        for(f=0;f<AYChangeCount;f++)
                AYChange[f].ofs=(unsigned short)((AYChange[f].tstates*m_SampleRate)/machine.clockspeed);

        for(f=0,ptr=Buffer;f<FrameSize;f++,ptr+=m_Channels)
        {
                // update ay registers. All this sub-frame change stuff
                // is pretty hairy, but how else would you handle the
                // samples in Robocop? :-) It also clears up some other
                // glitches.
                //
                // Ok, maybe that's no big deal on the ZX81, but even so. :-)
                // (Though, due to tstate-changing games in z80.c, we can
                // rarely `lose' one this way - hence "f==.." bit below
                // to catch any that slip through.)


                while(changes_left && (f>=change_ptr->ofs || f==FrameSize-1))
                {
                        reg=change_ptr->reg;
                        AYRegisters[reg]=change_ptr->val;
                        change_ptr++;
                        changes_left--;

                        // fix things as needed for some register changes
                        switch(reg)
                        {

                        case 0: case 1: case 2: case 3: case 4: case 5:
                                FineValue   = AYRegisters[reg & ~1];
                                CoarseValue = AYRegisters[reg |  1] & 15;
                                TonePeriod = (8 * (FineValue | (CoarseValue << 8))) << 16;

                                r = reg >> 1;
                                AYTonePeriod[r] = TonePeriod;

                                // important to get this right, otherwise e.g. Ghouls 'n' Ghosts
                                // has really scratchy, horrible-sounding vibrato.

                                if(AYTonePeriod[r] && AYToneTick[r]>=AYTonePeriod[r]*2)
                                        AYToneTick[r]%=AYTonePeriod[r]*2;
                                break;
                        case 6:
                                AYNoiseTick=0;
                                AYNoisePeriod=(16*(AYRegisters[reg]&31))<<16;
                                break;
                        case 11: case 12:
                                // this one *isn't* fixed-point
                                AYEnvPeriod=AYRegisters[11]|(AYRegisters[12]<<8);
                                break;

                        case 13:
                                AYEnvTick=AYEnvSubCycles=0;
                                EnvHeld=EnvAlternating=0;
                                env_level=0;
                                break;
                        }

                }

                // the tone level if no enveloping is being used
                for(g=0;g<3;g++)
                        tone_level[g]=AYToneLevels[AYRegisters[8+g]&15];

                // envelope
                envshape=AYRegisters[13];
                if(AYEnvPeriod)
                {
                        if(!EnvHeld)
                        {
                                v=((int)AYEnvTick*15)/AYEnvPeriod;
                                if(v<0) v=0;
                                if(v>15) v=15;
                                if((envshape&4)==0) v=15-v;
                                if(EnvAlternating) v=15-v;
                                env_level=AYToneLevels[v];
                        }
                }

                for(g=0;g<3;g++)
                        if(AYRegisters[8+g]&16)
                                tone_level[g]=env_level;

                if(AYEnvPeriod)
                {
                        // envelope gets incr'd every 256 AY cycles
                        AYEnvSubCycles+=AYTickIncr;
                        if(AYEnvSubCycles>=(256<<16))
                        {
                                AYEnvSubCycles-=(256<<16);

                                AYEnvTick++;
                                if(AYEnvTick>=AYEnvPeriod)
                                {
                                        AYEnvTick-=AYEnvPeriod;
                                        if(!EnvHeld && ((envshape&1) || (envshape&8)==0))
                                        {
                                                EnvHeld=1;
                                                if((envshape&2) || (envshape&0xc)==4)
                                                        env_level=AYToneLevels[15-v];
                                        }
                                        if(!EnvHeld && (envshape&2))
                                                EnvAlternating=!EnvAlternating;
                                }
                        }
                }

                // generate tone+noise
                // channel C first to make ACB easier
                mixer=AYRegisters[7];
                if((mixer&4)==0 || (mixer&0x20)==0)
                {
                        // channel C
                        level=(noise_toggle || (mixer&0x20))?tone_level[2]:0;
                        level=(level*VolumeLevel[2])/31;
                        AY_OVERLAY_TONE(ptr,2,level);
                        if(ACBMix)
                        {
                                // chan c shouldn't be full vol on both channels
                                int atv = *ptr * 3/4;
                                ptr[0]=(unsigned char)atv;
                                ptr[1]=(unsigned char)atv;
                        }
                }
                if((mixer&1)==0 || (mixer&0x08)==0)
                {
                        // channel A
                        level=(noise_toggle || (mixer&0x08))?tone_level[0]:0;
                        level=(level*VolumeLevel[1])/31;
                        AY_OVERLAY_TONE(ptr,0,level);
                }
                if((mixer&2)==0 || (mixer&0x10)==0)
                {
                        // channel B
                        level=(noise_toggle || (mixer&0x10))?tone_level[1]:0;
                        level=(level*VolumeLevel[0])/31;
                        AY_OVERLAY_TONE(ptr+(ACBMix?1:0),1,level);
                }

                if(!ACBMix)
                        ptr[1]=*ptr;

                // update noise RNG/filter
                AYNoiseTick+=AYTickIncr;
                if(AYNoiseTick>=AYNoisePeriod)
                {
                        if((rng&1)^((rng&2)?1:0))
                                noise_toggle=!noise_toggle;

                        // rng is 17-bit shift reg, bit 0 is output.
                        // input is bit 0 xor bit 2.

                        rng|=((rng&1)^((rng&4)?1:0))?0x20000:0;
                        rng>>=1;

                        AYNoiseTick-=AYNoisePeriod;
                }
        }
}


// don't make the change immediately; record it for later,
// to be made by sound_frame() (via sound_ay_overlay()).

void CSound::AYWrite(int reg, int val, int frametstates)
{

        //if(!sound_enabled || !sound_ay) return;

        AYRegisterStore[reg]=(unsigned char)val;

        // accept r15, in case of the two-I/O-port 8910
        if(reg>=16) return;
        if (reg==14) Midi.WriteBit(val);

//        if(frametstates>=0 && AYChangeCount<AY_CHANGE_MAX)
          if(AYChangeCount<AY_CHANGE_MAX)
        {
                AYChange[AYChangeCount].tstates=frametstates>0?frametstates:0;
                AYChange[AYChangeCount].reg=(unsigned char)reg;
                AYChange[AYChangeCount].val=(unsigned char)val;
                AYChangeCount++;
        }
}

int CSound::AYRead(int reg)
{

        return(AYRegisterStore[reg]);

}


// no need to call this initially, but should be called
// on reset otherwise.

void CSound::AYReset(void)
{
        int f;

        for(f=0;f<16;f++)
                AYWrite(f,0,0);

        AYOverlay();
}


// XXX currently using speccy beeper code verbatim for VSYNC.
// Not sure how plausible this is, but for now it'll do.
// It does *sound* pretty plausible.


// it should go without saying that the beeper was hardly capable of
// generating perfect square waves. :-) What seems to have happened is
// that after the `click' in the intended direction away from the rest
// (zero) position, it faded out gradually over about 1/50th of a second
// back down to zero - the bulk of the fade being over after about
// a 1/500th.

// The true behaviour is awkward to model accurately, but a compromise
// emulation (doing a linear fadeout over 1/150th) seems to work quite
// well and IMHO sounds a little more like a real speccy than most
// emulations. It also has the considerable advantage of having a zero
// rest position, which I'm a lot happier with. :-)


#define BEEPER_FADEOUT	(((1<<24)/150)/AMPL_BEEPER)

#define BEEPER_OLDVAL_ADJUST \
  BeeperTick+=BeeperTickIncr;	\
  if(BeeperTick>=BEEPER_FADEOUT)	\
    {					\
    BeeperTick-=BEEPER_FADEOUT;	\
    if(OldVal>128)		\
      OldVal--;			\
    else				\
      if(OldVal<128)		\
        OldVal++;			\
    }


void CSound::Frame(void)
{                  
        unsigned char *ptr;
        int f;

        // if(!sound_enabled) return;

        //if(zx81.vsyncsound)
        {
                ptr=Buffer+(m_Channels*FillPos);
                for(f=FillPos;f<FrameSize;f++)
                {
                        BEEPER_OLDVAL_ADJUST;
                        *ptr++=(unsigned char)OldVal;
                        if(m_Channels == 2)
                                *ptr++=(unsigned char)OldVal;
                }
        }
        //else
                // must be AY then, so `zero' buffer ready for it
        //        memset(Buffer,128,FrameSize*m_Channels);

        if (machine.aysound) AYOverlay();

        DXSound.Frame(Buffer, FrameSize*m_Channels);
        SoundOutput->UpdateImage(Buffer,FrameSize*m_Channels);

        OldPos=-1;
        FillPos=0;
        SoundPtr=Buffer;

        AYChangeCount=0;
}

void CSound::Beeper(int on, int frametstates)
{                                     
        unsigned char *ptr;
        int newpos,subpos;
        int val,subval;
        int f;

        // if(!sound_enabled) return;

        val=(on?128+VolumeLevel[3]:128-VolumeLevel[3]);

        if(val==OldValOrig) return;

        // XXX a lookup table might help here...
        newpos=(frametstates*FrameSize)/machine.tperframe;
        subpos=(frametstates*FrameSize*VolumeLevel[3])/machine.tperframe-VolumeLevel[3]*newpos;

        // if we already wrote here, adjust the level.

        if(newpos==OldPos)
        {
                // adjust it as if the rest of the sample period were all in
                // the new state. (Often it will be, but if not, we'll fix
                // it later by doing this again.)

                if(on)
                        BeeperLastSubpos+=VolumeLevel[3]-subpos;
                else
                        BeeperLastSubpos-=VolumeLevel[3]-subpos;
        }
        else
                BeeperLastSubpos=(on?VolumeLevel[3]-subpos:subpos);

        subval=128-AMPL_BEEPER+BeeperLastSubpos;

        if(newpos>=0)
        {
                //  fill gap from previous position
                ptr=Buffer+(m_Channels*FillPos);
                for(f=FillPos;f<newpos && f<FrameSize;f++)
                {
                        BEEPER_OLDVAL_ADJUST;
                        *ptr++=(unsigned char)OldVal;
                        if(m_Channels==2)
                        *ptr++=(unsigned char)OldVal;
                }

                if(newpos<FrameSize)
                {
                        // newpos may be less than FillPos, so...
                        ptr=Buffer+(m_Channels*newpos);

                        // limit subval in case of faded beeper level,
                        // to avoid slight spikes on ordinary tones.

                        if((OldVal<128 && subval<OldVal) ||
                                (OldVal>=128 && subval>OldVal))
                                        subval=OldVal;

                        // write subsample value
                        *ptr=(unsigned char)subval;
                        if(m_Channels==2)
                        *++ptr=(unsigned char)subval;
                }
        }

        OldPos=newpos;
        FillPos=newpos+1;
        OldVal=OldValOrig=val;
}



