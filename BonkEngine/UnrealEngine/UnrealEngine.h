#pragma once

#define M_PI 3.14159265358979323846
#define ToRadian(value) ((value) * (M_PI / 180.f))

#define CONCAT_IMPL(x, y) x##y
#define MACRO_CONCAT(x, y) CONCAT_IMPL(x, y)
#define PAD(SIZE) BYTE MACRO_CONCAT(_pad, __COUNTER__)[SIZE];



template<typename T>
struct TArray
{
	friend struct FString;

	constexpr TArray() noexcept
	{
		Data = nullptr;
		Count = Max = 0;
	};

	[[nodiscard]] constexpr auto Num() const noexcept
	{
		return Count;
	};

	[[nodiscard]] constexpr auto& operator[](std::int32_t i) noexcept
	{
		return Data[i];
	};

	[[nodiscard]] constexpr const auto& operator[](std::int32_t i) const noexcept
	{
		return Data[i];
	};

	[[nodiscard]] constexpr auto IsValidIndex(std::int32_t i) const noexcept
	{
		return i < Num();
	}

	T* Data;
	std::int32_t Count;
	std::int32_t Max;
};

class FString : public TArray<wchar_t>
{
public:
	constexpr FString() noexcept
	{
	};

	constexpr FString(const wchar_t* other) noexcept
	{
		Max = Count = *other ? static_cast<std::int32_t>(std::wcslen(other)) + 1 : 0;

		if (Count)
			Data = const_cast<wchar_t*>(other);
	};

	[[nodiscard]] constexpr auto IsValid() const noexcept
	{
		return Data != nullptr;
	}

	[[nodiscard]] constexpr auto c_str() const noexcept
	{
		return Data;
	}

	[[nodiscard]] auto ToString() const noexcept
	{
		const auto length = std::wcslen(Data);
		std::string str(length, '\0');
		std::use_facet<std::ctype<wchar_t>>(std::locale()).narrow(Data, Data + length, '?', &str[0]);
		return str;
	}
};

struct FNameEntryHandle
{
	uint32_t Block = 0;
	uint32_t Offset = 0;

	FNameEntryHandle(uint32_t block, uint32_t offset) : Block(block), Offset(offset) {};
	FNameEntryHandle(uint32_t id) : Block(id >> 16), Offset(id & 65535) {};
	operator uint32_t() const { return (Block << 16 | Offset); }
};

struct FNameEntry
{
	uint32_t Index;
	uint32_t Pad;
	FNameEntry* HashNext;

	union
	{
		char AnsiName[1024];
		wchar_t WideName[1024];
	};

	const int GetIndex() const { return Index >> 1; }
	const char* GetAnsiName() const { return AnsiName; }
};

struct TNameEntryArray
{
	bool IsValidIndex(uint32_t index) const { return index < NumElements; }

	FNameEntry const* GetByID(uint32_t index) const { return *GetItemPtr(index); }

	FNameEntry const* const* GetItemPtr(uint32_t Index) const
	{
		const auto ChunkIndex = Index / 16384;
		const auto WithinChunkIndex = Index % 16384;
		const auto Chunk = Chunks[ChunkIndex];

		return Chunk + WithinChunkIndex;
	}

	FNameEntry** Chunks[128]; // ChunkTableSize
	uint32_t NumElements = 0;
	uint32_t NumChunks = 0;
};

struct FName
{
	int ComparisonIndex = 0;
	int Number = 0;

	static inline TNameEntryArray* GNames = nullptr;

	static const char* GetNameByIDFast(int ID)
	{
		auto NameEntry = GNames->GetByID(ID);
		if (!NameEntry) return nullptr;
		return NameEntry->AnsiName;
	}
	static std::string GetNameByID(int ID)
	{
		auto NameEntry = GNames->GetByID(ID);
		if (!NameEntry) return std::string();
		return NameEntry->AnsiName;
	}
	const char* GetNameFast() const
	{
		auto NameEntry = GNames->GetByID(ComparisonIndex);
		if (!NameEntry) return nullptr;
		return NameEntry->AnsiName;
	}
	const std::string get_name()
	{
		static uintptr_t pFn = signature("48 89 5C 24 08 48 89 74 24 10 57 48 83 EC 20 48 8B DA 48 8B F1 E8 ?? ?? ?? ?? 8B").GetPointer();
		auto Fn = reinterpret_cast<void(__fastcall*)(FName * _this, FString * Out)>(pFn);

		FString out;
		Fn(this, &out);
		std::string name = out.ToString();

		if (Number > 0)
		{
			name += '_' + std::to_string(Number);
		}
		auto pos = name.rfind('/');
		if (pos != std::string::npos)
		{
			name = name.substr(pos + 1);
		}
		return name;
	}
	inline bool operator==(const FName& other) const
	{
		return ComparisonIndex == other.ComparisonIndex;
	}

