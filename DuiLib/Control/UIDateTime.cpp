#include "stdafx.h"
#include "UIDateTime.h"

namespace DuiLib
{
	////////////////////////////////////////////////////////////
	///
	CTimeWnd::CTimeWnd() : m_pOwner(NULL), 
		m_hBkBrush(NULL),
		m_iFlag(-1)
	{
	}

	void CTimeWnd::Init(CTimeControlUI* pOwner,int i)
	{
		m_pOwner = pOwner;

		if (m_hWnd == NULL)
		{
			RECT rcPos = CalPos(i);
			UINT uStyle = WS_CHILD | WS_TABSTOP;
			Create(m_pOwner->GetManager()->GetPaintWindow(), NULL, uStyle, 0, rcPos);
			SetWindowFont(m_hWnd, m_pOwner->GetManager()->GetFontInfo(m_pOwner->GetFont())->hFont, TRUE);
		}

		Edit_SetModify(m_hWnd, FALSE);
		SendMessage(EM_SETMARGINS, EC_LEFTMARGIN | EC_RIGHTMARGIN, MAKELPARAM(0, 0));
		Edit_Enable(m_hWnd, m_pOwner->IsEnabled() == true);
		Edit_SetReadOnly(m_hWnd, m_pOwner->IsReadOnly() == true);

		::ShowWindow(m_hWnd, SW_SHOWNOACTIVATE);
		m_iFlag = i;
		//::SetFocus(m_hWnd);  
	}

	RECT CTimeWnd::CalPos(int i)
	{
		CDuiRect rcPos = m_pOwner->GetSubPos(i);
		return rcPos;
	}

	LPCTSTR CTimeWnd::GetWindowClassName() const
	{
		return _T("TimeWnd");
	}

	LPCTSTR CTimeWnd::GetSuperClassName() const
	{
		return WC_EDIT;
	}

	CDuiString CTimeWnd::GetWndText()
	{
		char text[3]= {0,0,0};
		::GetWindowText(m_hWnd,text,3);
		CDuiString sReturn = text;
		return sReturn;
	}

	void CTimeWnd::OnFinalMessage(HWND /*hWnd*/)
	{
		if( m_hBkBrush != NULL ) 
			::DeleteObject(m_hBkBrush);
		delete this;
	}

	LRESULT CTimeWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		//TRACE("%s",TRACEMSG(uMsg));
		LRESULT lRes = 0;
		BOOL bHandled = TRUE;
		if( uMsg == OCM_COMMAND ) 
		{
			if( GET_WM_COMMAND_CMD(wParam, lParam) == EN_CHANGE ) 
				lRes = OnEditChanged(uMsg, wParam, lParam, bHandled);
			else if( GET_WM_COMMAND_CMD(wParam, lParam) == EN_UPDATE ) 
			{
				RECT rcClient;
				::GetClientRect(m_hWnd, &rcClient);
				::InvalidateRect(m_hWnd, &rcClient, FALSE);
			}
		}
		else if (uMsg == WM_SETFOCUS)
		{
			OnSetFoucs(uMsg, wParam, lParam, bHandled);
			bHandled = FALSE;
		}
		else if (uMsg == WM_KILLFOCUS)
		{
			OnKillFoucs(uMsg, wParam, lParam, bHandled);
			bHandled = FALSE;
		}
		else if( uMsg == WM_KEYDOWN && TCHAR(wParam) == VK_RETURN ) 
		{
			m_pOwner->GetManager()->SendNotify(m_pOwner, _T("return"));
		}
		else if (uMsg == WM_CHAR)
		{
			if(wParam>=48 && wParam <= 57 || wParam== 8)
				bHandled = FALSE;
		}
		else if( uMsg == OCM__BASE + WM_CTLCOLOREDIT  || uMsg == OCM__BASE + WM_CTLCOLORSTATIC ) 
		{
			if( m_pOwner->GetNativeEditBkColor() == 0xFFFFFFFF ) 
				return NULL;
			::SetBkMode((HDC)wParam, TRANSPARENT);
			DWORD dwTextColor = m_pOwner->GetTextColor();
			::SetTextColor((HDC)wParam, RGB(GetBValue(dwTextColor),GetGValue(dwTextColor),GetRValue(dwTextColor)));
			if( m_hBkBrush == NULL ) 
			{
				DWORD clrColor = m_pOwner->GetNativeEditBkColor();
				m_hBkBrush = ::CreateSolidBrush(RGB(GetBValue(clrColor), GetGValue(clrColor), GetRValue(clrColor)));
			}
			return (LRESULT)m_hBkBrush;
		}
		else if (uMsg == WM_TIMER)
		{
			if(wParam == SECOND_TIMER)
			{
				++(m_pOwner->m_iSec);
				m_pOwner->AdjustTime();
				m_pOwner->FormatTime();
			}
		}
		else bHandled = FALSE;
		if( !bHandled ) return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
		return lRes;
	}

	LRESULT CTimeWnd::OnEditChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
