#pragma once
//DX11
#include <d3d11.h>
#include <DirectXMath.h>

//windows
#include <Windows.h>

//std libraries
#include <stdlib.h>
#include <algorithm>
#include <regex>
#include <vector>
#include <iostream>
#include <map>
#include <fstream>
#include <cmath>
#include <atlbase.h>

//texture loading
#include "WICTextureLoader.h"
#include "DDSTextureLoader.h"

//fmod
#include "fmod.hpp"
#include "fmod_errors.h"

//bullet
#include "btBulletDynamicsCommon.h"
#include "BulletCollision/NarrowPhaseCollision/btRaycastCallback.h"

//cascioli black box
#include "SimpleShader.h"

//define macros
#define ZERO_VECTOR3 XMFLOAT3(0.0f,0.0f,0.0f)
#define X_AXIS XMFLOAT3(1.0f,0.0f,0.0f)
#define Y_AXIS XMFLOAT3(0.0f,1.0f,0.0f)
#define Z_AXIS XMFLOAT3(0.0f,0.0f,1.0f)
#define KILOBYTE 1024
#define MEGABYTE 1048576
#define GIGABYTE 1073741824
#define MAX_KERNEL_SAMPLES 128