	FName() {}
	FName(const char* nameToFind)
	{
		for (int i = 1000u; i < GNames->NumElements; i++)
		{
			auto Name = GetNameByIDFast(i);
			if (!Name) continue;
			if (strcmp(Name, nameToFind) == 0)
			{
				ComparisonIndex = i;
				return;
			}
		}
	}
};

struct FUObjectItem
{
	class UObject* Object;
	int32_t SerialNumber;
	unsigned char pad_C1AOV13XBK[0x4];

	enum class ObjectFlags : int32_t
	{
		None = 0,
		Native = 1 << 25,
		Async = 1 << 26,
		AsyncLoading = 1 << 27,
		Unreachable = 1 << 28,
		PendingKill = 1 << 29,
		RootSet = 1 << 30,
		NoStrongReference = 1 << 31
	};
};

struct TUObjectArray
{
	enum
	{
		NumElementsPerChunk = 64 * 1024,
	};
	inline int32_t Num() const
	{
		return NumElements;
	}
	inline int32_t Max() const
	{
		return MaxElements;
	}
	inline bool IsValidIndex(int32_t Index) const
	{
		return Index < Num() && Index >= 0;
	}
	inline FUObjectItem* GetObjectPtr(int32_t Index) const
	{
		const int32_t ChunkIndex = Index / NumElementsPerChunk;
		const int32_t WithinChunkIndex = Index % NumElementsPerChunk;
		if (!IsValidIndex(Index)) return nullptr;
		if (ChunkIndex > NumChunks) return nullptr;
		if (Index > MaxElements) return nullptr;
		FUObjectItem* Chunk = Objects[ChunkIndex];
		if (!Chunk) return nullptr;
		return Chunk + WithinChunkIndex;
	}
	inline UObject* GetByIndex(int32_t index) const
	{
		FUObjectItem* ItemPtr = GetObjectPtr(index);
		if (!ItemPtr) return nullptr;

		return (*ItemPtr).Object;
	}
	inline FUObjectItem* GetItemByIndex(int32_t index) const
	{
		FUObjectItem* ItemPtr = GetObjectPtr(index);
		if (!ItemPtr) return nullptr;
		return ItemPtr;
	}

	PAD(0x10);
	FUObjectItem** Objects;
	FUObjectItem* PreAllocatedObjects;
	int32_t MaxElements;
	int32_t NumElements;
	int32_t MaxChunks;
	int32_t NumChunks;
};



struct UObject
{
	static inline TUObjectArray* GObjects = nullptr; // 0x0000

	void** VfTable;                                                   // 0x0000
	int32_t Flags;                                                     // 0x0008
	int32_t InternalIndex;                                             // 0x000C
	struct UClass* Class;                                                     // 0x0010
	FName Name;                                                      // 0x0018
	struct UObject* Outer;                                                     // 0x0020

	std::string GetName();
	std::string GetFullName();
	bool IsA(void* cmp);
	void ProcessEvent(void* fn, void* parms);

	static inline TUObjectArray& GetGlobalObjects()
	{
		return *GObjects;
	}

	template<typename T>
	static T* FindObject(const std::string& name)
	{
		for (auto i = 0u; i < UObject::GObjects->NumElements; i++)
		{
			auto object = UObject::GObjects->GetByIndex(i);
			if (object && object->GetFullName() == name)
				return static_cast<T*>(object);
		}

		return nullptr;
	}

	inline std::string FixName(std::string Name)
	{
		std::string sTemp = Name;
		sTemp = sTemp.substr(0, sTemp.rfind('_'));
		return sTemp;
	}
};

// Class CoreUObject.Class
// Size: 0x230 (Inherited: 0xb0)


// Class CoreUObject.Field
// Size: 0x30 (Inherited: 0x28)
struct UField : UObject
{
	class UField* Next;                                        // 0x0028

	static struct UClass* StaticClass()
	{
		static auto ptr = UObject::FindObject<struct UClass>("Class CoreUObject.Field");
		return ptr;
	}
};

