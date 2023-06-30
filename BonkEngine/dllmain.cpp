// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        my_dll = hModule;
        if (AllocConsole())
        {
            FILE* f;
            freopen_s(&f, "CONIN$", "r", (__acrt_iob_func(0)));
            freopen_s(&f, "CONOUT$", "w", (__acrt_iob_func(1)));
            freopen_s(&f, "CONOUT$", "w", (__acrt_iob_func(2)));
        }
        system("cls");

        //"GameOverlayRenderer64.dll", "48 89 6C 24 ? 48 89 74 24 ? 41 56 48 83 EC 20 41 8B E8
        //"GameOverlayRenderer64.dll", "48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 41 56 41 57 48 83 EC 30 44 8B FA
        Present = D3D11PresentHook_t(find_sig("GameOverlayRenderer64.dll", "48 89 6C 24 ? 48 89 74 24 ? 41 56 48 83 EC 20 41 8B E8"));//"DiscordHook64.dll", "56 57 53 48 83 EC 30 44"
        ResizeBuffer = D3D11ResizeBufferHook_t(find_sig("GameOverlayRenderer64.dll", "48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 41 56 41 57 48 83 EC 30 44 8B FA")); //"DiscordHook64.dll", "56 57 55 53 48 83 EC 28 44 89 CD 44 89 C7 89 D3 48 89 CE E8 D8 C5 FF FF"
        
        
        GetViewPoint = GetViewPoint_t(find_sig("ShooterGame.exe", "4C 8B DC 55 57 49 8D AB A8"));//"DiscordHook64.dll", "56 57 53 48 83 EC 30 44"
        GetRecoilMultiplier = GetRecoilMultiplier_t(find_sig("ShooterGame.exe", "48 89 4C 24 08 48 83 EC 68 F3 0F 10 05 ? ? ? ? F3 0F 11 44 24 34 48 8D"));//"DiscordHook64.dll", "56 57 53 48 83 EC 30 44"
        //ClientUnlockExplorerNote = ClientUnlockExplorerNote_t(find_sig("ShooterGame.exe", "48 89 4C 24 08 48 83 EC 68 F3 0F 10 05 ? ? ? ? F3 0F 11 44 24 34 48 8D"));//"DiscordHook64.dll", "56 57 53 48 83 EC 30 44"

        DX11::hWnd = FindWindowA("UnrealWindow", NULL);

        DX11::WndProc = (WNDPROC)SetWindowLongPtr(FindWindowA("UnrealWindow", NULL), GWLP_WNDPROC, (uint64_t)DX11::MainProc);

        FName::GNames = (TNameEntryArray*)(signature("48 8B 05 ?? ?? ?? ?? 48 85 C0 75 79").instruction(3).add(7).GetPointer());
        UObject::GObjects = (TUObjectArray*)(signature("8B 05 ?? ?? ?? ?? 48 8B 5D B0").instruction(2).add(6).GetPointer());


        DetourStart(Present, Present_f);
        DetourStart(ResizeBuffer, ResizeBuffer_f);
        
        DetourStart(GetViewPoint, GetViewPoint_f);
        DetourStart(GetRecoilMultiplier, GetRecoilMultiplier_f);
        //DetourStart(ClientUnlockExplorerNote, ClientUnlockExplorerNote_f);
        break;
    case DLL_PROCESS_DETACH:
        DetourRemove(Present, Present_f);
        DetourRemove(ResizeBuffer, ResizeBuffer_f);

        DetourRemove(GetViewPoint, GetViewPoint_f);
        DetourRemove(GetRecoilMultiplier, GetRecoilMultiplier_f);


        imgui_shutdown();
        SetWindowLongPtr(FindWindowA("UnrealWindow", NULL), GWLP_WNDPROC, (uint64_t)DX11::WndProc);
        break;
    }
    return TRUE;
}