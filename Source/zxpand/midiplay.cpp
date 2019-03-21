// based on https://community.atmel.com/projects/sd-card-midi-player

// Controller
#define MF_Bank_Select_MSB    0x00	// 0x00 .Bank Select MSB (value 0x50 : Preset A Patch 1..128, 0x51 Preset B Patch 129..255)
#define MF_Bank_Select_LSB    0x20	// 0x20 .Bank Select LSB (value 0x00)
#define MF_Modulation         0x01	// 0x01 .Modulation
#define MF_Breath             0x02	// 0x02  Breath Controller
#define MF_Foot               0x04	// 0x04  Foot Controller
#define MF_Portamento_Time    0x05	// 0x05 .Portamento Time
#define MF_Main_Volume        0x07	// 0x07 .Main Volume
#define MF_Balance            0x08	// 0x08  Balance
#define MF_Pan                0x0A	// 0x0A .Pan
#define MF_Expression         0x0B	// 0x0B .Expression Controller
#define MF_Effect_1           0x0C	// 0x0C  Effect Control 1
#define MF_Effect_2           0x0D	// 0x0D  Effect Control 2
#define MF_General_1to4       0x13	// 0x13  General-Purpose Controllers 1-4
#define MF_Controller_LSB     0x3F	// 0x3F  LSB for controllers 0-31
#define MF_Sustain            0x40	// 0x40 .Sustain(Damper pedal / Hold 1)
#define MF_Portamento         0x41	// 0x41 .Portamento
#define MF_Sostenuto          0x42	// 0x42 .Sostenuto
#define MF_Soft               0x43	// 0x43 .Soft Pedal
#define MF_Legato             0x44	// 0x44  Legato Footswitch
#define MF_Hold               0x45	// 0x45  Hold 2
#define MF_Control_1          0x46	// 0x46  Sound Controller 1 (default: Timber Variation)
#define MF_Control_2          0x47	// 0x47  Sound Controller 2 (default: Timber/Harmonic Content)
#define MF_Control_3          0x48	// 0x48  Sound Controller 3 (default: Release Time)
#define MF_Control_4          0x49	// 0x49  Sound Controller 4 (default: Attack Time)
#define MF_Portamento_Ctrl    0x54	// 0x54  Portamento Control
#define MF_Reverb             0x5B	// 0x5B .Effects 1 Depth (M-GS64 : Reverb send level)
#define MF_Effects_2          0x5C	// 0x5C  Effects 2 Depth (formerly Tremolo Depth)
#define MF_Chorus             0x5D	// 0x5D .Effects 3 Depth (M-GS64 : Chorus send level)
#define MF_Effects_4          0x5E	// 0x5E  Effects 4 Depth (formerly Celeste Detune)
#define MF_Effects_5          0x5F	// 0x5F  Effects 5 Depth (formerly Phaser Depth)
#define MF_Data_Increment     0x60	// 0x60  Data Increment
#define MF_Data_Decrement     0x61	// 0x61  Data Decrement
#define MF_NRPN_LSB           0x62	// 0x62 .Non-Registered Parameter Number (LSB)
#define MF_NRPN_MSB           0x63	// 0x63 .Non-Registered Parameter Number (MSB)
#define MF_RPN_LSB            0x64	// 0x64 .Registered Parameter Number (LSB)
#define MF_RPN_MSB            0x65	// 0x65 .Registered Parameter Number (MSB)
#define MF_Mode_Message       0x7F	// 0x7F  Mode Messages
#define MF_Data_Entry_MSB     0x06	// 0x06  Data Entry (MSB)
#define MF_Data_Entry_LSB     0x26	// 0x26  Data Entry (LSB)

// MIDI File Formats
#define MF_Single_track       0x00
#define MF_Parallel_tracks    0x01
#define MF_Sequential_tracks  0x02

// Meta Events Type
#define MF_Meta_Sequence         0x00  // Sequence number
#define MF_Meta_Text             0x01  // Text event
#define MF_Meta_Copyright        0x02  // Copyright
#define MF_Meta_Track_name       0x03  // track name
#define MF_Meta_Instrument_name  0x04  // Instrument name
#define MF_Meta_Lyric            0x05  // Lyric text
#define MF_Meta_Marker           0x06  // Marker text
#define MF_Meta_Cue_point        0x07  // Cue point
#define MF_Meta_MIDI_channel     0x20  // MIDI channel
#define MF_Meta_MIDI_Port        0x21  // MIDI Port
#define MF_Meta_Track_End        0x2F  // End of track
#define MF_Meta_Tempo            0x51  // tempo setting
#define MF_Meta_SMPTE_offset     0x54  // SMPTE offset
#define MF_Meta_Time_signature   0x58  // Time signature
#define MF_Meta_Key_signature    0x59  // Key signature
#define MF_Meta_Special          0x7F  // Seq. special

