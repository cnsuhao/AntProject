#pragma once
#include <sapi.h>
#include <string>
#include <iostream>
#include <tchar.h>

#pragma comment(lib,"sapi.lib")

class Speech
{
public:
	Speech();
	~Speech();
	static std::string getSystemError(int error_code, bool is_chiness_lang=true);
	static void speak(LPCWSTR content);

};

