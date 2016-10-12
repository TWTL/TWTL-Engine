// MonitorFunc.cpp : Functions of monitor.

#include "stdafx.h"
#include "MonitorFunc.h"

DWORD __stdcall OpenRegisteryKey(PREGINFO CONST pReg, CONST DWORD32 target);
BOOL __stdcall SetTargetRegistryEntry(FILE* storage, PREGINFO CONST pReg, CONST DWORD32 target, CONST DWORD32 mode);
BOOL __stdcall WriteRegToTxt(FILE* storage, PREGINFO CONST pReg, CONST DWORD32 mode);

/*
	Description : Write current process entry and register ( Run ) 
				  in text file. ( txt export )

	Parameters : 
		( in )	DWORD32 mode :
			0 -> just print
			1 -> txt export
			2 -> push data to database
			else -> error
	Return value :
		0 = Error
		1 = Success
*/
TWTL_SNAPSHOT_API BOOL __stdcall SnapCurrentStatus(CONST DWORD32 mode) {
	FILE* storage = NULL;

	HANDLE hSnap;
	PROCESSENTRY32 proc32;

	LPWSTR Pname = { 0, };
	CHAR fileName[MAX_PATH] = "Snapshot";

	REGINFO reg;
	PREGINFO pReg = &reg;
	reg.bufSize = REGNAME_MAX - 1;
	reg.dataType = REG_SZ;

	if (!MakeFileName(fileName)) {
		return NULL;
	}
	if (mode == 0) {

	}
	else if (mode == 1) {
		if (fopen_s(&storage, fileName, "w")) {
			return NULL;
		}
	}
	else if (mode == 2) {

	}
	else {
		return NULL;
	}

	if ((hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0))
		== INVALID_HANDLE_VALUE) {
		if (mode == 1) {
			fclose(storage);
		}
		return NULL;
	}

	else {
		proc32.dwSize = sizeof(PROCESSENTRY32);
		DWORD32 PID = 0;
		if (Process32First(hSnap, &proc32)) {
			if (mode == 1) {
				fwprintf_s(storage, L"<Process List>\n");
			}
			// First Process' ID'll be 0, 
			// so if calling Process32Next before writing, it'll be valid process.
			//

			while ((Process32Next(hSnap, &proc32)))
			{
				if (mode == 0) {
					_tprintf_s(L"Process name : %s, PID : %d\n", proc32.szExeFile, proc32.th32ProcessID);
				}
				else if (mode == 1) {
					PrintCUI(proc32.szExeFile);
					fwprintf_s(storage, L"%s\n", &proc32.szExeFile);
				}
				else if (mode == 2) {

				}
			}
			if (mode == 1) {
				fwprintf_s(storage, L"</Process List>\n\n");
			}
		}
		CloseHandle(hSnap);
	}

	// Write value of register key value ( Run of current user )
	//

	if (!SetTargetRegistryEntry(storage, pReg, 1, mode)) {
		return NULL;
	}
	if (!SetTargetRegistryEntry(storage, pReg, 2, mode)) {
		return NULL;
	}
	if (!SetTargetRegistryEntry(storage, pReg, 3, mode)) {
		return NULL;
	}
	if (!SetTargetRegistryEntry(storage, pReg, 4, mode)) {
		return NULL;
	}
	if (mode == 1) {
		fclose(storage);
	}
	return TRUE;
}

/*
	Description : delete target registry value

	Parameter :
		( in )	TCHAR keyName[REGNAME_MAX] : deleted value's name
		( in )	DWORD32 targetKey  : 
			1 -> HKEY_CURRENT_USER/Microsoft/Windows/CurrentVersion/Run
			2 -> HKEY_LOCAL_MACHINE/Microsoft/Windows/CurrentVersion/Run
			3 -> HKEY_CURRENT_USER/Microsoft/Windows/CurrentVersion/RunOnce
			4 -> HKEY_LOCAL_MACHINE/Microsoft/Windows/CurrentVersion/RunOnce
			else -> Error
	Return value :
		0 = Error
		1 = Success
*/
TWTL_SNAPSHOT_API BOOL __stdcall DeleteRunKey(TCHAR CONST keyName[REGNAME_MAX], CONST DWORD32 targetKey) {
	REGINFO reg;
	PREGINFO pReg = &reg;
	
	wcscpy_s(pReg->keyName, sizeof(pReg->keyName), keyName);
	
	DWORD32 target = targetKey;
	
	if (!OpenRegisteryKey(pReg, target)) {		
		if (!RegDeleteValue(pReg->key, pReg->keyName)) {
			_tprintf_s(L"Delete Value Name : %s\n", pReg->keyName);
		}
		else {
			_tprintf_s(L"Can't find value :(\n");
		}
	}
	else {
		return NULL;
	}
	_tprintf_s(L"\n");
	return TRUE;
}

