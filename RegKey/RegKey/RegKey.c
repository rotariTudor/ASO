#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <winreg.h>

#pragma comment(lib, "Advapi32.lib")
#define nMAX_KEY_LENGTH 256
#define c_szSubkeyName "SOFTWARE\\Microsoft"

int fnShowSubkeys(HKEY hRootKey, const char* pszCale);

int main(void) {
	int nRezultat;

	nRezultat = fnShowSubkeys(HKEY_LOCAL_MACHINE, c_szSubkeyName);

	return nRezultat;
}

int fnShowSubkeys(HKEY hRootKey, const char* pszCale) {
	HKEY hKey;
	LONG lResult;

	DWORD dwIndex = 0;
	DWORD dwNameSize;

	char szSubkeyName[nMAX_KEY_LENGTH];

	lResult = RegOpenKeyExA(
		hRootKey, //[in] hKey
		pszCale,  //[in] lpSubKey
		0,        //[in] ulOptions - 0 opt standard
		KEY_READ, //[in] samDesired - readOnly
		&hKey     //[out] phkResult
	);

	ERROR_SUCCESS == lResult ? printf("Registry key opened successfully.\n") : printf("Error opening registry key: %ld\n", lResult);

	if (lResult == ERROR_FILE_NOT_FOUND){
		fprintf(stderr, "Registry key not found: %s\n", pszCale);
	}

	printf("Subkeys of %s:\n", pszCale);

	while (1) {
		dwNameSize = nMAX_KEY_LENGTH;

		lResult = RegEnumKeyExA(
			hKey,           // handle-ul cheii deschise   
			dwIndex,        // indexul subcheii curente   
			szSubkeyName,   // buffer pentru nume         
			&dwNameSize,    // dimensiunea bufferului     
			NULL,           // rezervat - NULL            
			NULL,           // clasa - NULL               
			NULL,           // dimensiunea clasei - NULL  
			NULL            // ultima scriere - NULL      
		);

		if (lResult == ERROR_NO_MORE_ITEMS) {
			break; // Nu mai sunt subchei de enumera
		}
		if(lResult == ERROR_SUCCESS) {
			printf("[%lu] %s\n", dwIndex+1, szSubkeyName);
		}
		dwIndex++;
	}

	printf("Total subkeys: %lu\n", dwIndex);
	RegCloseKey(hKey);

	return EXIT_SUCCESS;
}