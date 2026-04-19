#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsvc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char  szName[256];
    char  szDisplayName[256];
    DWORD dwCurrentState;
    DWORD dwServiceType;
    DWORD dwProcessId;
    char  szBinaryPath[MAX_PATH];
} ServiceInfo;

static const char* GetStateName(DWORD state)
{
    switch (state) {
        case SERVICE_STOPPED:          return "off";
        case SERVICE_START_PENDING:    return "turning on...";
        case SERVICE_STOP_PENDING:     return "turning off...";
        case SERVICE_RUNNING:          return "currently running";
        case SERVICE_CONTINUE_PENDING: return "continuing...";
        case SERVICE_PAUSE_PENDING:    return "pausing...";
        case SERVICE_PAUSED:           return "pause";
        default:                       return "unknown";
    }
}

static const char* GetTypeName(DWORD type)
{
    switch (type & ~(DWORD)SERVICE_INTERACTIVE_PROCESS) {
        case SERVICE_KERNEL_DRIVER:       return "Kernel Driver";
        case SERVICE_FILE_SYSTEM_DRIVER:  return "Sys files Driver";
        case SERVICE_WIN32_OWN_PROCESS:   return "Win32 Process";
        case SERVICE_WIN32_SHARE_PROCESS: return "Shared Win32 Process";
        default:                          return "Unknown type";
    }
}

static void FetchExtraInfo(SC_HANDLE hSCM, const char* szName, char* pathBuf, DWORD pathBufLen, DWORD* pPid)
{
    *pPid = 0;
    pathBuf[0] = '\0';

    SC_HANDLE hSvc = OpenServiceA(hSCM, szName, SERVICE_QUERY_CONFIG | SERVICE_QUERY_STATUS);
    if (!hSvc) return;

    SERVICE_STATUS_PROCESS ssp;
    DWORD needed = 0;
    if (QueryServiceStatusEx(hSvc, SC_STATUS_PROCESS_INFO, (LPBYTE)&ssp, sizeof(ssp), &needed))
    {
        *pPid = ssp.dwProcessId;
    }

    DWORD cbNeeded = 0;
    QueryServiceConfigA(hSvc, NULL, 0, &cbNeeded);
    if (cbNeeded > 0) {
        LPQUERY_SERVICE_CONFIGA pCfg = (LPQUERY_SERVICE_CONFIGA)malloc(cbNeeded);
        if (pCfg) {
            if (QueryServiceConfigA(hSvc, pCfg, cbNeeded, &cbNeeded)) {
                strncpy(pathBuf, pCfg->lpBinaryPathName, pathBufLen - 1);
                pathBuf[pathBufLen - 1] = '\0';
            }
            free(pCfg);
        }
    }

    CloseServiceHandle(hSvc);
}

int main(void)
{
    SC_HANDLE hSCM = OpenSCManagerA(NULL, NULL, SC_MANAGER_ENUMERATE_SERVICE);
    if (!hSCM) {
        fprintf(stderr, ">>Error: Couldn't Open SCM -> %lu\n", GetLastError());
        return 1;
    }

    DWORD bytesNeeded   = 0;
    DWORD servicesCount = 0;
    DWORD resumeHandle  = 0;

    EnumServicesStatusExA(hSCM, SC_ENUM_PROCESS_INFO, SERVICE_TYPE_ALL, SERVICE_STATE_ALL,
                          NULL, 0, &bytesNeeded, &servicesCount, &resumeHandle, NULL);

    if (bytesNeeded == 0) {
        fprintf(stderr, ">>Error: EnumServicesStatusEx failed -> %lu\n", GetLastError());
        CloseServiceHandle(hSCM);
        return 1;
    }

    LPENUM_SERVICE_STATUS_PROCESSA pServices = (LPENUM_SERVICE_STATUS_PROCESSA)malloc(bytesNeeded);
    if (!pServices) {
        fprintf(stderr, ">>Error: Mem alloc failed at pServices.\n");
        CloseServiceHandle(hSCM);
        return 1;
    }

    resumeHandle = 0;
    BOOL ok = EnumServicesStatusExA(hSCM, SC_ENUM_PROCESS_INFO, SERVICE_TYPE_ALL, SERVICE_STATE_ALL,
                                    (LPBYTE)pServices, bytesNeeded,
                                    &bytesNeeded, &servicesCount, &resumeHandle, NULL);

    if (!ok) {
        fprintf(stderr, ">>Error: EnumServicesStatusEx failed -> %lu\n", GetLastError());
        free(pServices);
        CloseServiceHandle(hSCM);
        return 1;
    }

    ServiceInfo* running = (ServiceInfo*)malloc(servicesCount * sizeof(ServiceInfo));
    if (!running) {
        fprintf(stderr, ">>Error: Mem Alloc failed at Service Info.\n");
        free(pServices);
        CloseServiceHandle(hSCM);
        return 1;
    }

    DWORD runCount = 0;
    for (DWORD i = 0; i < servicesCount; i++) {
        if (pServices[i].ServiceStatusProcess.dwCurrentState == SERVICE_RUNNING) {
            ServiceInfo* si = &running[runCount++];

            strncpy(si->szName,        pServices[i].lpServiceName,  sizeof(si->szName)        - 1);
            si->szName[sizeof(si->szName) - 1] = '\0';
            strncpy(si->szDisplayName, pServices[i].lpDisplayName,  sizeof(si->szDisplayName) - 1);
            si->szDisplayName[sizeof(si->szDisplayName) - 1] = '\0';

            si->dwCurrentState  = pServices[i].ServiceStatusProcess.dwCurrentState;
            si->dwServiceType   = pServices[i].ServiceStatusProcess.dwServiceType;
            si->dwProcessId     = pServices[i].ServiceStatusProcess.dwProcessId;
            si->szBinaryPath[0] = '\0';

            FetchExtraInfo(hSCM, si->szName, si->szBinaryPath, MAX_PATH, &si->dwProcessId);
        }
    }

    printf(">>Sys Services Running  -  Total: %lu\n", runCount);

    for (DWORD i = 0; i < runCount; i++) {
        ServiceInfo* si = &running[i];
        printf("---[ %lu / %lu ]--------------------------------------------------\n", i + 1, runCount);
        printf("Intern Name: %s\n",  si->szName);
        printf("Shown Name: %s\n",   si->szDisplayName);
        printf("State: %s\n",        GetStateName(si->dwCurrentState));
        printf("Type: %s\n",         GetTypeName(si->dwServiceType));
        if (si->dwProcessId) printf("PID: %lu\n", si->dwProcessId);
        else                 printf("PID: N/A\n");
        if (si->szBinaryPath[0]) printf("Executable: %s\n", si->szBinaryPath);
        printf("\n");
    }

    printf("\nAll found services (all states): %lu\n", servicesCount);
    printf("From which running: %lu\n\n", runCount);

    free(running);
    free(pServices);
    CloseServiceHandle(hSCM);
    return 0;
}