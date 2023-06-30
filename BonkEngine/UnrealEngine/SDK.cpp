#include "pch.h"
#include "SDK.h"
#include <d3d9types.h>

D3DMATRIX to_matrix(FRotator rot, FVector origin = FVector(0, 0, 0))
{
	float radpitch = (rot.x * M_PI / 180);
	float radyaw = (rot.y * M_PI / 180);
	float radroll = (rot.z * M_PI / 180);
	float sp = sinf(radpitch);
	float cp = cosf(radpitch);
	float sy = sinf(radyaw);
	float cy = cosf(radyaw);
	float sr = sinf(radroll);
	float cr = cosf(radroll);
	D3DMATRIX matrix{};
	matrix.m[0][0] = cp * cy;
	matrix.m[0][1] = cp * sy;
	matrix.m[0][2] = sp;
	matrix.m[0][3] = 0.f;
	matrix.m[1][0] = sr * sp * cy - cr * sy;
	matrix.m[1][1] = sr * sp * sy + cr * cy;
	matrix.m[1][2] = -sr * cp;
	matrix.m[1][3] = 0.f;
	matrix.m[2][0] = -(cr * sp * cy + sr * sy);
	matrix.m[2][1] = cy * sr - cr * sp * sy;
	matrix.m[2][2] = cr * cp;
	matrix.m[2][3] = 0.f;
	matrix.m[3][0] = origin.x;
	matrix.m[3][1] = origin.y;
	matrix.m[3][2] = origin.z;
	matrix.m[3][3] = 1.f;
	return matrix;
}

bool APlayerController::ProjectWorldLocationToScreen(FVector WorldLocation, FVector2D& ScreenLocation)
{
	if (!this->PlayerCameraManager)
		return false;

	D3DMATRIX temp_matrix = to_matrix(this->PlayerCameraManager->CameraCache.POV.Rotation);
	FVector vaxisx = FVector(temp_matrix.m[0][0], temp_matrix.m[0][1], temp_matrix.m[0][2]);
	FVector vaxisy = FVector(temp_matrix.m[1][0], temp_matrix.m[1][1], temp_matrix.m[1][2]);
	FVector vaxisz = FVector(temp_matrix.m[2][0], temp_matrix.m[2][1], temp_matrix.m[2][2]);

	FVector vdelta = WorldLocation - this->PlayerCameraManager->CameraCache.POV.Location;
	FVector vtransformed = FVector(vdelta.DotProduct(vaxisy), vdelta.DotProduct(vaxisz), vdelta.DotProduct(vaxisx));

	if (vtransformed.z < 0.098f)
		return false;

	auto screen_size = ImGui::GetIO().DisplaySize;
	auto screen_center_x = screen_size.x / 2;
	auto screen_center_y = screen_size.y / 2;

	ScreenLocation.x = screen_center_x + vtransformed.x * ((screen_center_x / tanf(this->PlayerCameraManager->CameraCache.POV.FOV * M_PI / 360))) / vtransformed.z;
	ScreenLocation.y = screen_center_y - vtransformed.y * ((screen_center_x / tanf(this->PlayerCameraManager->CameraCache.POV.FOV * M_PI / 360))) / vtransformed.z;

	return true;
}


void ABuff_TekArmor_Gloves_C::Server_SetPunchChargeState(TEnumAsByte<E_TekGlovePunchState> newPunchState)
{
	static auto fn = UObject::FindObject<UFunction>("Function Buff_TekArmor_Gloves.Buff_TekArmor_Gloves_C.Server_SetPunchChargeState");

	ABuff_TekArmor_Gloves_C_Server_SetPunchChargeState_Params params;
	params.newPunchState = newPunchState;

	ProcessEvent(fn, &params);
}


class APrimalBuff* APrimalCharacter::GetBuff(class UClass* BuffClass)
{
	static auto fn = UObject::FindObject<UFunction>("Function ShooterGame.PrimalCharacter.GetBuff");

	struct
	{
		UClass* BuffClass;
			APrimalBuff* return_value;
	} params{};

		params.BuffClass = BuffClass;

		UObject::ProcessEvent(fn, &params);

		return params.return_value;
}
