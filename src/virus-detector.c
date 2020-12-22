

/*
	Name		: virus-detector.c
	Copyright	: 2010 © Daniel Sirait
	Author		: Daniel Sirait
	Date		: 01/04/10 07:52
	License		: See `LICENSE.TXT'
	Description	: heavy machinery, no user serviceable parts inside
*/



/*
	Thus spake the Master Ninjei:

	"To the intelligent man, one word, to the fleet horse, one flick of the whip,
	to the well-written program, a single command."

			-- The Zen of Programming
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <process.h>
#include <sys\stat.h>

#include <windows.h>

#ifndef _MAX_PATH
#define _MAX_PATH 512
#endif

#define MAGIC_SIZE 10000
#define CRC_SIZE 32256
#define FINAL_CRC (MAGIC_SIZE + CRC_SIZE)


#define CAPTION_MSG "Virus Detector - by Daniel Sirait <dns@cpan.org>"

#define SECURE_MSG "Your System is 100% secure !\t\t\t"
#define DETECTED_MSG "Virus Detected !\nPlease install & run anti-virus now, \nDon't forget to backup all your important files!\t\t\t"

unsigned int check_size (const char *vd_path) {
	struct stat statbuf;
	FILE *stream;

	if ((stream = fopen(vd_path, "rb")) == NULL) {
		fprintf(stderr, "Cannot open output file.\n");
		abort();
	}
	fclose(stream);

	stat(vd_path, &statbuf);
	return statbuf.st_size;
}

void install_service (const char *vd_path) {
	char lpBuffer[64];
	DWORD nSize = 64;
	GetUserName(lpBuffer, &nSize);
	
	char start_buf[512];
	sprintf(start_buf, "C:\\Documents and Settings\\%s\\Start Menu\\Programs\\Startup\\virus-detector.exe", lpBuffer);
	char *vd_install_path = "C:\\virus-detector.exe";
	char vd_service_argv[256];
	sprintf(vd_service_argv, "%s %s", vd_install_path, "-daemon");
	CopyFile(vd_path, vd_install_path, TRUE);
	CopyFile(vd_path, start_buf, TRUE);
}

void uninstall_service () {
	char start_buf[512];
	char lpBuffer[64];
	DWORD nSize = 64;
	GetUserName(lpBuffer, &nSize);
	sprintf(start_buf, "C:\\Documents and Settings\\%s\\Start Menu\\Programs\\Startup\\virus-detector.exe", lpBuffer);
	char *vd_install_path = "C:\\virus-detector.exe";
	DeleteFile(start_buf);
	DeleteFile(vd_install_path);
}

/*
void install_service (const char *vd_path) {
	BOOL retval;
	char *vd_install_path = "C:\\virus-detector.exe";
	char vd_service_argv[256];
	sprintf(vd_service_argv, "%s %s", vd_install_path, "-daemon");
	retval = CopyFile(vd_path, vd_install_path, TRUE);
	SetFileAttributes(vd_install_path, FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM);

	if (retval == 0) {
        MessageBox(NULL, "Can't install Virus Detector !\t\t\t", CAPTION_MSG, MB_OK | MB_ICONERROR | MB_SYSTEMMODAL);
	}
	
	SC_HANDLE hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

	//SC_HANDLE hService = OpenService(hSCManager, "xlpServiceName" , SERVICE_ALL_ACCESS);
	//BOOL retval = DeleteService(hService);
	SC_HANDLE hService = CreateService(
	hSCManager,
	"VirusDetectorService",
	"Virus Detector",
	SERVICE_ALL_ACCESS,
	SERVICE_WIN32_OWN_PROCESS,
	SERVICE_AUTO_START,
	SERVICE_ERROR_NORMAL,
	vd_service_argv,		//"C:\\virus-detector.exe" -daemon,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL
	);

	CloseServiceHandle(hService);
	CloseServiceHandle(hSCManager);
}

void uninstall_service () {
	BOOL retval;

	SC_HANDLE hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

	SC_HANDLE hService = OpenService(hSCManager, "VirusDetectorService" , SERVICE_ALL_ACCESS);
	retval = DeleteService(hService);
	if (retval == 0) {
		MessageBox(NULL, "Can't uninstall Virus Detector !\t\t\t", CAPTION_MSG, MB_OK | MB_ICONERROR | MB_SYSTEMMODAL);
	}
	
	CloseServiceHandle(hService);
	CloseServiceHandle(hSCManager);
	char *vd_install_path = "C:\\virus-detector.exe";
	SetFileAttributes(vd_install_path, FILE_ATTRIBUTE_NORMAL);
	DeleteFile(vd_install_path);
}
*/

