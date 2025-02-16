#include <windows.h>
#include "utils.h"
#include "Joystick.h"
#include "zx81config.h"

KeyInfo JoystickLeft1  = {'O'};
KeyInfo JoystickRight1 = {'P'};
KeyInfo JoystickUp1    = {'Q'};
KeyInfo JoystickDown1  = {'A'};
KeyInfo JoystickFire1  = {'M'};

KeyInfo JoystickLeft2;
KeyInfo JoystickRight2;
KeyInfo JoystickUp2;
KeyInfo JoystickDown2;
KeyInfo JoystickFire2;

static DWORD xPosMinTrip1;
static DWORD xPosMaxTrip1;
static DWORD yPosMinTrip1;
static DWORD yPosMaxTrip1;
static DWORD zPosMinTrip1;
static DWORD zPosMaxTrip1;
static DWORD rPosMinTrip1;
static DWORD rPosMaxTrip1;
static JOYINFOEX joyInfo1;

static DWORD xPosMinTrip2;
static DWORD xPosMaxTrip2;
static DWORD yPosMinTrip2;
static DWORD yPosMaxTrip2;
static DWORD zPosMinTrip2;
static DWORD zPosMaxTrip2;
static DWORD rPosMinTrip2;
static DWORD rPosMaxTrip2;
static JOYINFOEX joyInfo2;

static int joystick1AutoFireReadCount;
static int joystick2AutoFireReadCount;

static const int JoystickReadsOn = 5;
static const int JoystickReadsOff = 5;
static const int JoystickTotalReads = JoystickReadsOn + JoystickReadsOff;

static bool joystick1Present;
static bool joystick2Present;

static int joystickId1;
static int joystickId2;

int NumberOfJoysticks()
{
        joyInfo2.dwFlags = JOY_RETURNBUTTONS;

        if (joyGetPosEx(joystickId2, &joyInfo2) == JOYERR_NOERROR)
        {
                return 2;
        }

        joyInfo1.dwFlags = JOY_RETURNBUTTONS;

        if (joyGetPosEx(joystickId1, &joyInfo1) == JOYERR_NOERROR)
        {
                return 1;
        }

        return 0;
}

void InitialiseJoysticks()
{
        JOYCAPS caps1;
        JOYCAPS caps2;
        DWORD xTripOffset;
        DWORD yTripOffset;
        DWORD zTripOffset;
        DWORD rTripOffset;

        joystick1Present = false;
        joystick2Present = false;

        if (machine.joysticksSwapped)
        {
                joystickId1 = JOYSTICKID2;
                joystickId2 = JOYSTICKID1;
        }
        else
        {
                joystickId1 = JOYSTICKID1;
                joystickId2 = JOYSTICKID2;
        }

        if (joyGetDevCaps(joystickId1, &caps1, sizeof(JOYCAPS)) == JOYERR_NOERROR)
        {
                joystick1Present = true;

                xTripOffset = (caps1.wXmax - caps1.wXmin) / 4;
                xPosMinTrip1 = caps1.wXmin + xTripOffset;
                xPosMaxTrip1 = caps1.wXmax - xTripOffset;

                yTripOffset = (caps1.wYmax - caps1.wYmin) / 4;
                yPosMinTrip1 = caps1.wYmin + yTripOffset;
                yPosMaxTrip1 = caps1.wYmax - yTripOffset;

                zTripOffset = (caps1.wZmax - caps1.wZmin) / 4;
                zPosMinTrip1 = caps1.wZmin + zTripOffset;
                zPosMaxTrip1 = caps1.wZmax - zTripOffset;

                rTripOffset = (caps1.wRmax - caps1.wRmin) / 4;
                rPosMinTrip1 = caps1.wRmin + rTripOffset;
                rPosMaxTrip1 = caps1.wRmax - rTripOffset;
        }

        if (joyGetDevCaps(joystickId2, &caps2, sizeof(JOYCAPS)) == JOYERR_NOERROR)
        {
                joystick2Present = true;

                xTripOffset = (caps2.wXmax - caps2.wXmin) / 4;
                xPosMinTrip2 = caps2.wXmin + xTripOffset;
                xPosMaxTrip2 = caps2.wXmax - xTripOffset;

                yTripOffset = (caps2.wYmax - caps2.wYmin) / 4;
                yPosMinTrip2 = caps2.wYmin + yTripOffset;
                yPosMaxTrip2 = caps2.wYmax - yTripOffset;

                zTripOffset = (caps2.wZmax - caps2.wZmin) / 4;
                zPosMinTrip2 = caps2.wZmin + zTripOffset;
                zPosMaxTrip2 = caps2.wZmax - zTripOffset;

                rTripOffset = (caps2.wRmax - caps2.wRmin) / 4;
                rPosMinTrip2 = caps2.wRmin + rTripOffset;
                rPosMaxTrip2 = caps2.wRmax - rTripOffset;
        }

        joyInfo1.dwSize = sizeof(joyInfo1);
        joyInfo2.dwSize = sizeof(joyInfo2);

        joystick1AutoFireReadCount = 0;
        joystick2AutoFireReadCount = JoystickReadsOn; //Ensure the two reads do not collide to allow reading from BASIC
}               

