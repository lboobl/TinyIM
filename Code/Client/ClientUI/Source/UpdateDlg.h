
/**
 * @file UpdateDlg.h
 * @author DennisMi (https://www.dennisthink.com/)
 * @brief 自动更新类对话框头文件
 * @version 0.1
 * @date 2019-08-26
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#ifndef _TINY_IM_UPDATE_DLG_H_
#define _TINY_IM_UPDATE_DLG_H_
#include "resource.h"
#include "SkinLib/SkinLib.h"
#include <vector>



//自动解压下载下来的升级包文件，成功返回true，失败返回false
bool Unzip();

class CUpdateDlg : public CDialogImpl<CUpdateDlg>
{
public:
	CUpdateDlg();
	virtual ~CUpdateDlg();

	enum {IDD = IDD_UPDATE_DLG};

	BEGIN_MSG_MAP_EX(CUpdateDlg)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_CLOSE(OnClose)
		MSG_WM_DESTROY(OnDestroy)
	END_MSG_MAP()

private:
	BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam);
	void OnClose();
	void OnDestroy();
	BOOL InitUI();
	void UninitUI();
	
	void ParseFileInfo();	//解析升级包文件信息

public:

	std::vector<CString>	 m_aryFileInfo;    //需要下载的文件列表

	std::vector<CString>	 m_aryFileDesc;		//升级包中文描述
	std::vector<CString>	 m_aryFileName;		//升级包名
	std::vector<CString>	 m_aryFileVersion;	//升级包版本号
	

public:
	CSkinDialog			m_SkinDlg;
	CSkinStatic			m_UpdateFileName; //显示更新文件名
	CSkinStatic         m_UpdateRate; //显示更新速率
	CProgressBarCtrl	m_UpdateProgressBar; //文件更新的进度条
	CSkinListCtrl		m_UpdateListCtrl; //文件更新空间

	HANDLE				m_hDownloadThread;              //下载线程句柄
	HANDLE				m_hExitEvent;					//对话框退出事件

	static	DWORD WINAPI DownloadThread(LPVOID lpParameter);
};

#endif