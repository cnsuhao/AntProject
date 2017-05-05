#ifndef __UIDATETIME_H__
#define __UIDATETIME_H__

#pragma once

namespace DuiLib
{
	enum Time
	{
		SECOND_TIMER = 0
	};

	class CTimeControlUI;
	class CTimeWnd : public CWindowWnd
	{
	public:
		CTimeWnd();

		void Init(CTimeControlUI* pOwner,int i);
		RECT CalPos(int i);

		LPCTSTR GetWindowClassName() const;
		LPCTSTR GetSuperClassName() const;
		CDuiString GetWndText();
		void OnFinalMessage(HWND hWnd);

		LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
		LRESULT OnEditChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		LRESULT OnSetFoucs(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		LRESULT OnKillFoucs(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	protected:
		CTimeControlUI* m_pOwner;
		HBRUSH m_hBkBrush;
		int m_iFlag;
		CDuiString m_stext;
	};

	////////////////////////////////////////////////////////////
	///
	class UILIB_API CTimeControlUI :public CLabelUI
	{
		friend class CTimeWnd;
	public:
		CTimeControlUI();
		LPCTSTR GetClass() const;
		LPVOID  GetInterface(LPCTSTR pstrName);
		UINT	GetControlFlags() const;

		bool IsReadOnly() const;
		void	SetNativeEditBkColor(DWORD dwBkColor);//背景色
		DWORD	GetNativeEditBkColor() const;

		void Init();
		void NewWnd();
		void CloseWnd();
		CDuiRect GetSubPos(int i);
		void SetPos(RECT rc);
		void PaintStatusImage(HDC hDC);
		void PainSubRect(TEventUI& event);
		void DoEvent(TEventUI& event);
		void FormatTime();
		void AdjustTime();
		void OnBtnUp(TEventUI& event);
		void OnBtnDown(TEventUI& event);
		CDuiString GetTime();

	protected:
		CTimeWnd* m_pHourWnd;
		CTimeWnd* m_pMinWnd;
		CTimeWnd* m_pSecWnd;
		CDuiRect m_rcHourWnd;
		CDuiRect m_rcMinWnd;
		CDuiRect m_rcSecWnd;
		int m_iFocus;
		bool m_bInit;

		bool m_bReadOnly;
		DWORD m_dwEditbkColor;//背景色
		UINT m_uButtonState;

		CDuiRect m_rcBtnUp;
		CDuiRect m_rcBtnDown;
		int   m_iHour;
		int   m_iMin;
		int   m_iSec;

	};

	////////////////////////////////////////////////////////////
	////// 时间选择控件
	class CDateTimeWnd;
	class UILIB_API CDateTimeUI : public CLabelUI
	{
		friend class CDateTimeWnd;
	public:
		CDateTimeUI();
		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		SYSTEMTIME& GetTime();
		void SetTime(SYSTEMTIME* pst);

		void SetReadOnly(bool bReadOnly);
		bool IsReadOnly() const;

		void UpdateText();

		void DoEvent(TEventUI& event);

	protected:
		SYSTEMTIME m_sysTime;
		int        m_nDTUpdateFlag;
		bool       m_bReadOnly;

		CDateTimeWnd* m_pWindow;
	};
}
#endif // __UIDATETIME_H__