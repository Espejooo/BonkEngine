#pragma once
#include "UnrealEngine.h"


struct UWorld : UObject
{
	char pad_28[0xD0];
	struct ULevel* PersistentLevel; // 0x00F8(0x0008) BlueprintVisible, BlueprintReadOnly, ZeroConstructor, Transient, IsPlainOldData, NoDestructor
	char pad1234[0x28];
	class AShooterGameState* GameState;// 0x0128(0x0008) (ZeroConstructor, Transient, IsPlainOldData)
	char pad_100[0x138];
	TArray<class ULevel*> Levels; // 0x0268(0x0010) (ZeroConstructor, Transient)
	char padd_168[0x18];
	struct UGameInstance* OwningGameInstance; // 0x0290(0x0008)

	/*
	char pad_30[0xD0];
	struct ULevel* PersistentLevel; // 0x00F8(0x0008) BlueprintVisible, BlueprintReadOnly, ZeroConstructor, Transient, IsPlainOldData, NoDestructor
	class AGameState* GameState;// 0x0128(0x0008) (ZeroConstructor, Transient, IsPlainOldData)
	char pad_100[0x168]; //190
	TArray<class ULevel*> Levels;    // 0x0268
	char padd_168[0x18];
	struct UGameInstance* OwningGameInstance;                                      // 0x0290
	static UWorld* Singleton()
	*/

	static UWorld* Singleton()
	{
		static uint64_t gworld_ptr = 0;
		if (!gworld_ptr)
		{
			auto gworld = signature("48 8D 0D ? ? ? ? E8 ? ? ? ? 48 83 B8 ? ? ? ? ?").instruction(3).add(7);

			gworld_ptr = gworld.GetPointer();
		}
		return (*(UWorld**)(gworld_ptr));
	}
};

struct UGameInstance : UObject
{
	char pad_40[0x10];  // 0x28(0x10)
	struct TArray<struct ULocalPlayer*> LocalPlayers;  // 0x38(0x10)

	static UGameInstance* Singleton()
	{
		auto world = UWorld::Singleton();
		if (!world)
			return nullptr;
		return world->OwningGameInstance;
	}
};

// Class Engine.Player
// Size: 0x48(Inherited: 0x28) 
struct UPlayer : public UObject
{
	unsigned char UnknownData_086D[0x8];                                   // 0x0028(0x0008) MISSED OFFSET (FIX SPACE BETWEEN PREVIOUS PROPERTY)
	struct APlayerController* PlayerController; // 0x0030(0x0008) ZeroConstructor, Transient, IsPlainOldData, NoDestructor
	int32_t CurrentNetSpeed;                                         // 0x0038(0x0004) ZeroConstructor, IsPlainOldData, NoDestructor
	int32_t ConfiguredInternetSpeed;                                 // 0x003C(0x0004) ZeroConstructor, Config, GlobalConfig, IsPlainOldData, NoDestructor
	int32_t ConfiguredLanSpeed;                                      // 0x0040(0x0004) ZeroConstructor, Config, GlobalConfig, IsPlainOldData, NoDestructor
	unsigned char                                              UnknownData_XSQ4[0xC];                                   // 0x0044(0x000C) MISSED OFFSET (PADDING)

};

struct ULocalPlayer : UPlayer
{

	static ULocalPlayer* Singleton()
	{
		auto gameinstance = UGameInstance::Singleton();
		if (!gameinstance)
			return nullptr;
		if (!gameinstance->LocalPlayers[0])
			return nullptr;
		return gameinstance->LocalPlayers[0];
	}
};

struct ULevel
{
	char pad_88[0x88];
	TArray<struct AActor*> AActor;

	static ULevel* Singleton()
	{
		auto world = UWorld::Singleton();
		if (!world)
			return nullptr;
		return world->PersistentLevel;
	}
};

struct USceneComponent
{
	char pad_00[0x0158];
	struct FVector                                             RelativeLocation;
	struct FRotator                                            RelativeRotation;
	struct FVector                                             RelativeTranslation;
};

