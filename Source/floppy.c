#if __CODEGEARC__ < 0x0620
#pragma warn -8080
#endif

#include <stdlib.h>
#include <mem.h>
#include <string.h>
#include <io.h>
#include <fcntl.h>
#include <windows.h>
#include <errno.h>

#include "floppy.h"
#include "libdsk/config.h"
#include "libdsk.h"
#include "765.h"
#include "wd1770.h"
#include "zx81config.h"
#include "parallel.h"
#include "z80.h"

extern void fdl_setfilename(FDRV_PTR fd, const char *s);
extern FDRV_PTR fd_newldsk(void);

FDC_PTR p3_fdc=NULL;
FDRV_PTR p3_drive_a=NULL;
FDRV_PTR p3_drive_b=NULL;
FDRV_PTR p3_drive_null=NULL;
wd1770_drive PlusDDrives[2], *PlusDCur;

#define LARKENSIZE (80*1984)
unsigned char LarkenDrive[LARKENSIZE*2];
_TCHAR LarkenPath0[MAXPATH], LarkenPath1[MAXPATH];

#include "larhead.h"

extern int USEFDC765DLL;
HANDLE DLLHandle;

typedef void _stdcall (*u765_InitialiseT)(void);
u765_InitialiseT u765_Initialise;
typedef void _stdcall (*u765_InsertDiskT)(LPCSTR lpFileName, BYTE unit);
u765_InsertDiskT u765_InsertDisk;
typedef void _stdcall (*u765_EjectDiskT)(BYTE unit);
u765_EjectDiskT u765_EjectDisk;
typedef void _stdcall (*u765_SetMotorStateT)(BYTE state);
u765_SetMotorStateT u765_SetMotorState;
typedef BYTE _stdcall (*u765_StatusPortReadT)(void);
u765_StatusPortReadT u765_StatusPortRead;
typedef BYTE _stdcall (*u765_DataPortReadT)(void);
u765_DataPortReadT u765_DataPortRead;
typedef void _stdcall (*u765_DataPortWriteT)(BYTE data);
u765_DataPortWriteT u765_DataPortWrite;
typedef void _stdcall (*u765_SetActiveCallbackT)(DWORD lpCallback);
u765_SetActiveCallbackT u765_SetActiveCallback;
typedef void _stdcall (*u765_ShutdownT)(void);
u765_ShutdownT u765_Shutdown;
typedef DWORD _stdcall (*u765_DiskInsertedT)(BYTE unit);
u765_DiskInsertedT u765_DiskInserted;
typedef DWORD _stdcall (*u765_SetRandomMethodT)(BYTE method);
u765_SetRandomMethodT u765_SetRandomMethod;

void LoadFDC765DLL(void)
{
        USEFDC765DLL=1;
        if ((DLLHandle=LoadLibrary(_TEXT("fdc765"))) != 0) { USEFDC765DLL=0; return; }
        if ((u765_Initialise=(u765_InitialiseT)GetProcAddress(DLLHandle,"u765_Initialise")) == 0) USEFDC765DLL=0;
        if ((u765_InsertDisk=(u765_InsertDiskT)GetProcAddress(DLLHandle,"u765_InsertDisk")) == 0) USEFDC765DLL=0;
        if ((u765_EjectDisk=(u765_EjectDiskT)GetProcAddress(DLLHandle,"u765_EjectDisk")) == 0) USEFDC765DLL=0;
        if ((u765_SetMotorState=(u765_SetMotorStateT)GetProcAddress(DLLHandle,"u765_SetMotorState")) == 0) USEFDC765DLL=0;
        if ((u765_StatusPortRead=(u765_StatusPortReadT)GetProcAddress(DLLHandle,"u765_StatusPortRead")) == 0) USEFDC765DLL=0;
        if ((u765_DataPortRead=(u765_DataPortReadT)GetProcAddress(DLLHandle,"u765_DataPortRead")) == 0) USEFDC765DLL=0;
        if ((u765_DataPortWrite=(u765_DataPortWriteT)GetProcAddress(DLLHandle,"u765_DataPortWrite")) == 0) USEFDC765DLL=0;
        if ((u765_SetActiveCallback=(u765_SetActiveCallbackT)GetProcAddress(DLLHandle,"u765_SetActiveCallback")) == 0) USEFDC765DLL=0;
        if ((u765_Shutdown=(u765_ShutdownT)GetProcAddress(DLLHandle,"u765_Shutdown")) == 0) USEFDC765DLL=0;
        if ((u765_DiskInserted=(u765_DiskInsertedT)GetProcAddress(DLLHandle,"u765_DiskInserted")) == 0) USEFDC765DLL=0;
        if ((u765_SetRandomMethod=(u765_SetRandomMethodT)GetProcAddress(DLLHandle,"u765_SetRandomMethod")) == 0) USEFDC765DLL=0;

        if (USEFDC765DLL) u765_Initialise();
        else if (DLLHandle) FreeLibrary(DLLHandle);
}

