/**
 * @file ChatTabMgr.cpp
 * @author DennisMi (https://www.dennisthink.com/)
 * @brief 
 * @version 0.1
 * @date 2020-02-29
 * 
 * @copyright Copyright (c) 2020
 * 
 */
//TODO: 需要区分和层叠窗口管理器的区别

#include "stdafx.h"
#include "ChatTabMgr.h"
#include "GDIFactory.h"

/**
 * @brief Construct a new CChatTabMgr::CChatTabMgr object
 * 
 */
CChatTabMgr::CChatTabMgr()
{
	m_nSelectionIndex = -1;
	m_rtClient.SetRectEmpty();
	m_nLeft = 0;
}

/**
 * @brief Destroy the CChatTabMgr::CChatTabMgr object
 * 
 */
CChatTabMgr::~CChatTabMgr()
{
	
}

/**
 * @brief 增加一个Tab项
 * 
 * @param pszTitle 
 * @param hwndItem 
 * @param bEnableClose 
 */
void CChatTabMgr::AddItem(PCTSTR pszTitle, HWND hwndItem, BOOL bEnableClose/*=TRUE*/)
{
	size_t nSize = m_arrWindowInfo.size();
	for(size_t i=0; i<nSize; ++i)
	{
		if(m_arrWindowInfo[i].hwndItem == hwndItem)
			return;
	}
	
	TAB_WINDOW_INFO tbi = {0};
	tbi.bEnableClose = bEnableClose;
	tbi.hwndItem = hwndItem;
	_tcscpy_s(tbi.szTitle, ARRAYSIZE(tbi.szTitle), pszTitle);
	m_arrWindowInfo.push_back(tbi);
	
	ShowWindow(SW_SHOW);
	Active(hwndItem);

	Invalidate(FALSE);
}

/**
 * @brief 删除具体的Tab项
 * 
 * @param hwndItem 
 */
void CChatTabMgr::RemoveItem(HWND hwndItem)
{
	std::vector<TAB_WINDOW_INFO>::iterator iter = m_arrWindowInfo.begin();
	for(; iter!=m_arrWindowInfo.end(); ++iter)
	{
		if(iter->hwndItem == hwndItem)
		{
			m_arrWindowInfo.erase(iter);
			Invalidate(FALSE);
			::ShowWindow(hwndItem, SW_HIDE);
			break;
		}
	}

	if(m_arrWindowInfo.empty())
		ShowWindow(SW_HIDE);
	else
		Active(m_arrWindowInfo[0].hwndItem);
}

/**
 * @brief 获取子项总数
 * 
 * @return long 
 */
long CChatTabMgr::GetItemCount() const
{
	return (long)m_arrWindowInfo.size();
}

/**
 * @brief 响应界面激活消息
 * 
 * @param hwndItem 
 */
void CChatTabMgr::Active(HWND hwndItem)
{
	size_t nSize = m_arrWindowInfo.size();
	for(size_t i=0; i<nSize; ++i)
	{
		if(m_arrWindowInfo[i].hwndItem == hwndItem)
		{
			::ShowWindow(hwndItem, SW_SHOW);
			m_nSelectionIndex = i;
			::InvalidateRect(hwndItem, NULL, FALSE);
		}
		else
			::ShowWindow(m_arrWindowInfo[i].hwndItem, SW_HIDE);
	}

	Invalidate(FALSE);
}

/**
 * @brief 响应窗口创建消息
 * 
 * @param lpCreateStruct 
 * @return int 
 */
int CChatTabMgr::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	//TODO: 测试数据
	//TAB_WINDOW_INFO twi = {0};
	//_tcscpy_s(twi.szTitle, ARRAYSIZE(twi.szTitle), _T("消息记录"));
	//m_arrWindowInfo.push_back(twi);

	//_tcscpy_s(twi.szTitle, ARRAYSIZE(twi.szTitle), _T("传送文件(0/1)"));
	//m_arrWindowInfo.push_back(twi);

	//_tcscpy_s(twi.szTitle, ARRAYSIZE(twi.szTitle), _T("单身的我以为可以一辈子不靠谁"));
	//m_arrWindowInfo.push_back(twi);

	CRect rcClient;
	GetClientRect(&m_rtClient);

	return 1;
}

