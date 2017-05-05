#include "Speech.h"
#include <windows.h>
#include <tchar.h>
using namespace std;

Speech::Speech()
{
}


Speech::~Speech()
{
}

void Speech::speak(LPCWSTR content)
{
	ISpVoice* voice = nullptr;
	// [init com interface]
	if (FAILED(::CoInitialize(nullptr)))
	{
		cout << getSystemError(GetLastError());
		return;
	}

	HRESULT hr = CoCreateInstance(CLSID_SpVoice, nullptr, CLSCTX_ALL, IID_ISpVoice, (void**)&voice);

	if (SUCCEEDED(hr))
	{
		voice->SetVolume(100);// 0-100
		voice->SetRate(0); // -10-10
		hr = voice->Speak(content, 0, nullptr);
		voice->Release();
		voice = nullptr;
	}
	::CoUninitialize();
}

std::string Speech::getSystemError(int error_code, bool is_chiness_lang)
{
	LPVOID strbuffer = NULL;
	FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ARGUMENT_ARRAY |
		FORMAT_MESSAGE_IGNORE_INSERTS, NULL, error_code,
		MAKELANGID(is_chiness_lang ? LANG_CHINESE : LANG_ENGLISH, is_chiness_lang ? SUBLANG_CHINESE_SIMPLIFIED : SUBLANG_ENGLISH_US), (LPSTR)&strbuffer, 0, NULL);
	string ret = (strbuffer == NULL) ? "" : (char*)strbuffer;
	if (strbuffer)
		LocalFree(strbuffer);
	return ret;
}
