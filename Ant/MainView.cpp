#include "MainView.h"

#include <typeinfo>

MainView::MainView()
{
}


MainView::~MainView()
{
}

LPCTSTR MainView::GetWindowClassName() const
{
	return _T("MainView");
}

DuiLib::CDuiString MainView::GetSkinFile()
{
	return _T("MainView.xml");
}

DuiLib::CDuiString MainView::GetSkinFolder()
{
	return _T("skin");
}