#include "zxpandio.h"

typedef unsigned char uint8_t;
typedef signed char int8_t;

typedef unsigned short uint16_t;
typedef signed short int16_t;

typedef unsigned int uint32_t;
typedef signed int int32_t;


extern int16_t globalAmount;
extern char globalData[512];
extern void comFileRead(void);

extern void serialInit(int brg, int withRxIrq);
extern void serialWrite(uint8_t b);

extern void delayMillis(short);

uint8_t* sdData = (uint8_t*)globalData;


// FILE header INFORMATION
typedef struct
{
  uint8_t  chk[5];
  uint32_t length;
  uint16_t format;
  uint16_t ntracks;
  uint16_t division;
} MTHD;

// TRACK INFORMATION
typedef struct
{
  uint8_t  chk[5];
  uint32_t length;
} MTRK;

// EVENT INFORMATION
#define maxdata 127
typedef struct
{
  uint32_t wait;
  uint8_t  event;
  uint8_t  mtype; // only for Meta Events
  uint32_t nbdata;
  uint8_t  data[maxdata];
} MTEV;

// RETURN CODES
enum MIDIerrors
{
  NoError        = 0,
  badFileheader  = 1,
  badTrackheader = 2,
  badEvent       = 3,
  endOfFile      = 4,
  userStop       = 5
};

// FUNCTIONS
void     clearBuffer(void);
uint8_t  readByte(void);
uint8_t  readTrackByte(void);
uint8_t  readHeaderChunk(void);
uint8_t  readTrackChunk(void);
uint16_t read16(void);
uint32_t read32(void);
uint32_t readVariableLength(void);
uint8_t  readNdata(uint8_t start);
uint8_t  readTrackEvent(void);
void     allSoundOff(void);

// Position in track
uint32_t tpos     = 0;
uint32_t prevpos  = 0;

// LAST EVENT READ
uint8_t runningEvent = 0;

// TEMPO (microsec/beat)
uint32_t tempo = 500000;

// Shared variables
MTHD midiheader;
MTRK miditrack;
MTEV midievent;

uint32_t millis, nextTime = 0;
int sdDatIdx = 255;

// length is tracked by midi reader so we don't need to do it here
//
uint8_t SDgetc(void)
{
  ++sdDatIdx;
  sdDatIdx &= 255;
  if (sdDatIdx == 0) {
    globalAmount = 256;
    comFileRead();
  }

  return sdData[sdDatIdx];
}

void MIDIinit(void)
{
  serialInit(15, 0);
}


union {
  uint8_t b[4];
  uint32_t l;
  uint16_t w;
} b2w;

// Read a 16 bits integer
uint16_t read16(void)
{
  b2w.b[1] = SDgetc();
  b2w.b[0] = SDgetc();
  return b2w.w;
}


// Read a 32 bits integer
uint32_t read32(void)
{
  b2w.b[3] = SDgetc();
  b2w.b[2] = SDgetc();
  b2w.b[1] = SDgetc();
  b2w.b[0] = SDgetc();
  return b2w.l;
}


// Read a byte but stops if size of the track is excessed
uint8_t readTrackByte(void)
{
  uint8_t c = 0;
  if (tpos < miditrack.length)
  {
    c = SDgetc();
    tpos++;
  }
  return c;
}


// Read a MIDI "variable length" integer
uint32_t readVariableLength()
{
  uint32_t v = 0;
  uint8_t c;
  c = readTrackByte();
  v = c & 0x7F;
  while (c & 0x80)
  {
    c = readTrackByte();
    v = (v << 7) | (c & 0x7F);
  }
  return v;
}


// Read "midievent.nbdata" bytes in "midievent.data[]" starting at "midievent.data[start]"
// midievent.nbdata is not limited but we will store only "maxdata" and discard extra data
uint8_t readNdata(uint8_t start)
{
  uint8_t c;
  uint32_t i;
  for (i=start; i<midievent.nbdata; i++)
  {
    c = readTrackByte();
    if (i < maxdata) midievent.data[i] = c;
  }
  return 0;
}


