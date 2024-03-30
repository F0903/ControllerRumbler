#pragma once
#include <Windows.h>
#include <iostream>

#define PRINTL(expr) std::cout << expr << '\n';

#define DISPLAY_ERROR(msg) MessageBox(NULL, msg, L"Error!", MB_OK | MB_ICONERROR | MB_SYSTEMMODAL)

#define ASSERT(expr, msg) if (expr != S_OK) DISPLAY_ERROR(msg)