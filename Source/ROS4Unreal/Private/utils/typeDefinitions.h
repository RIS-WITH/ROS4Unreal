#pragma once
#include <string>

inline std::string fstring2string(const FString& msg) {
	return std::string(TCHAR_TO_UTF8(*msg));
};

inline FString string2Fstring(const std::string& msg) {
	return FString(msg.c_str());
};
