#include "stdafx.h"
#include "Dialog.h"
#include "keyBoardMouse.h"

DWORD playCount=1;

extern HWND hMainWnd;
extern Bound virtualScreenBound;
extern std::vector<Event> events;
extern std::atomic<DWORD> clockTimeEvent;

HHOOK keyHook=NULL;
HHOOK mouseHook=NULL;

char HELP_STRING[] = "F5 for start record\r\nF6 for stop record\r\nF7 for play macro\r\nF8 for stop macro\r\n\r\n\0";
// �Ի�����Ϣ��������
BOOL CALLBACK DialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{

	switch (uMsg)
	{
	case WM_INITDIALOG: // ��ʼ���Ի���:
		OnInitDialog(hWnd);
		//EnablePrivilege();
		break;
	case WM_COMMAND: // �����ť
		switch (LOWORD(wParam))
		{
		case IDC_BTN_RECORD: // ���¼�ư�ť
			OnRecord();
			break;
		case IDC_BTN_PLAY: // ������Ű�ť
			OnPlay();
			break;
		case IDC_BTN_PAUSE: // �����ͣ��ť
			OnResume();
			break;
		case IDC_BTN_STOP: // ���ֹͣ��ť
			OnStop();
			break;
		case IDC_BTN_SAVE: // ������水ť
			OnSave();
			break;
		case IDC_BTN_LOAD: // ������ذ�ť
			OnLoad();
			break;
		default:
			break;
		}
		break;
	case WM_CLOSE: // �رմ���
		DestroyWindow(hWnd);
		break;
	case WM_DESTROY: // ��������
		PostQuitMessage(0);
		break;
	}
	return FALSE;
}

