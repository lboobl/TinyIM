/**
 * @file RegisterDialog.cpp
 * @author DennisMi (https://www.dennisthink.com/)
 * @brief 账号注册对话框的实现
 * @version 0.1
 * @date 2019-08-05
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#include "stdafx.h"
#include "RegisterDialog.h"
#include "Utils.h"
#include "EncodingUtil.h"
#include "UIText.h"
#include "Proto.h"

/**
 * @brief Construct a new CRegisterDialog::CRegisterDialog object
 * 
 */
CRegisterDialog::CRegisterDialog()
{

}

/**
 * @brief Destroy the CRegisterDialog::CRegisterDialog object
 * 
 */
CRegisterDialog::~CRegisterDialog()
{

}

/**
 * @brief 初始化对话框
 * 
 * @param wndFocus 
 * @param lInitParam 
 * @return BOOL 
 */
BOOL CRegisterDialog::OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
{
	CenterWindow();
	InitUI();

	//ModifyStyleEx(WS_EX_APPWINDOW, WS_EX_TOOLWINDOW);
	return TRUE;
}

/**
 * @brief 初始化UI
 * 
 * @return BOOL 
 */
BOOL CRegisterDialog::InitUI()
{
	m_RegDlg.SetBgPic(_T("DlgBg\\GeneralBg.png"));
	m_RegDlg.SetCloseSysBtnPic(_T("SysBtn\\btn_close_normal.png"), _T("SysBtn\\btn_close_highlight.png"), _T("SysBtn\\btn_close_down.png"));
	m_RegDlg.SubclassWindow(m_hWnd);
	m_RegDlg.SetTitleText(_T("新用户注册"));
	m_RegDlg.MoveWindow(0, 0, 550, 350, TRUE);
	
	//窗口居中
	CenterWindow();

	HDC hDlgBgDC = m_RegDlg.GetBgDC();

	m_edtRegId.SetBgNormalPic(_T("frameBorderEffect_normalDraw.png"), CRect(2,2,2,2));
	m_edtRegId.SetBgHotPic(_T("frameBorderEffect_mouseDownDraw.png"), CRect(2,2,2,2));
	m_edtRegId.SetTransparent(TRUE, hDlgBgDC);
	m_edtRegId.SubclassWindow(GetDlgItem(IDC_EDIT_REGID));
	m_edtRegId.MoveWindow(200, 60, 190, 30, TRUE);

	m_edtRegName.SetBgNormalPic(_T("frameBorderEffect_normalDraw.png"), CRect(2,2,2,2));
	m_edtRegName.SetBgHotPic(_T("frameBorderEffect_mouseDownDraw.png"), CRect(2,2,2,2));
	m_edtRegName.SubclassWindow(GetDlgItem(IDC_EDIT_REG_NAME));
	m_edtRegName.MoveWindow(200, 115, 190, 30, TRUE);

	m_edtPwd.SetBgNormalPic(_T("frameBorderEffect_normalDraw.png"), CRect(2,2,2,2));
	m_edtPwd.SetBgHotPic(_T("frameBorderEffect_mouseDownDraw.png"), CRect(2,2,2,2));
	m_edtPwd.SubclassWindow(GetDlgItem(IDC_EDIT_REG_PWD));
	m_edtPwd.MoveWindow(200, 170, 190, 30, TRUE);
	m_edtPwd.SetPasswordChar(0x25CF);

	m_edtCheckPwd.SetBgNormalPic(_T("frameBorderEffect_normalDraw.png"), CRect(2,2,2,2));
	m_edtCheckPwd.SetBgHotPic(_T("frameBorderEffect_mouseDownDraw.png"), CRect(2,2,2,2));
	m_edtCheckPwd.SubclassWindow(GetDlgItem(IDC_EDIT_CHECK_PWD));
	m_edtCheckPwd.MoveWindow(200, 170 + 55, 190, 30, TRUE);
	m_edtCheckPwd.SetPasswordChar(0x25CF);

	m_btnRegist.SetButtonType(SKIN_PUSH_BUTTON);
	m_btnRegist.SetTransparent(TRUE, hDlgBgDC);
	m_btnRegist.SetBgPic(_T("Button\\btn_normal.png"), _T("Button\\btn_highlight.png"),
		_T("Button\\btn_down.png"), _T("Button\\btn_focus.png"));
	m_btnRegist.SetRound(4, 4);
	m_btnRegist.SubclassWindow(GetDlgItem(ID_BTN_REGIST));
	m_btnRegist.MoveWindow(165, 265 + 30, 94, 30, TRUE);

	m_btnCancel.SetButtonType(SKIN_PUSH_BUTTON);
	m_btnCancel.SetTransparent(TRUE, hDlgBgDC);
	m_btnCancel.SetBgPic(_T("Button\\btn_normal.png"), _T("Button\\btn_highlight.png"),
		_T("Button\\btn_down.png"), _T("Button\\btn_focus.png"));
	m_btnCancel.SetRound(4, 4);
	m_btnCancel.SubclassWindow(GetDlgItem(ID_BTN_CANCEL));
	m_btnCancel.MoveWindow(290, 265 + 30, 94, 30, TRUE);

	return TRUE;
}

