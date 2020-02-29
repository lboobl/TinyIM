/**
 * @file ChangePicHead.h
 * @author DennisMi (https://www.dennisthink.com/)
 * @brief 更换头像功能
 * @version 0.1
 * @date 2019-08-07
 * 
 * @copyright Copyright (c) 2019
 * 
 */


#ifndef _TINY_IM_C_CHANGE_PIC_HEAD_H_ 
#define _TINY_IM_C_CHANGE_PIC_HEAD_H_
#define C_PAGES 3 
#include "resource.h"
#include "SkinLib/SkinLib.h"

class CChangePicHead : public CDialogImpl<CChangePicHead>
{
public:
	CChangePicHead();
	virtual ~CChangePicHead();

	enum { IDD = IDD_CHANGE_PIC_DLG };

	BEGIN_MSG_MAP_EX(CChangePicHead)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_ACTIVATE(OnActivate)
		MSG_WM_PAINT(OnPaint)
		MSG_WM_CLOSE(OnClose)
		MSG_WM_DESTROY(OnDestroy)
	END_MSG_MAP()

public:
	BOOL Init();
	BOOL InitTabCtrl();			// 初始化Tab栏
private:
	BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam);
	void OnActivate(UINT nState, BOOL bMinimized, CWindow wndOther);
	void OnPaint(CDCHandle dc);
	void OnClose();
	void OnDestroy();

private:
	CSkinDialog			m_SkinDlg;//主界面对话框	
	CSkinTabCtrl		m_TabCtrl;//Tab控件
	CSkinButton			m_btnChange;//修改按钮
	CSkinButton         m_btnUnChange;//不修改按钮
};
#endif