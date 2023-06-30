#include "pch.h"
#include "DX11.h"
#include "Font.h"


D3D11PresentHook_t Present;
D3D11ResizeBufferHook_t ResizeBuffer;
#include "ImGui/imgui_internal.h"

IMGUI_IMPL_API LRESULT  ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

bool should_ignore_msg(UINT msg)
{
	switch (msg)
	{
	case WM_MOUSEACTIVATE:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MBUTTONDBLCLK:
	case WM_MOUSEMOVE:
	case WM_NCHITTEST:
	case WM_MOUSEWHEEL:
	case WM_MOUSEHOVER:
	case WM_ACTIVATEAPP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_RBUTTONDBLCLK:
	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_CHAR:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_LBUTTONDBLCLK:
	case WM_SETCURSOR:
		return true;
	default:
		return false;
	}
}

HMODULE my_dll;

//Main
bool _RenderMenu = false;

//Aimbot
bool _Aimlock = false;
bool _FovAimbot = false;
bool _SilentAimbot = false;

//Visuals
bool _EnableESP = false;

bool _TeamESP = false;
static float _TeamColor[4] = { 0.0f, 1.0f, 0.0f, 1.0f };
bool _TeamSnaplines = false;

bool _AllyESP = false;
static float _AllyColor[4] = { 0.0f, 0.0f, 1.0f, 1.0f };
bool _AllySnaplines = false;

