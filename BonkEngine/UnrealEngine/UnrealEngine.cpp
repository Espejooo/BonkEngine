#include "pch.h"
#include "UnrealEngine.h"


std::string UObject::GetName()
{
	return this->FixName(Name.get_name());

}

std::string UObject::GetFullName()
{
	std::string name;
	for (auto outer = Outer; outer; outer = outer->Outer) { name = outer->GetName() + "." + name; }
	name = Class->GetName() + " " + name + this->GetName();
	return name;
}

bool UObject::IsA(void* cmp)
{
	for (auto super = Class; super; super = static_cast<UClass*>(super->SuperField))
	{
		if (super == cmp)
		{
			return true;
		}
	}
	return false;
}

void UObject::ProcessEvent(void* fn, void* parms)
{
	static uintptr_t pFn = signature("40 55 53 56 57 41 54 41 56 41 57 48 81").GetPointer();
	auto Fn = reinterpret_cast<void(__fastcall*)(UObject*, void*, void*)>(pFn);
	Fn(this, fn, parms);
}

FVector UKismetMathLibrary::Conv_RotatorToVector(struct FRotator InRot)
{
	static auto fn = UObject::FindObject<UFunction>("Function Engine.KismetMathLibrary.Conv_RotatorToVector");
	struct {
		FRotator InRot;
		FVector returnValue;
	} parms;

	parms.InRot = InRot;
	ProcessEvent(fn, &parms);
	return parms.returnValue;
}

float UKismetMathLibrary::GetPI()
{
	static auto fn = UObject::FindObject<UFunction>("Function Engine.KismetMathLibrary.GetPI");
	struct {
		float returnValue;
	} parms;

	ProcessEvent(fn, &parms);
	return parms.returnValue;
}

FRotator UKismetMathLibrary::FindLookAtRotation(FVector Start, FVector Target)
{
	static auto fn = UObject::FindObject<UFunction>("Function Engine.KismetMathLibrary.FindLookAtRotation");

	struct {
		FVector Start;
		FVector Target;
		FRotator returnValue;
	}parms;

	parms.Start = Start;
	parms.Target = Target;

	ProcessEvent(fn, &parms);

	return parms.returnValue;
}