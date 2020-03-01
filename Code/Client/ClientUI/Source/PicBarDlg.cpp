/**
 * @file PicBarDlg.cpp
 * @author DennisMi (https://www.dennisthink.com/)
 * @brief 表情工具对话框的实现类
 * @version 0.1
 * @date 2020-03-01
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#include "stdafx.h"
#include "PicBarDlg.h"
#include "Path.h"

/**
 * @brief Construct a new CPicBarDlg::CPicBarDlg object
 * 
 */
CPicBarDlg::CPicBarDlg(void)
{
}

/**
 * @brief Destroy the CPicBarDlg::CPicBarDlg object
 * 
 */
CPicBarDlg::~CPicBarDlg(void)
{
}

/**
 * @brief 响应初始化对话框
 * 
 * @param wndFocus 
 * @param lInitParam 
 * @return BOOL 
 */
BOOL CPicBarDlg::OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
{
	CString strCurDir = Hootina::CPath::GetAppPath().c_str();

	int nIndex = m_ToolBar.AddItem(301, STBI_STYLE_BUTTON);
	m_ToolBar.SetItemSize(nIndex, 22, 20);
	m_ToolBar.SetItemPadding(nIndex, 2);
	m_ToolBar.SetItemToolTipText(nIndex, _T("点击添加表情"));
	m_ToolBar.SetItemBgPic(nIndex, NULL, strCurDir+_T("Image\\FaceEditTipBtn_selected.png"), 
		strCurDir+_T("Image\\FaceEditTipBtn_selected.png"), CRect(0,0,0,0));
	m_ToolBar.SetItemIconPic(nIndex, strCurDir+_T("Image\\face_add.png"));

	nIndex = m_ToolBar.AddItem(302, STBI_STYLE_BUTTON);
	m_ToolBar.SetItemSize(nIndex, 22, 20);
	m_ToolBar.SetItemPadding(nIndex, 2);
	m_ToolBar.SetItemToolTipText(nIndex, _T("点击另存为..."));
	m_ToolBar.SetItemBgPic(nIndex, NULL, strCurDir+_T("Image\\FaceEditTipBtn_selected.png"), 
		strCurDir+_T("Image\\FaceEditTipBtn_selected.png"), CRect(0,0,0,0));
	m_ToolBar.SetItemIconPic(nIndex, strCurDir+_T("Image\\save_16.png"));

	nIndex = m_ToolBar.AddItem(303, STBI_STYLE_BUTTON);
	m_ToolBar.SetItemSize(nIndex, 22, 20);
	m_ToolBar.SetItemPadding(nIndex, 2);
	m_ToolBar.SetItemToolTipText(nIndex, _T("点击涂鸦编辑表情"));
	m_ToolBar.SetItemBgPic(nIndex, NULL, strCurDir+_T("Image\\FaceEditTipBtn_selected.png"), 
		strCurDir+_T("Image\\FaceEditTipBtn_selected.png"), CRect(0,0,0,0));
	m_ToolBar.SetItemIconPic(nIndex, strCurDir+_T("Image\\emotionEdit_20.png"));

	nIndex = m_ToolBar.AddItem(304, STBI_STYLE_BUTTON);
	m_ToolBar.SetItemSize(nIndex, 22, 20);
	m_ToolBar.SetItemPadding(nIndex, 2);
	m_ToolBar.SetItemToolTipText(nIndex, _T("搜索相关表情"));
	m_ToolBar.SetItemBgPic(nIndex, NULL, strCurDir+_T("Image\\FaceEditTipBtn_selected.png"), 
		strCurDir+_T("Image\\FaceEditTipBtn_selected.png"), CRect(0,0,0,0));
	m_ToolBar.SetItemIconPic(nIndex, strCurDir+_T("Image\\search.png"));

	nIndex = m_ToolBar.AddItem(305, STBI_STYLE_BUTTON);
	m_ToolBar.SetItemSize(nIndex, 22, 20);
	m_ToolBar.SetItemPadding(nIndex, 2);
	m_ToolBar.SetItemToolTipText(nIndex, _T("分享到空间或者微博"));
	m_ToolBar.SetItemBgPic(nIndex, NULL, strCurDir+_T("Image\\FaceEditTipBtn_selected.png"), 
		strCurDir+_T("Image\\FaceEditTipBtn_selected.png"), CRect(0,0,0,0));
	m_ToolBar.SetItemIconPic(nIndex, strCurDir+_T("Image\\qzonewb.png"));

	m_ToolBar.SetLeftTop(2, 2);
	m_ToolBar.SetBgPic(strCurDir+_T("Image\\toolbar_back.png"), CRect(2,2,2,2));

	CRect rcToolBar(0, 0, 122, 24);
	m_ToolBar.Create(m_hWnd, rcToolBar, NULL, WS_CHILD|WS_VISIBLE, NULL, 1001);

	return TRUE;
}

/**
 * @brief 响应对话框销毁消息
 * 
 */
void CPicBarDlg::OnDestroy()
{
	if (m_ToolBar.IsWindow())
		m_ToolBar.DestroyWindow();
}

/**
 * @brief 响应按钮点击消息
 * 
 * @param uNotifyCode 
 * @param nID 
 * @param wndCtl 
 */
void CPicBarDlg::OnBtn_Clicked(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	::PostMessage(::GetParent(m_hWnd), WM_COMMAND, MAKEWPARAM(nID, uNotifyCode), (LPARAM)(HWND)wndCtl);
	::ShowWindow(m_hWnd, SW_HIDE);
}