#pragma once

typedef HRESULT(__stdcall* D3D11ResizeBufferHook_t) (IDXGISwapChain* pThis, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags);
typedef HRESULT(__stdcall* D3D11PresentHook_t) (IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
extern D3D11PresentHook_t Present;
extern D3D11ResizeBufferHook_t ResizeBuffer;


namespace DX11
{
	extern HWND hWnd;
	extern HMODULE hModule;
	extern ID3D11Device* pDevice;
	extern ID3D11DeviceContext* pContext;
	extern IDXGISwapChain* pSwapChain;

	extern WNDPROC WndProc;

	LRESULT __stdcall MainProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

}

HRESULT __stdcall Present_f(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
HRESULT ResizeBuffer_f(IDXGISwapChain* pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags);

void imgui_shutdown();

extern HMODULE my_dll;

extern bool _Recoil;
extern float _FOV;