// Class CoreUObject.Struct
// Size: 0xb0 (Inherited: 0x30)
struct UStruct : UField
{
	class UStruct* SuperField;                                                // 0x0030
	class UField* Children;                                                  // 0x0038
	int32_t                                             PropertySize;                                              // 0x0040
	PAD(0x4);
	TArray<unsigned char>								Script;													   // 0x0048
	int32_t                                             MinAlignment;                                              // 0x0058
	PAD(0x4);
	class UProperty* PropertyLink;                                              // 0x0060
	class UProperty* RefLink;                                                   // 0x0068
	class UProperty* DestructorLink;                                            // 0x0070
	class UProperty* PostConstructLink;                                         // 0x0078
	TArray<UObject*>                                    ScriptAndPropertyObjectReferences;                         // 0x0080

	static struct UClass* StaticClass()
	{
		static auto ptr = UObject::FindObject<struct UClass>("Class CoreUObject.Struct");
		return ptr;
	}
};

struct UClass : UStruct
{
	unsigned char                                      UnknownData_BH42[0xF8];

	static UClass* StaticClass()
	{
		static auto ptr = UObject::FindObject<UClass>("Class CoreUObject.Class");
		return ptr;
	}
};

// Class CoreUObject.Function
// Size: 0xe0 (Inherited: 0xb0)
struct UFunction : UStruct {
	uint32_t										   FunctionFlags;                                             // 0x0090
	uint16_t                                           RepOffset;                                                 // 0x0094
	PAD(0x2);
	uint16_t                                           ParmsSize;                                                 // 0x0098
	uint16_t                                           ReturnValueOffset;                                         // 0x009A
	uint16_t                                           RPCId;                                                     // 0x009C
	uint16_t                                           RPCResponseId;                                             // 0x009E
	class UProperty* FirstPropertyToInit;                                       // 0x00A0
	void* Func;                                                      // 0x00A8

	static UClass* StaticClass()
	{
		static auto ptr = UObject::FindObject<UClass>("Class CoreUObject.Function");
		return ptr;
	}
};


template<class TEnum>
class TEnumAsByte
{
public:
	inline TEnumAsByte()
	{
	}

	inline TEnumAsByte(TEnum _value)
		: value(static_cast<uint8_t>(_value))
	{
	}

	explicit inline TEnumAsByte(int32_t _value)
		: value(static_cast<uint8_t>(_value))
	{
	}

	explicit inline TEnumAsByte(uint8_t _value)
		: value(_value)
	{
	}

	inline operator TEnum() const
	{
		return (TEnum)value;
	}

	inline TEnum GetValue() const
	{
		return (TEnum)value;
	}

private:
	uint8_t value;
};

#pragma region Math

inline float deg2rad(float deg)
{
	return deg * M_PI / 180.f;
}

inline float rad2deg(float radian)
{
	float pi = 3.14159;
	return(radian * (180 / pi));
}

struct FVector2D
{
	union
	{
		struct { float X, Y; };
		struct { float x, y; };
		struct { float pitch, yaw; };
		struct { float value[2]; };
	};

	FVector2D() : x(0), y(0) {}
	FVector2D(float x) : x(x), y(0) {}
	FVector2D(float x, float y) : x(x), y(y) {}
	FVector2D(float value[2]) : x(value[0]), y(value[1]) {}
};

struct FVector
{
	union
	{
		struct { float X, Y, Z; };
		struct { float x, y, z; };
		struct { float pitch, yaw, roll; };
		struct { float value[3]; };
	};

	constexpr FVector(float x = 0.f, float y = 0.f, float z = 0.f) noexcept : X{ x }, Y{ y }, Z{ z } {}

	[[nodiscard]] friend constexpr auto operator-(const FVector& a, const FVector& b) noexcept -> FVector
	{
		return { a.X - b.X, a.Y - b.Y, a.Z - b.Z };
	}

	[[nodiscard]] friend constexpr auto operator+(const FVector& a, const FVector& b) noexcept -> FVector
	{
		return { a.X + b.X, a.Y + b.Y, a.Z + b.Z };
	}

	[[nodiscard]] friend constexpr auto operator*(const FVector& a, const FVector& b) noexcept -> FVector
	{
		return { a.X * b.X, a.Y * b.Y, a.Z * b.Z };
	}

	[[nodiscard]] friend constexpr auto operator*(const FVector& v, float f) noexcept -> FVector
	{
		return { v.X * f, v.Y * f, v.Z * f };
	}

	[[nodiscard]] friend constexpr auto operator/(const FVector& v, float f) noexcept -> FVector
	{
		return { v.X / f, v.Y / f, v.Z / f };
	}