struct AActor : UObject
{
	char pad_28[0x228];

	class USceneComponent* RootComponent; // 0x0250


	char pad_258[0x210];

	class UPrimalPlayerData* GetPlayerData()
	{
		static auto fn = UObject::FindObject<UFunction>("Function ShooterGame.ShooterCharacter.GetPlayerData");
		struct {
			UPrimalPlayerData* ReturnValue;
		} Params;
		ProcessEvent(fn, &Params);
		return Params.ReturnValue;
	}

	inline bool IsDino()
	{
		static auto obj = UObject::FindObject<UClass>("Class ShooterGame.PrimalDinoCharacter");
		return IsA(obj);
	}
	inline bool IsPlayer()
	{
		static auto obj = UObject::FindObject<UClass>("Class ShooterGame.ShooterCharacter");
		return IsA(obj);
	}
	inline bool IsTurret(std::string Name, std::string& ShortName)
	{
		if (Name == "StructureTurretBaseBP_C")
		{
			ShortName = "AT";
			return true;
		}
		if (Name == "StructureTurretBaseBP_Heavy_C")
		{
			ShortName = "HT";
			return true;
		}
		if (Name == "StructureTurretTek_C")
		{
			ShortName = "TT";
			return true;
		}
		static auto Obj = UObject::FindObject<UClass>("Class ShooterGame.PrimalStructureTurretPlant");
		if (IsA(Obj))
		{
			ShortName = "XPlant";
			return true;
		}
		return false;
	}
	inline bool IsFish(std::string DinoName)
	{
		if (DinoName == "Coel") return true;
		if (DinoName == "Trilobite") return true;
		if (DinoName == "Salmon") return true;
		return false;
	}

	inline bool IsAlpha(std::string DinoName)
	{
		if (DinoName == "Elite Raptor") return true;
		if (DinoName == "Elite Mega") return true;
		if (DinoName == "Elite Rex") return true;
		if (DinoName == "Elite Carno") return true;
		return false;
	}

	inline bool IsManta(std::string DinoName)
	{
		if (DinoName == "Manta") return true;
		return false;
	}

