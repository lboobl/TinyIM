/**
 * @file ChangePicHeadDlg.h
 * @author DennisMi (https://www.dennisthink.com/)
 * @brief 修改头像对话框
 * @version 0.1
 * @date 2019-08-07
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#ifndef _TINY_IM_CHANGE_PIC_HEAD_DLG_H_ 
#define _TINY_IM_CHANGE_PIC_HEAD_DLG_H_
#include "resource.h"
#include "SkinLib/SkinLib.h"

#define THUMB_WINDOW_BASE_ID 0x4000

class CChangePicHeadDlg : public CDialogImpl<CChangePicHeadDlg>
{
public:
	CChangePicHeadDlg();
	virtual ~CChangePicHeadDlg();

	enum { IDD = IDD_CHANGE_PIC_DLG };

	BEGIN_MSG_MAP_EX(CChangePicHeadDlg)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_CLOSE(OnClose)
		MSG_WM_DESTROY(OnDestroy)

		COMMAND_HANDLER_EX(IDOK, BN_CLICKED, OnOK)
		COMMAND_HANDLER_EX(IDCANCEL, BN_CLICKED, OnCancel)
		COMMAND_RANGE_HANDLER_EX(THUMB_WINDOW_BASE_ID, THUMB_WINDOW_BASE_ID+35, OnSelectThumb);
	END_MSG_MAP()

	long GetSelection();
	void SetSelection(long nIndex);

private:
	BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam);
	void OnClose();
	void OnDestroy();

	BOOL Init();
	BOOL InitThumbImages();
	void DestroyThumbImages();

	void OnOK(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnCancel(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnSelectThumb(UINT uNotifyCode, int nID, CWindow wndCtl);

private:
	CSkinDialog							m_SkinDlg;	//主对话框
	CSkinButton							m_btnOK;//OK按钮
	CSkinButton							m_btnCancel;

	CSkinPictureBox						m_ThumbBox;

	std::vector<CSkinPictureBox*>		m_aryThumbWindows;	//供选择的头像列表

	long								m_SelectedIndex;	//选中的头像索引
	long								m_LastSelectedIndex;//上一次选中的头像索引
};

#endif