/*
|	HackSimulator v0.0.4
|
|	CodingFunction.cpp
	this cpp is for some coding functions
	这个文件用于一些编码相关函数
|
|	https://github.com/FlixKlein/HackSimulator
|	https://gitee.com/rosemarychn/HackSimulator
|	License : MIT
|	Personal Blog : https://undertopia.top
*/
#include "hacksimulator.h"
//copilot编写
#include <Windows.h>
inline std::string to_string(const char8_t* u8_str) {
	return std::string(reinterpret_cast<const char*>(u8_str));
}

// 将 std::string (本地编码/ANSI) 转换为 UTF-8 编码的 std::string
std::string to_utf8(const std::string& input) {
	// 1. 先将 input 转为宽字符
	int wide_len = MultiByteToWideChar(CP_ACP, 0, input.c_str(), -1, nullptr, 0);
	std::wstring wide_string(wide_len, L'\0');
	MultiByteToWideChar(CP_ACP, 0, input.c_str(), -1, &wide_string[0], wide_len);

	// 2. 再将宽字符转为 UTF-8
	int utf8_len = WideCharToMultiByte(CP_UTF8, 0, wide_string.c_str(), -1, nullptr, 0, nullptr, nullptr);
	std::string utf8_string(utf8_len, '\0');
	WideCharToMultiByte(CP_UTF8, 0, wide_string.c_str(), -1, &utf8_string[0], utf8_len, nullptr, nullptr);

	// 去除末尾的 '\0'
	if (!utf8_string.empty() && utf8_string.back() == '\0') {
		utf8_string.pop_back();
	}
	return utf8_string;
}

// 将 UTF-8 编码的 std::string 转回本地编码 std::string
std::string from_utf8(const std::string& utf8_str) {
	// 1. 先将 UTF-8 转为宽字符
	int wide_len = MultiByteToWideChar(CP_UTF8, 0, utf8_str.c_str(), -1, nullptr, 0);
	std::wstring wide_string(wide_len, L'\0');
	MultiByteToWideChar(CP_UTF8, 0, utf8_str.c_str(), -1, &wide_string[0], wide_len);

	// 2. 再将宽字符转为本地编码
	int ansi_len = WideCharToMultiByte(CP_ACP, 0, wide_string.c_str(), -1, nullptr, 0, nullptr, nullptr);
	std::string ansi_string(ansi_len, '\0');
	WideCharToMultiByte(CP_ACP, 0, wide_string.c_str(), -1, &ansi_string[0], ansi_len, nullptr, nullptr);

	// 去除末尾的 '\0'
	if (!ansi_string.empty() && ansi_string.back() == '\0') {
		ansi_string.pop_back();
	}
	return ansi_string;
}

//一个hash加密编码方法，单向的
//当然也是gemini写的，要是我自己写我会用费马小定理RSA，有空了再说吧(2025/12/07)
std::string simple_fnv_hash(const std::string& key, const std::string& message) {
	// 结合 key 和 message
	std::string combined = key + message;

	// FNV 初始值 (FNV offset basis)
	unsigned int hash = 2166136261U;
	unsigned int FNV_PRIME = 16777619U;

	// 1. 遍历字符串，进行哈希运算
	for (char byte : combined) {
		// 乘法
		hash *= FNV_PRIME;
		// 异或
		hash ^= (unsigned char)byte;
	}

	// 2. 将 32 位整数哈希值转换为 8 个字符的十六进制字符串
	std::stringstream ss;
	ss << std::hex << std::setw(8) << std::setfill('0') << hash;
	return ss.str();
}