/**
 * @brief 响应关闭对话框
 * 
 */
void CRegisterDialog::OnClose()
{
	EndDialog(IDCANCEL);
}


/**
 * @brief 响应销毁对话框
 * 
 */
void CRegisterDialog::OnDestroy()
{
	UninitUI();
}

/**
 * @brief 反初始化UI
 * 
 */
void CRegisterDialog::UninitUI()
{
	if (m_edtRegId.IsWindow())
	{
		m_edtRegId.DestroyWindow();
	}

	if (m_edtRegName.IsWindow())
	{
		m_edtRegName.DestroyWindow();
	}

	if (m_edtPwd.IsWindow())
	{
		m_edtPwd.DestroyWindow();
	}

	if (m_btnRegist.IsWindow())
	{
		m_btnRegist.DestroyWindow();
	}

	if (m_btnCancel.IsWindow())
	{
		m_btnCancel.DestroyWindow();
	}
}

/**
 * @brief 处理注册流程
 * 
 * @param pParam 
 * @return UINT 
 */
UINT CRegisterDialog::RegisterThreadProc(void* pParam)
{
    CRegisterDialog* pRegisterDlg = (CRegisterDialog*)pParam;
	if (pRegisterDlg == NULL)
	{
		return 0;
	}

    char szUser[64] = { 0 };
    EncodeUtil::UnicodeToUtf8(pRegisterDlg->m_strMobile, szUser, ARRAYSIZE(szUser));
    char szNickName[64] = { 0 };
    EncodeUtil::UnicodeToUtf8(pRegisterDlg->m_strNickName, szNickName, ARRAYSIZE(szNickName));
    char szPassword[64] = { 0 };
    EncodeUtil::UnicodeToUtf8(pRegisterDlg->m_strPassword, szPassword, ARRAYSIZE(szPassword));

	auto pProto = CMsgProto::GetInstance();
	//超时时间设置为3秒
	UserRegisterRspMsg rspMsg =  pProto->SendRegisterSync(szUser, szPassword, szNickName);

    C_WND_MSG_RegisterRequest* pRegisterResult = new C_WND_MSG_RegisterRequest();
    pRegisterResult->m_nResultCode = E_UI_REGISTER_RESULT_CODE::REGISTER_FAILED;
	
	//Error Code Translate
	{
		if (rspMsg.m_eErrCode == ERROR_CODE_TYPE::E_CODE_SUCCEED)
		{
			pRegisterResult->m_nResultCode = E_UI_REGISTER_RESULT_CODE::REGISTER_SUCCESS;
			strcpy(pRegisterResult->m_szMobile, rspMsg.m_strUserName.c_str());
		}
		else if (rspMsg.m_eErrCode == ERROR_CODE_TYPE::E_CODE_USER_HAS_EXIST) {
			pRegisterResult->m_nResultCode = E_UI_REGISTER_RESULT_CODE::REGISTER_EXIST;
		}
		else if (rspMsg.m_eErrCode == ERROR_CODE_TYPE::E_CODE_CONNECT_FAILED) {
			pRegisterResult->m_nResultCode = E_UI_REGISTER_RESULT_CODE::REGISTER_SERVER_NOT_RESPONSE;
		}
	}

    ::PostMessage(pRegisterDlg->m_hWnd, FMG_MSG_REGISTER, 0, (LPARAM)pRegisterResult);

    return 1;
}

/**
 * @brief 获取用户账号
 * 
 * @return PCTSTR 
 */
PCTSTR CRegisterDialog::GetAccountName()
{
	return m_strAccount;
}

/**
 * @brief 获取密码
 * 
 * @return PCTSTR 
 */
PCTSTR CRegisterDialog::GetPassword()
{
	return m_strPassword;
}