	inline int RetrievePlayerGender(std::string Name)
	{
		if (Name == "PlayerPawnTest_Male_C") return 1;
		if (Name == "PlayerPawnTest_Female_C") return 2;
		return 0;
	}
	inline struct AController* GetCharacterController()
	{
		static auto fn = UObject::FindObject<UFunction>("Function Engine.Actor.GetCharacterController");
		struct {
			struct AController* ReturnValue;
		} Params;
		ProcessEvent(fn, &Params);
		return Params.ReturnValue;
	}
	void GetActorBounds(bool bOnlyCollidingComponents, struct FVector& Origin, struct FVector& BoxExtent)
	{
		static auto fn = UObject::FindObject<UFunction>("Function Engine.Actor.GetActorBounds");
		struct {
			bool bOnlyCollidingComponents;
			FVector Origin;
			FVector BoxExtent;
		} Params;

		Params.bOnlyCollidingComponents = bOnlyCollidingComponents;
		ProcessEvent(fn, &Params);
		Origin = Params.Origin;
		BoxExtent = Params.BoxExtent;
	}
	bool IsAlliedWith(int OtherTeamID)
	{
		static auto fn = UObject::FindObject<UFunction>("Function ShooterGame.PrimalCharacter.IsAlliedWithOtherTeam");
		struct {
			int OtherTeamID;
			bool ReturnValue;
		} Params;
		Params.OtherTeamID = OtherTeamID;
		ProcessEvent(fn, &Params);
		return Params.ReturnValue;
	}
	struct AShooterWeapon* GetWeapon()
	{
		static auto fn = UObject::FindObject<UFunction>("Function ShooterGame.ShooterCharacter.GetWeapon");
		struct {
			struct AShooterWeapon* ReturnValue;
		} Params;
		ProcessEvent(fn, &Params);
		return Params.ReturnValue;
	}
	FVector GetVelocity(bool ForRagDoll)
	{
		static auto fn = UObject::FindObject<UFunction>("Function Engine.Actor.GetVelocity");
		struct {
			bool ForRagDoll;
			FVector ReturnValue;
		} Params;
		Params.ForRagDoll = ForRagDoll;
		ProcessEvent(fn, &Params);
		return Params.ReturnValue;
	}
	FVector K2_GetActorLocation()
	{
		static auto fn = UObject::FindObject<UFunction>("Function Engine.Actor.K2_GetActorLocation");
		struct
		{
			FVector ReturnVector;
		} Params;
		ProcessEvent(fn, &Params);
		return Params.ReturnVector;
	}
	FRotator K2_GetActorRotation()
	{
		static auto fn = UObject::FindObject<UFunction>("Function Engine.Actor.K2_GetActorRotation");
		struct {
			FRotator ReturnValue;
		} Params;
		ProcessEvent(fn, &Params);
		return Params.ReturnValue;
	}
	bool IsTamed()
	{
		static auto fn = UObject::FindObject<UFunction>("Function ShooterGame.PrimalDinoCharacter.BPIsTamed");
		struct {
			bool ReturnValue;
		} Params;
		ProcessEvent(fn, &Params);
		return Params.ReturnValue;
	}
	bool IsLocalPlayer()
	{
		static auto fn = UObject::FindObject<UFunction>("Function ShooterGame.PrimalCharacter.IsOwningClient");
		struct {
			bool ReturnValue;
		} Params;
		ProcessEvent(fn, &Params);
		return Params.ReturnValue;
	}
	struct APrimalDinoCharacter* GetBasedOrSeatingOnDino()
	{
		static auto fn = UObject::FindObject<UFunction>("Function ShooterGame.PrimalCharacter.GetBasedOrSeatingOnDino");
		struct {
			struct APrimalDinoCharacter* ReturnValue;
		} Params;
		ProcessEvent(fn, &Params);
		return Params.ReturnValue;
	}
	//struct UOnlineSessionEntryButton* OnlineSessionEntryButton()
	//{
	//	static auto fn = UObject::FindObject<UFunction>("Function ShooterGame.OnlineSessionEntryButton");
	//	struct {
	//		struct UOnlineSessionEntryButton* ReturnValue;
	//	} Params;
	//	ProcessEvent(fn, &Params);
	//	return Params.ReturnValue;
	//}
	bool IsConscious()
	{
		static auto fn = UObject::FindObject<UFunction>("Function ShooterGame.PrimalCharacter.BPIsConscious");
		struct {
			bool ReturnValue;
		} Params;
		ProcessEvent(fn, &Params);
		return Params.ReturnValue;
	}
	bool IsFriendly(int OtherTeam)
	{
		static auto fn = UObject::FindObject<UFunction>("Function ShooterGame.ShooterPlayerState.IsFriendly");
		struct {
			int OtherTeam;
			bool ReturnValue;
		} Params;
		//auto flags = fn->FunctionFlags;
		//fn->FunctionFlags |= 0x00000400;
		Params.OtherTeam = OtherTeam;
		ProcessEvent(fn, &Params);
		//fn->FunctionFlags = flags;
		return Params.ReturnValue;
	}

