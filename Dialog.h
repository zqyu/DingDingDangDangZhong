
#include "stdafx.h"

#include <iostream>
#include <Windows.h>
#include <vector>
#include <atomic>
#include "resource.h"
#include "Commdlg.h"

// �Ի�����Ϣ��������
BOOL CALLBACK DialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
//��ʼ��
void OnInitDialog(HWND hWnd);
//¼��
void OnRecord();
//����
void OnPlay();
//��ͣ
void OnResume();
//ֹͣ
void OnStop();
//
void UpDateData(bool dir);
//����
void OnSave();
//����
void OnLoad();