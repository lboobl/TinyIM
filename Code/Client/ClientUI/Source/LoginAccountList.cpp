/**
 * @file LoginAccountList.cpp
 * @author DennisMi (https://www.dennisthink.com/)
 * @brief 登录列表实现类
 * @version 0.1
 * @date 2020-03-01
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#include "stdafx.h"
#include "LoginAccountList.h"

/**
 * @brief Construct a new CLoginAccountList::CLoginAccountList object
 * 
 */
CLoginAccountList::CLoginAccountList(void)
{
	m_nLastLoginUser = 0;
}

/**
 * @brief Destroy the CLoginAccountList::CLoginAccountList object
 * 
 */
CLoginAccountList::~CLoginAccountList(void)
{
	Clear();
}


/**
 * @brief 加载登录帐号列表文件
 * 
 * @param lpszFileName 
 * @return BOOL 
 */
BOOL CLoginAccountList::LoadFile(LPCTSTR lpszFileName)
{
	if (NULL == lpszFileName || NULL ==*lpszFileName)
		return FALSE;

	FILE* fp = _tfopen(lpszFileName, _T("rb"));
	if (NULL == fp)
		return FALSE;

	int nCount = 0;
	_fread(&nCount, sizeof(nCount), 1, fp);

	_fread(&m_nLastLoginUser, sizeof(m_nLastLoginUser), 1, fp);
	
	for (int i = 0; i < nCount; i++)
	{
		LOGIN_ACCOUNT_INFO* lpAccount = new LOGIN_ACCOUNT_INFO;
		if (lpAccount != NULL)
		{
			_fread(lpAccount, sizeof(LOGIN_ACCOUNT_INFO), 1, fp);
			m_arrLoginAccount.push_back(lpAccount);
		}
	}
	
	fclose(fp);
	return TRUE;
}


/**
 * @brief 保存登录帐号列表到文件
 * 
 * @param lpszFileName 
 * @return BOOL 
 */
BOOL CLoginAccountList::SaveFile(LPCTSTR lpszFileName)
{
	if (NULL == lpszFileName || NULL ==*lpszFileName)
		return FALSE;

	int nCount = (int)m_arrLoginAccount.size();
	if (nCount <= 0)
		return FALSE;

	FILE* fp = _tfopen(lpszFileName, _T("wb"));
	if (NULL == fp)
		return FALSE;

	_fwrite(&nCount, sizeof(nCount), 1, fp);
	_fwrite(&m_nLastLoginUser, sizeof(m_nLastLoginUser), 1, fp);

	int nCount2 = 0;
	for (int i = 0; i < nCount; i++)
	{
		LOGIN_ACCOUNT_INFO* lpAccount = m_arrLoginAccount[i];
		if (lpAccount != NULL)
		{
			_fwrite(lpAccount, sizeof(LOGIN_ACCOUNT_INFO), 1, fp);
			nCount2++;
		}
	}

	if (nCount2 != nCount)
	{
		fseek(fp, 0, SEEK_SET);
		_fwrite(&nCount2, sizeof(nCount2), 1, fp);
	}

	fclose(fp);
	return TRUE;
}

 
/**
 * @brief 添加帐号到列表
 * 
 * @param lpszUser 用户名
 * @param lpszPwd 登录密码
 * @param nStatus 状态
 * @param bRememberPwd 是否记住密码 
 * @param bAutoLogin 是否自动登录
 * @return BOOL 
 */
BOOL CLoginAccountList::Add(LPCTSTR lpszUser, LPCTSTR lpszPwd, 
	E_UI_ONLINE_STATUS nStatus, BOOL bRememberPwd, BOOL bAutoLogin)
{
	if (NULL == lpszUser)
		return FALSE;

	LOGIN_ACCOUNT_INFO* lpAccount = Find(lpszUser);
	if (lpAccount != NULL)
		return FALSE;

	lpAccount = new LOGIN_ACCOUNT_INFO;
	if (NULL == lpAccount)
		return FALSE;

	int nCount = sizeof(lpAccount->szUser)/sizeof(TCHAR);
	_tcsncpy(lpAccount->szUser, lpszUser, nCount);
	lpAccount->szUser[nCount-1] = _T('\0');

	if (bRememberPwd && lpszPwd != NULL)
	{
		nCount = sizeof(lpAccount->szPwd)/sizeof(TCHAR);
		_tcsncpy(lpAccount->szPwd, lpszPwd, nCount);
		lpAccount->szPwd[nCount-1] = _T('\0');
	}

	lpAccount->nStatus = nStatus;
	lpAccount->bRememberPwd = bRememberPwd;
	lpAccount->bAutoLogin = bAutoLogin;
	m_arrLoginAccount.push_back(lpAccount);
	return TRUE;
}


