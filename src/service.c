
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <process.h>
#include <sys\stat.h>

#include <windows.h>

SERVICE_STATUS MyServiceStatus;  
SERVICE_STATUS_HANDLE MyServiceStatusHandle; 
 
VOID  MyServiceStart (DWORD argc, LPTSTR *argv);
VOID  MyServiceCtrlHandler (DWORD opcode);
DWORD MyServiceInitialization (DWORD argc, LPTSTR *argv, DWORD *specificError); 
 
 VOID SvcDebugOut(LPSTR String, DWORD Status) 
{ 
    CHAR  Buffer[1024]; 
    if (strlen(String) < 1000) 
    { 
        sprintf(Buffer, String, Status); 
        OutputDebugStringA(Buffer); 
    } 
}

VOID MyServiceCtrlHandler (DWORD Opcode)  
{ 
    DWORD status; 
 
    switch(Opcode) 
    { 
        case SERVICE_CONTROL_PAUSE: 
        // Do whatever it takes to pause here. 
            MyServiceStatus.dwCurrentState = SERVICE_PAUSED; 
            break; 
 
        case SERVICE_CONTROL_CONTINUE: 
        // Do whatever it takes to continue here. 
            MyServiceStatus.dwCurrentState = SERVICE_RUNNING; 
            break; 
 
        case SERVICE_CONTROL_STOP: 

        // Do whatever it takes to stop here. 
            MyServiceStatus.dwWin32ExitCode = 0; 
            MyServiceStatus.dwCurrentState  = SERVICE_CONTROL_STOP;	//SERVICE_STOPPED_PENDING; 
            MyServiceStatus.dwCheckPoint    = 0; 
            MyServiceStatus.dwWaitHint      = 0; 
 
            if (!SetServiceStatus (MyServiceStatusHandle, 
                &MyServiceStatus))
            { 
                status = GetLastError(); 
                SvcDebugOut(" [VirusDetectorService] SetServiceStatus error %ld\n",status); 
            } 
 
            SvcDebugOut(" [VirusDetectorService] Leaving MyService \n",0); 
            return; 
 
        case SERVICE_CONTROL_INTERROGATE: 
        // Fall through to send current status. 
            break; 
 
        default: 
            SvcDebugOut(" [VirusDetectorService] Unrecognized opcode %ld\n", 
                Opcode); 
    } 
 
    // Send current status. 
    if (!SetServiceStatus (MyServiceStatusHandle,  &MyServiceStatus)) 

    { 
        status = GetLastError(); 
        SvcDebugOut(" [VirusDetectorService] SetServiceStatus error %ld\n",status); 
    } 
    return; 
} 
 

void MyServiceStart (DWORD argc, LPTSTR *argv)  
{ 
    DWORD status; 
    DWORD specificError; 
 
    MyServiceStatus.dwServiceType        = SERVICE_WIN32; 
    MyServiceStatus.dwCurrentState       = SERVICE_START_PENDING; 
    MyServiceStatus.dwControlsAccepted   = SERVICE_ACCEPT_STOP | 
        SERVICE_ACCEPT_PAUSE_CONTINUE; 
    MyServiceStatus.dwWin32ExitCode      = 0; 
    MyServiceStatus.dwServiceSpecificExitCode = 0; 
    MyServiceStatus.dwCheckPoint         = 0; 

    MyServiceStatus.dwWaitHint           = 0; 
 
    MyServiceStatusHandle = RegisterServiceCtrlHandler( 
        TEXT("VirusDetectorService"), 
        (LPHANDLER_FUNCTION) MyServiceCtrlHandler); 
 
    if (MyServiceStatusHandle == (SERVICE_STATUS_HANDLE) 0) 
    { 
        SvcDebugOut(" [VirusDetectorService] RegisterServiceCtrlHandler failed %d\n", GetLastError()); 
        return; 
    } 
 
    // Initialization code goes here. 
    status = MyServiceInitialization(argc,argv, &specificError); 

 
    // Handle error condition 
    if (status != NO_ERROR) 
    { 
        MyServiceStatus.dwCurrentState       = SERVICE_STOPPED; 
        MyServiceStatus.dwCheckPoint         = 0; 
        MyServiceStatus.dwWaitHint           = 0; 
        MyServiceStatus.dwWin32ExitCode      = status; 
        MyServiceStatus.dwServiceSpecificExitCode = specificError; 
 
        SetServiceStatus (MyServiceStatusHandle, &MyServiceStatus); 
        return; 
    } 
 
    // Initialization complete - report running status. 

    MyServiceStatus.dwCurrentState       = SERVICE_RUNNING; 
    MyServiceStatus.dwCheckPoint         = 0; 
    MyServiceStatus.dwWaitHint           = 0; 
 
    if (!SetServiceStatus (MyServiceStatusHandle, &MyServiceStatus)) 
    { 
        status = GetLastError(); 
        SvcDebugOut(" [VirusDetectorService] SetServiceStatus error %ld\n",status); 
    } 
 
    // This is where the service does its work. 
    SvcDebugOut(" [VirusDetectorService] Returning the Main Thread \n",0); 

 
    return; 
} 
 
// Stub initialization function. 
DWORD MyServiceInitialization(DWORD   argc, LPTSTR  *argv, 
    DWORD *specificError) 
{ 
    argv; 
    argc; 
    specificError; 
    return(0); 
} 
 
 
int main () { 
    SERVICE_TABLE_ENTRY   DispatchTable[] = 
    { 
        { TEXT("VirusDetectorService"), MyServiceStart}, 
        { NULL,              NULL                } 
    }; 

 
    if (!StartServiceCtrlDispatcher( DispatchTable)) 
    { 
        SvcDebugOut(" [VirusDetectorService] StartServiceCtrlDispatcher error = %d\n", GetLastError()); 
    }
    return 0;
} 
 

