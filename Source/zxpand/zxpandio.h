#ifndef _ZXPANDIO

#define PB_RST
#define PB_CD

#define PB_BLK0WREN

#define PB_ROMBK1
#define PB_ROMBK0

#define PB_LOW

#define PB_RESET
#define TB_RESET

#define PB_BLK0EN

#define CARDPRESENT() 1
#define RESETPRESSED() 0

#define LEDPINSOUT()

extern unsigned char LEDs;

#define REDLEDON()  LEDs|=1;
#define REDLEDOFF() LEDs&=0xfe;
#define GREENLEDON()  LEDs|=0x80;
#define GREENLEDOFF() LEDs&=0x7f;

#define ACTIVITYSTROBE(x)

#define ppCommandWaiting() false
#define ppCommand() 0
#define ppPort() 0

#define _ZXPANDIO
#endif
