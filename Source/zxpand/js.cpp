#include <windows.h>

static JOYCAPS caps;

void initJoy(void)
{
   // don't bother with error checking;
   // if this fails the joyGetPos call will fail too.
   //
   joyGetDevCaps(0, &caps, sizeof(JOYCAPS));
}

unsigned char readJoy(void)
{
   JOYINFO joyInfo;
   unsigned char joypos = 0xfe;

   if (JOYERR_NOERROR == joyGetPos(0, &joyInfo))
   {
      if (joyInfo.wYpos == caps.wYmin) joypos &= 0x7f;
      if (joyInfo.wYpos == caps.wYmax) joypos &= 0xbf;
      if (joyInfo.wXpos == caps.wXmin) joypos &= 0xdf;
      if (joyInfo.wXpos == caps.wXmax) joypos &= 0xef;
      if (joyInfo.wButtons & JOY_BUTTON1) joypos &= 0xf7;
   }

   if (GetKeyState(VK_NUMPAD8) & 0x8000)
   {
      joypos &= 0x7f;
   }
   else if (GetKeyState(VK_NUMPAD2) & 0x8000)
   {
      joypos &= 0xbf;
   }

   if (GetKeyState(VK_NUMPAD4) & 0x8000)
   {
      joypos &= 0xdf;
   }
   else if (GetKeyState(VK_NUMPAD6) & 0x8000)
   {
      joypos &= 0xef;
   }

   if (GetKeyState(VK_NUMPAD0) & 0x8000)
   {
      joypos &= 0xf7;
   }

   return joypos;
}