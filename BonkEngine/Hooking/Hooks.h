#pragma once

//ViewPoint (FOV)
typedef void(*GetViewPoint_t)(ULocalPlayer* _this, FMinimalViewInfo* OutViewInfo);
extern GetViewPoint_t GetViewPoint;
void GetViewPoint_f(ULocalPlayer* _this, FMinimalViewInfo* OutViewInfo);

//No Recoil
typedef float(*GetRecoilMultiplier_t)(ULocalPlayer* _this);
extern GetRecoilMultiplier_t GetRecoilMultiplier;
float GetRecoilMultiplier_f(ULocalPlayer* _this);

//UnlockExplorerNote
typedef float(*ClientUnlockExplorerNote_t)(ULocalPlayer* _this);
extern ClientUnlockExplorerNote_t ClientUnlockExplorerNote;
float ClientUnlockExplorerNote_f(ULocalPlayer* _this);