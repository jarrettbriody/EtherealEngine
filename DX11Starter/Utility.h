#pragma once
#include <DirectXMath.h>
#include <string>
#include <regex>
#include "WICTextureLoader.h"
#include "DDSTextureLoader.h"
#include "Config.h"

#define ZERO_VECTOR3 XMFLOAT3(0.0f,0.0f,0.0f);
#define X_AXIS XMFLOAT3(1.0f,0.0f,0.0f);
#define Y_AXIS XMFLOAT3(0.0f,1.0f,0.0f);
#define Z_AXIS XMFLOAT3(0.0f,0.0f,1.0f);

namespace Utility {
	enum MESH_TYPE {
		LOAD_FAILURE = -1,
		DEFAULT_MESH = 0,
		GENERATED_MESH = 1,
	};

	static void ParseFloat3FromString(std::string s, DirectX::XMFLOAT3& f) {
		std::smatch match;
		int i = 0;
		while (i < 3)
		{
			std::regex_search(s, match, std::regex("^(\\d*\\.?\\d*)"));
			switch (i) {
			case 0:
				f.x = std::stof(match.str());
				break;
			case 1:
				f.y = std::stof(match.str());
				break;
			case 2:
				f.z = std::stof(match.str());
			default:
				break;
			}
			i++;
			s = std::regex_replace(s, std::regex("^(\\d*\\.?\\d* *)"), "");
		}
	}

	static void ParseFloatFromString(std::string s, float& f) {
		std::smatch match;
		if (std::regex_search(s, match, std::regex("\\d*\\.?\\d*"))) {
			f = std::stof(match[0]);
		}
	}

	static void ParseIntFromString(std::string s, int& i) {
		std::smatch match;
		if (std::regex_search(s, match, std::regex("\\d+"))) {
			i = std::stoi(match[0]);
		}
	}

	static std::wstring MultiByteToWideChar(std::string& as)
	{
		wchar_t* buf = new wchar_t[as.size() * 2 + 2];
		swprintf(buf, L"%S", as.c_str());
		std::wstring rval = buf;
		delete[] buf;
		return rval;
	}

	static ID3D11ShaderResourceView* LoadSRV(std::string texture){
		ID3D11ShaderResourceView* srv;
		wchar_t path[100] = L"../../Assets/Textures/";
		wchar_t fileName[50];
		::MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, &texture.c_str()[0], -1, &fileName[0], 50);
		DirectX::CreateWICTextureFromFile(Config::Device, Config::Context, wcsncat(path, fileName, 100), 0, &srv);
		return srv;
	}
}