/**
 * @brief 响应“注册”按钮
 * 
 * @param uNotifyCode 
 * @param nID 
 * @param wndCtl 
 */
void CRegisterDialog::OnBtn_Regist(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	m_edtRegId.GetWindowText(m_strMobile);
	m_strMobile.Trim();
    m_strAccount = m_strMobile;
    if (m_strMobile.IsEmpty())
	{
		::MessageBox(m_hWnd, _T("无效的手机号码！"), g_strAppTitle.c_str(), MB_OK|MB_ICONINFORMATION);
		m_edtRegId.SetFocus();
		return;
	}

	m_edtRegName.GetWindowText(m_strNickName);
	m_strNickName.Trim();
	if(m_strNickName.IsEmpty() || m_strNickName.GetLength()>15)
	{
		::MessageBox(m_hWnd, _T("昵称不能为空且长度不能超过15个字符！"), g_strAppTitle.c_str(), MB_OK|MB_ICONINFORMATION);
		m_edtRegName.SetFocus();
		return;
	}

	m_edtPwd.GetWindowText(m_strPassword);
	m_strPassword.Trim();
	if(m_strPassword.IsEmpty() || m_strPassword.GetLength()<3 || m_strPassword.GetLength()>15)
	{
		::MessageBox(m_hWnd, _T("密码长度必须在6～15个字符之间！"), g_strAppTitle.c_str(), MB_OK|MB_ICONINFORMATION);
		return;
	}

	m_edtCheckPwd.GetWindowText(m_strCheckPassword);
	if(m_strCheckPassword.IsEmpty() || m_strCheckPassword.GetLength()<3 || m_strCheckPassword.GetLength()>15)
	{
		::MessageBox(m_hWnd, _T("确认密码长度必须在6～15个字符之间！"), g_strAppTitle.c_str(), MB_OK|MB_ICONINFORMATION);
		return;
	}
	m_strCheckPassword.Trim();

	if (0 != m_strPassword.Compare(m_strCheckPassword))
	{
		::MessageBox(m_hWnd, _T("两次输入的密码不相同，请重新确认！"), g_strAppTitle.c_str(), MB_OK|MB_ICONINFORMATION);
		return;
	}
	

	m_btnRegist.EnableWindow(FALSE);

    HANDLE hRegisterThread = (HANDLE)::_beginthreadex(NULL, 0, RegisterThreadProc, this, 0, NULL);
	if (hRegisterThread != NULL)
	{
		::CloseHandle(hRegisterThread);
	}
		
}

/**
 * @brief 响应取消注册按钮
 * 
 * @param uNotifyCode 
 * @param nID 
 * @param wndCtl 
 */
void CRegisterDialog::OnBtn_Cancel(UINT uNotifyCode, int nID, CWindow wndCtl)	
{
	EndDialog(IDCANCEL);
}

/**
 * @brief 响应注册结果
 * 
 * @param uMsg 
 * @param wParam 
 * @param lParam 
 * @param bHandled 
 * @return LRESULT 
 */
LRESULT CRegisterDialog::OnRegisterResult(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	C_WND_MSG_RegisterRequest* pResult = (C_WND_MSG_RegisterRequest*)lParam;
	if(pResult == NULL)
	{
		return 0;
	}
	
	E_UI_REGISTER_RESULT_CODE  nRegisterResult = pResult->m_nResultCode;
	if (nRegisterResult == E_UI_REGISTER_RESULT_CODE::REGISTER_EXIST)
	{
		::MessageBox(m_hWnd, _T("您注册的手机号已经被注册，请更换其它手机号后重试！"), g_strAppTitle.c_str(), MB_OK | MB_ICONINFORMATION);
	}
	else if(nRegisterResult == E_UI_REGISTER_RESULT_CODE::REGISTER_SUCCESS)
	{
		TCHAR szSuccessInfo[64] = {0};
        _stprintf_s(szSuccessInfo, 64, _T("恭喜您，注册成功，您的账号是[%s]！"), m_strMobile.GetBuffer());
		m_strMobile.ReleaseBuffer();
		::MessageBox(m_hWnd, szSuccessInfo, g_strAppTitle.c_str(), MB_OK|MB_ICONINFORMATION);

		EndDialog(IDOK);
	}
	else
	{
		::MessageBox(m_hWnd, _T("网络故障，注册失败，请稍后重试！"), g_strAppTitle.c_str(), MB_OK | MB_ICONERROR);
	}

	
	m_btnRegist.EnableWindow(TRUE);
	
	delete pResult;

	return (LRESULT)1;
}