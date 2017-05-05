#ifndef __UIMENU_H__
#define __UIMENU_H__

#ifdef _MSC_VER
#pragma once
#endif

#include <map>

namespace DuiLib {

struct ContextMenuParam
{
	// 1: remove all
	// 2: remove the sub menu
	WPARAM wParam;
	HWND hWnd;
};

enum MenuAlignment
{
	eMenuAlignment_Left = 1 << 1,
	eMenuAlignment_Top = 1 << 2,
	eMenuAlignment_Right = 1 << 3,
	eMenuAlignment_Bottom = 1 << 4,
};


enum MenuItemDefaultInfo
{
	ITEM_DEFAULT_HEIGHT = 24,		//ÿһ��item��Ĭ�ϸ߶ȣ�ֻ����״����ʱ�Զ��壩
	ITEM_DEFAULT_WIDTH = 134,		//���ڵ�Ĭ�Ͽ���

	ITEM_DEFAULT_ICON_WIDTH = 28,	////Ĭ��ͼ����ռ����
	ITEM_DEFAULT_ICON_SIZE = 16,	//Ĭ��ͼ��Ĵ�С
	ITEM_DEFAULT_EXPLAND_ICON_WIDTH = 30,	//Ĭ���¼��˵���չͼ����ռ����
	ITEM_DEFAULT_EXPLAND_ICON_SIZE = 9,		//Ĭ���¼��˵���չͼ��Ĵ�С

	DEFAULT_LINE_LEFT_WIDTH = 5,	////Ĭ�Ϸָ��ߵ���߾�
	DEFAULT_LINE_HEIGHT = 10,		//Ĭ�Ϸָ�����ռ�߶�
	DEFAULT_LINE_COLOR = 0xFFB5B5B5	//Ĭ�Ϸָ�����ɫ

};

#define WM_MENU WM_USER +121  //�������հ�ť��������Ϣ

///////////////////////////////////////////////
class ReceiverImplBase;
class ObserverImplBase
{
public:
	virtual void AddReceiver(ReceiverImplBase* receiver) = 0;
	virtual void RemoveReceiver(ReceiverImplBase* receiver) = 0;
	virtual BOOL RBroadcast(ContextMenuParam param) = 0;
};
/////////////////////////////////////////////////
class ReceiverImplBase
{
public:
	virtual void AddObserver(ObserverImplBase* observer) = 0;
	virtual void RemoveObserver() = 0;
	virtual BOOL Receive(ContextMenuParam param) = 0;
};
/////////////////////////////////////////////////

class ReceiverImpl;
class ObserverImpl : public ObserverImplBase
{
	friend class Iterator;
public:
	ObserverImpl():
	m_pMainWndPaintManager(NULL),
	m_pMenuCheckInfo(NULL)
	{
	}

	virtual void AddReceiver(ReceiverImplBase* receiver)
	{
		if (receiver == NULL)
			return;

		receivers_.push_back(receiver);
		receiver->AddObserver(this);
	}

	virtual void RemoveReceiver(ReceiverImplBase* receiver)
	{
		if (receiver == NULL)
			return;

		ReceiversVector::iterator it = receivers_.begin();
		for (; it != receivers_.end(); ++it)
		{
			if (*it == receiver)
			{
				receivers_.erase(it);
				break;
			}
		}
	}

	virtual BOOL RBroadcast(ContextMenuParam param)
	{
		ReceiversVector::reverse_iterator it = receivers_.rbegin();
		for (; it != receivers_.rend(); ++it)
		{
			(*it)->Receive(param);
		}

		return BOOL();
	}


	class Iterator
	{
		ObserverImpl & _tbl;
		DWORD index;
		ReceiverImplBase* ptr;
	public:
		Iterator( ObserverImpl & table )
			: _tbl( table ), index(0), ptr(NULL)
		{}

		Iterator( const Iterator & v )
			: _tbl( v._tbl ), index(v.index), ptr(v.ptr)
		{}

		ReceiverImplBase* next()
		{
			if ( index >= _tbl.receivers_.size() )
				return NULL;

			for ( ; index < _tbl.receivers_.size(); )
			{
				ptr = _tbl.receivers_[ index++ ];
				if ( ptr )
					return ptr;
			}
			return NULL;
		}
	};
public:
	CPaintManagerUI* m_pMainWndPaintManager;
	map<CDuiString,bool>* m_pMenuCheckInfo;
protected:
	typedef std::vector<ReceiverImplBase*> ReceiversVector;
	ReceiversVector receivers_;
};

////////////////////////////////////////////////////
class ReceiverImpl : public ReceiverImplBase
{
public:
	virtual void AddObserver(ObserverImplBase* observer)
	{
		observers_.push_back(observer);
	}

