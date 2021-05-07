#pragma once
#include "pch.h"
#include "Config.h"

namespace Utility {
	struct Callback {
		virtual void CallbackFunc() {};
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

	/*
	static std::wstring MultiByteToWideChar(std::string& as)
	{
		wchar_t* buf = new wchar_t[as.size() * 2 + 2];
		swprintf(buf, L"%S", as.c_str());
		std::wstring rval = buf;
		delete[] buf;
		return rval;
	}
	*/

	static ID3D11ShaderResourceView* LoadSRV(std::string texture, ID3D11Resource** texturePtr = nullptr) {
		ID3D11ShaderResourceView* srv;
		wchar_t path[100] = L"../../Assets/Textures/";
		wchar_t fileName[50];
		::MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, &texture.c_str()[0], -1, &fileName[0], 50);
		DirectX::CreateWICTextureFromFile(Config::Device, Config::Context, wcsncat(path, fileName, 100), 0, &srv);
		if (texturePtr != nullptr) {
			HRESULT hr = DirectX::CreateWICTextureFromFileEx(
				Config::Device,
				path,
				INT_MAX,
				D3D11_USAGE_STAGING,
				0,
				D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ,
				0,
				0,
				texturePtr,
				0);
		}
		return srv;
	}

	static ID3D11ShaderResourceView* LoadDDSSRV(std::string texture) {
		ID3D11ShaderResourceView* srv;
		wchar_t path[100] = L"../../Assets/Textures/SKYBOXES/";
		wchar_t fileName[50];
		::MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, &texture.c_str()[0], -1, &fileName[0], 50);
		DirectX::CreateDDSTextureFromFile(Config::Device, wcsncat(path, fileName, 100), 0, &srv);
		return srv;
	}

	static LPCWSTR StringToWideString(std::string str) {
		wchar_t* wideBuf = new wchar_t[100];
		::MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, str.c_str(), -1, wideBuf, 100);
		return wideBuf;
	}

	static float DegToRad(float deg) {
		return (deg * DirectX::XM_PI) / 180.0f;
	}

	static float RadToDeg(float rad) {
		return (rad * 180.0f) / DirectX::XM_PI;
	}

	static void GenerateSSAOKernel(unsigned int sampleCount, DirectX::XMFLOAT4* kernel) {
		//kernel.reserve(sampleCount);
		DirectX::XMVECTOR calculableVector;
		for (size_t i = 0; i < sampleCount; i++)
		{
			const float x = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
			const float y = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
			const float z = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
			kernel[i] = DirectX::XMFLOAT4(x, y, z, 1.0f);
			DirectX::XMFLOAT4& currentSample = kernel[i];
			calculableVector = DirectX::XMLoadFloat4(&currentSample);
			calculableVector = DirectX::XMVector4Normalize(calculableVector);
			const float scalar = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
			calculableVector = DirectX::XMVectorScale(calculableVector, scalar);
			DirectX::XMStoreFloat4(&currentSample, calculableVector);
		}
	}

	static DirectX::XMFLOAT3 BulletVectorToFloat3(btVector3 vec)
	{
		return DirectX::XMFLOAT3(vec.getX(), vec.getY(), vec.getZ());
	}

	static btVector3 Float3ToBulletVector(DirectX::XMFLOAT3 vec)
	{
		return btVector3(vec.x, vec.y, vec.z);
	}

	static float FloatLerp(float a, float b, float f)
	{
		return a + f * (b - a);
	}

	static float ClipValue(float n, float lower, float upper)
	{
		return max(lower, min(n, upper));
	}

	static btCollisionWorld::ClosestRayResultCallback BulletRaycast(btVector3 from, btVector3 to)
	{
		Config::DynamicsWorld->updateAabbs();
		Config::DynamicsWorld->computeOverlappingPairs();

		// Create variable to store the ray hit and set flags
		btCollisionWorld::ClosestRayResultCallback closestResult(from, to);
		closestResult.m_flags &= btTriangleRaycastCallback::kF_FilterBackfaces;

		Config::DynamicsWorld->rayTest(from, to, closestResult); // Raycast

		return closestResult;
	}
}