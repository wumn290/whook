#include <stdio.h>

#ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

#include "process.h"
#include "info.h"
#include "utils.h"
#include "pestuff.h"
#include "network.h"
#include "modules.h"
#include "file.h"
#include "dump.h"
#include "memory.h"

#define processname "calc.exe"
//#define processname "whook_test.exe"

//#define FILENAME "C:\\Work\\Code\\whook\\cmake\\bin\\Debug\\calc.exe"
#define FILENAME "C:\\Work\\Code\\whook\\cmake\\bin\\Debug\\dbghelp_32.dll"

VOID TestMemory(VOID)
{
    DWORD dwPid;
    std::list<MEMORY_BASIC_INFORMATION32> lmbi32;
    std::list<MEMORY_BASIC_INFORMATION64> lmbi64;

    dwPid = GetPidProcess("calc.exe");
    
    if (Is64BitProcess(dwPid) == TRUE) {
        lmbi64 = GetMemoryInformation64(dwPid);
        PrintMemoryInfo(lmbi64);
    }
    else {
        lmbi32 = GetMemoryInformation32(dwPid);
        PrintMemoryInfo(lmbi32);
        //lModules = GetModuleList(dwPid);
        //PrintModulesList(lModules);  
    }
    dwPid = GetPidProcess("STFU.exe");
    if (dwPid == DWORD(-1)) {
        ExitProcess(42);
    }
}

VOID TestDump(VOID)
{
    DWORD dwPid;

    dwPid = GetPidProcess("calc.exe");


    DumpPE(dwPid, "dumped.exe");
    
    dwPid = GetPidProcess("STFU.exe");
    if (dwPid == DWORD(-1)) {
        ExitProcess(42);
    }
}

VOID TestFile(VOID)
{
    WHOOK_FILE wFile;
    std::list<EXPORTENTRY> lExport;

    wFile = WhookFileOpen(FILENAME);
    lExport = GetExport((HMODULE)wFile.buf);
    PrintExportEntry(lExport);
}

VOID TestModules(VOID)
{
    std::list<MODULEENTRY32> lModules;
    std::list<MODULEENTRY64> lModules64;
    DWORD dwPid;
    
    dwPid = GetPidProcess(processname);

    if (Is64BitProcess(dwPid) == TRUE) {
        lModules64 = GetModuleList64(dwPid);
        PrintModulesList(lModules64);      
    }
    else {
        lModules = GetModuleList(dwPid);
        PrintModulesList(lModules);  
    }
    
    dwPid = GetPidProcess("STFU.exe");
    if (dwPid == DWORD(-1)) {
        ExitProcess(42);
    }
}

int main(int argc, char *argv[])
{
    std::list<PROCESSENTRY32> lProcess;
    std::list<MIB_TCPROW_OWNER_PID> lMib;
    std::list<MIB_UDPROW_OWNER_PID> lMuib;
    std::list<MIB_TCP6ROW_OWNER_PID> lMibv6;
    std::list<MIB_UDP6ROW_OWNER_PID> lMuibv6;
    DWORD dwPid;
    (void)argc;
    (void)argv;
    
    if (!EnablePrivilege(SE_DEBUG_NAME, TRUE)) {
        printf("[-] EnablePrivilege failed : %u\n", GetLastError());
        //return -1;
    }    
    
    TestMemory();
    TestDump();
    TestFile();
    TestModules();
    
    if (!EnablePrivilege(SE_DEBUG_NAME, TRUE)) {
        printf("[-] EnablePrivilege failed : %u\n", GetLastError());
        return -1;
    }
    if (IsElevated() == FALSE) {
        printf("[-] Not elevated privileges, some features will fail\n");
    }    
    
    printf("[+] Is64bitOS : %d\n", Is64bitOS());
    printf("[+] IsWow64(GetCurrentProcess()) : %d\n", IsWow64(GetCurrentProcess()));
    
    lProcess = GetProcessList();
    PrintProcessList(lProcess);    
    printf("%d\n", IsInProcessList(lProcess, "calc.exe"));
    dwPid = GetPidProcess("calc.exe");
    printf("%08X\n", GetHandleProcess(dwPid));
    printf("%llX\n", GetRemoteBaseAddress(dwPid));
    
    lMib = GetTCPConnections();
    PrintTCPConnections(lMib);
    lMuib = GetUDPConnections();
    PrintUDPConnections(lMuib);
    lMibv6 = GetTCPConnectionsv6();
    PrintTCPConnectionsv6(lMibv6);
    lMuibv6 = GetUDPConnectionsv6();
    PrintUDPConnectionsv6(lMuibv6);
    
    CloseTCPConnectionRemote(lMib, "88.198.127.131", 13337);
    
    return 0;
}