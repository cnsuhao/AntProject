#include "DuiHead.h"

#include "MainView.h"
#include <vld.h>
int  _tWinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPTSTR    lpCmdLine,
	int       nCmdShow)
{
	//½ø³Ì»¥³â
	HANDLE Hmutex = CreateMutex(NULL, TRUE, _T("AppName"));
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		HWND h = ::FindWindow(NULL, _T("AppName"));
		if (h)
		{
			::SetForegroundWindow(h);
		}
		CloseHandle(Hmutex);
		::MessageBox(0, _T("App Running..."), _T("Tip"), MB_OK);
		return EXIT_FAILURE;
	}

	HRESULT Hr = ::CoInitialize(NULL);
	if (FAILED(Hr))
		return 0;

	CPaintManagerUI::SetInstance(hInstance);
	CPaintManagerUI::SetResourcePath(CPaintManagerUI::GetInstancePath());
	//CPaintManagerUI::SetResourceZip("skin.zip");
	MainView* mainvew = new MainView;
	mainvew->Create(NULL, _T("mainvew"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
	mainvew->FullScreen();
	mainvew->ShowModal();

	::CoUninitialize();
	return EXIT_SUCCESS;

}