static BYTE ReadJoystick(bool readFireButton)
{
        BYTE result = 0xFF;

        joyInfo1.dwFlags = JOY_RETURNBUTTONS | JOY_RETURNX | JOY_RETURNY | JOY_RETURNZ | JOY_RETURNR;

        if (joystick1Present && joyGetPosEx(joystickId1, &joyInfo1) == JOYERR_NOERROR)
        {
                if (joyInfo1.dwRpos <= rPosMinTrip1) result &= JoystickUp1.Data;
                if (joyInfo1.dwRpos >= rPosMaxTrip1) result &= JoystickDown1.Data;
                if (joyInfo1.dwZpos <= zPosMinTrip1) result &= JoystickLeft1.Data;
                if (joyInfo1.dwZpos >= zPosMaxTrip1) result &= JoystickRight1.Data;

                if (joyInfo1.dwYpos <= yPosMinTrip1) result &= JoystickUp1.Data;
                if (joyInfo1.dwYpos >= yPosMaxTrip1) result &= JoystickDown1.Data;
                if (joyInfo1.dwXpos <= xPosMinTrip1) result &= JoystickLeft1.Data;
                if (joyInfo1.dwXpos >= xPosMaxTrip1) result &= JoystickRight1.Data;

                if ((joyInfo1.dwButtons & 0x03FF) || readFireButton) result &= JoystickFire1.Data;
        }

        if (emulator.UseNumericPadForJoystick && (GetKeyState(VK_NUMLOCK) & 0x0001) == 0x0001)
        {
                if (IsKeyPressed(VK_NUMPAD8)) result &= JoystickUp1.Data;
                if (IsKeyPressed(VK_NUMPAD2)) result &= JoystickDown1.Data;
                if (IsKeyPressed(VK_NUMPAD4)) result &= JoystickLeft1.Data;
                if (IsKeyPressed(VK_NUMPAD6)) result &= JoystickRight1.Data;
                if ((IsKeyPressed(VK_NUMPAD0)) || readFireButton) result &= JoystickFire1.Data;
        }

        return result;
}

BYTE ReadJoystick1()
{
        bool readFireButton = (machine.joystick1AutoFireEnabled && (joystick1AutoFireReadCount < JoystickReadsOn));
        BYTE result = ReadJoystick(readFireButton);

        joystick1AutoFireReadCount++;
        if (joystick1AutoFireReadCount == JoystickTotalReads) joystick1AutoFireReadCount = 0;

        return result;
}

BYTE ReadJoystick2()
{
        BYTE result = 0xFF;

        if (machine.joystick1Connected || joystick2Present)
        {
                bool readFireButton = (machine.joystick2AutoFireEnabled && (joystick2AutoFireReadCount < JoystickReadsOn));

                joyInfo2.dwFlags = JOY_RETURNBUTTONS | JOY_RETURNX | JOY_RETURNY | JOY_RETURNZ | JOY_RETURNR;

                if (joystick2Present && joyGetPosEx(joystickId2, &joyInfo2) == JOYERR_NOERROR)
                {
                        if (joyInfo2.dwRpos <= rPosMinTrip2) result &= JoystickUp2.Data;
                        if (joyInfo2.dwRpos >= rPosMaxTrip2) result &= JoystickDown2.Data;
                        if (joyInfo2.dwZpos <= zPosMinTrip2) result &= JoystickLeft2.Data;
                        if (joyInfo2.dwZpos >= zPosMaxTrip2) result &= JoystickRight2.Data;

                        if (joyInfo2.dwYpos <= yPosMinTrip2) result &= JoystickUp2.Data;
                        if (joyInfo2.dwYpos >= yPosMaxTrip2) result &= JoystickDown2.Data;
                        if (joyInfo2.dwXpos <= xPosMinTrip2) result &= JoystickLeft2.Data;
                        if (joyInfo2.dwXpos >= xPosMaxTrip2) result &= JoystickRight2.Data;

                        if ((joyInfo2.dwButtons & 0x03FF) || readFireButton) result &= JoystickFire2.Data;
                }
        }
        else
        {
                bool readFireButton = (machine.joystick2AutoFireEnabled && (joystick2AutoFireReadCount < JoystickReadsOn));
                result = ReadJoystick(readFireButton);
        }

        joystick2AutoFireReadCount++;
        if (joystick2AutoFireReadCount == JoystickTotalReads) joystick2AutoFireReadCount = 0;

        return result;
}

