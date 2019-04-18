#pragma once
#include <DirectXMath.h>
#include <string>
#include <regex>

namespace Utility {
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
}