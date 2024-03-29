#include <Windows.h>
#include <string.h>
#include <direct.h>

static WIN32_FIND_DATA findFileData;
static HANDLE hFind = INVALID_HANDLE_VALUE;


const int FR_OK = 0;
   //FR_DISK_ERR;		   /* 1 */
	//FR_INT_ERR;			/* 2 */
	//FR_NOT_READY,		/* 3 */
	//FR_NO_FILE,			/* 4 */
const int FR_NO_PATH = 5;
	//FR_INVALID_NAME,	/* 6 */
	//FR_DENIED,			/* 7 */
	//FR_EXIST,			   /* 8 */
	//FR_INVALID_OBJECT,	/* 9 */
	//FR_WRITE_PROTECTED,/* 10 */
	//FR_INVALID_DRIVE,	/* 11 */
	//FR_NOT_ENABLED,		/* 12 */
	//FR_NO_FILESYSTEM,	/* 13 */
	//FR_MKFS_ABORTED,	/* 14 */
	//FR_TIMEOUT			/* 15 */

void dirGetExeLocation(char* buffer, int bufSize)
{
#ifdef _UNICODE
   _TCHAR *tempbuff = (_TCHAR *)malloc(bufSize*sizeof(_TCHAR));
   GetModuleFileName(NULL, tempbuff, bufSize);
   wcstombs(buffer, tempbuff, bufSize);
   free(tempbuff);
#else
   GetModuleFileName(NULL, buffer, bufSize);
#endif
   char* lastSlash = strrchr(buffer, '\\');
   if (lastSlash)
   {
      *(lastSlash + 1) = 0;
   }
}
              
int dirOpen(const char* path)
{
   _TCHAR pathPlusExt[MAX_PATH];

   if (hFind != INVALID_HANDLE_VALUE)
   {
      FindClose(hFind);
      hFind = INVALID_HANDLE_VALUE;
   }

   if (path == NULL) {
      return FR_NO_PATH;
   }

   strcpy(pathPlusExt, path);
   if (path[strlen(path) - 1] != '/' && path[strlen(path) - 1] != '\\')
   {
	  _tcscat(pathPlusExt, _TEXT("\\"));
   }
   _tcscat(pathPlusExt, _TEXT("*.*"));

   // I will assume here that the 1st file found will always be '.', which for the purposes
   // of this code we can safely sink. The alternative is to work with a 'file-in-hand' and
   // I think this would complicate things. There won't be a '.' in the root of a drive tho,
   // so caveat developer!
   //
   hFind = FindFirstFile(pathPlusExt, &findFileData);
   if (hFind == INVALID_HANDLE_VALUE)
   {
      // no-path
      return FR_NO_PATH;
   }

   return 0;
}


// CAREFUL! MAKE SURE THIS IS ALWAYS UP TO DATE 
typedef struct _FILINFO_ {
   DWORD fsize;      /* File size */
   WORD  fdate;      /* Last modified date */
   WORD  ftime;      /* Last modified time */
   BYTE  fattrib; /* Attribute */
   char  fname[13];  /* Short file name (8.3 format) */
} FILINFO;

#define	AM_RDO	0x01	/* Read only */
#define	AM_HID	0x02	/* Hidden */
#define	AM_SYS	0x04	/* System */
#define	AM_VOL	0x08	/* Volume label */
#define AM_LFN	0x0F	/* LFN entry */
#define AM_DIR	0x10	/* Directory */
#define AM_ARC	0x20	/* Archive */
#define AM_MASK	0x3F	/* Mask of defined bits */

void dirReadNext(void* fil)
{
   FILINFO* fileinfo = reinterpret_cast<FILINFO*>(fil);

   // potentially terminate the search
   //
   memset(&fileinfo->fname[0], 0, 13);

   if (FindNextFile(hFind, &findFileData))
   {
      byte attrib = 0;

      // Atom client only uses DIR flag, ZXpand uses hidden & read only also
      if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_READONLY)
      {
         attrib |= AM_RDO;
      }
      if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN)
      {
         attrib |= AM_HID;
      }
      if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
      {
         attrib |= AM_DIR;
      }

      fileinfo->fsize = findFileData.nFileSizeLow;
      fileinfo->fattrib = attrib;
      strcpy(&fileinfo->fname[0], &findFileData.cFileName[0]);
   }
}