#define NMIREADTICKER 80
#define NMIWRITETICKER 70

void OpusNMI( wd1770_drive *d )
{
        nmiOccurred = 1;
}

//
// OpusD6821Access(Reg, Data, Dir)
//
// Reg - Register to access:
//
// Data - If Dir=1, The value being written else ignored.
//
// Dir - Direction of Data.  0=read, 1=write;
//
// Returns:  value of register if Dir=0, else 0;
static BYTE Data_Reg_A=0, Data_Dir_A=0, Control_A=0;
static BYTE Data_Reg_B=0, Data_Dir_B=0, Control_B=0;

BYTE OpusD6821Access(BYTE reg, BYTE Data, BYTE Dir)
{

        switch(reg&3)
        {
        case 0:
                if (Dir)
                {
                        if (Control_A&4)
                        {
                                Data_Reg_A=Data;
                                floppy_set_motor(Data);
                       }
                        else
                                Data_Dir_A=Data;
                }
                else
                {
                        if (Control_A&4)
                        {
                                Data_Reg_A &= ~64;
                                Data_Reg_A |= (BYTE)(((~PrinterBusy())&64)<<6);
                                return(Data_Reg_A);
                        }
                        else
                                return(Data_Dir_A);
                }
                break;
        case 1:
                if (Dir)
                        Control_A=Data;
                else
                        return(Control_A);
                break;
        case 2:
                if (Dir)
                {
                        if (Control_B&4)
                        {
                                Data_Reg_B=Data;
                                PrinterWriteData(Data);
                                PrinterSetStrobe(0);
                                PrinterSetStrobe(1);
                                PrinterSetStrobe(0);
                        }
                        else
                                Data_Dir_B=Data;
                }
                else
                {
                        if (Control_B&4)
                                return(Data_Reg_B);
                        else
                                return(Data_Dir_B);
                }
                break;
        case 3:
                if (Dir)
                        Control_B=Data;
                else
                        return(Control_B);
                break;
        }

        return(0);
}

BYTE floppy_get_state(void)
{
        BYTE b=0;

        if (!PlusDCur->cmdint) b |= 128;
        if (!PlusDCur->datarq) b |= 64;

        return(b);
}

void floppy_set_motor(BYTE Data)
{
        switch(spectrum.floppytype)
        {
        case FLOPPYPLUS3:
                if (USEFDC765DLL) u765_SetMotorState(Data);
                else if (p3_fdc) fdc_set_motor(p3_fdc, (fdc_byte)((Data&8) ? 15:0));
                break;

        case FLOPPYDISCIPLE:
                PlusDCur=&PlusDDrives[1-(Data&1)];
                PlusDCur->side=(Data&2)>>1;
                PrinterSetStrobe((unsigned char)(Data&64));
                break;

        case FLOPPYPLUSD:
                switch(Data&2)
                {
                case 2:
                        PlusDCur=&PlusDDrives[1];
                        break;

                default:
                case 0:
                case 1:
                case 3:
                        PlusDCur=&PlusDDrives[0];
                }

                PlusDCur->side=(Data&128)>>7;
                PrinterSetStrobe((unsigned char)(Data&64));
                break;

        case FLOPPYOPUSD:
                if (PlusDCur->state == wd1770_state_read) PlusDCur->state = wd1770_state_none;
                switch(Data&3)
                {
                case 0:
                case 1:
                        PlusDCur=&PlusDDrives[0];
                        break;
                case 2:
                case 3:
                        PlusDCur=&PlusDDrives[1];
                        break;
                }
                PlusDCur->side=(Data&16)>>4;
                break;

        case FLOPPYBETA:
                PlusDCur = &PlusDDrives[(Data&1)];
                PlusDCur->side=(Data&16)>>4;
                break;
        }
}

