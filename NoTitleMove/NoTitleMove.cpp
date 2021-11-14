// NoTitleMove.cpp : 定义应用程序的入口点。

#include "framework.h"
#include "NoTitleMove.h"

constexpr int MAX_LOADSTRING = 128;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    WCHAR szTitle[MAX_LOADSTRING];       // 标题栏文本
    WCHAR szWindowClass[MAX_LOADSTRING]; // 主窗口类名

    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_NOTITLEMOVE, szWindowClass, MAX_LOADSTRING);

    WNDCLASSEXW wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = NULL;

    ::RegisterClassExW(&wcex);

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_POPUP | WS_SYSMENU | WS_SIZEBOX,
                              300, 300, 300, 300, nullptr, nullptr, hInstance, nullptr);
    if (hWnd == NULL)
    {
        ::MessageBox(NULL, _T("创建窗口出错"), _T("Error"), MB_OK);
        return -1;
    }

    ::ShowWindow(hWnd, nCmdShow);
    ::UpdateWindow(hWnd);

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_NOTITLEMOVE));
    MSG msg;
    while (::GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        // TODO: 在此处添加使用 hdc 的任何绘图代码...
        EndPaint(hWnd, &ps);
    }
    break;
    case WM_NCHITTEST:
    {
        UINT nHitTest;
        nHitTest = ::DefWindowProc(hWnd, msg, wParam, lParam);
        // 如果鼠标左键按下, GetAsyncKeyState 的返回值小于0
        if (nHitTest == HTCLIENT && ::GetAsyncKeyState(MK_LBUTTON) < 0)
        {
            nHitTest = HTCAPTION;
        }
        return nHitTest;
    }

    case WM_DESTROY:
    {
        ::PostQuitMessage(0);
        return 0;
    }
    }
    return ::DefWindowProc(hWnd, msg, wParam, lParam);
}