BYTE ReadJoystick1_Left()
{
        BYTE result = 0xFF;

        joyInfo1.dwFlags = JOY_RETURNX | JOY_RETURNZ;

        if (joystick1Present && joyGetPosEx(joystickId1, &joyInfo1) == JOYERR_NOERROR)
        {
                if (joyInfo1.dwZpos <= zPosMinTrip1) result &= JoystickUp1.Data;
                if (joyInfo1.dwXpos <= xPosMinTrip1) result &= JoystickLeft1.Data;
        }

        if (IsKeyPressed(VK_NUMPAD4)) result &= JoystickLeft1.Data;

        return result;
}

BYTE ReadJoystick1_Right()
{
        BYTE result = 0xFF;

        joyInfo1.dwFlags = JOY_RETURNX | JOY_RETURNZ;

        if (joystick1Present && joyGetPosEx(joystickId1, &joyInfo1) == JOYERR_NOERROR)
        {
                if (joyInfo1.dwZpos >= zPosMaxTrip1) result &= JoystickRight1.Data;
                if (joyInfo1.dwXpos >= xPosMaxTrip1) result &= JoystickRight1.Data;
        }

        if (IsKeyPressed(VK_NUMPAD6)) result &= JoystickRight1.Data;

        return result;
}

BYTE ReadJoystick1_Up()
{
        BYTE result = 0xFF;

        joyInfo1.dwFlags = JOY_RETURNY | JOY_RETURNR;

        if (joystick1Present && joyGetPosEx(joystickId1, &joyInfo1) == JOYERR_NOERROR)
        {
                if (joyInfo1.dwRpos <= rPosMinTrip1) result &= JoystickUp1.Data;
                if (joyInfo1.dwYpos <= yPosMinTrip1) result &= JoystickUp1.Data;
        }

        if (IsKeyPressed(VK_NUMPAD8)) result &= JoystickUp1.Data;

        return result;
}

BYTE ReadJoystick1_Down()
{
        BYTE result = 0xFF;

        joyInfo1.dwFlags = JOY_RETURNY | JOY_RETURNR;

        if (joystick1Present && joyGetPosEx(joystickId1, &joyInfo1) == JOYERR_NOERROR)
        {
                if (joyInfo1.dwRpos >= rPosMaxTrip1) result &= JoystickDown1.Data;
                if (joyInfo1.dwYpos >= yPosMaxTrip1) result &= JoystickDown1.Data;
        }

        if (IsKeyPressed(VK_NUMPAD2)) result &= JoystickDown1.Data;

        return result;
}

BYTE ReadJoystick1_Fire()
{
        BYTE result = 0xFF;

        bool readFireButton = (machine.joystick1AutoFireEnabled && (joystick1AutoFireReadCount < JoystickReadsOn));

        joyInfo1.dwFlags = JOY_RETURNBUTTONS;

        if (joystick1Present && joyGetPosEx(joystickId1, &joyInfo1) == JOYERR_NOERROR)
        {
                if ((joyInfo1.dwButtons & 0x03FF) || readFireButton) result &= JoystickFire1.Data;
        }

        if (IsKeyPressed(VK_NUMPAD0) || readFireButton) result &= JoystickFire1.Data;

        joystick1AutoFireReadCount++;
        if (joystick1AutoFireReadCount == JoystickTotalReads) joystick1AutoFireReadCount = 0;

        return result;
}

BYTE ReadJoystick1_RightUpDownFire()
{
        BYTE result = 0xFF;

        bool readFireButton = (machine.joystick1AutoFireEnabled && (joystick1AutoFireReadCount < JoystickReadsOn));

        joyInfo1.dwFlags = JOY_RETURNBUTTONS | JOY_RETURNX | JOY_RETURNY | JOY_RETURNR | JOY_RETURNZ;

        if (joystick1Present && joyGetPosEx(joystickId1, &joyInfo1) == JOYERR_NOERROR)
        {
                if (joyInfo1.dwRpos <= rPosMinTrip1) result &= JoystickUp1.Data;
                if (joyInfo1.dwRpos >= rPosMaxTrip1) result &= JoystickDown1.Data;
                if (joyInfo1.dwZpos >= zPosMaxTrip1) result &= JoystickRight1.Data;

                if (joyInfo1.dwYpos <= yPosMinTrip1) result &= JoystickUp1.Data;
                if (joyInfo1.dwYpos >= yPosMaxTrip1) result &= JoystickDown1.Data;
                if (joyInfo1.dwXpos >= xPosMaxTrip1) result &= JoystickRight1.Data;

                if ((joyInfo1.dwButtons & 0x03FF) || readFireButton) result &= JoystickFire1.Data;
        }

        if (IsKeyPressed(VK_NUMPAD8)) result &= JoystickUp1.Data;
        if (IsKeyPressed(VK_NUMPAD2)) result &= JoystickDown1.Data;
        if (IsKeyPressed(VK_NUMPAD6)) result &= JoystickRight1.Data;
        if ((IsKeyPressed(VK_NUMPAD0)) || readFireButton) result &= JoystickFire1.Data;

        joystick1AutoFireReadCount++;
        if (joystick1AutoFireReadCount == JoystickTotalReads) joystick1AutoFireReadCount = 0;

        return result;
}

