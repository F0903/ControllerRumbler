module;
#include <Windows.h>
#include <iostream>
#include "MacoUtils.h"
export module Console;

export void RedirectConsoleIO() {
	FILE* fp;

	if (GetStdHandle(STD_INPUT_HANDLE) != INVALID_HANDLE_VALUE)
		if (freopen_s(&fp, "CONIN$", "r", stdin) != 0)
			throw "Could not redirect STDIN to new console!";
		else
			setvbuf(stdin, NULL, _IONBF, 0);

	if (GetStdHandle(STD_OUTPUT_HANDLE) != INVALID_HANDLE_VALUE)
		if (freopen_s(&fp, "CONOUT$", "w", stdout) != 0)
			throw "Could not redirect STDOUT to new console!";
		else
			setvbuf(stdout, NULL, _IONBF, 0);

	if (GetStdHandle(STD_ERROR_HANDLE) != INVALID_HANDLE_VALUE)
		if (freopen_s(&fp, "CONOUT$", "w", stderr) != 0)
			throw "Could not redirect STDERR to new console!";
		else
			setvbuf(stderr, NULL, _IONBF, 0);

	std::ios::sync_with_stdio(true);

	std::wcout.clear();
	std::cout.clear();
	std::wcerr.clear();
	std::cerr.clear();
	std::wcin.clear();
	std::cin.clear();
}

export void ReleaseConsole() {
	FILE* fp;

	// Just to be safe, redirect standard IO to NUL before releasing.

	if (freopen_s(&fp, "NUL:", "r", stdin) != 0)
		throw "Could not redirect STDIN to NUL!";
	else
		setvbuf(stdin, NULL, _IONBF, 0);

	if (freopen_s(&fp, "NUL:", "w", stdout) != 0)
		throw "Could not redirect STDOUT to NUL!";
	else
		setvbuf(stdout, NULL, _IONBF, 0);

	if (freopen_s(&fp, "NUL:", "w", stderr) != 0)
		throw "Could not redirect STDERR to NUL!";
	else
		setvbuf(stderr, NULL, _IONBF, 0);

	if (!FreeConsole())
		throw "Could not free console!";
}

export void AdjustConsoleBuffer(int16_t minLength) {
	CONSOLE_SCREEN_BUFFER_INFO conInfo;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &conInfo);
	if (conInfo.dwSize.Y < minLength)
		conInfo.dwSize.Y = minLength;
	SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), conInfo.dwSize);
}

export void DisableConsoleInteraction() {
	const auto con = GetStdHandle(STD_INPUT_HANDLE);
	SetConsoleMode(con, ENABLE_EXTENDED_FLAGS);
}

export void CreateNewConsole(int16_t minLength) {
	ReleaseConsole();

	if (AllocConsole()) {
		AdjustConsoleBuffer(minLength);
		RedirectConsoleIO();
	}
}

export void AttachParentConsole(int16_t minLength) {
	ReleaseConsole();

	if (AttachConsole(ATTACH_PARENT_PROCESS)) {
		AdjustConsoleBuffer(minLength);
		RedirectConsoleIO();
	}
}