#if 1
		return 0;
#else
		if( m_pOwner == NULL ) return 0;
		int cchLen = ::GetWindowTextLength(m_hWnd) + 1;
		LPTSTR pstr = static_cast<LPTSTR>(_alloca(cchLen * sizeof(TCHAR)));
		ASSERT(pstr);
		if( pstr == NULL ) return 0;
		::GetWindowText(m_hWnd, pstr, cchLen);
		//::SetWindowText(m_hWnd,"11");
		return 0;
#endif
	}

	LRESULT CTimeWnd::OnSetFoucs(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if(m_pOwner)
			m_pOwner->m_iFocus = m_iFlag;
		int cchLen = ::GetWindowTextLength(m_hWnd) + 1;
		LPTSTR pstr = static_cast<LPTSTR>(_alloca(cchLen * sizeof(TCHAR)));
		ASSERT(pstr);
		if( pstr == NULL ) return 0;
		::GetWindowText(m_hWnd, pstr, cchLen);
		m_stext = pstr;
		return 0;
	}

	LRESULT CTimeWnd::OnKillFoucs(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		char text[3]= {0,0,0};
		::GetWindowText(m_hWnd,text,3);
		if (text[1] == 0)
		{
			CDuiString str_text = "0";
			str_text += text[0];
			::SetWindowText(m_hWnd,str_text.GetData());
		}
		else if(text[0]>49)
		{
			if(m_iFlag == 0)
			{
				if(text[0]==50 && text[1]>51 || text[0]>50)
					::SetWindowText(m_hWnd,m_stext.GetData());
			}
			else if(text[0]>53)
				::SetWindowText(m_hWnd,m_stext.GetData());
		}
		return 0;
	}

	////////////////////////////////////////////////////////////
	///

	CTimeControlUI::CTimeControlUI():m_bReadOnly(false),
		m_dwEditbkColor(0xFFFFFFFF),
		m_pHourWnd(NULL),
		m_pMinWnd(NULL),
		m_pSecWnd(NULL),
		m_uButtonState(0),
		m_bInit(false)
	{
		SetTextPadding(CDuiRect(4, 3, 16, 3));
	}

	//CTimeControlUI::~CTimeControlUI()
	//{
	//	::KillTimer(*m_pSecWnd,SECOND_TIMER);
	//}

	LPCTSTR CTimeControlUI::GetClass() const
	{
		return "TimeControl";
	}

	LPVOID CTimeControlUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcscmp(pstrName, _T("TimeControl")) == 0 ) 
			return static_cast<CTimeControlUI*>(this);
		return CLabelUI::GetInterface(pstrName);
	}

	UINT  CTimeControlUI::GetControlFlags() const
	{
		return UIFLAG_TABSTOP | (IsEnabled() ? UIFLAG_SETCURSOR : 0);
	}

	bool CTimeControlUI::IsReadOnly() const
	{
		return m_bReadOnly;
	}

	void CTimeControlUI::SetNativeEditBkColor(DWORD dwBkColor)
	{
		m_dwEditbkColor = dwBkColor;
	}

	DWORD CTimeControlUI::GetNativeEditBkColor() const
	{
		return m_dwEditbkColor;
	}

	void CTimeControlUI::Init()
	{
		time_t timer = time(NULL);
		struct tm *tblock = localtime(&timer);
		m_iHour = tblock->tm_hour;
		m_iMin = tblock->tm_min;
		m_iSec = tblock->tm_sec;
		//////////////////////
		CDuiString text,temp;
		if(m_iHour<10)
			text.Format("%d%d",0,m_iHour);
		else
			text.Format("%d",m_iHour);
		text += ":";
		if(m_iMin<10)
			temp.Format("%d%d",0,m_iMin);
		else
			temp.Format("%d",m_iMin);
		text += temp;
		text += ":";
		if (m_iSec<10)
			temp.Format("%d%d",0,m_iSec);
		else
			temp.Format("%d",m_iSec);
		text += temp;
		SetText(text);
		//////////////////
	}

	void CTimeControlUI::NewWnd()
	{
		m_pHourWnd = new CTimeWnd();
		ASSERT(m_pHourWnd);
		m_pHourWnd->Init(this,0);

		m_pMinWnd = new CTimeWnd();
		ASSERT(m_pMinWnd);
		m_pMinWnd->Init(this,1);

		m_pSecWnd = new CTimeWnd();
		ASSERT(m_pSecWnd);
		m_pSecWnd->Init(this,2);

		FormatTime();
	}

	void CTimeControlUI::CloseWnd()
	{
		SetText(GetTime());
		m_pManager->SendNotify(this,_T("udpate_time"));
		if(m_pHourWnd)
		{
			::PostMessage(*m_pHourWnd,WM_CLOSE,0,0);
			m_pHourWnd = NULL;
		}
		if(m_pMinWnd)
		{
			::PostMessage(*m_pMinWnd,WM_CLOSE,0,0);
			m_pMinWnd = NULL;
		}
		if(m_pSecWnd)
		{
			::PostMessage(*m_pSecWnd,WM_CLOSE,0,0);
			m_pSecWnd = NULL;
		}
	}

	CDuiRect CTimeControlUI::GetSubPos(int i)
	{
		CDuiRect rcPos = m_rcItem;
		LONG lEditHeight = m_pManager->GetDefaultFontInfo()->tm.tmHeight;
		if( lEditHeight < rcPos.GetHeight() ) 
		{
			rcPos.top += (rcPos.GetHeight() - lEditHeight) / 2;
			rcPos.bottom = rcPos.top + lEditHeight;
		}
		/////////////////////////////////////
		CDuiRect rcHour = rcPos;
		rcHour.left = rcPos.left + m_rcTextPadding.left;
		rcHour.right = rcHour.left + 12;
		if(i == 0)
			return rcHour;

		CDuiRect rcMin = rcHour;
		rcMin.left = rcMin.right + 6;//冒号
		rcMin.right = rcMin.left + 12;
		if(i == 1)
			return rcMin;

		CDuiRect rcSec = rcMin;
		rcSec.left = rcSec.right + 6;
		rcSec.right = rcSec.left + 12;
		return rcSec;
	}

	void CTimeControlUI::SetPos(RECT rc)
	{
		CControlUI::SetPos(rc);
		if( m_pHourWnd != NULL ) 
		{
			CDuiRect rcTemp = m_pHourWnd->CalPos(0);
			::SetWindowPos(m_pHourWnd->GetHWND(), NULL, rcTemp.left, rcTemp.top,0,0, SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE);
			rcTemp = m_pMinWnd->CalPos(1);
			::SetWindowPos(m_pMinWnd->GetHWND(), NULL, rcTemp.left, rcTemp.top,0,0, SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE);
			rcTemp = m_pMinWnd->CalPos(2);
			::SetWindowPos(m_pSecWnd->GetHWND(), NULL, rcTemp.left, rcTemp.top,0,0, SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE);
		}
	}

	void CTimeControlUI::FormatTime()
	{
		CDuiString text;
		if(m_iHour<10)
			text.Format("%d%d",0,m_iHour);
		else
			text.Format("%d",m_iHour);
		Edit_SetText(*m_pHourWnd,text);

		if(m_iMin<10)
			text.Format("%d%d",0,m_iMin);
		else
			text.Format("%d",m_iMin);
		Edit_SetText(*m_pMinWnd,text);

		if (m_iSec<10)
			text.Format("%d%d",0,m_iSec);
		else
			text.Format("%d",m_iSec);
		Edit_SetText(*m_pSecWnd,text);
		SetText(GetTime());
	}

	void CTimeControlUI::AdjustTime()
	{
		if (m_iSec == 60)
		{
			m_iSec = 0;
			++m_iMin;
		}
		else if (m_iSec == -1)
		{
			m_iSec = 59;
			--m_iMin;
		}
		if (m_iMin == 60)
		{
			m_iMin = 0;
			++m_iHour;
		}
		else if (m_iMin == -1)
		{
			m_iMin = 59;
			--m_iHour;
		}
		if (m_iHour == 24)
		{
			m_iHour = 0;
		}
		else if (m_iHour == -1)
		{
			m_iHour = 23;
		}
	}

	void CTimeControlUI::PaintStatusImage(HDC hDC)
	{
		m_rcBtnUp = m_rcItem;
		m_rcBtnUp.left = m_rcBtnUp.right - m_rcTextPadding.right;
		int height = m_rcBtnUp.GetHeight()/2;
		m_rcBtnUp.bottom = m_rcBtnUp.top + height;
		CDuiString image = "btn_up.png";
		CDuiString image_modify = "dest='4,2,12,8' source='0,0,8,6'";
		CRenderEngine::DrawImageString(hDC,m_pManager,m_rcBtnUp,m_rcBtnUp,image,image_modify);

		m_rcBtnDown = m_rcBtnUp;
		m_rcBtnDown.top = m_rcBtnDown.bottom;
		m_rcBtnDown.bottom = m_rcBtnDown.top + height;
		image = "btn_down.png";
		CRenderEngine::DrawImageString(hDC,m_pManager,m_rcBtnDown,m_rcBtnDown,image,image_modify);

		if(m_uButtonState & UISTATE_HOT)
		{
			image_modify = "dest='4,2,12,8' source='8,0,16,6'";
			image = "btn_up.png";
			CRenderEngine::DrawImageString(hDC,m_pManager,m_rcBtnUp,m_rcBtnUp,image,image_modify);
			image = "btn_down.png";
			CRenderEngine::DrawImageString(hDC,m_pManager,m_rcBtnDown,m_rcBtnDown,image,image_modify);
		}
		/////////////////////////////
		if (m_rcHourWnd.left == 0)
		{
			m_rcHourWnd = m_rcItem;
			m_rcHourWnd.left = m_rcItem.left + m_rcTextPadding.left;
			m_rcHourWnd.right = m_rcHourWnd.left + 12;
			LONG lEditHeight = m_pManager->GetDefaultFontInfo()->tm.tmHeight;
			if( lEditHeight < m_rcHourWnd.GetHeight() ) 
			{
				m_rcHourWnd.top += (m_rcHourWnd.GetHeight() - lEditHeight) / 2;
				m_rcHourWnd.bottom = m_rcHourWnd.top + lEditHeight;
			}

			m_rcMinWnd = m_rcHourWnd;
			m_rcMinWnd.left = m_rcMinWnd.right + 6;//冒号
			m_rcMinWnd.right = m_rcMinWnd.left + 12;

			m_rcSecWnd = m_rcMinWnd;
			m_rcSecWnd.left = m_rcSecWnd.right + 6;
			m_rcSecWnd.right = m_rcSecWnd.left + 12;
		}
		////////////////////////////////
		CDuiRect rc_temp = m_rcHourWnd;
		rc_temp.left = rc_temp.right;
		rc_temp.right = rc_temp.left + 6;
		CRenderEngine::DrawText(hDC,m_pManager,rc_temp,":",0x0,-1,DT_VCENTER | DT_CENTER);

		rc_temp.left = m_rcMinWnd.right;
		rc_temp.right = rc_temp.left + 6;
		CRenderEngine::DrawText(hDC,m_pManager,rc_temp,":",0x0,-1,DT_VCENTER | DT_CENTER);
	}

	void CTimeControlUI::PainSubRect(TEventUI& event)
	{
		PAINTSTRUCT  ps;
		HDC hdc;
		DWORD dwColor = 0xff2272b4;
		if (::PtInRect(&m_rcHourWnd,event.ptMouse))
		{
			hdc = ::BeginPaint(m_pHourWnd->GetHWND(), &ps);
			CRenderEngine::DrawColor(hdc,m_rcHourWnd,dwColor);
			::EndPaint(m_pHourWnd->GetHWND(), &ps);
		}
		else if (::PtInRect(&m_rcMinWnd,event.ptMouse))
		{

		}
		else if (::PtInRect(&m_rcSecWnd,event.ptMouse))
		{

		}
	}

	void CTimeControlUI::DoEvent(TEventUI& event)
	{
		if( event.Type == UIEVENT_SETCURSOR )//使光标变为手型 
		{
			if(::PtInRect(&m_rcBtnUp,event.ptMouse)||::PtInRect(&m_rcBtnDown,event.ptMouse))
				::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_HAND)));
			else if( IsEnabled() )
				::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_IBEAM)));
			return;
		}
		if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) 
		{
			if( m_pParent != NULL )
				m_pParent->DoEvent(event);
			else 
				CLabelUI::DoEvent(event);
			return;
		}
		if( event.Type == UIEVENT_KILLFOCUS && IsEnabled() ) 
		{
			CloseWnd();
			Invalidate();
		}
		if( event.Type == UIEVENT_SETFOCUS && IsEnabled() ) 
		{
			if(m_pHourWnd == NULL)
				NewWnd();
			//::SetTimer(*m_pSecWnd,SECOND_TIMER,1000,NULL);
			Invalidate();
			return;
		}	
		if( event.Type == UIEVENT_BUTTONDOWN) 
		{
			if( IsEnabled() ) 
			{
				GetManager()->ReleaseCapture();
				////////////////////////
				if (::PtInRect(&m_rcBtnUp,event.ptMouse))
					OnBtnUp(event);
				else if (::PtInRect(&m_rcBtnDown,event.ptMouse))
					OnBtnDown(event);
				//else 
				//PainSubRect(event);
				////////////////////////
			}
			return;
		}
		if( event.Type == UIEVENT_MOUSEMOVE ) 
		{
			return;
		}
		if( event.Type == UIEVENT_BUTTONUP ) 
		{
			return;
		}
		if( event.Type == UIEVENT_CONTEXTMENU )
		{
			return;
		}
		if( event.Type == UIEVENT_MOUSEENTER )
		{
			if( IsEnabled() ) 
			{
				m_uButtonState |= UISTATE_HOT;
				Invalidate();
			}
			return;
		}
		if( event.Type == UIEVENT_MOUSELEAVE )
		{
			if( IsEnabled() )
			{
				m_uButtonState &= ~UISTATE_HOT;
				Invalidate();
			}
			return;
		}
		CLabelUI::DoEvent(event);

	}

	void CTimeControlUI::OnBtnUp(TEventUI& event)
	{
		if(m_iFocus == 0)
			++m_iHour;
		else if(m_iFocus == 1)
			++m_iMin;
		else 
			++m_iSec;
		AdjustTime();
		FormatTime();
		m_pManager->SendNotify(this,_T("udpate_time"));
	}

	void CTimeControlUI::OnBtnDown(TEventUI& event)
	{
		if(m_iFocus == 0)
			--m_iHour;
		else if(m_iFocus == 1)
			--m_iMin;
		else 
			--m_iSec;
		AdjustTime();
		FormatTime();
		m_pManager->SendNotify(this,_T("udpate_time"));
	}

	CDuiString CTimeControlUI::GetTime()
	{
		CDuiString text = "";
		CDuiString temp = "";
		if(m_pHourWnd)
		{
			temp = m_pHourWnd->GetWndText();
			m_iHour = atoi(temp.GetData());
			text += temp;
			text += ":";
		}
		if(m_pMinWnd)
		{
			temp = m_pMinWnd->GetWndText();
			m_iMin = atoi(temp.GetData());
			text += temp;
			text += ":";
		}
		if(m_pSecWnd)
		{
			temp = m_pSecWnd->GetWndText();
			m_iSec = atoi(temp.GetData());
			text += temp;
		}
		return text;
	}

	////////////////////////////////////////////////////////////
	///
	#define DT_NONE   0
	#define DT_UPDATE 1
	#define DT_DELETE 2
	#define DT_KEEP   3

	class CDateTimeWnd : public CWindowWnd
	{
	public:
		CDateTimeWnd();

		void Init(CDateTimeUI* pOwner);
		RECT CalPos();

		LPCTSTR GetWindowClassName() const;
		LPCTSTR GetSuperClassName() const;
		void OnFinalMessage(HWND hWnd);

		LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
		LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		//LRESULT OnEditChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	protected:
		CDateTimeUI* m_pOwner;
		HBRUSH m_hBkBrush;
		bool m_bInit;
	};

	CDateTimeWnd::CDateTimeWnd() : m_pOwner(NULL), m_hBkBrush(NULL), m_bInit(false)
	{
	}

	void CDateTimeWnd::Init(CDateTimeUI* pOwner)
	{
		m_pOwner = pOwner;
		m_pOwner->m_nDTUpdateFlag = DT_NONE;

		if (m_hWnd == NULL)
		{
			RECT rcPos = CalPos();
			UINT uStyle = WS_CHILD;
			Create(m_pOwner->GetManager()->GetPaintWindow(), NULL, uStyle, 0, rcPos);
			SetWindowFont(m_hWnd, m_pOwner->GetManager()->GetFontInfo(m_pOwner->GetFont())->hFont, TRUE);
		}

		if (m_pOwner->GetText().IsEmpty())
			::GetLocalTime(&m_pOwner->m_sysTime);

		::SendMessage(m_hWnd, DTM_SETSYSTEMTIME, 0, (LPARAM)&m_pOwner->m_sysTime);
		::ShowWindow(m_hWnd, SW_SHOWNOACTIVATE);
		::SetFocus(m_hWnd);

		m_bInit = true;    
	}

	RECT CDateTimeWnd::CalPos()
	{
		CDuiRect rcPos = m_pOwner->GetPos();
		return rcPos;
	}

	LPCTSTR CDateTimeWnd::GetWindowClassName() const
	{
		return _T("DateTimeWnd");
	}

	LPCTSTR CDateTimeWnd::GetSuperClassName() const
	{
		return DATETIMEPICK_CLASS;
	}

	void CDateTimeWnd::OnFinalMessage(HWND /*hWnd*/)
	{
		// Clear reference and die
		if( m_hBkBrush != NULL ) ::DeleteObject(m_hBkBrush);
		m_pOwner->m_pWindow = NULL;
		delete this;
	}

	LRESULT CDateTimeWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		LRESULT lRes = 0;
		BOOL bHandled = TRUE;
		
		if(WM_NOTIFY==uMsg)
		{
			::SetFocus(m_hWnd);
		}
		else if( uMsg == WM_KILLFOCUS)
		{
			//这里肯可能需要优化，因为FindWindow找出来的窗口不一定是本进程的窗口
			HWND hh=::FindWindow(_T("SysMonthCal32"),NULL);
			if(::IsWindow(hh))
			{
				MCHITTESTINFO pp;
				memset(&pp,0,sizeof(pp));
				GetCursorPos(&pp.pt);
				::ScreenToClient(hh,&pp.pt);
				pp.cbSize=sizeof(pp);
				MonthCal_HitTest(hh,&pp);
				//下一个月
				if(pp.uHit==MCHT_TITLEBTNNEXT)
					return 1;
				//上一个月
				if(pp.uHit==MCHT_TITLEBTNPREV)
					return 1;
			}
			else
			{
				POINT pt;
				::GetCursorPos(&pt); 
				RECT rt;
				::GetWindowRect(m_hWnd,&rt);
				if( !(pt.x>=rt.left&&pt.x<=rt.right)|| !(pt.x>=rt.top&&pt.x<=rt.bottom) )
					lRes= OnKillFocus(uMsg,wParam, lParam,bHandled);
			}
		}
		else if (uMsg == WM_KEYUP && (wParam == VK_DELETE || wParam == VK_BACK))
		{
			LRESULT lRes = ::DefWindowProc(m_hWnd, uMsg, wParam, lParam);
			m_pOwner->m_nDTUpdateFlag = DT_DELETE;
			m_pOwner->UpdateText();
			PostMessage(WM_CLOSE);
			return lRes;
		}
		else if (uMsg == WM_KEYUP && wParam == VK_ESCAPE)
		{
			LRESULT lRes = ::DefWindowProc(m_hWnd, uMsg, wParam, lParam);
			m_pOwner->m_nDTUpdateFlag = DT_KEEP;
			PostMessage(WM_CLOSE);
			return lRes;
		}
		else 
			bHandled = FALSE;

		if(!bHandled ) 
			return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
		return lRes;
	}

	LRESULT CDateTimeWnd::OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		LRESULT lRes = ::DefWindowProc(m_hWnd, uMsg, wParam, lParam);
		if (m_pOwner->m_nDTUpdateFlag == DT_NONE)
		{
			::SendMessage(m_hWnd, DTM_GETSYSTEMTIME, 0, (LPARAM)&m_pOwner->m_sysTime);
			m_pOwner->m_nDTUpdateFlag = DT_UPDATE;
			m_pOwner->UpdateText();
		}
		PostMessage(WM_CLOSE);
		return lRes;
	}

	// LRESULT CDateTimeWnd::OnEditChanged(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	// {
	// 	if( !m_bInit ) return 0;
	// 	if( m_pOwner == NULL ) return 0;
	// 	// Copy text back
	// 	int cchLen = ::GetWindowTextLength(m_hWnd) + 1;
	// 	LPTSTR pstr = static_cast<LPTSTR>(_alloca(cchLen * sizeof(TCHAR)));
	// 	ASSERT(pstr);
	// 	if( pstr == NULL ) return 0;
	// 	::GetWindowText(m_hWnd, pstr, cchLen);
	// 	m_pOwner->m_sText = pstr;
	// 	m_pOwner->GetManager()->SendNotify(m_pOwner, DUI_MSGTYPE_TEXTCHANGED);
	// 	return 0;
	// }

	//////////////////////////////////////////////////////////////////////////
	//
	CDateTimeUI::CDateTimeUI()
	{
		::GetLocalTime(&m_sysTime);
		m_bReadOnly = false;
		m_pWindow = NULL;
		m_nDTUpdateFlag=DT_UPDATE;
		UpdateText();		// add by:daviyang35 初始化界面时显示时间
		m_nDTUpdateFlag = DT_NONE;
	}

	LPCTSTR CDateTimeUI::GetClass() const
	{
		return _T("DateTimeUI");
	}

	LPVOID CDateTimeUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcscmp(pstrName, DUI_CTR_DATETIME) == 0 ) return static_cast<CDateTimeUI*>(this);
		return CLabelUI::GetInterface(pstrName);
	}

	SYSTEMTIME& CDateTimeUI::GetTime()
	{
		return m_sysTime;
	}

	void CDateTimeUI::SetTime(SYSTEMTIME* pst)
	{
		m_sysTime = *pst;
		Invalidate();
	}

	void CDateTimeUI::SetReadOnly(bool bReadOnly)
	{
		m_bReadOnly = bReadOnly;
		Invalidate();
	}

	bool CDateTimeUI::IsReadOnly() const
	{
		return m_bReadOnly;
	}

	void CDateTimeUI::UpdateText()
	{
		if (m_nDTUpdateFlag == DT_DELETE)
			SetText(_T(""));
		else if (m_nDTUpdateFlag == DT_UPDATE)
		{
			CDuiString sText;
			sText.SmallFormat(_T("%4d-%02d-%02d"),
				m_sysTime.wYear, m_sysTime.wMonth, m_sysTime.wDay, m_sysTime.wHour, m_sysTime.wMinute);
			SetText(sText);
		}
	}

	void CDateTimeUI::DoEvent(TEventUI& event)
	{
		if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
			if( m_pParent != NULL ) m_pParent->DoEvent(event);
			else CLabelUI::DoEvent(event);
			return;
		}

		if( event.Type == UIEVENT_SETCURSOR && IsEnabled() )
		{
			::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_IBEAM)));
			return;
		}
		if( event.Type == UIEVENT_WINDOWSIZE )
		{
			if( m_pWindow != NULL ) m_pManager->SetFocusNeeded(this);
		}
		if( event.Type == UIEVENT_SCROLLWHEEL )
		{
			if( m_pWindow != NULL ) return;
		}
		if( event.Type == UIEVENT_SETFOCUS && IsEnabled() ) 
		{
			if( m_pWindow ) return;
			m_pWindow = new CDateTimeWnd();
			ASSERT(m_pWindow);
			m_pWindow->Init(this);
			m_pWindow->ShowWindow();
		}
		if( event.Type == UIEVENT_KILLFOCUS && IsEnabled() ) 
		{
			Invalidate();
		}
		if( event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_DBLCLICK || event.Type == UIEVENT_RBUTTONDOWN) 
		{
			if( IsEnabled() ) {
				GetManager()->ReleaseCapture();
				if( IsFocused() && m_pWindow == NULL )
				{
					m_pWindow = new CDateTimeWnd();
					ASSERT(m_pWindow);
				}
				if( m_pWindow != NULL )
				{
					m_pWindow->Init(this);
					m_pWindow->ShowWindow();
				}
			}
			return;
		}
		if( event.Type == UIEVENT_MOUSEMOVE ) 
		{
			return;
		}
		if( event.Type == UIEVENT_BUTTONUP ) 
		{
			return;
		}
		if( event.Type == UIEVENT_CONTEXTMENU )
		{
			return;
		}
		if( event.Type == UIEVENT_MOUSEENTER )
		{
			return;
		}
		if( event.Type == UIEVENT_MOUSELEAVE )
		{
			return;
		}

		CLabelUI::DoEvent(event);
	}
}
