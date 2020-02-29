/**
 * @file FaceSelDlg.cpp
 * @author DennisMi (https://www.dennisthink.com/)
 * @brief 头像选择对话框的实现文件
 * @version 0.1
 * @date 2020-02-29
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#include "stdafx.h"
#include "FaceSelDlg.h"

/**
 * @brief Construct a new CFaceSelDlg::CFaceSelDlg object
 * 
 */
CFaceSelDlg::CFaceSelDlg(void)
{
	m_lpFaceList = NULL;
	m_nSelFaceId = -1;
	m_nSelFaceIndex = -1;
	m_strSelFaceFileName = _T("");
}

/**
 * @brief Destroy the CFaceSelDlg::CFaceSelDlg object
 * 
 */
CFaceSelDlg::~CFaceSelDlg(void)
{
}

/**
 * @brief 设置头像列表
 * 
 * @param lpFaceList 
 */
void CFaceSelDlg::SetFaceList(CFaceList* lpFaceList)
{
	m_lpFaceList = lpFaceList;
}

/**
 * @brief 获取选中的头像id
 * 
 * @return int 
 */
int CFaceSelDlg::GetSelFaceId()
{
	return m_nSelFaceId;
}

/**
 * @brief 获取选中的头像索引
 * 
 * @return int 
 */
int CFaceSelDlg::GetSelFaceIndex()
{
	return m_nSelFaceIndex;
}

/**
 * @brief 获取选中头像的文件名
 * 
 * @return CString 
 */
CString CFaceSelDlg::GetSelFaceFileName()
{
	return m_strSelFaceFileName;
}

/**
 * @brief 响应初始化对话框
 * 
 * @param wndFocus 
 * @param lInitParam 
 * @return BOOL 
 */
BOOL CFaceSelDlg::OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
{
	CRect rcClient;
	GetClientRect(&rcClient);

	m_FaceCtrl.SetBgColor(RGB(255, 255, 255));
	m_FaceCtrl.SetLineColor(RGB(223, 230, 246));
	m_FaceCtrl.SetFocusBorderColor(RGB(0, 0, 255));
	m_FaceCtrl.SetZoomBorderColor(RGB(0, 138, 255));
	m_FaceCtrl.SetRowAndCol(10, 15);
	m_FaceCtrl.SetItemSize(28, 28);
	m_FaceCtrl.SetZoomSize(84, 84);
	m_FaceCtrl.SetFaceList(m_lpFaceList);
	m_FaceCtrl.SetCurPage(0);

	CRect rcFaceCtrl(rcClient);
	rcFaceCtrl.left += 6;
	rcFaceCtrl.right = rcFaceCtrl.left + 420;
	rcFaceCtrl.top += 6;
	rcFaceCtrl.bottom = rcFaceCtrl.top + 280;
	m_FaceCtrl.Create(m_hWnd, rcFaceCtrl, NULL, WS_CHILD | WS_VISIBLE, 
		NULL, ID_FACE_CTRL, NULL);

	m_nSelFaceId = -1;
	m_nSelFaceIndex = -1;
	m_strSelFaceFileName = _T("");

	ModifyStyleEx(WS_EX_APPWINDOW, WS_EX_TOOLWINDOW);

	return TRUE;
}

/**
 * @brief 响应对话框激活
 * 
 * @param nState 
 * @param bMinimized 
 * @param wndOther 
 */
void CFaceSelDlg::OnActivate(UINT nState, BOOL bMinimized, CWindow wndOther)
{
	SetMsgHandled(FALSE);

	if (WA_INACTIVE == nState)
	{
		PostMessage(WM_CLOSE, NULL, NULL);
	}
}

/**
 * @brief 响应对话框绘制
 * 
 * @param dc 
 */
void CFaceSelDlg::OnPaint(CDCHandle dc)
{
	CPaintDC PaintDC(m_hWnd);

	HDC hDC = PaintDC.m_hDC;

	CRect rcClient;
	GetClientRect(&rcClient);

	HPEN hPen = ::CreatePen(PS_SOLID, 1, RGB(72,121,184));
	HBRUSH hBrush = ::CreateSolidBrush(RGB(233,246,254));
	HPEN hOldPen = (HPEN)::SelectObject(hDC, hPen);
	HBRUSH hOldBrush = (HBRUSH)::SelectObject(hDC, hBrush);
	::Rectangle(hDC, rcClient.left, rcClient.top, rcClient.right, rcClient.bottom);
	::SelectObject(hDC, hOldBrush);
	::SelectObject(hDC, hOldPen);
	::DeleteObject(hPen);
	::DeleteObject(hBrush);
}

/**
 * @brief 响应对话框关闭
 * 
 */
void CFaceSelDlg::OnClose()
{
	::PostMessage(GetParent(), FACE_CTRL_SEL, NULL, NULL);
	DestroyWindow();
}

/**
 * @brief 销毁对话框
 * 
 */
void CFaceSelDlg::OnDestroy()
{
	SetMsgHandled(FALSE);

	m_FaceCtrl.DestroyWindow();
}

/**
 * @brief 响应头像选择消息
 * 
 * @param uMsg 
 * @param wParam 
 * @param lParam 
 * @return LRESULT 
 */
LRESULT CFaceSelDlg::OnFaceCtrlSel(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int nSelIndex = (int)lParam;
	CFaceInfo* lpFaceInfo = m_FaceCtrl.GetFaceInfo(nSelIndex);
	if (lpFaceInfo != NULL)
	{
		m_nSelFaceId = lpFaceInfo->m_nId;
		m_nSelFaceIndex = lpFaceInfo->m_nIndex;
		m_strSelFaceFileName = lpFaceInfo->m_strFileName.c_str();
	}
	PostMessage(WM_CLOSE, NULL, NULL);
	return 0;
}