#include "pch.h"

//ViewPoint (FOV)
GetViewPoint_t GetViewPoint;
void GetViewPoint_f(ULocalPlayer* _this, FMinimalViewInfo* OutViewInfo)
{
	GetViewPoint(_this, OutViewInfo);

	OutViewInfo->FOV = _FOV;
}

//Recoil
GetRecoilMultiplier_t GetRecoilMultiplier;
float GetRecoilMultiplier_f(ULocalPlayer* _this) {
	if (_Recoil)
		return 0;
}

//UnlockExplorerNotes
//ClientUnlockExplorerNote_t ClientUnlockExplorerNote;
//float ClientUnlockExplorerNote_f(AShooterPlayerController* _this, int ExplorerNoteIndex, const bool forceShowExplorerNoteUI) {
//	return 0;
//}