/**
 * @brief 聊天的Tab页面绘制
 * 
 * @param dc 
 */
void CChatTabMgr::OnPaint(CDCHandle dc)
{
	CPaintDC PaintDC(m_hWnd);
	CMemoryDC MemDC(PaintDC.m_hDC, m_rtClient);
	
	//画白色背景
	HBRUSH hBrush = ::CreateSolidBrush(RGB(245, 251, 255));
	HBRUSH hOldBrush = (HBRUSH)::SelectObject(MemDC.m_hDC, hBrush);
	::PatBlt(MemDC.m_hDC, m_rtClient.left, m_rtClient.top, m_rtClient.Width(), m_rtClient.Height(), PATCOPY);
	::SelectObject(MemDC.m_hDC, hOldBrush);
	::DeleteObject(hBrush);

	size_t nSize = m_arrWindowInfo.size();
	for(size_t i=0; i<nSize; ++i)
	{
		DrawTabItem(MemDC.m_hDC, i);
	}

	m_nLeft = 0;
}


/**
 * @brief 绘制Tab的每一项
 * 
 * @param hDC 
 * @param nIndex 
 */
void CChatTabMgr::DrawTabItem(HDC hDC, size_t nIndex)
{
	HFONT hFont = CGDIFactory::GetFont(18);
	HPEN hPenBorder =  ::CreatePen(PS_SOLID, 1, RGB(175, 175, 175));	
	//HPEN hPenBorder =  ::CreatePen(PS_SOLID, 1, RGB(255, 0, 0));	

	HFONT hOldFont = (HFONT)::SelectObject(hDC, hFont);
	
	if(m_nSelectionIndex == nIndex)
		::SetTextColor(hDC, RGB(0, 0, 0));
	else
		::SetTextColor(hDC, RGB(128, 128, 128));
	CRect rtRect(0, 0, 0, 0);
	long nLength =  _tcslen(m_arrWindowInfo[nIndex].szTitle);
	//先计算文字的长度
	::DrawText(hDC, m_arrWindowInfo[nIndex].szTitle, nLength, &rtRect, DT_SINGLELINE|DT_CALCRECT);
	rtRect.bottom = m_rtClient.Height();
	rtRect.left += m_nLeft;
	rtRect.right += m_nLeft;
	
	//画边框
	CRect rtBorder;
	if(m_arrWindowInfo[nIndex].bEnableClose)
		::SetRect(&rtBorder, rtRect.left, rtRect.top, rtRect.right+22, rtRect.bottom);
	else
		::SetRect(&rtBorder, rtRect.left, rtRect.top, rtRect.right+14, rtRect.bottom);
	HPEN hOldPen = (HPEN)::SelectObject(hDC, hPenBorder);
	::Rectangle(hDC, rtBorder.left, rtBorder.top, rtBorder.right, rtBorder.bottom);

	if(nIndex == 0)
	{
		::MoveToEx(hDC, rtBorder.left, rtBorder.bottom-1, NULL);
		::LineTo(hDC, rtBorder.right+300, rtBorder.bottom-1);
	}
	
	//画标签文字
	CRect rtText(rtRect.left+5, rtRect.top, rtRect.right+5, rtRect.bottom);
	::DrawText(hDC, m_arrWindowInfo[nIndex].szTitle, nLength, &rtText, DT_LEFT|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);
	
	//画关闭按钮
	HPEN hPenClose = NULL;
	if(m_arrWindowInfo[nIndex].bHover)
		hPenClose = ::CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
	else
		hPenClose = ::CreatePen(PS_SOLID, 1, RGB(175, 175, 175));

	::SelectObject(hDC, hPenClose);
	::DeleteObject(hPenBorder);
	CRect rtClose(0, 0, 0, 0);
	if(m_arrWindowInfo[nIndex].bEnableClose)
	{
		::SetRect(&rtClose, rtText.right+2, rtText.top+12, rtText.right+10, rtText.top+20);
		::MoveToEx(hDC, rtClose.left, rtClose.top, NULL);
		::LineTo(hDC, rtClose.right, rtClose.bottom);
		::MoveToEx(hDC, rtClose.right, rtClose.top, NULL);
		::LineTo(hDC, rtClose.left, rtClose.bottom);
	}
			
	
	::SelectObject(hDC, hOldFont);
	::DeleteObject(hPenClose);

	::SetRect(&m_arrWindowInfo[nIndex].rtArea, rtBorder.left, rtBorder.top, rtBorder.right-22, rtBorder.bottom);
	::CopyRect(&m_arrWindowInfo[nIndex].rtClose, &rtClose);



	m_nLeft = rtBorder.right-1;
}


