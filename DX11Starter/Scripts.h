#pragma once
#include "Barrel.h"
#include "FPSController.h"

namespace Scripts {
	enum class SCRIPT_NAMES {
		BARREL,
		FPSCONTROLLER,
	};

	static ScriptManager* CreateScript(SCRIPT_NAMES scriptName, Entity* e) {
		ScriptManager* script = nullptr;

		switch (scriptName)
		{
		case Scripts::SCRIPT_NAMES::BARREL:
			script = new TestScript();
			break;
		case Scripts::SCRIPT_NAMES::FPSCONTROLLER:
			script = new FPSController();
			break;
		default:
			break;
		}

		if (script != nullptr) script->Setup(e);

		return script;
	}
}