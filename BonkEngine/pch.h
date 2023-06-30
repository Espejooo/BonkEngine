// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef PCH_H
#define PCH_H


#define M_PI 3.14159265358979323846
#define NOMINMAX 1
#define _SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS 1

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "Hooking/detours.lib")

// add headers that you want to pre-compile here
#include "framework.h"

#include <Windows.h>
#include <sdkddkver.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <stdexcept>
#include <type_traits>
#include <vector>
#include <map>
#include <unordered_map>
#include <thread>
#include <string>
#include <cstring>
#include <math.h>
#include <conio.h>
#include <functional>
#include <optional>
#include <d3d11.h>
#include <fstream>
#include <deque>
#include <mutex>
#include <assert.h> 
#include <filesystem>
#include <codecvt>
#include <queue> 
#include <iterator>
#include <csetjmp>
#include <unordered_map>
#include <unordered_set>



#define DetourStart(sym, des) \
DetourUpdateThread(GetCurrentThread());\
DetourTransactionBegin(); \
DetourAttach(&(LPVOID&)sym, des); \
DetourTransactionCommit(); \

#define DetourRemove(sym, des) \
DetourUpdateThread(GetCurrentThread());\
DetourTransactionBegin(); \
DetourDetach(&(LPVOID&)sym, des); \
DetourTransactionCommit(); \

//DirectX and ImGui
#include "DirectX/ImGui/imgui.h"
#include "DirectX/ImGui/imgui_impl_dx11.h"
#include "DirectX/ImGui/imgui_impl_win32.h"
#include "DirectX/ImGui/imgui_stdlib.h"
#include "DirectX/DX11.h"

//UndealEngine
#include "UnrealEngine/signature.h"
#include "UnrealEngine/UnrealEngine.h"
#include "UnrealEngine/SDK.h"

//Detours
#include "Hooking/detours.h"
#include "Hooking/Hooks.h"
#include <iostream>

#endif
