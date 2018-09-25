
#include "stdafx.h"

#include <iostream>
#include <Windows.h>
#include <vector>
#include <atomic>
#include "resource.h"
#include "Commdlg.h"

// 对话框消息出来程序
BOOL CALLBACK DialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
//初始化
void OnInitDialog(HWND hWnd);
//录制
void OnRecord();
//播放
void OnPlay();
//暂停
void OnResume();
//停止
void OnStop();
//
void UpDateData(bool dir);
//保存
void OnSave();
//加载
void OnLoad();