	[[nodiscard]] friend constexpr auto operator-(float f, const FVector& v) noexcept -> FVector
	{
		return{ f - v.X, f - v.Y, f - v.Z };
	}

	constexpr auto& operator+=(const FVector& v) noexcept
	{
		X += v.X;
		Y += v.Y;
		Z += v.Z;
		return *this;
	}

	constexpr auto& operator-=(const FVector& v) noexcept
	{
		X -= v.X;
		Y -= v.Y;
		Z -= v.Z;
		return *this;
	}

	constexpr auto& operator*=(const FVector& v) noexcept
	{
		X *= v.X;
		Y *= v.Y;
		Z *= v.Z;
		return *this;
	}

	constexpr auto& operator/=(const FVector& v) noexcept
	{
		X /= v.X;
		Y /= v.Y;
		Z /= v.Z;
		return *this;
	}

	const float DotProduct(FVector coords)
	{
		return (this->X * coords.X) + (this->Y * coords.Y) + (this->Z * coords.Z);
	}

	bool isValid()
	{
		return (this->X != 0 && this->Y != 0 && this->Z != 0);
	}

	float length()
	{
		return (float)sqrt(X * X + Y * Y + Z * Z);
	}

	float distance(FVector vec)
	{
		float _x = this->X - vec.X;
		float _y = this->Y - vec.Y;
		float _z = this->Z - vec.Z;
		return sqrt((_x * _x) + (_y * _y) + (_z * _z)) * 0.03048f;
	}

	FVector normalize()
	{
		FVector newvec;
		newvec.X = this->X / length();
		newvec.Y = this->Y / length();
		newvec.Z = this->Z / length();
		return newvec;
	}

	FVector ToSin()
	{
		return { sin(this->x), sin(this->y), this->z };
	}
};

struct FRotator
{
	union
	{
		struct { float X, Y, Z; };
		struct { float x, y, z; };
		struct { float pitch, yaw, roll; };
		struct { float Pitch, Yaw, Roll; };
		struct { float value[3]; };
	};

	[[nodiscard]] friend constexpr auto operator-(const FRotator& a, const FRotator& b) noexcept -> FRotator
	{
		return { a.Pitch - b.Pitch, a.Yaw - b.Yaw, a.Roll - b.Roll };
	}

	[[nodiscard]] friend constexpr auto operator+(const FRotator& a, const FRotator& b) noexcept -> FRotator
	{
		return { a.Pitch + b.Pitch, a.Yaw + b.Yaw, a.Roll + b.Roll };
	}

	[[nodiscard]] friend constexpr auto operator*(const FRotator& a, const FRotator& b) noexcept -> FRotator
	{
		return { a.Pitch * b.Pitch, a.Yaw * b.Yaw, a.Roll * b.Roll };
	}

	[[nodiscard]] friend constexpr auto operator*(const FRotator& a, float b) noexcept -> FRotator
	{
		return { a.Pitch * b, a.Yaw * b, a.Roll * b };
	}

	[[nodiscard]] friend constexpr auto operator/(const FRotator& a, float b) noexcept -> FRotator
	{
		return { a.Pitch / b, a.Yaw / b, a.Roll / b };
	}

	float* fromAngle() const
	{
		float temp[3] = {
			std::cos(deg2rad(Pitch)) * std::cos(deg2rad(Yaw)),
			std::cos(deg2rad(Pitch)) * std::sin(deg2rad(Yaw)),
			std::sin(deg2rad(Pitch))
		};

		return temp;
	}

	constexpr auto& operator+=(const FRotator& o) noexcept
	{
		Pitch += o.Pitch;
		Yaw += o.Yaw;
		Roll += o.Roll;
		return *this;
	}



	constexpr auto& normalize() noexcept
	{
		Pitch = std::isfinite(Pitch) ? std::remainder(Pitch, 360.f) : 0.f;
		Yaw = std::isfinite(Yaw) ? std::remainder(Yaw, 360.f) : 0.f;
		Roll = 0.f;
		return *this;
	}
};


#pragma endregion

struct UKismetMathLibrary : UObject
{
	struct FVector Conv_RotatorToVector(struct FRotator InRot);
	float GetPI();

	struct FRotator FindLookAtRotation(struct FVector Start, struct FVector Target); // Function Engine.KismetMathLibrary.FindLookAtRotation

	static UClass* StaticClass()
	{
		static UClass* pStaticClass = 0;
		if (!pStaticClass)
			pStaticClass = UObject::FindObject<UClass>("Class Engine.KismetMathLibrary");
		return pStaticClass;
	}
};
