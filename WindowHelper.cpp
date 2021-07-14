#include "WindowHelper.h"
#include <iostream>

LRESULT CALLBACK WindowProc(HWND hWind, UINT message, WPARAM wParam, LPARAM lParam) // Callback = executable code sent as an argument to another code
{
    switch (message)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    default:
        break;
    }

    return DefWindowProc(hWind, message, wParam, lParam);
}

bool SetupWindow(HINSTANCE instance, UINT width, UINT height, int nCmdShow, HWND& window)
{
    const wchar_t CLASS_NAME[] = L"Demo Window Class";

    WNDCLASS wc = {};

    wc.lpfnWndProc = WindowProc;
    wc.hInstance = instance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    window = CreateWindowEx(0, CLASS_NAME, L"Demo Window", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, width, height, nullptr, nullptr, instance, nullptr);

    if (window == nullptr)
    {
        std::cerr << "HWND was nullptr, last error: " << GetLastError() << std::endl;
        return false;
    }

    ShowWindow(window, nCmdShow);

    return true;
}
