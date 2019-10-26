//80*27
#define _CRT_SECURE_NO_WARNINGS
#pragma comment(lib, "WINMM.LIB")

#include <Windows.h>
#include <tchar.h>
#include <stdio.h>
//----------------------------------------
#define WIN_WIDTH 818
#define WIN_HEIGH 553
#define FPS 32



HDC g_hdc = NULL;
//HDC g_mdc = NULL;
HDC g_BufferDC = NULL;
DWORD g_tPre = 0, g_tNow = 0;
char g_screen[27][80] = {0};
FILE *g_fp = NULL;
void readframe();
int g_flag_read = 0;
int g_drop = 0;
int g_height = 19;


LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL BA_Init(HWND hwnd);
VOID BA_Paint(HWND hwnd);
BOOL BA_CleanUP(HWND hwnd);
//-----------------------------------------
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	//定义窗口类
	WNDCLASSEX wndClass = { 0 };
	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = WndProc;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = hInstance;
	//wndClass.hIcon = (HICON)::LoadImage(NULL, L"icon.ico", IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_LOADFROMFILE);//加载图标
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);//画背景色
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = _T("Bad Apple");//窗口类名字

	//注册窗口类
	if (!RegisterClassEx(&wndClass))
	{
		MessageBox(NULL, _T("Error!"), _T("Error"), MB_OK);
		return -1;
	}

	//创建窗口
	HWND hwnd = CreateWindow(_T("Bad Apple"), _T("Bad Apple"), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, WIN_WIDTH, WIN_HEIGH, NULL, NULL, hInstance, NULL);

	MoveWindow(hwnd, 250, 80, WIN_WIDTH, WIN_HEIGH, true);
	ShowWindow(hwnd, nShowCmd);
	UpdateWindow(hwnd);
	
	if (!BA_Init(hwnd))
	{
		MessageBox(NULL, _T("资源初始化失败"), _T("Message"), MB_OK);
		return FALSE;
	}

	MSG msg = { 0 };
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			g_tNow = GetTickCount();
			if (g_tNow - g_tPre >= FPS)
			{
				BA_Paint(hwnd);
			}
		}
	}

	UnregisterClass(_T("Bad Apple"), wndClass.hInstance);

	return 0;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
		BA_CleanUP(hwnd);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
	}

	return 0;
}

void readframe()
{
	int i = 0;
	int j = 0;
	char c = 0;
	for (i = 0; i < 27; i++)
	{
		for (j = 0; j < 80; j++)
		{
			if (g_flag_read == 0)
			{
				if (fscanf(g_fp, "%c", &c))
				{
					g_screen[i][j] = c;
				}
				else
				{
					g_flag_read = 1;
				}
			}
		}
		fgetc(g_fp);
	}
	//fgetc(g_fp);
}
BOOL BA_Init(HWND hwnd)
{
	g_fp = fopen(_T("text.txt"), "r");
	g_hdc = GetDC(hwnd);

	HBITMAP bmp;
	//g_mdc = CreateCompatibleDC(g_hdc);
	g_BufferDC = CreateCompatibleDC(g_hdc);
	

	bmp = CreateCompatibleBitmap(g_hdc,WIN_WIDTH,WIN_HEIGH);
	SelectObject(g_BufferDC, bmp);
	PlaySound(_T("BadApple.wav"), NULL, SND_ASYNC);
	return true;
}
VOID BA_Paint(HWND hwnd)
{
	readframe();
	g_drop++;
	if (g_drop %20 == 0)
	{
		readframe();
	}
	DeleteDC(g_BufferDC);
	g_BufferDC = CreateCompatibleDC(g_hdc);

	HFONT g_hFont;
	g_hFont = CreateFont(g_height, 0, 0, 0, 0, 0, 0, 0, GB2312_CHARSET, 0, 0, 0, 0, _T("黑体"));
	SelectObject(g_BufferDC, g_hFont);
	SetBkMode(g_BufferDC, TRANSPARENT);
	
	HBITMAP bmp = NULL;
	
	bmp = CreateCompatibleBitmap(g_hdc, WIN_WIDTH, WIN_HEIGH);
	SelectObject(g_BufferDC, bmp);
	SetTextColor(g_BufferDC, RGB(255, 255, 255));
	for (int i = 0,y=0; i < 27; i++, y += g_height)
	{
		TextOut(g_BufferDC, 0, y, g_screen[i], 80);
	}

	BitBlt(g_hdc, 0, 0, WIN_WIDTH, WIN_HEIGH, g_BufferDC, 0, 0, SRCCOPY);
	DeleteObject(g_hFont);
	DeleteObject(bmp);
	g_tPre = GetTickCount();
}
BOOL BA_CleanUP(HWND hwnd)
{
	DeleteDC(g_BufferDC);
	return true;
}