BOOL virus_check (const char *path_buf) {
	BOOL retval = FALSE;
	/* vd log *

	/* virus detector */

	/*MessageBox(NULL, argv[1], CAPTION_MSG, MB_OK | MB_ICONERROR | MB_SYSTEMMODAL);  // MB_ICONWARNING | MB_ICONERROR */
	unsigned int current_size = check_size(path_buf);
	
	if (FINAL_CRC == (current_size + MAGIC_SIZE)) {
		retval = FALSE;
		
		/*
		puts(SECURE_MSG);
		MessageBox(NULL, SECURE_MSG, CAPTION_MSG, MB_OK | MB_ICONASTERISK | MB_SYSTEMMODAL);
		*/
	} else {
		retval = TRUE;
		puts(DETECTED_MSG);
		MessageBox(NULL, DETECTED_MSG, CAPTION_MSG, MB_OK | MB_ICONERROR | MB_SYSTEMMODAL);
	}

	return retval;
}


/*
VOID WINAPI ServiceMain(DWORD dwArgc, LPTSTR *lpszArgv) {

}

SERVICE_STATUS          MyServiceStatus;
SERVICE_STATUS_HANDLE   MyServiceStatusHandle;

VOID  MyServiceStart (DWORD argc, LPTSTR *argv);
VOID  MyServiceCtrlHandler (DWORD opcode);
DWORD MyServiceInitialization (DWORD argc, LPTSTR *argv,
        DWORD *specificError);

VOID SvcDebugOut(LPSTR String, DWORD Status)
{
    CHAR  Buffer[1024];
    if (strlen(String) < 1000)
    {
        sprintf(Buffer, String, Status);
        OutputDebugStringA(Buffer);
    }
}

VOID  MyServiceStart (DWORD argc, LPTSTR *argv) {

}
*/

int main (argc, argv, env) const int argc; const char **argv; const char **env; {
	char path_buf[_MAX_PATH];
	_fullpath(path_buf, argv[0], _MAX_PATH);

	/* parameter */
	if (argv[1] != NULL) {
		if (stricmp("-install", argv[1]) == 0) {
			install_service(path_buf);
			BOOL retval = virus_check(path_buf);
			if (retval == FALSE) {
				MessageBox(NULL, SECURE_MSG, CAPTION_MSG, MB_OK | MB_ICONASTERISK | MB_SYSTEMMODAL);
			}
		} else if (stricmp("-uninstall", argv[1]) == 0) {
			uninstall_service(path_buf);
		} else if (stricmp("-checksize", argv[1]) == 0) {
			unsigned int current_size = check_size(path_buf);
			char buf[512];
			sprintf(buf, "current_size: %d\n", current_size);
			puts(buf);
			MessageBox(NULL, buf, CAPTION_MSG, MB_OK | MB_ICONASTERISK | MB_SYSTEMMODAL);
		} else if (stricmp("-daemon", argv[1]) == 0) {
			while (1) {
				virus_check(path_buf);
				_sleep(3600);
			}
			
		}
	} else {
		virus_check(path_buf);
	}
	
	
	
	return 0;
}

