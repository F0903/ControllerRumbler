#include <Windows.h>
#include <GameInput.h>
#include <iostream>
#include "MacoUtils.h"

import Console;
import InputManager;

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ char* pCmdLine, _In_ int nCmdShow) {
	CreateNewConsole(256);
	DisableConsoleInteraction();

	auto inputManager = InputManager();
	auto controller = inputManager.GetConnectedController();

	ButtonStates lastButtons;
	while (true) {
		if (controller->IsDisconnected()) controller = inputManager.GetConnectedController();
		const auto buttons = controller->GetButtons();
		if (buttons != lastButtons) {
			if (buttons.A) {
				controller->ToggleRumbleEnabled();
			}
			else if (buttons.B) {
				controller->ToggleLowFreq();
			}
			else if (buttons.Y) {
				controller->ToggleHighFreq();
			}
			else if (buttons.up) {
				controller->IncrementLowFreqRumbleIntensity(0.1f);
			}
			else if (buttons.down) {
				controller->IncrementLowFreqRumbleIntensity(-0.1f);
			}
			else if (buttons.left) {
				controller->IncrementHighFreqRumbleIntensity(0.1f);
			}
			else if (buttons.right) {
				controller->IncrementHighFreqRumbleIntensity(-0.1f);
			}
		}

		lastButtons = buttons;
		Sleep(100);
	}
}