void floppy_write_cmdreg(BYTE Data)
{
        wd1770_cr_write(PlusDCur, Data);
}

void floppy_write_trackreg(BYTE Data)
{
        wd1770_tr_write(PlusDCur, Data);
}

void floppy_write_secreg(BYTE Data)
{
        if (spectrum.floppytype==FLOPPYOPUSD) wd1770_sec_write(PlusDCur, (BYTE)(Data+1));
        else wd1770_sec_write(PlusDCur, Data);
}

void floppy_write_datareg(BYTE Data)
{
        switch(spectrum.floppytype)
        {
        case FLOPPYPLUS3:
                if (USEFDC765DLL) u765_DataPortWrite(Data);
                else if (p3_fdc) fdc_write_data(p3_fdc, Data);
                break;

        case FLOPPYDISCIPLE:
        case FLOPPYPLUSD:
        case FLOPPYOPUSD:
        case FLOPPYBETA:
                wd1770_dr_write(PlusDCur, Data);
                break;
        }
}

BYTE floppy_read_datareg(void)
{
        switch(spectrum.floppytype)
        {
        case FLOPPYPLUS3:
                if (USEFDC765DLL) return(u765_DataPortRead());
                else if (p3_fdc) return(fdc_read_data(p3_fdc));

        case FLOPPYDISCIPLE:
        case FLOPPYPLUSD:
        case FLOPPYOPUSD:
        case FLOPPYBETA:
                return(wd1770_dr_read(PlusDCur));

        default:
                return(255);
        }
}

BYTE floppy_read_statusreg(void)
{
        switch(spectrum.floppytype)
        {
        case FLOPPYPLUS3:
                if (USEFDC765DLL) return(u765_StatusPortRead());
                else if (p3_fdc) return(fdc_read_ctrl(p3_fdc));

        case FLOPPYDISCIPLE:
        case FLOPPYPLUSD:
        case FLOPPYOPUSD:
        case FLOPPYBETA:
                return(wd1770_sr_read(PlusDCur));

        default:
                return(255);
        }

}
BYTE floppy_read_trackreg(void)
{
        return(wd1770_tr_read(PlusDCur));
}

BYTE floppy_read_secreg(void)
{
        if (spectrum.floppytype==FLOPPYOPUSD) return (BYTE)(wd1770_sec_read(PlusDCur)-1);
        else return(wd1770_sec_read(PlusDCur));
}

void floppy_ClockTick(int ts)
{
        static int index_pulse=0;
        static int counter=190;
        static int NMICount=NMIREADTICKER;

        //int a=WD1770_SR_BUSY;

        if (spectrum.floppytype==FLOPPYPLUS3 && !USEFDC765DLL)
        {
                if (p3_fdc) fdc_tick(p3_fdc);
        }

        if (spectrum.floppytype==FLOPPYPLUSD
                || spectrum.floppytype==FLOPPYDISCIPLE
                || spectrum.floppytype==FLOPPYOPUSD
                || spectrum.floppytype==FLOPPYBETA)
        {
                int i;

                if (PlusDCur->busy_counter != -99999)
                {
                        if (PlusDCur->busy_counter > 0)
                                PlusDCur->busy_counter-=ts;
                        else
                        {
                                PlusDCur->status_register &= ~1;
                                wd1770_reset_cmdint(PlusDCur);
                                PlusDCur->busy_counter=-99999;
                        }
                }


                if (PlusDCur->state == wd1770_state_read
                        || PlusDCur->state == wd1770_state_write
                        || PlusDCur->state == wd1770_state_readid)
                {
                        NMICount -= ts;
                        if (NMICount<0)
                        {
                                wd1770_set_datarq(PlusDCur);
                                NMICount+=(PlusDCur->state == wd1770_state_read)? NMIREADTICKER
                                                                        : NMIWRITETICKER;
                        }
                }
                else    NMICount=NMIWRITETICKER;


                counter -= ts;
                while(counter<0)
                {
                        index_pulse = !index_pulse;

                        for( i = 0; i < 2; i++ )
                        {
                                wd1770_drive *d = &PlusDDrives[ i ];

                                d->index_pulse = index_pulse;
                                if( !index_pulse && d->index_interrupt )
                                {
                                        wd1770_set_cmdint( d );
                                        d->index_interrupt = 0;
                                }

                                if (d->cmdint && d->state==wd1770_state_none)
                                        wd1770_reset_cmdint(d);
                        }

                        counter += (index_pulse ? 10 : 190) * 3500;
                }
        }
}

