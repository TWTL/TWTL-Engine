// Main.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

int main()
{	
	DWORD select=NULL;
	DWORD32 targetKey = NULL;
	TCHAR keyName[REGNAME_MAX] = { 0, };
	TCHAR keyValue[REGVALUE_MAX] = { 0, };

	while (TRUE) {
		_tprintf_s(L"1. current snapshot\n");
		_tprintf_s(L"2. current snapshot ( txt export )\n");
		_tprintf_s(L"3. delete register key\n");
		_tprintf_s(L"\n");
		_tprintf_s(L"Type Number : ");

		scanf_s("%d", &select);
		if (select == 1) {
			if (!SnapCurrentStatus(0)) {
				ErrMsg();
				DelayWait(5000);
			}
		}
		else if (select == 2) {
			if (!SnapCurrentStatus(1)) {
				ErrMsg();
				DelayWait(5000);
			}
		}
		else if (select == 3) {
			_tprintf_s(L"Type (keyName) (target): ");
			wscanf_s(L"%s%d", &keyName, REGNAME_MAX-1, &targetKey);
			if (!DeleteRunKey(keyName, targetKey)) {
				ErrMsg();
				DelayWait(5000);
			}
		}
		else {
			break;
		}
		_tprintf_s(L"\n");
	}
	
	_tprintf_s(L"\nBye!\n");
	DelayWait(5000);
	
	return 0;
}