/**
 * @brief 响应背景擦除消息
 * 
 * @param dc 
 * @return BOOL 
 */
BOOL CChatTabMgr::OnEraseBkgnd(CDCHandle dc)
{
	return TRUE;
}

/**
 * @brief 响应鼠标左键按下
 * 
 * @param nFlags 
 * @param point 
 */
void CChatTabMgr::OnLButtonDown(UINT nFlags, CPoint point)
{
	TWI_HEADER stNmdhdrEx = {0};
	stNmdhdrEx.nmhdr.hwndFrom = m_hWnd;
	stNmdhdrEx.nmhdr.idFrom = GetDlgCtrlID();
	stNmdhdrEx.nmhdr.code = NM_CLICK;

	size_t nSize = m_arrWindowInfo.size();
	for(size_t i=0; i<nSize; ++i)
	{
		if(::PtInRect(&m_arrWindowInfo[i].rtArea, point))
		{
			m_nSelectionIndex = i;
			Invalidate(FALSE);
			stNmdhdrEx.nClickType = CLICK_TYPE_ACTIVATE;
			stNmdhdrEx.hwndItem = m_arrWindowInfo[i].hwndItem;
			Active(m_arrWindowInfo[i].hwndItem);
			::SendMessage(::GetParent(m_hWnd), WM_NOTIFY, GetDlgCtrlID(), (LPARAM)&stNmdhdrEx);
			break;
		}
		else if(::PtInRect(&m_arrWindowInfo[i].rtClose, point) && m_arrWindowInfo[i].bEnableClose)
		{
			Invalidate(FALSE);
			stNmdhdrEx.nClickType = CLICK_TYPE_CLOSE;
			stNmdhdrEx.hwndItem = m_arrWindowInfo[i].hwndItem;
			RemoveItem(m_arrWindowInfo[i].hwndItem);
			::SendMessage(::GetParent(m_hWnd), WM_NOTIFY, GetDlgCtrlID(), (LPARAM)&stNmdhdrEx);
			break;
		}
	}
}

/**
 * @brief 响应鼠标左键抬起
 * 
 * @param nFlags 
 * @param point 
 */
void CChatTabMgr::OnLButtonUp(UINT nFlags, CPoint point)
{

}

/**
 * @brief 响应鼠标移动消息
 * 
 * @param nFlags 
 * @param point 
 */
void CChatTabMgr::OnMouseMove(UINT nFlags, CPoint point)
{
	size_t nSize = m_arrWindowInfo.size();
	for(size_t i=0; i<nSize; ++i)
	{
		if(::PtInRect(&m_arrWindowInfo[i].rtClose, point))
			m_arrWindowInfo[i].bHover = TRUE;
		else
			m_arrWindowInfo[i].bHover = FALSE;
	}

	Invalidate(FALSE);
}