/**
 * @brief 根据索引,删除对应账号的信息
 * 
 * @param nIndex 
 * @return BOOL 
 */
BOOL CLoginAccountList::Del(int nIndex)
{
	if (nIndex < 0 || nIndex >= (int)m_arrLoginAccount.size())
		return FALSE;

	LOGIN_ACCOUNT_INFO* lpAccount = m_arrLoginAccount[nIndex];
	if (lpAccount != NULL)
		delete lpAccount;
	m_arrLoginAccount.erase(m_arrLoginAccount.begin()+nIndex);
	return TRUE;
}


/**
 * @brief 根据索引,修改帐号信息
 * 
 * @param nIndex 索引值
 * @param lpszUser 用户名
 * @param lpszPwd 用户密码
 * @param nStatus 用户状态
 * @param bRememberPwd 是否记住密码
 * @param bAutoLogin 是否自动登录
 * @return BOOL 
 */
BOOL CLoginAccountList::Modify(int nIndex, LPCTSTR lpszUser, LPCTSTR lpszPwd, 
	E_UI_ONLINE_STATUS nStatus, BOOL bRememberPwd, BOOL bAutoLogin)
{
	if (nIndex < 0 || nIndex >= (int)m_arrLoginAccount.size() || NULL == lpszUser)
		return FALSE;

	LOGIN_ACCOUNT_INFO* lpAccount = m_arrLoginAccount[nIndex];
	if (NULL == lpAccount)
		return FALSE;

	int nCount = sizeof(lpAccount->szUser)/sizeof(TCHAR);
	_tcsncpy(lpAccount->szUser, lpszUser, nCount);
	lpAccount->szUser[nCount-1] = _T('\0');

	if (bRememberPwd && lpszPwd != NULL)
	{
		nCount = sizeof(lpAccount->szPwd)/sizeof(TCHAR);
		_tcsncpy(lpAccount->szPwd, lpszPwd, nCount);
		lpAccount->szPwd[nCount-1] = _T('\0');
	}

	lpAccount->nStatus = nStatus;
	lpAccount->bRememberPwd = bRememberPwd;
	lpAccount->bAutoLogin = bAutoLogin;
	return TRUE;
}

/**
 * @brief 清除所有帐号
 * 
 */
void CLoginAccountList::Clear()
{
	for (int i = 0; i < (int)m_arrLoginAccount.size(); i++)
	{
		LOGIN_ACCOUNT_INFO* lpAccount = m_arrLoginAccount[i];
		if (lpAccount != NULL)
			delete lpAccount;
	}
	m_arrLoginAccount.clear();
}


/**
 * @brief 获取帐号总数
 * 
 * @return int 
 */
int CLoginAccountList::GetCount()
{
	return (int)m_arrLoginAccount.size();
}

// 
/**
 * @brief 根据索引,获取帐号信息
 * 
 * @param nIndex 
 * @return LOGIN_ACCOUNT_INFO* 
 */
LOGIN_ACCOUNT_INFO* CLoginAccountList::GetAccountInfo(int nIndex)
{
	if (nIndex < 0 || nIndex >= (int)m_arrLoginAccount.size())
		return NULL;

	return m_arrLoginAccount[nIndex];
}


/**
 * @brief 根据用户名,查找帐号相关信息
 * 
 * @param lpszUser 
 * @return LOGIN_ACCOUNT_INFO* 
 */