void floppy_shutdown()
{

        if (USEFDC765DLL) u765_Shutdown();
        if (DLLHandle) FreeLibrary(DLLHandle);
}

void floppy_init()
{
        int i=0;
        _TCHAR filename[MAXPATH]=_TEXT("\0");

        Data_Reg_A=0; Data_Dir_A=0; Control_A=0;
        Data_Reg_B=0; Data_Dir_B=0; Control_B=0;

        if (spectrum.floppytype==FLOPPYLARKEN81)
        {
                memset(LarkenDrive, 0, LARKENSIZE*2);
                LarkenPath0[0]='\0';
                LarkenPath1[0]='\0';
                if (_tcslen(filename)) floppy_setimage(i,filename);
                return;
        }

        if (spectrum.floppytype==FLOPPYPLUSD
                || spectrum.floppytype==FLOPPYDISCIPLE
                || spectrum.floppytype==FLOPPYOPUSD
                || spectrum.floppytype==FLOPPYBETA)
        {
                for( i = 0; i < 2; i++ )
                {
                        PlusDCur= &PlusDDrives[ i ];

                        if (PlusDCur->disk)
                        {
                                if (PlusDCur->disk->fd!=-1)
                                {
                                        _tcscpy(filename, PlusDCur->disk->filename);
                                        floppy_eject(i);
                                }

                                free(PlusDCur->disk->buffer);
                                free(PlusDCur->disk->dirty);
                                free(PlusDCur->disk->present);
                                free(PlusDCur->disk);
                        }

                        PlusDCur->disk = malloc( sizeof( *PlusDCur->disk ) );
                        if( !PlusDCur->disk ) return;

                        wd1770_reset(PlusDCur);

                        PlusDCur->disk->fd = -1;
                        PlusDCur->disk->alternatesides = 0;
                        PlusDCur->disk->numlayers = 2;
                        PlusDCur->disk->numtracks = 80;
                        PlusDCur->disk->numsectors = 10;
                        PlusDCur->disk->sectorsize = 512;

                        PlusDCur->disk->buffer = calloc( PlusDCur->disk->numlayers
                                                                * PlusDCur->disk->numtracks
                                                                * PlusDCur->disk->numsectors
                                                                * PlusDCur->disk->sectorsize, sizeof( BYTE ) );

                        PlusDCur->disk->dirty = calloc( PlusDCur->disk->numlayers
                                                        * PlusDCur->disk->numtracks
                                                        * PlusDCur->disk->numsectors, sizeof( BYTE ) );

                        PlusDCur->disk->present = calloc( PlusDCur->disk->numlayers
                                                                * PlusDCur->disk->numtracks
                                                                * PlusDCur->disk->numsectors, sizeof( BYTE ) );

                        if( !( PlusDCur->disk->buffer && PlusDCur->disk->dirty && PlusDCur->disk->present ) )
                        {
                                if( PlusDCur->disk->buffer ) free( PlusDCur->disk->buffer );
                                if( PlusDCur->disk->dirty ) free( PlusDCur->disk->dirty );
                                if( PlusDCur->disk->present ) free( PlusDCur->disk->present );
                                PlusDCur->disk->buffer = NULL;
                                PlusDCur->disk->dirty = NULL;
                                PlusDCur->disk->present = NULL;
                                free( PlusDCur->disk );
                                PlusDCur->disk = NULL;
                        }

                        PlusDCur->rates[ 0 ] = 6;
                        PlusDCur->rates[ 1 ] = 12;
                        PlusDCur->rates[ 2 ] = 2;
                        PlusDCur->rates[ 3 ] = 3;

                        PlusDCur->set_cmdint = NULL;
                        PlusDCur->reset_cmdint = NULL;
                        PlusDCur->set_datarq = NULL;
                        PlusDCur->reset_datarq = NULL;
                        PlusDCur->iface = NULL;

                        if (spectrum.floppytype==FLOPPYOPUSD) PlusDCur->set_datarq=OpusNMI;

                        if (_tcslen(filename)) floppy_setimage(i,filename);
                }
                PlusDCur= &PlusDDrives[0];
                return;
        }

        if (USEFDC765DLL)
        {
                u765_Shutdown();
                u765_Initialise();
                floppy_setimage(0,spectrum.driveaimg);
                floppy_setimage(1,spectrum.drivebimg);
                return;
        }

        if (p3_fdc) { fdc_destroy(&p3_fdc); p3_fdc=NULL; }
        if (p3_drive_a) { fd_destroy(&p3_drive_a); p3_drive_a=NULL; }
        if (p3_drive_b) { fd_destroy(&p3_drive_b); p3_drive_b=NULL; }
        if (p3_drive_null) { fd_destroy(&p3_drive_null); p3_drive_null=NULL; }

        if (spectrum.floppytype==FLOPPYPLUS3)
        {
                p3_fdc = fdc_new();
                p3_drive_null = fd_new();

                switch(spectrum.driveatype)
                {
                case DRIVENONE:
                        p3_drive_a = fd_new();
                        break;

                case DRIVE3INCHSS:
                        p3_drive_a = fd_newldsk();
                        fd_settype (p3_drive_a, FD_30);
                        fd_setheads(p3_drive_a, 1);
                        fd_setcyls (p3_drive_a, 40);
                        fd_setreadonly( p3_drive_a, 0);
                        break;

                case DRIVE3INCHDS:
                        p3_drive_a = fd_newldsk();
                        fd_settype (p3_drive_a, FD_30);
                        fd_setheads(p3_drive_a, 2);
                        fd_setcyls (p3_drive_a, 40);
                        fd_setreadonly( p3_drive_a, 0);
                        break;

                case DRIVE35INCHDS:
                        p3_drive_a = fd_newldsk();
                        fd_settype (p3_drive_a, FD_35);
                        fd_setheads(p3_drive_a, 2);
                        fd_setcyls (p3_drive_a, 80);
                        fd_setreadonly( p3_drive_a, 0);
                        break;
                }

                switch(spectrum.drivebtype)
                {
                case DRIVENONE:
                        p3_drive_b = fd_new();
                        break;

                case DRIVE3INCHSS:
                        p3_drive_b = fd_newldsk();
                        fd_settype (p3_drive_b, FD_30);
                        fd_setheads(p3_drive_b, 1);
                        fd_setcyls (p3_drive_b, 40);
                        fd_setreadonly(p3_drive_b, 0);
                        break;

                case DRIVE3INCHDS:
                        p3_drive_b = fd_newldsk();
                        fd_settype (p3_drive_b, FD_30);
                        fd_setheads(p3_drive_b, 2);
                        fd_setcyls (p3_drive_b, 40);
                        fd_setreadonly(p3_drive_b, 0);
                        break;

                case DRIVE35INCHDS:
                        p3_drive_b = fd_newldsk();
                        fd_settype (p3_drive_b, FD_35);
                        fd_setheads(p3_drive_b, 2);
                        fd_setcyls (p3_drive_b, 80);
                        fd_setreadonly(p3_drive_b, 0);
                        break;
                }

                fdc_reset(p3_fdc);
                //lib765_register_error_function( fdc_error );
	        fdc_setisr(p3_fdc, NULL);
                fdc_setdrive(p3_fdc, 0, p3_drive_a);
        	fdc_setdrive(p3_fdc, 1, p3_drive_b);
	        fdc_setdrive(p3_fdc, 2, p3_drive_null);
	        fdc_setdrive(p3_fdc, 3, p3_drive_null);

                floppy_setimage(0,spectrum.driveaimg);
                floppy_setimage(1,spectrum.drivebimg);
        }
}