	bool IsPrimalCharFriendly(class APrimalCharacter* primalChar)
	{
		static auto fn = UObject::FindObject<UFunction>("Function ShooterGame.PrimalCharacter.IsPrimalCharFriendly");

		struct {
			class APrimalCharacter* primalChar;
			bool ReturnValue;
		}params;
		params.primalChar = primalChar;

		//auto flags = fn->FunctionFlags;
		//fn->FunctionFlags |= 0x00000400;

		ProcessEvent(fn, &params);
		//fn->FunctionFlags = flags;


		return params.ReturnValue;
	}
	void DisableInput(class APlayerController* PlayerController)
	{
		static auto fn = UObject::FindObject<UFunction>("Function Engine.Actor.DisableInput");

		struct {
			class APlayerController* PlayerController;
		}params;
		params.PlayerController = PlayerController;

		//auto flags = fn->FunctionFlags;
		//fn->FunctionFlags |= 0x00000400;

		ProcessEvent(fn, &params);
		//fn->FunctionFlags = flags;

	}
	bool IsDead()
	{
		static auto fn = UObject::FindObject<UFunction>("Function Engine.Actor.IsDead");
		struct {
			bool ReturnValue;
		} Params;
		ProcessEvent(fn, &Params);
		return Params.ReturnValue;
	}
	bool IsStructure()
	{
		static auto fn = UObject::FindObject<UFunction>("Function Engine.Actor.IsPrimalStructure");

		struct {
			bool                           ReturnValue;
		}params;

		ProcessEvent(fn, &params);

		return params.ReturnValue;
	}
	bool IsItemContainer()
	{
		static auto fn = UObject::FindObject<UFunction>("Function Engine.Actor.IsPrimalStructureItemContainer");
		struct {
			bool ReturnValue;
		} Params;
		ProcessEvent(fn, &Params);
		return Params.ReturnValue;
	}
	/*bool IsExcludedContainer(std::string DescriptiveName)
	{
		std::array<std::string, 6> ExcludedContainerNames = { "Automated Turret", "Heavy Automated Turret", "Tek Turret", "Small Crop Plot", "Medium Crop Plot", "Large Crop Plot" };
		for (int i = 0; i < ExcludedContainerNames.size(); i++)
		{
			if (DescriptiveName == ExcludedContainerNames[i]) return true;
		}
		return false;
	}*/
	bool SetActorRotation(const FRotator& NewRotation)
	{
		static auto fn = UObject::FindObject<UFunction>("Function Engine.Actor.SetActorRotation");
		struct {
			FRotator NewRotation;
			bool ReturnValue;
		} Params;
		Params.NewRotation = NewRotation;
		ProcessEvent(fn, &Params);
		return Params.ReturnValue;
	}

	void TurnAtRate(float val)
	{
		static auto fn = UObject::FindObject<UFunction>("Function ShooterGame.PrimalCharacter.TurnAtRate");
		struct {
			float val;
			//bool ReturnValue;
		} Params;
		Params.val = val;
		//auto flags = fn->FunctionFlags;
		//fn->FunctionFlags |= 0x00000400;
		ProcessEvent(fn, &Params);
	}
};




// Class Engine.Controller
// Size: 0x04C0(Inherited: 0x0468) 
struct AController : AActor
{
	char pad_468[0x18];  // 0x0468(0x8)
	struct APlayerState* PlayerState;  // 0x0480(0x8)
	char pad_330[0x38];
};

struct APrimalCharacter : AActor //468
{
	char padHealth[0x4C4];
	float ReplicatedCurrentHealth; // 0x092C(0x0004) (Net, ZeroConstructor, Transient, IsPlainOldData)
	//float ReplicatedCurrentTorpor; // 0x0934(0x0004) (Net, ZeroConstructor, Transient, IsPlainOldData)
	//float OrbitCamMinZoomLevel; // 0x0DFC(0x0004) (Edit, BlueprintVisible, BlueprintReadOnly, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	//float OrbitCamMaxZoomLevel; // 0x0E00(0x0004) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)


	static UClass* StaticClass()
	{
		static auto ptr = UObject::FindObject<UClass>("Class ShooterGame.PrimalCharacter");
		return ptr;
	}


	bool IsConscious()
	{
		static auto fn = UObject::FindObject<UFunction>("Function ShooterGame.PrimalCharacter.BPIsConscious");
		struct {
			bool ReturnValue;
		} Params;
		ProcessEvent(fn, &Params);
		return Params.ReturnValue;
	}
	bool IsLocalPlayer()
	{
		static auto fn = UObject::FindObject<UFunction>("Function ShooterGame.PrimalCharacter.IsOwningClient");
		struct {
			bool ReturnValue;
		} Params;
		ProcessEvent(fn, &Params);
		return Params.ReturnValue;
	}