// Send "All Sound Off" message to MIDI out
void allSoundOff(void)
{
  uint8_t i;
  for (i = 0; i < 16; i++)
  {
    serialWrite(0xB0 | i);  // command: Channel Mode Message
    serialWrite(0x78);      // data1:   All sounds Off : 0x78=120
    serialWrite(0x00);      // data2:   "0"
    serialWrite(0xB0 | i);  // command: Channel Mode Message
    serialWrite(0x7B);      // data1:   All Notes  Off : 0x7B=123
    serialWrite(0x00);      // data2:   "0"
  }
}


// Read MIDI file header Chunk
uint8_t readHeaderChunk(void)
{
  int i;
  for (i = 0; i < 4; i++) midiheader.chk[i] = SDgetc();
  midiheader.length = read32();

  midiheader.format   = read16();
  midiheader.ntracks  = read16();
  midiheader.division = read16();

  tempo = 500000; // Default tempo : 500000 microsec / beat

  return midiheader.chk[0]=='M' && midiheader.chk[1]=='T' && midiheader.chk[2]=='h' && midiheader.chk[3]=='d' && midiheader.length == 6 ? NoError : badFileheader;
}


// Read MIDI file track Chunk
uint8_t readTrackChunk(void)
{
  int i;
  for (i = 0; i < 4; i++) miditrack.chk[i] = SDgetc();
  miditrack.length  = read32();
  return miditrack.chk[0]=='M' && miditrack.chk[1]=='T' && miditrack.chk[2]=='r' && miditrack.chk[3]=='k' ? NoError : badTrackheader;
}


// Read MIDI file track event
uint8_t readTrackEvent(void)
{
  uint8_t c;
  uint32_t ms;
  // Read time
  midievent.wait = readVariableLength();
  // Read track event
  midievent.event = readTrackByte();
  if (midievent.event == 0xFF)
  {
    // Meta event
    // read Meta event type
    midievent.mtype = readTrackByte();

    // read data length
    midievent.nbdata = readVariableLength();
    // read data
    readNdata(0);
    if (midievent.mtype == MF_Meta_Tempo) // tempo
    {
      tempo = midievent.data[0] * 65536 + midievent.data[1] * 256 + midievent.data[2];
   }
  }
  else if (midievent.event == 0xF0 || midievent.event == 0xF7)
  {
    // SysEx event
    midievent.nbdata = 0;
    do
    {
      // read one byte
      c = readTrackByte();
      if (midievent.nbdata < maxdata) midievent.data[midievent.nbdata++] = c;
    } while (c != 0xF7 && tpos < miditrack.length);
  }
  else if (midievent.event & 0x80)
  {
    // Midi event
    // calculate the number of data bytes
    midievent.nbdata = ((midievent.event & 0xE0) == 0xC0 ? 1 : 2);
    // Read data bytes
    readNdata(0);
  }
  else
  {
    // Running event
    // transfer first byte from event to data
    midievent.data[0] = midievent.event;
    // recall last event value
    midievent.event = runningEvent;
    // calculate the number of data bytes
    midievent.nbdata = ((runningEvent & 0xE0) == 0xC0 ? 1 : 2);
    // Read data bytes (starting from the second one since the first byte is alread in data)
    readNdata(1);
  }

  // Calculate next time on which data shall be played
  ms = (midievent.wait * tempo) / midiheader.division / 1000;
  nextTime += ms;

  // Output to MIDI device
  if (midievent.event != 0xFF)
  {
    if (nextTime > millis) {
      delayMillis(nextTime - millis);
      millis = nextTime;
    }

    serialWrite(midievent.event);

    {
      uint32_t i;
      for (i = 0; i < midievent.nbdata && i<maxdata; i++)
      {
        serialWrite(midievent.data[i]);
      }
    }
  }

  return NoError;
}


// Read MIDI file (main part)
void playMidiFile(int(*stopFunc)(void))
{
  uint16_t i;
  uint8_t err;

  // Setup MIDI device
  MIDIinit();

  // Read File header Chunk
  err = readHeaderChunk();

  // Read succesive Tracks
  for (i=1; i<=midiheader.ntracks && !err; i++)
  {
    // Read track header Chunk
    err = readTrackChunk();

    // Read succesive Events
    for (tpos=0; tpos < miditrack.length && !err;) 
    {
        err = readTrackEvent();
        err |= stopFunc();
    }
  }
  allSoundOff();
}