//��ʼ��
void OnInitDialog(HWND hWnd)
{
	char status[]={"IDLE\0"};
	WCHAR wszClassName[512];

	//help message
	memset(wszClassName,0,sizeof(wszClassName));
	MultiByteToWideChar(CP_ACP,0,HELP_STRING,strlen(HELP_STRING)+1,wszClassName,
	sizeof(wszClassName)/sizeof(wszClassName[0]));
	SetDlgItemTextW(hWnd,IDC_EDIT_MSG,wszClassName);

	//status	
	memset(wszClassName,0,sizeof(wszClassName));
	MultiByteToWideChar(CP_ACP,0,status,strlen(status)+1,wszClassName,
	sizeof(wszClassName)/sizeof(wszClassName[0]));
	SetDlgItemTextW(hWnd,IDC_EDIT_STATUS,wszClassName);
	
	////playCount
	char cstr[10];
	memset(cstr,0,sizeof(cstr));
	sprintf_s(cstr,"%d",playCount);
	memset(wszClassName,0,sizeof(wszClassName));
	MultiByteToWideChar(CP_ACP,0,cstr,strlen(cstr)+1,wszClassName,
	sizeof(wszClassName)/sizeof(wszClassName[0]));
	SetDlgItemTextW(hWnd,IDC_EDIT_PLAYCOUNT,wszClassName);


	return;
}
void UpDateData(bool dir)
{
	if(!dir)
	{
		//playCount 
		char str[50];
		memset(str,'\0',sizeof(str));
		GetDlgItemText(hMainWnd,IDC_EDIT_PLAYCOUNT,str,50);
		playCount =atoi(str);
	}else
	{
		WCHAR wszClassName[512];
		char cstr[10];
		memset(cstr,0,sizeof(cstr));
		sprintf_s(cstr,"%d",playCount);
		memset(wszClassName,0,sizeof(wszClassName));
		MultiByteToWideChar(CP_ACP,0,cstr,strlen(cstr)+1,wszClassName,
		sizeof(wszClassName)/sizeof(wszClassName[0]));
		SetDlgItemTextW(hMainWnd,IDC_EDIT_PLAYCOUNT,wszClassName);

	}

}
//¼��
void OnRecord()
{
	if(keyHook==NULL)
	{
		keyHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, 0, 0);
	}
	if(mouseHook==NULL)
	{
		mouseHook = SetWindowsHookEx(WH_MOUSE_LL, LowLevelMouseProc, 0, 0);
	}
	virtualScreenBound.x = GetSystemMetrics(SM_XVIRTUALSCREEN);
	virtualScreenBound.y = GetSystemMetrics(SM_YVIRTUALSCREEN);
	virtualScreenBound.w = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	virtualScreenBound.h = GetSystemMetrics(SM_CYVIRTUALSCREEN);

	//clockTimeEvent = clockTime;
	clockTimeEvent = GetTickCount();
	events.clear();
	SetState(State::RECORD);

	UpDateData(FALSE);
	UpDateData(TRUE);

	ShowWindow(hMainWnd, SW_MINIMIZE);
}
//����
void OnPlay()
{
	UpDateData(FALSE);
	UpDateData(TRUE);
	SetState(State::PLAY);
	CreateThread(0, 0, ThreadProcess, 0, 0, 0);
	ShowWindow(hMainWnd, SW_MINIMIZE);
}
//��ͣ
void OnResume()
{
	SetState(State::RESUME);
}
//ֹͣ
void OnStop()
{
	if(mouseHook!=NULL)
	{
		UnhookWindowsHookEx(mouseHook);
		mouseHook=NULL;
	}
	if(keyHook!=NULL)
	{
		UnhookWindowsHookEx(keyHook);
		keyHook=NULL;
	}
	SetState(State::IDLE);
	ShowWindow(hMainWnd, SW_SHOW);
	ShowWindow(hMainWnd, SW_NORMAL);
}
//����
void OnSave()
{
	char szFileName[MAX_PATH]={0};  
	OPENFILENAME openFileName = {0};  
	openFileName.lStructSize = sizeof(OPENFILENAME);  
	openFileName.nMaxFile = MAX_PATH;  //����������ã������õĻ�������ִ��ļ��Ի���  
	openFileName.lpstrFilter = "�ı��ļ�(*.txt)\0*.txt\0�����ļ�(*.*)\0*.*\0\0";  
	openFileName.lpstrFile = szFileName;  
	openFileName.nFilterIndex = 1;  
	openFileName.Flags = OFN_PATHMUSTEXIST;  
	if (::GetOpenFileName(&openFileName))  
	{  
		 ::MessageBoxA(hMainWnd, openFileName.lpstrFile, "", MB_OK);  
	 }
	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	WCHAR wszClassName[512];
	memset(wszClassName,0,sizeof(wszClassName));
	MultiByteToWideChar(CP_ACP,0,szFileName,strlen(szFileName)+1,wszClassName,
	sizeof(wszClassName)/sizeof(wszClassName[0]));
	SetDlgItemTextW(hMainWnd,IDC_EDIT_SAVE,wszClassName);

	return;
}
//����
void OnLoad()
{
	char szFileName[MAX_PATH]={0};  
	OPENFILENAME openFileName = {0};  
	openFileName.lStructSize = sizeof(OPENFILENAME);  
	openFileName.nMaxFile = MAX_PATH;  //����������ã������õĻ�������ִ��ļ��Ի���  
	openFileName.lpstrFilter = "�ı��ļ�(*.txt)\0*.txt\0�����ļ�(*.*)\0*.*\0\0";  
	openFileName.lpstrFile = szFileName;  
	openFileName.nFilterIndex = 1;  
	openFileName.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;  
	if (::GetOpenFileName(&openFileName))  
	{  
		 ::MessageBoxA(hMainWnd, openFileName.lpstrFile, "", MB_OK);  
	 }
	//---------------------------------------------------------------------------

	//---------------------------------------------------------------------------
	WCHAR wszClassName[512];
	memset(wszClassName,0,sizeof(wszClassName));
	MultiByteToWideChar(CP_ACP,0,szFileName,strlen(szFileName)+1,wszClassName,
	sizeof(wszClassName)/sizeof(wszClassName[0]));
	SetDlgItemTextW(hMainWnd,IDC_EDIT_LOAD,wszClassName);
}