	bool IsAlive()
	{
		static auto fn = UObject::FindObject<UFunction>("Function ShooterGame.PrimalCharacter.IsAlive");
		struct {
			bool ReturnValue;
		} Params;
		ProcessEvent(fn, & Params);
		return Params.ReturnValue;
	}


	class APrimalBuff* GetBuff(class UClass* BuffClass);
};

struct APrimalDinoCharacter : APrimalCharacter //930
{


	static UClass* StaticClass()
	{
		static auto ptr = UObject::FindObject<UClass>("Class ShooterGame.PrimalDinoCharacter");
		return ptr;
	}
};


struct AShooterCharacter : APrimalCharacter //930
{
	char padWeight[0x129C];
	float ReplicatedWeight; // 0x1BCC(0x0004) (Net, ZeroConstructor, Transient, IsPlainOldData)
	char padHexagons[0xA8];
	int PlayerHexagonCount; // 0x1C78(0x0004) (BlueprintVisible, Net, ZeroConstructor, SaveGame, IsPlainOldData)

	static UClass* StaticClass()
	{
		static auto ptr = UObject::FindObject<UClass>("Class ShooterGame.ShooterCharacter");
		return ptr;
	}
};

struct AShooterGameState
{
	char padPlayerActors[0x520];
	int NumPlayerActors; // 0x0548(0x0004) (Net, ZeroConstructor, Transient, IsPlainOldData)
	int NumPlayerConnected; // 0x054C(0x0004) (Net, ZeroConstructor, Transient, IsPlainOldData)
	char padServerFPS[0x20];
	float ServerFramerate; // 0x0570(0x0004) (Net, ZeroConstructor, Transient, IsPlainOldData)
	char padDayNum[0x14];
	int DayNumber; // 0x0588(0x0004) (BlueprintVisible, BlueprintReadOnly, Net, ZeroConstructor, Transient, SaveGame, IsPlainOldData)
	float DayTime; // 0x058C(0x0004) (BlueprintVisible, BlueprintReadOnly, Net, ZeroConstructor, Transient, SaveGame, IsPlainOldData)
	double NetworkTime; // 0x0590(0x0008) (BlueprintVisible, BlueprintReadOnly, Net, ZeroConstructor, Transient, SaveGame, IsPlainOldData)
	char padTamedDinos[0x18];
	int NumTamedDinos; // 0x05B0(0x0004) (Net, ZeroConstructor, Transient, IsPlainOldData)
	char padMaxTamedDinos[0x30];
	int MaxTamedDinos; // 0x05E4(0x0004) (Net, ZeroConstructor, Transient, IsPlainOldData)
	
	//struct FString PlayerListString; // 0x05F0(0x0010) (Net, ZeroConstructor, Transient)
	//
	//double LastServerSaveTime; // 0x0698(0x0008) (Net, ZeroConstructor, Transient, SaveGame, IsPlainOldData)
	//
	//float ServerSaveInterval; // 0x06A0(0x0004) (Net, ZeroConstructor, Transient, IsPlainOldData)
	//
	//struct FString ClusterId; // 0x08D8(0x0010) (Net, ZeroConstructor, Transient)

	static UClass* StaticClass()
	{
		static auto ptr = UObject::FindObject<UClass>("Class ShooterGame.ShooterGameState");
		return ptr;
	}
};


struct APrimalStructure : AActor
{


	static UClass* StaticClass()
	{
		static auto ptr = UObject::FindObject<UClass>("Class ShooterGame.PrimalStructure");
		return ptr;
	}
};

struct APrimalStructureItemContainer : APrimalStructure
{

	static UClass* StaticClass()
	{
		static auto ptr = UObject::FindObject<UClass>("Class ShooterGame.PrimalStructureItemContainer");
		return ptr;
	}
};

struct APrimalStructureItemContainer_SupplyCrate : APrimalStructureItemContainer
{
	char padRequiredLevel[0x9F0];
	int RequiredLevelToAccess; // 0x0E58(0x0004) (Edit, ZeroConstructor, IsPlainOldData)


