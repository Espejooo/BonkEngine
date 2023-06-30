#pragma once
#include <Windows.h>
#include <mutex>
#include <vector>

std::uint8_t* find_sig(LPCSTR module_name, const std::string& byte_array);

class signature
{
public:

	signature(const std::string& sig);

	signature import(const std::string& module_name = ("ShooterGame.exe"));

	signature add(uint32_t offset);
	signature sub(uint32_t offset);
	signature instruction(uint32_t offset);

	signature dump(const std::string& func_name);

#ifdef _WIN64
	uint64_t GetPointer();
#else
	uint32_t GetPointer();
#endif

private:
#ifdef _WIN64
	uint64_t pointer;
#else
	uint32_t pointer;
#endif

	bool imported;
	std::string sig;
	std::string module_name;

};