bool _EnemyESP = false;
static float _EnemyColor[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
bool _EnemySnaplines = false;

bool _DinoESP = false;
static float _DinoColor[4] = { 0.56f, 0.93f, 0.56f, 1.0f };
bool _DinoSnaplines = false;

bool _TurretESP = false;
static float _TurretColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

bool _StructureESP = false;
static float _StructureColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
int _StructureCount;

bool _ContainerESP = false;
static float _ContainerColor[4] = { 0.4f, 0.4f, 0.4f, 1.0f };

bool _SupplyCrateESP = false;
static float _SupplyCrateColor[4] = { 0.0f, 1.0f, 0.0f, 1.0f };

bool _DeadBodyESP = false;
static float _DeadBodyColor[4] = { 0.0f, 1.0f, 0.0f, 1.0f };
bool _DeadBodySnaplines = false;

//Misc
bool _Recoil = false;
float _FOV = 90.0f;
bool _ServerInfo = false;


namespace Fonts
{
	ImFont* esp_Font;
}


inline void RenderCollapseFriendlyDisplayList(ImVec2 ScreenStartPosition, ImColor Color, std::vector<std::string> DisplayArray, float HeightFactor)
{
	auto StartPosition = *reinterpret_cast<const ImVec2*>(&ScreenStartPosition);
	for (size_t s = 0; s < DisplayArray.size(); s++)
	{
		auto& DisplayString = DisplayArray[s];
		if (DisplayString.length() < 1) continue;
		auto CurrentPosition = ImVec2(StartPosition.x, StartPosition.y);
		auto StringSize = ImGui::CalcTextSize(DisplayString.c_str());
		CurrentPosition.y += StringSize.y;
		ImGui::GetBackgroundDrawList()->AddText(Fonts::esp_Font, 16, ImVec2(CurrentPosition.x, StartPosition.y), Color, DisplayString.c_str(), DisplayString.c_str() + strlen(DisplayString.c_str()), 500);
		StartPosition.y += HeightFactor;
	}
}

void RenderLine(FVector2D start, FVector2D end, ImColor color, float size)
{
	ImGui::GetBackgroundDrawList()->AddLine(ImVec2(start.X, start.Y), ImVec2(end.X, end.Y), color, size);
}

void RenderText(FVector2D pos, ImColor color,  const char* string)
{
	ImGui::GetBackgroundDrawList()->AddText(Fonts::esp_Font, 16, ImVec2(pos.X, pos.Y), color, string);
}



void ServerInfo()
{
	std::vector<std::string> DisplayStrings{};


	/*int PlayerActors = UWorld::Singleton()->GameState->NumPlayerActors;
	DisplayStrings.push_back("Actors: " + std::to_string((int)PlayerActors));

	int PlayersConnected = UWorld::Singleton()->GameState->NumPlayerConnected;
	DisplayStrings.push_back("Players: " + std::to_string((int)PlayersConnected));*/

	auto IO = ImGui::GetIO();
	DisplayStrings.push_back("Client FPS: " + std::to_string((int)IO.Framerate));

	/*int ServerFPS = UWorld::Singleton()->GameState->ServerFramerate;
	DisplayStrings.push_back("Server FPS: " + std::to_string((int)ServerFPS));

	int DayNumber = UWorld::Singleton()->GameState->DayNumber;
	DisplayStrings.push_back("Day: " + std::to_string((int)DayNumber));

	float TimeDay = UWorld::Singleton()->GameState->DayTime;
	DisplayStrings.push_back("Time of Day: " + std::to_string((float)TimeDay));

	double TimeNetwork = UWorld::Singleton()->GameState->NetworkTime;
	DisplayStrings.push_back("Network Time: " + std::to_string((double)TimeNetwork));

	int TamedDinos = UWorld::Singleton()->GameState->NumTamedDinos;
	DisplayStrings.push_back("Tamed Dinos: " + std::to_string((int)TamedDinos));

	int TamedDinoMax = UWorld::Singleton()->GameState->MaxTamedDinos;
	DisplayStrings.push_back("Max Dinos: " + std::to_string((int)TamedDinoMax));*/

	RenderCollapseFriendlyDisplayList(ImVec2(10, 22), ImColor(255, 255, 255, 255), DisplayStrings, 12.f);

	//RenderText((10,22), {_StructureColor[0], _StructureColor[1], _StructureColor[2], _StructureColor[3]}, ("Day: " + std::to_string((int)DayNumber));
}

void RenderESP()
{
	if (!UWorld::Singleton())
		return;

	if (!ULevel::Singleton())
		return;

	auto Levels = UWorld::Singleton()->Levels;
	if (!Levels.Data)
		return;
	for (auto levelIndex = 0; levelIndex < Levels.Num(); levelIndex++)
	{
		auto level = Levels[levelIndex];
		if (!level)
			continue;

		for (int index{ 0 }; index < level->AActor.Num(); index++)
		{
			auto actor = level->AActor[index];

			if (!actor)
				continue;
			if (!actor->RootComponent)
				continue;
			if (!APlayerController::Singleton())
				continue;

			auto location = actor->RootComponent->RelativeLocation;

			auto _Names = actor->GetName().data();

			FVector2D out;
			if (!APlayerController::Singleton()->ProjectWorldLocationToScreen(location, out))
				continue;

			//if (actor->IsA(APrimalDinoCharacter::StaticClass()))
			if (actor->IsDino())
			{

				if (_DinoESP) {
					RenderText(out, { _DinoColor[0], _DinoColor[1], _DinoColor[2], _DinoColor[3] }, _Names);
				}

				if (_DinoSnaplines) {
					RenderLine(out, { 1920 / 2, 1080 }, { _DinoColor[0], _DinoColor[1], _DinoColor[2], _DinoColor[3] }, 1);
				}
			}

			//else if (actor->IsA(APrimalCharacter::StaticClass()))
			else if(actor->IsPlayer() && actor->IsConscious() && !actor->IsDead())
			{

				if (_TeamESP) {
					auto _Health = reinterpret_cast<APrimalCharacter*>(actor);
					auto _Weight = reinterpret_cast<AShooterCharacter*>(actor);
					auto _Hexagons = reinterpret_cast<AShooterCharacter*>(actor);
					
					_Health->ReplicatedCurrentHealth;
					_Weight->ReplicatedWeight;
					_Hexagons->PlayerHexagonCount;
					RenderText(out, { _TeamColor[0], _TeamColor[1], _TeamColor[2], _TeamColor[3] }, _Names);
					RenderText({ out.x, out.y + 12 }, { _TeamColor[0], _TeamColor[1], _TeamColor[2], _TeamColor[3] }, ("Health: " + std::to_string(_Health->ReplicatedCurrentHealth)).data());
					RenderText({ out.x, out.y + 24 }, { _TeamColor[0], _TeamColor[1], _TeamColor[2], _TeamColor[3] }, ("Weight: " + std::to_string(_Weight->ReplicatedWeight)).data());
					RenderText({ out.x, out.y + 36 }, { _TeamColor[0], _TeamColor[1], _TeamColor[2], _TeamColor[3] }, ("Hexagons: " + std::to_string(_Hexagons->PlayerHexagonCount)).data());

				}
				if (_TeamSnaplines) {
					RenderLine(out, { 1920 / 2, 1080 }, { _TeamColor[0], _TeamColor[1], _TeamColor[2], _TeamColor[3] }, 1);
				}
			}



			//else if (actor->IsA(APrimalStructureTurret::StaticClass()))
			std::string TurretShortName;
			if (actor->IsTurret(actor->Name.get_name(), TurretShortName))
			{
				auto turret = reinterpret_cast<APrimalStructureTurret*>(actor);

				if (_TurretESP) {
					turret->NumBullets;
					RenderText(out, { _TurretColor[0], _TurretColor[1], _TurretColor[2], _TurretColor[3] }, TurretShortName.c_str());
					RenderText({ out.x, out.y + 12 }, { _TurretColor[0], _TurretColor[1], _TurretColor[2], _TurretColor[3] }, ("Ammo: " + std::to_string(turret->NumBullets)).data());
				}
			}

			//else if (actor->IsA(APrimalStructureItemContainer::StaticClass()))
			if (actor->IsItemContainer())
			{

				if (_ContainerESP) {
					RenderText(out, { _ContainerColor[0], _ContainerColor[1], _ContainerColor[2], _ContainerColor[3] }, _Names);
				}
			}

			if (actor->IsA(APrimalStructureItemContainer_SupplyCrate::StaticClass()))
			{

				auto _RequiredLevel = reinterpret_cast<APrimalStructureItemContainer_SupplyCrate*>(actor);

				_RequiredLevel->RequiredLevelToAccess;
				if (_SupplyCrateESP) {
					RenderText(out, { _SupplyCrateColor[0], _SupplyCrateColor[1], _SupplyCrateColor[2], _SupplyCrateColor[3] }, _Names);
					RenderText({ out.x, out.y + 12 }, { _TeamColor[0], _TeamColor[1], _TeamColor[2], _TeamColor[3] }, ("Health: " + std::to_string(_RequiredLevel->RequiredLevelToAccess)).data());
				}
			}

			//else if (actor->IsA(APrimalStructure::StaticClass()))
			else if (actor->IsStructure())
			{

				if (_StructureESP) {
					RenderText(out, { _StructureColor[0], _StructureColor[1], _StructureColor[2], _StructureColor[3] }, _Names);
				}
			}

			else if (actor->IsPlayer() && actor->IsDead())
			{


				if (_DeadBodyESP) {
					RenderText(out, { _DeadBodyColor[0], _DeadBodyColor[1], _DeadBodyColor[2], _DeadBodyColor[3] }, _Names);
				}
			}
		}
	}
}


namespace DX11
{
	HWND hWnd = nullptr;
	HMODULE hModule = nullptr;
	ID3D11Device* pDevice = nullptr;
	ID3D11DeviceContext* pContext = nullptr;
	IDXGISwapChain* pSwapChain = nullptr;
	std::once_flag onStart;
	WNDPROC WndProc;

	LRESULT __stdcall MainProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
		{
			const auto should_ignore{ _RenderMenu && !ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam) };

			if (should_ignore && should_ignore_msg(uMsg))
			{
				return true;
			}

			return CallWindowProcA(DX11::WndProc, hWnd, uMsg, wParam, lParam);
		}
	}
}


