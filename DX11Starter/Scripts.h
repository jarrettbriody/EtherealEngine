#pragma once
#include "Barrel.h"
#include "FPSController.h"
#include "BloodIcicle.h"
#include "EnemyTest.h"

namespace Scripts {
	enum class SCRIPT_NAMES {
		BARREL,
		FPSCONTROLLER,
		BLOODICICLE,
		ENEMYTEST,
	};

	static map<string, SCRIPT_NAMES> scriptNameMap = {
		{"BARREL", SCRIPT_NAMES::BARREL},
		{"FPSCONTROLLER", SCRIPT_NAMES::FPSCONTROLLER},
		{"BLOODICICLE", SCRIPT_NAMES::BLOODICICLE},
		{"ENEMYTEST", SCRIPT_NAMES::ENEMYTEST}
	};

	namespace {
		static ScriptManager* Factory(Entity* e, SCRIPT_NAMES n) {
			ScriptManager* script = nullptr;

			switch (n)
			{
			case Scripts::SCRIPT_NAMES::BARREL:
				script = new TestScript();
				break;
			case Scripts::SCRIPT_NAMES::FPSCONTROLLER:
				script = new FPSController();
				break;
			case Scripts::SCRIPT_NAMES::BLOODICICLE:
				script = new BloodIcicle();
				break;
			case Scripts::SCRIPT_NAMES::ENEMYTEST:
				script = new EnemyTest();
				break;
			default:
				break;
			}

			if (script != nullptr) script->Setup(e);

			return script;
		}
	}

	static ScriptManager* CreateScript(Entity* e, string scriptNameStr) {
		SCRIPT_NAMES scriptName = SCRIPT_NAMES::BARREL;
		if (scriptNameMap.count(scriptNameStr)) {
			scriptName = scriptNameMap[scriptNameStr];
			return Factory(e, scriptName);
		}
		else return nullptr;
	}
	static ScriptManager* CreateScript(Entity* e, SCRIPT_NAMES scriptName) {
		return Factory(e, scriptName);
	}
}