void floppy_eject(int drive)
{
        if (spectrum.floppytype==FLOPPYLARKEN81)
        {
                int a;
                _TCHAR *filename;

                if (drive==0) filename=LarkenPath0;
                else filename=LarkenPath1;

                a=_topen( filename, O_CREAT | O_RDWR | O_BINARY);
                if (a>0)
                {
                        write(a, LarkenDrive + (LARKENSIZE*drive), LARKENSIZE);
                        close(a);
                }

                memset(LarkenDrive + (LARKENSIZE*drive), 0, LARKENSIZE);
                filename[0]='\0';
        }

        if (spectrum.floppytype==FLOPPYPLUS3)
        {
                if (USEFDC765DLL)
                {
                        u765_EjectDisk((BYTE)drive);
                }
                else
                {
                        if ((drive==0) && (p3_drive_a)) fd_eject(p3_drive_a);
                        if ((drive==1) && (p3_drive_b)) fd_eject(p3_drive_b);
                }
        }

        if (spectrum.floppytype==FLOPPYPLUSD
                || spectrum.floppytype==FLOPPYDISCIPLE
                || spectrum.floppytype==FLOPPYOPUSD
                || spectrum.floppytype==FLOPPYBETA)
        {
                wd1770_drive *d;

                if(drive >= 2) return;

                d = &PlusDDrives[ drive ];

                if( !d->disk || d->disk->fd == -1 ) return;

                if( d->disk->changed ) disk_image_write( d->disk);
                close( d->disk->fd );
                d->disk->fd = -1;
        }
}