	static UClass* StaticClass()
	{
		static auto ptr = UObject::FindObject<UClass>("Class ShooterGame.PrimalStructureItemContainer_SupplyCrate");
		return ptr;
	}
};

struct APrimalStructureTurret : AActor
{ 
	char padd69[0xA48];

	//unsigned char RangeSetting; // 0x0EAA(0x0001) (Net, ZeroConstructor, Transient, SaveGame, IsPlainOldData)
	int NumBullets; // 0x0EB0(0x0004) (Net, ZeroConstructor, Transient, SaveGame, IsPlainOldData)


	static UClass* StaticClass()
	{
		static auto ptr = UObject::FindObject<UClass>("Class ShooterGame.PrimalStructureTurret");
		return ptr;
	}
};

struct FMinimalViewInfo
{
	struct FVector Location; // 0x0000(0x000C) Edit, BlueprintVisible, ZeroConstructor, IsPlainOldData, NoDestructor
	struct FRotator Rotation; // 0x000C(0x000C) Edit, BlueprintVisible, ZeroConstructor, IsPlainOldData, NoDestructor
	struct FRotator AimRotation; // 0x0018(0x000C) Edit, BlueprintVisible, ZeroConstructor, IsPlainOldData, NoDestructor
	char pad_00[4];
	float FOV; // 0x0028
};

struct FCameraCacheEntry
{
	float                                                      TimeStamp;                                               // 0x0000(0x0004) ZeroConstructor, IsPlainOldData, NoDestructor
	unsigned char                                              UnknownData_YIQV[0x4];                                   // 0x0004(0x0004) MISSED OFFSET (FIX SPACE BETWEEN PREVIOUS PROPERTY)
	struct FMinimalViewInfo                                    POV;                                                     // 0x0008(0x0398)
};

struct APlayerCameraManager
{
	char pad_0[0x04D0];
	struct FCameraCacheEntry CameraCache; // 0x04D0
};

struct APlayerController : public AController
{
	char pad_4C0[0x10];
	struct APawn* AcknowledgedPawn;  // 0x04D0(0x8)
	char pad_4D8[0x18];
	struct APlayerCameraManager* PlayerCameraManager;  // 0x04F0(0x8)

	static APlayerController* Singleton()
	{
		auto player = ULocalPlayer::Singleton();
		if (!player)
			return nullptr;
		return player->PlayerController;
	}

	bool ProjectWorldLocationToScreen(struct FVector WorldLocation, struct FVector2D& ScreenLocation); // Function Engine.PlayerController.ProjectWorldLocationToScreen
};

// UserDefinedEnum E_TekGlovePunchState.E_TekGlovePunchState
enum class E_TekGlovePunchState : uint8_t
{
	E_TekGlovePunchState__NewEnumerator0 = 0,
	E_TekGlovePunchState__NewEnumerator1 = 1,
	E_TekGlovePunchState__NewEnumerator4 = 2,
	E_TekGlovePunchState__NewEnumerator5 = 3,
	E_TekGlovePunchState__NewEnumerator6 = 4,
	E_TekGlovePunchState__E_MAX = 5
};

// Function Buff_TekArmor_Gloves.Buff_TekArmor_Gloves_C.Server_SetPunchChargeState
struct ABuff_TekArmor_Gloves_C_Server_SetPunchChargeState_Params
{
	TEnumAsByte<E_TekGlovePunchState> newPunchState;                                            // (Parm, ZeroConstructor, IsPlainOldData)
};

struct ABuff_TekArmor_Gloves_C : UObject
{
	char pad69420[0xB48];
	TEnumAsByte<E_TekGlovePunchState> CurrentTekPunchState; // 0x0B70(0x0001) (Edit, BlueprintVisible, Net, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)


	static UClass* StaticClass()
	{
		static auto ptr = UObject::FindObject<UClass>("BlueprintGeneratedClass Buff_TekArmor_Gloves.Buff_TekArmor_Gloves_C");
		return ptr;
	}

	void Server_SetPunchChargeState(TEnumAsByte<E_TekGlovePunchState> newPunchState);

};

