module;
#include <Windows.h>
#include <GameInput.h>
#include <wrl.h>
#include <memory>
#include <condition_variable>
#include "MacoUtils.h"
export module InputManager;

import Controller;

using namespace Microsoft::WRL;

export class InputManager
{
	ComPtr<IGameInput> gInput;

	std::unique_ptr<Controller> selectedController;
	std::mutex controllerWaiterMutex;
	std::condition_variable controllerWaiter;

public:
	InputManager() noexcept {
		ASSERT(GameInputCreate(&gInput), L"Could not create GameInput interface! Is it installed?");

		PRINTL("Setting up input manager...");
		GameInputCallbackToken deviceCallbackToken;
		const auto callbackResult = gInput->RegisterDeviceCallback(NULL, GameInputKindController, GameInputDeviceAnyStatus, GameInputBlockingEnumeration, this, &DeviceCallback, &deviceCallbackToken);
		ASSERT(callbackResult, L"Could not setup device callback!");
	}

private:
	static void DeviceCallback(GameInputCallbackToken callbackToken, void* context, IGameInputDevice* device, uint64_t timestamp, GameInputDeviceStatus currentStatus, GameInputDeviceStatus previousStatus) noexcept {
		InputManager* me = reinterpret_cast<InputManager*>(context);

		if (currentStatus == 0 && previousStatus != 0) {
			PRINTL("Controller has been disconnected!");
			me->selectedController->Disconnect();
			me->selectedController = nullptr;
			return;
		}

		const auto info = device->GetDeviceInfo();
		PRINTL("Device connected. ID: " << info->deviceId.value);
		me->selectedController = std::make_unique<Controller>(me->gInput.Get(), device);
		me->controllerWaiter.notify_all();
	}

public:
	Controller* GetConnectedController() noexcept {
		PRINTL("Waiting for controller...");
		std::unique_lock<std::mutex> lock(controllerWaiterMutex);
		controllerWaiter.wait(lock, [this]{ return selectedController != nullptr; });
		return selectedController.get();
	}
};