LOGIN_ACCOUNT_INFO* CLoginAccountList::Find(LPCTSTR lpszUser)
{
	if (NULL == lpszUser)
		return NULL;

	for (int i = 0; i < (int)m_arrLoginAccount.size(); i++)
	{
		LOGIN_ACCOUNT_INFO* lpAccount = m_arrLoginAccount[i];
		if (lpAccount != NULL && _tcscmp(lpAccount->szUser, lpszUser) == 0)
			return lpAccount;
	}
	return NULL;
}

/**
 * @brief 获取最后登录账号的索引值
 * 
 * @return int 
 */
int CLoginAccountList::GetLastLoginUser()
{
	return m_nLastLoginUser;
}

/**
 * @brief 获取最后登录账号的账号信息
 * 
 * @return LOGIN_ACCOUNT_INFO* 
 */
LOGIN_ACCOUNT_INFO* CLoginAccountList::GetLastLoginAccountInfo()
{
	return GetAccountInfo(m_nLastLoginUser);
}

/**
 * @brief 获取最后登录的账号的信息
 * 
 * @param lpAccount 
 * @return BOOL 
 */
BOOL CLoginAccountList::GetLastLoginAccountInfo(LOGIN_ACCOUNT_INFO* lpAccount)
{
	if (NULL == lpAccount)
		return FALSE;

	LOGIN_ACCOUNT_INFO* lpAccount2 = GetAccountInfo(m_nLastLoginUser);
	if (NULL == lpAccount2)
		return FALSE;

	memcpy(lpAccount, lpAccount2, sizeof(LOGIN_ACCOUNT_INFO));
	return TRUE;
}

/**
 * @brief 设置最后登录的账号
 * 
 * @param lpszUser 最后登录的用户名
 */
void CLoginAccountList::SetLastLoginUser(LPCTSTR lpszUser)
{
	if (NULL == lpszUser)
		return;

	for (int i = 0; i < (int)m_arrLoginAccount.size(); i++)
	{
		LOGIN_ACCOUNT_INFO* lpAccount = m_arrLoginAccount[i];
		if (lpAccount != NULL && _tcscmp(lpAccount->szUser, lpszUser) == 0)
		{
			m_nLastLoginUser = i;
			return;
		}
	}
}

/**
 * @brief 最后一个用户是否是自动登录
 * 
 * @return BOOL 
 */
BOOL CLoginAccountList::IsAutoLogin()
{
	LOGIN_ACCOUNT_INFO* lpAccount = GetAccountInfo(m_nLastLoginUser);
	return (lpAccount != NULL) ? lpAccount->bAutoLogin : FALSE;
}


/**
 * @brief 就地加密
 * 
 * @param lpBuf 
 * @param nLen 
 */
void CLoginAccountList::Encrypt(char* lpBuf, int nLen)
{
	for (int i = 0; i < nLen; i++)
	{
		lpBuf[i] = lpBuf[i] ^ 0x88;
	}
}

/**
 * @brief 就地解密
 * 
 */
void CLoginAccountList::Decrypt(char* lpBuf, int nLen)
{
	for (int i = 0; i < nLen; i++)
	{
		lpBuf[i] = lpBuf[i] ^ 0x88;
	}
}

/**
 * @brief 读取相关的数据
 * 
 * @param lpBuf 
 * @param nSize 
 * @param nCount 
 * @param fp 
 * @return int 
 */
int CLoginAccountList::_fread(void* lpBuf, int nSize, int nCount, FILE* fp)
{
	int nRet = fread(lpBuf, nSize, nCount, fp);
	Decrypt((char*)lpBuf, nSize*nCount);
	return nRet;
}

/**
 * @brief 写入相关的数据
 * 
 * @param lpBuf 
 * @param nSize 
 * @param nCount 
 * @param fp 
 * @return int 
 */
int CLoginAccountList::_fwrite(const void* lpBuf, int nSize, int nCount, FILE* fp)
{
	int nLen = nSize*nCount;
	if (nLen > sizeof(LOGIN_ACCOUNT_INFO))
		return 0;
	char cBuf[sizeof(LOGIN_ACCOUNT_INFO)];
	memcpy(cBuf, lpBuf, nLen);
	Encrypt(cBuf, nLen);
	return fwrite(cBuf, nSize, nCount, fp);
}