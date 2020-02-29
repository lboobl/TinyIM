/**
 * @file ChatTabMgr.h
 * @author DennisMi (https://www.dennisthink.com/)
 * @brief 聊天TAB的管理
 * @version 0.1
 * @date 2019-08-07
 * 
 * @copyright Copyright (c) 2019
 * 
 */


#ifndef _TINY_IM_CHAT_TAB_MGR_H_
#define _TINY_IM_CHAT_TAB_MGR_H_
#include <vector>

struct TAB_WINDOW_INFO
{
	BOOL	bEnableClose;
	HWND	hwndItem;
	BOOL	bHover;
	RECT	rtArea;			//非关闭区域
	RECT	rtClose;		//关闭按钮区域
	TCHAR	szTitle[64];
};

enum CLICK_TYPE
{
	CLICK_TYPE_ACTIVATE,
	CLICK_TYPE_CLOSE
};

struct TWI_HEADER
{
	NMHDR		nmhdr;
	CLICK_TYPE	nClickType;
	HWND		hwndItem;
};

class CChatTabMgr : public CWindowImpl<CChatTabMgr, CWindow>
{
public:
	CChatTabMgr();
	~CChatTabMgr();

	DECLARE_WND_CLASS(_T("__ChatTabMgr__"))

	BEGIN_MSG_MAP_EX(CChatTabMgr)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_PAINT(OnPaint)
		MSG_WM_ERASEBKGND(OnEraseBkgnd)
		MSG_WM_LBUTTONDOWN(OnLButtonDown)
		MSG_WM_LBUTTONUP(OnLButtonUp)
		MSG_WM_MOUSEMOVE(OnMouseMove)
	END_MSG_MAP()

public:
	void AddItem(PCTSTR pszTitle, HWND hwndItem, BOOL bEnableClose=TRUE);
	void RemoveItem(HWND hwndItem);

	long GetItemCount() const;
	void Active(HWND hwndItem);

protected:
	int OnCreate(LPCREATESTRUCT lpCreateStruct);
	void OnPaint(CDCHandle dc);
	BOOL OnEraseBkgnd(CDCHandle dc);
	void OnLButtonDown(UINT nFlags, CPoint point);
	void OnLButtonUp(UINT nFlags, CPoint point);
	void OnMouseMove(UINT nFlags, CPoint point);

	void DrawTabItem(HDC hDC, size_t nIndex);

private:
	std::vector<TAB_WINDOW_INFO>	m_arrWindowInfo;
	long							m_nSelectionIndex;
	CRect							m_rtClient;
	long							m_nLeft;
};
#endif