int do_format(char *outfile, char *outtyp, char *outcomp, int forcehead, dsk_format_t format);

void floppy_setimage(int drive, _TCHAR *filename)
{
#if _UNICODE
        char temp[MAXPATH];
#endif
        int a;

        if (spectrum.floppytype==FLOPPYLARKEN81)
        {
                floppy_eject(drive);
                if (_tcslen(filename))
                {
                        a=_taccess(filename, 0);
                        if (!a)
                        {
                                a=_topen( filename, O_RDWR | O_BINARY);
                                read(a, LarkenDrive + (LARKENSIZE*drive), LARKENSIZE);
                                if (drive==0) _tcscpy(LarkenPath0, filename);
                                if (drive==1) _tcscpy(LarkenPath1, filename);
                        }
                        else
                        {
                                if (errno==ENOENT)
                                {
                                        if (drive==0) _tcscpy(LarkenPath0, filename);
                                        if (drive==1) _tcscpy(LarkenPath1, filename);
                                        memset(LarkenDrive + (LARKENSIZE*drive), 0, 1984);
                                        memcpy(LarkenDrive + (LARKENSIZE*drive), LarkenHeader,1984);
                                }
                        }
                }
                return;
        }

        if (spectrum.floppytype==FLOPPYPLUSD
                || spectrum.floppytype==FLOPPYDISCIPLE
                || spectrum.floppytype==FLOPPYOPUSD
                || spectrum.floppytype==FLOPPYBETA)
        {
                int l;
                wd1770_drive *d;

                if( drive >= 2 ) return;

                d = &PlusDDrives[ drive ];
                if( !( d->disk && d->disk->buffer && d->disk->dirty && d->disk->present ) ) return;

                l = _tcslen( filename );
                if( l >= 5 )
                {
                        d->disk->numlayers = 2;
                        d->disk->numtracks = 80;
                        d->disk->numsectors = 10;
                        d->disk->sectorsize = 512;

                        if( !_tcscmp( filename + ( l - 4 ), _TEXT(".dsk") ) ) d->disk->alternatesides = 1;
                        else if( !_tcscmp( filename + ( l - 4 ), _TEXT(".mgt") ) ) d->disk->alternatesides = 1;
                        else if( !_tcscmp( filename + ( l - 4 ), _TEXT(".img") ) ) d->disk->alternatesides = 0;
                        else if( !_tcscmp( filename + ( l - 4 ), _TEXT(".opd") )
                                   || !_tcscmp( filename + ( l - 4 ), _TEXT(".opu") ))
                        {
                                d->disk->alternatesides = 1;
                                d->disk->numlayers = 1;
                                d->disk->numtracks = 40;
                                d->disk->numsectors = 18;
                                d->disk->sectorsize = 256;
                        }
                        else if( !_tcscmp( filename + ( l - 4 ), _TEXT(".trd") ))
                        {
                                d->disk->alternatesides = 1;
                                d->disk->numlayers = 2;
                                d->disk->numtracks = 80;
                                d->disk->numsectors = 16;
                                d->disk->sectorsize = 256;
                        }
                        else return;
                }

                if( d->disk->fd != -1 ) floppy_eject( drive );

                d->disk->readonly = 0;
                d->disk->changed = 0;

                if( ( d->disk->fd = _topen( filename, O_RDWR | O_BINARY) ) == -1 )
                {
                        if((d->disk->fd = _topen( filename, O_RDONLY | O_BINARY )) == -1 )
                        {
                                /*fprintf( stderr, "disciple_disk_insert: open() failed: %s\n",
                                                strerror( errno ) ); */
                                return;
                        }

                        d->disk->readonly = 1;
                }

                memset( d->disk->present, 0, 2 * d->disk->numtracks * d->disk->numsectors );
                memset( d->disk->dirty, 0, 2 * d->disk->numtracks * d->disk->numsectors );

                _tcsnccpy( d->disk->filename, filename, MAXPATH );
                d->disk->filename[ MAXPATH - 1 ] = '\0';

                return;
        }

        if (spectrum.floppytype==FLOPPYPLUS3)
        {
                if (_tcslen(filename) && _taccess(filename,0))
                {
                        int drivetype;
                        dsk_format_t format;

                        if (drive==0)
                                drivetype=spectrum.driveatype;
                        else
                                drivetype=spectrum.drivebtype;

                        switch(drivetype)
                        {
                        case DRIVE3INCHSS: format=FMT_180K; break;
                        case DRIVE3INCHDS: format=FMT_360K; break;
                        case DRIVE35INCHDS: format=FMT_720K; break;
                        }

#if _UNICODE
                        wcstombs(temp, filename, MAXPATH);
                        do_format(temp, "dsk", NULL, -1, format);
#else
                        do_format(filename, "dsk", NULL, -1, format);
#endif
                }

                if (USEFDC765DLL)
                {
                        if (u765_DiskInserted((BYTE)drive))
                                u765_EjectDisk((BYTE)drive);
#if _UNICODE
                        if (_tcslen(filename))
                        {
                            char temp[MAXPATH];
                            wcstombs(temp, filename, MAXPATH);
                            u765_InsertDisk(temp,drive);
                        }
#else
                        if (_tcslen(filename)) u765_InsertDisk(filename,(BYTE)drive);
#endif
                        u765_DiskInserted(0);
                        return;
                }

                if ((drive==0) && (p3_drive_a))
                {
                        fd_eject(p3_drive_a);
                        if (_tcslen(filename))
#if _UNICODE
                        {
                            char temp[MAXPATH];
                            wcstombs(temp, filename, MAXPATH);
                            fdl_setfilename(p3_drive_a, temp);
                        }
#else
                                fdl_setfilename(p3_drive_a, filename);
#endif
                }

                if ((drive==1) && (p3_drive_b))
                {
                        fd_eject(p3_drive_b);
                        if (_tcslen(filename))
#if _UNICODE
                        {
                            char temp[MAXPATH];
                            wcstombs(temp, filename, MAXPATH);
                            fdl_setfilename(p3_drive_b, temp);
                        }
#else
                                fdl_setfilename(p3_drive_b, filename);
#endif
                }
        }
}