	virtual void RemoveObserver()
	{
		ObserversVector::iterator it = observers_.begin();
		for (; it != observers_.end(); ++it)
		{
			(*it)->RemoveReceiver(this);
		}
	}

	virtual BOOL Receive(ContextMenuParam param)
	{
		return BOOL();
	}

protected:
	typedef std::vector<ObserverImplBase*> ObserversVector;
	ObserversVector observers_;
};


/////////////////////////////////////////////////////////////////////////////////////
//

extern ObserverImpl s_context_menu_observer;

class CListUI;
class UILIB_API CMenuUI : public CListUI
{
public:
	CMenuUI();
	virtual ~CMenuUI();

    LPCTSTR GetClass() const;
    LPVOID GetInterface(LPCTSTR pstrName);

	virtual void DoEvent(TEventUI& event);

    virtual bool Add(CControlUI* pControl);
    virtual bool AddAt(CControlUI* pControl, int iIndex);

    virtual int GetItemIndex(CControlUI* pControl) const;
    virtual bool SetItemIndex(CControlUI* pControl, int iIndex);
    virtual bool Remove(CControlUI* pControl);

	SIZE EstimateSize(SIZE szAvailable);

	void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
};

/////////////////////////////////////////////////////////////////////////////////////
//

class CMenuElementUI;
class UILIB_API CMenuWnd : public CWindowWnd, public ReceiverImpl, public INotifyUI, public IDialogBuilderCallback
{
public:
	CMenuWnd(HWND hParent = NULL);
	~CMenuWnd();

    void Init(CMenuElementUI* pOwner, STRINGorID xml, POINT point,
		CPaintManagerUI* pMainPaintManager = NULL, map<CDuiString,bool>* pMenuCheckInfo = NULL, bool bShowOnTop = false);
    LPCTSTR GetWindowClassName() const;
    void OnFinalMessage(HWND hWnd);
	void Notify(TNotifyUI& msg);
	CControlUI* CreateControl(LPCTSTR pstrClassName);

	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

	BOOL Receive(ContextMenuParam param);

public:
	HWND m_hParent;
	POINT m_BasedPoint;
	bool  m_bShowOnTop;
	STRINGorID m_xml;
    CPaintManagerUI m_pm;
    CMenuElementUI* m_pOwner;
    CMenuUI* m_pLayout;

	bool	m_bFirstEnterSub;
};

class CListContainerElementUI;
class UILIB_API CMenuElementUI : public CListContainerElementUI
{
	friend CMenuWnd;
public:
    CMenuElementUI();
	~CMenuElementUI();

    LPCTSTR GetClass() const;
    LPVOID GetInterface(LPCTSTR pstrName);
    void DoPaint(HDC hDC, const RECT& rcPaint);
	void DrawItemText(HDC hDC, const RECT& rcItem);
	SIZE EstimateSize(SIZE szAvailable);

	bool Activate();
	void DoEvent(TEventUI& event);

	CMenuWnd* GetMenuWnd();
	void CreateMenuWnd();
	
	void SetLineType();
	void SetLineColor(DWORD color);

	void SetIcon(LPCTSTR strIcon);
	void SetIconSize(LONG cx, LONG cy);
	void DrawItemIcon(HDC hDC, const RECT& rcItem);
	void SetChecked(bool bCheck = true);
	bool GetChecked() const;
	void SetCheckItem(bool bCheckItem = false);
	bool GetCheckItem() const;

	void SetShowExplandIcon(bool bShow);
	void DrawItemExpland(HDC hDC, const RECT& rcItem);

	void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

protected:
	CMenuWnd* m_pWindow;

	bool		m_bDrawLine;	//���ָ���
	DWORD		m_dwLineColor;  //�ָ�����ɫ

	SIZE		m_szIconSize; 	//��ͼ��
	CDuiString	m_strIcon;
	bool		m_bCheckItem;

	bool		m_bShowExplandIcon;
};

} // namespace DuiLib

#endif // __UIMENU_H__