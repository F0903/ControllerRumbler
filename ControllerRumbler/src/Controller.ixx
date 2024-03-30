module;
#include <Windows.h>
#include <GameInput.h>
#include <wrl.h>
#include "MacoUtils.h"
export module Controller;

using namespace Microsoft::WRL;

export struct ButtonStates //TODO: too lazy to fill the rest
{
	bool A, B, X, Y;
	bool up, down, left, right;

	bool operator==(const ButtonStates& other) {
		return
			A == other.A &&
			B == other.B &&
			X == other.X &&
			Y == other.Y &&
			up == other.up &&
			down == other.down &&
			left == other.left && 
			right == other.right;
	}
};

export class Controller
{
	IGameInput* gInput;

	ComPtr<IGameInputDevice> device;

	bool rumbleEnabled = true;
	bool highFreqEnabled = true;
	float currentHighFreqIntensity = 1.0f;
	bool lowFreqEnabled = false;
	float currentLowFreqIntensity = 1.0f;

	bool disconnected = false;

public:
	Controller(IGameInput* gInput, IGameInputDevice* device) : gInput(gInput), device(device) {
		UpdateRumble();
	}

private:
	void UpdateRumble() noexcept {
		GameInputRumbleParams params{
			.lowFrequency = currentLowFreqIntensity * lowFreqEnabled * rumbleEnabled,
			.highFrequency = currentHighFreqIntensity * highFreqEnabled * rumbleEnabled,
		};
		device->SetRumbleState(&params);
	}

public:
	inline bool IsDisconnected() const noexcept {
		return disconnected;
	}

	inline void Disconnect() noexcept {
		disconnected = true;
	}

	void ToggleRumbleEnabled() {
		if (disconnected) throw "Controller has been disconnected.";
		rumbleEnabled = !rumbleEnabled;
		UpdateRumble();
		PRINTL("Set rumble to " << rumbleEnabled);
	}

	void ToggleHighFreq() {
		if (disconnected) throw "Controller has been disconnected.";
		highFreqEnabled = !highFreqEnabled;
		UpdateRumble();
		PRINTL("Set high freq rumble to " << highFreqEnabled);
	}

	void ToggleLowFreq() {
		if (disconnected) throw "Controller has been disconnected.";
		lowFreqEnabled = !lowFreqEnabled;
		UpdateRumble();
		PRINTL("Set low freq rumble to " << lowFreqEnabled);
	}

	void IncrementLowFreqRumbleIntensity(float increment) {
		if (disconnected) throw "Controller has been disconnected.";
		if (currentLowFreqIntensity + increment > 1.0f) {
			currentLowFreqIntensity = 1.0f;
		}
		else if (currentLowFreqIntensity + increment < 0.0f) {
			currentLowFreqIntensity = 0.0f;
		}
		else currentLowFreqIntensity += increment;

		UpdateRumble();
		PRINTL("Set low freq rumble intensity to " << currentLowFreqIntensity);
	}

	void IncrementHighFreqRumbleIntensity(float increment) {
		if (disconnected) throw "Controller has been disconnected.";
		if (currentHighFreqIntensity + increment > 1.0f) {
			currentHighFreqIntensity = 1.0f;
		}
		else if (currentHighFreqIntensity + increment < 0.0f) {
			currentHighFreqIntensity = 0.0f;
		}
		else currentHighFreqIntensity += increment;

		UpdateRumble();
		PRINTL("Set high freq rumble intensity to " << currentHighFreqIntensity);
	}

	//TODO: Use reading callback when it has been implemented.
	ButtonStates GetButtons() const {
		if (disconnected) throw "Controller has been disconnected.";
		IGameInputReading* inputReading;
		ASSERT(gInput->GetCurrentReading(GameInputKindController, device.Get(), &inputReading), L"Could not read input!");

		const auto buttonCount = inputReading->GetControllerButtonCount();
		if (buttonCount != 14)
			throw "Controller does not have 14 buttons. Currently unsupported!";

		bool* buttonStates = reinterpret_cast<bool*>(alloca(sizeof(bool) * buttonCount));
		const auto validStateEntries = inputReading->GetControllerButtonState(buttonCount, buttonStates);

		// This obv. wont work for all controllers.
		auto buttons = ButtonStates{
			.A = buttonStates[0],
			.B = buttonStates[1],
			.X = buttonStates[2],
			.Y = buttonStates[3],
			.up = buttonStates[10],
			.down = buttonStates[12],
			.left = buttonStates[13],
			.right = buttonStates[11],
		};

		return buttons;
	}
};