/*
	Description : open register key

	Parameter :
		( in )	PREGINFO pReg : information of target entry
		( in )	DWORD target  :
			1 -> HKEY_CURRENT_USER/Microsoft/Windows/CurrentVersion/Run
			2 -> HKEY_LOCAL_MACHINE/Microsoft/Windows/CurrentVersion/Run
			3 -> HKEY_CURRENT_USER/Microsoft/Windows/CurrentVersion/RunOnce
			4 -> HKEY_LOCAL_MACHINE/Microsoft/Windows/CurrentVersion/RunOnce
			else -> Error
	return value :
		0 = Success
		else = Error
*/
DWORD __stdcall OpenRegisteryKey(PREGINFO CONST pReg, CONST DWORD32 target) {
	HKEY mainEntry;

	if (target == 1 || target == 3) {
		mainEntry = HKEY_CURRENT_USER;
	}
	else if (target == 2 || target == 4) {
		mainEntry = HKEY_LOCAL_MACHINE;
	}
	else {
		return 1;
	}

	if (target == 1 || target == 2) {
		return	RegOpenKeyEx(mainEntry,
				L"Software\\Microsoft\\Windows\\CurrentVersion\\Run",
				NULL,
				KEY_ALL_ACCESS,
				&pReg->key);
	}
	else if (target == 3 || target == 4) {
		return	RegOpenKeyEx(mainEntry,
				L"Software\\Microsoft\\Windows\\CurrentVersion\\RunOnce",
				NULL,
				KEY_ALL_ACCESS,
				&pReg->key);
	}
	else {
		return 1;
	}
}

/*
	Description : set target registry entry

	Parameter :
		( out ) FILE* storage : written txt file
		( in )	PREGINFO pReg : information of target entry
		( in )	DWORD target  : 
			1 -> HKEY_CURRENT_USER/Microsoft/Windows/CurrentVersion/Run
			2 -> HKEY_LOCAL_MACHINE/Microsoft/Windows/CurrentVersion/Run
			3 -> HKEY_CURRENT_USER/Microsoft/Windows/CurrentVersion/RunOnce
			4 -> HKEY_LOCAL_MACHINE/Microsoft/Windows/CurrentVersion/RunOnce
			else -> Error
		( in )	DWORD32 mode :
			0 -> just print
			1 -> txt export
			2 -> push data to database
			else -> error
	Return value :
		0 = Error
		1 = Success
*/
BOOL __stdcall SetTargetRegistryEntry(FILE* storage, PREGINFO CONST pReg, CONST DWORD32 target, CONST DWORD32 mode) {
	if (!OpenRegisteryKey(pReg, target))
	{
		if (mode == 0) {
			if (!WriteRegToTxt(storage, pReg, mode)) {
				return NULL;
			}
		}
		else if (mode == 1) {
			if (target == 1) {
				fwprintf_s(storage, L"<Register Run List ( for current users )>\n");
			}
			else if (target == 2) {
				fwprintf_s(storage, L"<Register Run List ( for all users )>\n");
			}
			else if (target == 3) {
				fwprintf_s(storage, L"<Register RunOnce List ( for current users )>\n");
			}
			else if (target == 4) {
				fwprintf_s(storage, L"<Register RunOnce List ( for all users )>\n");
			}
			else {
				return NULL;
			}

			// call writing entry of target
			//
			if (!WriteRegToTxt(storage, pReg, mode)) {
				return NULL;
			}

			if (target == 1) {
				fwprintf_s(storage, L"</Register Run List ( for current users )>\n\n");
			}
			else if (target == 2) {
				fwprintf_s(storage, L"</Register Run List ( for all users )>\n\n");
			}
			else if (target == 3) {
				fwprintf_s(storage, L"</Register RunOnce List ( for current users )>\n\n");
			}
			else if (target == 4) {
				fwprintf_s(storage, L"</Register RunOnce List ( for all users )>\n\n");
			}
			else {
				return NULL;
			}
		}
		else if (mode == 2) {

		}
		else {
			return NULL;
		}

		RegCloseKey(pReg->key);
	}
	else {
		fwprintf_s(storage, L"Can't get register key. :(\n");
		if (mode == 1) {
			fclose(storage);
		}
		return NULL;
	}

	return TRUE;
}

/*
	Description : Write register value in text file. ( txt export )

	Parameters : 
		( out ) FILE* storage : written by this function.
		( in )	PREGINFO pReg : information of target register entry.
		( in )	DWORD32 mode :
			0 -> just print
			1 -> txt export
			2 -> push data to database
			else -> error
	Return value :
		0 = Error
		1 = Success
*/
BOOL __stdcall WriteRegToTxt(FILE* storage, PREGINFO CONST pReg, CONST DWORD32 mode) {
	DWORD result = 0;

	for (int i = 0; result == ERROR_SUCCESS; i++)
	{
		result = RegEnumValue(pReg->key, i, pReg->keyName, &pReg->bufSize, NULL, NULL, NULL, NULL);

		if (result == ERROR_SUCCESS)
		{
			// must initialize reg.bufsize
			//
			if (!InitRegSize(pReg, 2)) {
				return NULL;
			}

			if (RegQueryValueEx(pReg->key,
				pReg->keyName,
				NULL,
				&pReg->dataType,
				(LPBYTE)pReg->keyValue,
				&pReg->bufSize)
				!= 0)
			{
				return NULL;
			}
			// must initialize reg.bufsize
			//
			if (!InitRegSize(pReg, 1)) {
				return NULL;
			}
			if (mode == 0) {
				_tprintf_s(L"Register Name : %s, Value : %s\n", pReg->keyName, pReg->keyValue);
			}
			else if (mode == 1) {
				PrintCUI(pReg->keyName);
				PrintCUI(pReg->keyValue);
				fwprintf_s(storage, L"%s, ", pReg->keyName);
				fwprintf_s(storage, L"%s\n", pReg->keyValue);
			}
			else if (mode == 2) {

			}
			else{
				return NULL;
			}
		}
	}
	return TRUE;
}