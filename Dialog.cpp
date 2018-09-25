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
// 对话框消息出来程序
BOOL CALLBACK DialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{

	switch (uMsg)
	{
	case WM_INITDIALOG: // 初始化对话框:
		OnInitDialog(hWnd);
		//EnablePrivilege();
		break;
	case WM_COMMAND: // 点击按钮
		switch (LOWORD(wParam))
		{
		case IDC_BTN_RECORD: // 点击录制按钮
			OnRecord();
			break;
		case IDC_BTN_PLAY: // 点击播放按钮
			OnPlay();
			break;
		case IDC_BTN_PAUSE: // 点击暂停按钮
			OnResume();
			break;
		case IDC_BTN_STOP: // 点击停止按钮
			OnStop();
			break;
		case IDC_BTN_SAVE: // 点击保存按钮
			OnSave();
			break;
		case IDC_BTN_LOAD: // 点击加载按钮
			OnLoad();
			break;
		default:
			break;
		}
		break;
	case WM_CLOSE: // 关闭窗口
		DestroyWindow(hWnd);
		break;
	case WM_DESTROY: // 窗口销毁
		PostQuitMessage(0);
		break;
	}
	return FALSE;
}

//初始化
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
//录制
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
//播放
void OnPlay()
{
	UpDateData(FALSE);
	UpDateData(TRUE);
	SetState(State::PLAY);
	CreateThread(0, 0, ThreadProcess, 0, 0, 0);
	ShowWindow(hMainWnd, SW_MINIMIZE);
}
//暂停
void OnResume()
{
	SetState(State::RESUME);
}
//停止
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
//保存
void OnSave()
{
	char szFileName[MAX_PATH]={0};  
	OPENFILENAME openFileName = {0};  
	openFileName.lStructSize = sizeof(OPENFILENAME);  
	openFileName.nMaxFile = MAX_PATH;  //这个必须设置，不设置的话不会出现打开文件对话框  
	openFileName.lpstrFilter = "文本文件(*.txt)\0*.txt\0所有文件(*.*)\0*.*\0\0";  
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
//加载
void OnLoad()
{
	char szFileName[MAX_PATH]={0};  
	OPENFILENAME openFileName = {0};  
	openFileName.lStructSize = sizeof(OPENFILENAME);  
	openFileName.nMaxFile = MAX_PATH;  //这个必须设置，不设置的话不会出现打开文件对话框  
	openFileName.lpstrFilter = "文本文件(*.txt)\0*.txt\0所有文件(*.*)\0*.*\0\0";  
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