ID3D11RenderTargetView* g_mainRenderTargetView;
HRESULT __stdcall Present_f(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
{
	std::call_once(DX11::onStart, [&]()
		{

			DXGI_SWAP_CHAIN_DESC desc;
			pSwapChain->GetDesc(&desc);

			pSwapChain->GetDevice(__uuidof(DX11::pDevice), (void**)&DX11::pDevice);

			DX11::pDevice->GetImmediateContext(&DX11::pContext);

			ID3D11Texture2D* pBackBuffer;
			pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
			DX11::pDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_mainRenderTargetView);
			pBackBuffer->Release();

			ImGui::CreateContext();
			ImGui_ImplWin32_Init(DX11::hWnd);
			ImGui_ImplDX11_Init(DX11::pDevice, DX11::pContext);

			ImGuiStyle& style = ImGui::GetStyle();
			style.Alpha = 1.0;
			style.WindowRounding = 3;
			style.GrabRounding = 1;
			style.GrabMinSize = 20;
			style.FrameRounding = 3;


			style.Colors[ImGuiCol_Text] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
			style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.00f, 0.40f, 0.41f, 1.00f);
			style.Colors[ImGuiCol_WindowBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
			style.Colors[ImGuiCol_Border] = ImVec4(0.00f, 1.00f, 1.00f, 0.65f);
			style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
			style.Colors[ImGuiCol_FrameBg] = ImVec4(0.44f, 0.80f, 0.80f, 0.18f);
			style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.44f, 0.80f, 0.80f, 0.27f);
			style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.44f, 0.81f, 0.86f, 0.66f);
			style.Colors[ImGuiCol_TitleBg] = ImVec4(0.14f, 0.18f, 0.21f, 0.73f);
			style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.54f);
			style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.00f, 1.00f, 1.00f, 0.27f);
			style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.20f);
			style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.22f, 0.29f, 0.30f, 0.71f);
			style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.00f, 1.00f, 1.00f, 0.44f);
			style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.00f, 1.00f, 1.00f, 0.74f);
			style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
			style.Colors[ImGuiCol_CheckMark] = ImVec4(0.00f, 1.00f, 1.00f, 0.68f);
			style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.00f, 1.00f, 1.00f, 0.36f);
			style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.00f, 1.00f, 1.00f, 0.76f);
			style.Colors[ImGuiCol_Button] = ImVec4(0.00f, 0.65f, 0.65f, 0.46f);
			style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.01f, 1.00f, 1.00f, 0.43f);
			style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.00f, 1.00f, 1.00f, 0.62f);
			style.Colors[ImGuiCol_Header] = ImVec4(0.00f, 1.00f, 1.00f, 0.33f);
			style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.00f, 1.00f, 1.00f, 0.42f);
			style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.00f, 1.00f, 1.00f, 0.54f);
			style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 1.00f, 1.00f, 0.54f);
			style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.00f, 1.00f, 1.00f, 0.74f);
			style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
			style.Colors[ImGuiCol_PlotLines] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
			style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
			style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
			style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
			style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.00f, 1.00f, 1.00f, 0.22f);

			ImGui::GetIO().WantSaveIniSettings = false;
			ImGui::GetIO().IniFilename = NULL;
			Fonts::esp_Font = ImGui::GetIO().Fonts->AddFontFromMemoryTTF(font_data.data(), font_data.size(), 18);


		});

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();


	if (GetAsyncKeyState(VK_F8)) {
		_RenderMenu = !_RenderMenu;
	}

	ImGui::GetBackgroundDrawList()->AddText(Fonts::esp_Font, 16, ImVec2(10, 10), IM_COL32(255, 255, 255, 255), "Bonk Engine - ARK | Diverse#7661");
	if (_RenderMenu) {
		ImGui::SetNextWindowSize(ImVec2(340, 400));
		ImGui::Begin("Bonk Engine - ARK", nullptr, 64);
		if (ImGui::CollapsingHeader("Main")) {

		}
		if (ImGui::CollapsingHeader("Aimbot")) {
			/*ImGui::Checkbox("Aimlock", &_Aimlock);
			ImGui::Checkbox("FOV Aimbot", &_FovAimbot);
			ImGui::Checkbox("Silent Aimbot", &_SilentAimbot);*/
		}
		if (ImGui::CollapsingHeader("Visuals")) {
			ImGui::Checkbox("Enable ESP", &_EnableESP);

			ImGui::Indent(23);
			ImGui::Checkbox("Team", &_TeamESP); ImGui::SameLine(); ImGui::Indent(100); ImGui::Checkbox("Team Lines", &_TeamSnaplines); ImGui::Unindent(100);
			ImGui::SameLine(); ImGui::Indent(250); ImGui::ColorEdit4("Team Color", _TeamColor, 32 | 128); ImGui::Unindent(250);

			ImGui::Checkbox("Ally", &_AllyESP); ImGui::SameLine(); ImGui::Indent(100); ImGui::Checkbox("Ally Lines", &_AllySnaplines); ImGui::Unindent(100);
			ImGui::SameLine(); ImGui::Indent(250);  ImGui::ColorEdit4("Ally Color", _AllyColor, 32 | 128); ImGui::Unindent(250);

			ImGui::Checkbox("Enemy", &_EnemyESP); ImGui::SameLine(); ImGui::Indent(100); ImGui::Checkbox("Enemy Lines", &_EnemySnaplines); ImGui::Unindent(100);
			ImGui::SameLine(); ImGui::Indent(250); ImGui::ColorEdit4("Enemy Color", _EnemyColor, 32 | 128); ImGui::Unindent(250);

			ImGui::Checkbox("Dino", &_DinoESP); ImGui::SameLine(); ImGui::Indent(100); ImGui::Checkbox("Dino Lines", &_DinoSnaplines); ImGui::Unindent(100);
			ImGui::SameLine(); ImGui::Indent(250); ImGui::ColorEdit4("Dino Color", _DinoColor, 32 | 128); ImGui::Unindent(250);

			ImGui::Checkbox("Turret", &_TurretESP);
			ImGui::SameLine(); ImGui::Indent(250); ImGui::ColorEdit4("Turret Color", _TurretColor, 32 | 128); ImGui::Unindent(250);

			ImGui::Checkbox("Structure", &_StructureESP);
			ImGui::SameLine(); ImGui::Indent(250); ImGui::ColorEdit4("Structure Color", _StructureColor, 32 | 128); ImGui::Unindent(250);

			ImGui::Checkbox("Container", &_ContainerESP);
			ImGui::SameLine(); ImGui::Indent(250); ImGui::ColorEdit4("Container Color", _ContainerColor, 32 | 128); ImGui::Unindent(250);

			ImGui::Checkbox("Supply Crate", &_SupplyCrateESP);
			ImGui::SameLine(); ImGui::Indent(250); ImGui::ColorEdit4("SupplyCrate Color", _SupplyCrateColor, 32 | 128); ImGui::Unindent(250);

			ImGui::Checkbox("Dead Body", &_DeadBodyESP); ImGui::SameLine(); ImGui::Indent(100); ImGui::Checkbox("Dead Body Lines", &_DeadBodySnaplines); ImGui::Unindent(100);
			ImGui::SameLine(); ImGui::Indent(250); ImGui::ColorEdit4("Dead Color", _DeadBodyColor, 32 | 128); ImGui::Unindent(250);
			ImGui::Unindent(23);
		}
		if (ImGui::CollapsingHeader("Misc")) {
			ImGui::SliderFloat("FOV", &_FOV, 0.0f, 200.0f);
			ImGui::Checkbox("No Recoil", &_Recoil);
		}
		if (ImGui::CollapsingHeader("Console")) {


		}
		if (ImGui::CollapsingHeader("Config")) {
			ImGui::Checkbox("Session Info", &_ServerInfo);
		}
		if (ImGui::Button("SizeOfs", ImVec2(50, 23))) {
			printf("AActor: %llx \n", sizeof(AActor)); //468
			printf("PrimalCharacter: %llx \n", sizeof(APrimalCharacter)); //930
			printf("ShooterCharacter: %llx \n", sizeof(AShooterCharacter)); //1C80
			printf("UObject: %llx \n", sizeof(UObject)); //28
			printf("ShooterGameState: %llx \n", sizeof(AShooterGameState)); //58
			printf("APrimalStructureItemContainer: %llx \n", sizeof(APrimalStructureItemContainer)); //468
		}
		ImGui::End();
	}
	if (_ServerInfo) {
		ServerInfo();
	}

	if (_EnableESP) {
		RenderESP();
	}

		//if (GetKeyState(VK_CAPITAL)) {
		//	if (auto GloveBUFF = (ABuff_TekArmor_Gloves_C*)ugvggd->GetBuff(ABuff_TekArmor_Gloves_C::StaticClass()))
		//		GloveBUFF->Server_SetPunchChargeState(E_TekGlovePunchState(3));
		//}


	//if (GetKeyState(VK_CAPITAL)) {
	//	auto ABuff_TekArmor_Gloves = reinterpret_cast<ABuff_TekArmor_Gloves_C*>(ABuff_TekArmor_Gloves_C::StaticClass());
	//	ABuff_TekArmor_Gloves->Server_SetPunchChargeState(E_TekGlovePunchState(3));
	//}



	ImGui::Render();
	DX11::pContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	return Present(pSwapChain, SyncInterval, Flags);
}

HRESULT ResizeBuffer_f(IDXGISwapChain* pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags)
{
	if (g_mainRenderTargetView)
	{
		DX11::pContext->OMSetRenderTargets(0, 0, 0);
		g_mainRenderTargetView->Release();
	}

	auto hr = ResizeBuffer(pSwapChain, BufferCount, Width, Height, NewFormat, SwapChainFlags);

	ID3D11Texture2D* pBuffer;
	pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBuffer);
	// Perform error handling here!

	DX11::pDevice->CreateRenderTargetView(pBuffer, NULL, &g_mainRenderTargetView);
	// Perform error handling here!
	pBuffer->Release();

	DX11::pContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);

	// Set up the viewport.
	D3D11_VIEWPORT vp;
	vp.Width = Width;
	vp.Height = Height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	DX11::pContext->RSSetViewports(1, &vp);
	return hr;
}

void imgui_shutdown()
{
	ImGui_ImplWin32_Shutdown();
	ImGui_ImplDX11_Shutdown();

	ImGui::DestroyContext();

	if (DX11::pDevice)
		DX11::pDevice->Release();

	if (DX11::pContext)
		DX11::pContext->Release();

	if (g_mainRenderTargetView)
		g_mainRenderTargetView->Release();

}