// ---------------------------------------------------------------------------

static unsigned char spec169 [10] = { 0,    0, 40, 9, 2, 2, 3, 2, 0x2A, 0x52 };
static unsigned char spec180 [10] = { 0,    0, 40, 9, 2, 1, 3, 2, 0x2A, 0x52 };
static unsigned char spec360 [10] = { 3,    1, 40, 9, 2, 1, 3, 2, 0x2A, 0x52 };
static unsigned char spec200 [10] = { 0,    0, 40,10, 2, 1, 3, 3, 0x0C, 0x17 };
static unsigned char spec720 [10] = { 3, 0x81, 80, 9, 2, 1, 4, 4, 0x2A, 0x52 };
static unsigned char spec800 [10] = { 3, 0x81, 80,10, 2, 1, 4, 4, 0x0C, 0x17 };

int do_format(char *outfile, char *outtyp, char *outcomp, int forcehead, dsk_format_t format)
{
	DSK_PDRIVER outdr = NULL;
	dsk_err_t e;
	dsk_pcyl_t cyl;
	dsk_phead_t head;
	DSK_GEOMETRY dg;
	dsk_cchar_t fdesc;
        int retries=1;

	//dsk_reportfunc_set(report, report_end);
	e = dsk_creat(&outdr, outfile, outtyp, outcomp);
	if (!e) e = dsk_set_retry(outdr, retries);
	if (!e && forcehead >= 0) e = dsk_set_forcehead(outdr, forcehead);
	if (!e) e = dg_stdformat(&dg, format, NULL, &fdesc);
	if (!e)
	{
		// printf("Formatting %s as %s\n", outfile, fdesc);
		for (cyl = 0; cyl < dg.dg_cylinders; cyl++)
		{
		    for (head = 0; head < dg.dg_heads; head++)
		    {
			// printf("Cyl %02d/%02d Head %d/%d\r",
			//	cyl +1, dg.dg_cylinders,
			// 	head+1, dg.dg_heads);
			// fflush(stdout);

			if (!e) e = dsk_apform(outdr, &dg, cyl, head, 0xE5);
			if (e) break;
		    }
		}

	}
	/* Create a disc spec on the first sector, if the format's recognised */
	if (!e)
	{
		unsigned char bootsec[512];
		unsigned int do_bootsec = 0;

		memset(bootsec, 0xE5, sizeof(bootsec));
		switch(format)
		{
			case FMT_180K:
				memcpy(bootsec, spec180, 10);
				do_bootsec = 1;
				break;
                        case FMT_360K:
				memcpy(bootsec, spec360, 10);
				do_bootsec = 1;
				break;
			case FMT_CPCSYS:
				memcpy(bootsec, spec169, 10);
				do_bootsec = 1;
				break;
			case FMT_CPCDATA:
				break;
			case FMT_200K:
				memcpy(bootsec, spec200, 10);
				do_bootsec = 1;
				break;
			case FMT_720K:
				memcpy(bootsec, spec720, 10);
				do_bootsec = 1;
				break;
			case FMT_800K:
				memcpy(bootsec, spec800, 10);
				do_bootsec = 1;
				break;
			default:
				break;
		}
		if (do_bootsec) e = dsk_lwrite(outdr, &dg, bootsec, 0);
	}
	// printf("\r                                     \r");
	if (outdr) dsk_close(&outdr);
	if (e)
	{
		// fprintf(stderr, "%s\n", dsk_strerror(e));
		return 1;
	}
	return 0;
}

int LarkenLoadTrack(int Drive, int TrackNo, unsigned char *buf)
{
        int offset;

        offset=Drive*LARKENSIZE;

        if (LarkenDrive[offset+140]!=0xff)
        {
                memset(buf, 0, 1984);
                return(0);
        }


        offset += TrackNo * 1984;

        memcpy(buf, LarkenDrive + offset, 1984);
        return(1);
}

void LarkenSaveTrack(int Drive, int TrackNo, unsigned char *buf)
{
        unsigned char *p;

        p=LarkenDrive + LARKENSIZE*Drive;
        p += TrackNo*1984;

        memcpy(p, buf, 1984);
}







