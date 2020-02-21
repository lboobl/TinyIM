/**
 * @file UI_USER_INFO.cpp
 * @author DennisMi (https://www.dennisthink.com/)
 * @brief 用户信息实现文件
 * @version 0.1
 * @date 2019-10-16
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include "stdafx.h"
#include "UI_USER_INFO.h"
#include "net/IUProtocolData.h"
#include "EncodingUtil.h"
#include "Utils.h"
#include "Path.h"
//#include "Buffer.h"

//主程序目录
TCHAR g_szHomePath[MAX_PATH] = {0};
char g_szHomePathAscii[MAX_PATH] = {0};

std::wstring g_strAppTitle;

//系统库中用户头像的个数
const UINT USER_THUMB_COUNT = 36;


std::string C_UI_FontInfo::ColorHexString() const 
{
	TCHAR buff[32] = { 0 };
	RGBToHexStr(m_clrText, buff, 32);
	return EncodeUtil::UnicodeToAnsi(buff);
}
bool C_UI_FontInfo::ColorHexString(const std::string strColor)
{
	TCHAR buff[32] = { 0 };
	EncodeUtil::AnsiToUnicode(strColor.c_str(), buff, 32);
	m_clrText = HexStrToRGB(buff);
	return true;
}


/**
 * @brief Construct a new C_UI_BuddyInfo::C_UI_BuddyInfo object
 * 
 */
C_UI_BuddyInfo::C_UI_BuddyInfo(void)
{
    Reset();
    //FillFakeData();
}


/**
 * @brief Destroy the C_UI_BuddyInfo::C_UI_BuddyInfo object
 * 
 */
C_UI_BuddyInfo::~C_UI_BuddyInfo(void)
{

}


/**
 * @brief 重置好友信息
 * 
 */
void C_UI_BuddyInfo::Reset()
{
    //m_uUserID = 0;
    m_strAccount = _T("");
    m_nTeamIndex = 0;
    m_strNickName = _T("");
    m_strMarkName = _T("");
    m_nFace = 0;
    m_strCustomFace = _T("");
    m_nStatus = E_UI_ONLINE_STATUS::STATUS_OFFLINE;
    m_nClientType = E_UI_CLIENT_TYPE::CLIENT_TYPE_UNKNOWN;
    m_strSign = _T("");
    m_nGender = 1;
    m_nBirthday = 0;
    m_strMobile = _T("");
    m_strEmail = _T("");
    m_strAddress = _T("");
    m_uMsgID = 0;
    m_bUseCustomFace = FALSE;
    m_bCustomFaceAvailable = FALSE;

    m_nTeamIndex = 0;
}

/**
 * @brief 填充假数据
 * 
 */
void C_UI_BuddyInfo::FillFakeData()
{
    m_uUserIndex = 0;
    m_nTeamIndex = 0;
    m_strNickName = _T("孙悟空");
    m_strMarkName = _T("");
    m_nFace = 0;
    m_nStatus = E_UI_ONLINE_STATUS::STATUS_OFFLINE;
    m_strSign = _T("观乎其上,得乎其中,观乎其中,得乎其下。");
    m_nBirthday = 0;
    m_strMobile = _T("");
    m_strEmail = _T("balloonwj@UTalk.com");
    m_strAddress = _T("上海市闵行区东川路555号");
    m_uMsgID = 0;
    m_bUseCustomFace = FALSE;
    m_bCustomFaceAvailable = FALSE;
}


/**
 * @brief 用另一个数据填充
 * 
 * @param lpBuddyInfo 
 */
void C_UI_BuddyInfo::SetBuddyInfo(C_UI_BuddyInfo* lpBuddyInfo)
{
    if (NULL == lpBuddyInfo)
    {
        return;
    }   

    m_strAccount = lpBuddyInfo->m_strAccount;
    m_strNickName = lpBuddyInfo->m_strNickName;
    m_nFace = lpBuddyInfo->m_nFace;
    m_strCustomFace = lpBuddyInfo->m_strCustomFace;
    m_nGender = lpBuddyInfo->m_nGender;
    m_nBirthday = lpBuddyInfo->m_nBirthday;
    m_strMobile = lpBuddyInfo->m_strMobile;
    m_strEmail = lpBuddyInfo->m_strEmail;
    m_strAddress = lpBuddyInfo->m_strAddress;
    m_strSign = lpBuddyInfo->m_strSign;
}

/**
 * @brief Construct a new C_UI_BuddyTeamInfo::C_UI_BuddyTeamInfo object
 * 
 */
C_UI_BuddyTeamInfo::C_UI_BuddyTeamInfo(void)
{
    Reset();
}

/**
 * @brief Destroy the C_UI_BuddyTeamInfo::C_UI_BuddyTeamInfo object
 * 
 */
C_UI_BuddyTeamInfo::~C_UI_BuddyTeamInfo(void)
{

}

/**
 * @brief 重置好友分组信息
 * 
 */
void C_UI_BuddyTeamInfo::Reset()
{
    m_nIndex = 0;
    m_nSort = 0;
    m_strName = _T("");

    m_arrBuddyInfo.clear();
}

/**
 * @brief 获取分组好友的个数
 * 
 * @return int 分组好友个数
 */
int C_UI_BuddyTeamInfo::GetBuddyCount()
{
    return m_arrBuddyInfo.size();
}

/**
 * @brief 获取分组在线好友个数
 * 
 * @return int 在线好友个数
 */
int C_UI_BuddyTeamInfo::GetOnlineBuddyCount()
{
    int nCount = 0;
    for (int i = 0; i < (int)m_arrBuddyInfo.size(); i++)
    {
        C_UI_BuddyInfo* lpBuddyInfo = m_arrBuddyInfo[i];
        if ( (lpBuddyInfo != NULL) && 
             (lpBuddyInfo->m_nStatus != E_UI_ONLINE_STATUS::STATUS_OFFLINE) &&
             (lpBuddyInfo->m_nStatus != E_UI_ONLINE_STATUS::STATUS_INVISIBLE) )
        {
            nCount++;
        }   
    }
    return nCount;
}


/**
 * @brief 根据索引获好友分组的好友信息
 * 
 * @param nIndex 好友索引
 * @return C_UI_BuddyInfo* 好友信息
 */
C_UI_BuddyInfo* C_UI_BuddyTeamInfo::GetBuddy(int nIndex)
{
    if (nIndex >= 0 && nIndex < (int)m_arrBuddyInfo.size())
    {
        return m_arrBuddyInfo[nIndex];
    }   
    else
    {
        return NULL;
    } 
}

/**
 * @brief 排序分组中的好友
 * 
 */
void C_UI_BuddyTeamInfo::Sort()
{
    C_UI_BuddyInfo* lpBuddyInfo1;
    C_UI_BuddyInfo* lpBuddyInfo2;
    BOOL bExchange;
    int nCount;

    nCount = (int)m_arrBuddyInfo.size();
    for (int i = 0; i < nCount - 1; i++)
    {
        bExchange = FALSE;
        for (int j = nCount - 1; j > i; j--)
        {
            lpBuddyInfo1 = m_arrBuddyInfo[j - 1];
            lpBuddyInfo2 = m_arrBuddyInfo[j];
            if (lpBuddyInfo1 != NULL && lpBuddyInfo2 != NULL)
            {
                if (lpBuddyInfo2->m_nStatus > lpBuddyInfo1->m_nStatus)
                {
                    m_arrBuddyInfo[j - 1] = lpBuddyInfo2;
                    m_arrBuddyInfo[j] = lpBuddyInfo1;
                    bExchange = TRUE;
                }
            }
        }
        if (!bExchange)
        {
            break;
        }   
    }
}

/**
 * @brief Construct a new C_UI_OnlineBuddyInfo::C_UI_OnlineBuddyInfo object
 * 
 */
C_UI_OnlineBuddyInfo::C_UI_OnlineBuddyInfo(void)
{
    Reset();
}


/**
 * @brief Destroy the C_UI_OnlineBuddyInfo::C_UI_OnlineBuddyInfo object
 * 
 */
C_UI_OnlineBuddyInfo::~C_UI_OnlineBuddyInfo(void)
{

}

/**
 * @brief 重置在线好友信息
 * 
 */
void C_UI_OnlineBuddyInfo::Reset()
{
    m_nUin = 0;
    m_nStatus = E_UI_ONLINE_STATUS::STATUS_OFFLINE;
    m_nClientType = 0;
}

/**
 * @brief Construct a new C_UI_BuddyList::C_UI_BuddyList object
 * 
 */
C_UI_BuddyList::C_UI_BuddyList(void)
{
    Reset();
}

/**
 * @brief Destroy the C_UI_BuddyList::C_UI_BuddyList object
 * 
 */
C_UI_BuddyList::~C_UI_BuddyList(void)
{
    Reset();
}

/**
 * @brief 重置好友列表
 * 
 */
void C_UI_BuddyList::Reset()
{
    m_nRetCode = 0;

    for (int i = 0; i < (int)m_arrBuddyTeamInfo.size(); i++)
    {
        C_UI_BuddyTeamInfo* lpBuddyTeamInfo = m_arrBuddyTeamInfo[i];
        if (lpBuddyTeamInfo != NULL)
        {
            delete lpBuddyTeamInfo;
        }    
    }
    m_arrBuddyTeamInfo.clear();
}

/**
 * @brief 获取分组个数
 * 
 * @return int 分组个数
 */
int C_UI_BuddyList::GetBuddyTeamCount()
{
    return m_arrBuddyTeamInfo.size();
}

bool C_UI_BuddyList::IsFriend(const std::string strFriendId)
{
	for (std::size_t i = 0; i < this->m_arrBuddyTeamInfo.size(); i++)
	{
		for (std::size_t j = 0; j < this->m_arrBuddyTeamInfo[i]->m_arrBuddyInfo.size(); j++)
		{
			if (m_arrBuddyTeamInfo[i]->m_arrBuddyInfo[j]->m_strUserId == strFriendId)
			{
				return true;
			}
		}
	}
	return false;
}
/**
 * @brief 根据分组ID获取好友分组信息
 * 
 * @param nTeamIndex 分组ID
 * @return C_UI_BuddyTeamInfo* 分组信息
 */
C_UI_BuddyTeamInfo* C_UI_BuddyList::GetBuddyTeam(int nTeamIndex)
{
    if (nTeamIndex >= 0 && nTeamIndex < (int)m_arrBuddyTeamInfo.size())
    {
        return m_arrBuddyTeamInfo[nTeamIndex];
    }  
    else
    {
        return NULL;
    } 
}

/**
 * @brief 根据分组索引获取分组信息
 * 
 * @param nIndex 分组索引
 * @return C_UI_BuddyTeamInfo* 分组信息
 */
C_UI_BuddyTeamInfo* C_UI_BuddyList::GetBuddyTeamByIndex(int nIndex)
{
    for (int i = 0; i < (int)m_arrBuddyTeamInfo.size(); i++)
    {
        C_UI_BuddyTeamInfo* lpBuddyTeamInfo = m_arrBuddyTeamInfo[i];
        if (lpBuddyTeamInfo != NULL && nIndex == lpBuddyTeamInfo->m_nIndex)
        {
            return lpBuddyTeamInfo;
        }
    }
    return NULL;
}


/**
 * @brief 获取好友总数
 * 
 * @return int 好友总数
 */
int C_UI_BuddyList::GetBuddyTotalCount()
{
    size_t nTotalBuddyCount = 0;

    size_t nTeamCount = GetBuddyTeamCount();
    for (size_t i = 0; i<nTeamCount; ++i)
    {
        nTotalBuddyCount += GetBuddyCount(i);
    }

    return nTotalBuddyCount;
}

/**
 * @brief 根据分组ID，获取分组的好友数量
 * 
 * @param nTeamIndex 分组ID
 * @return int 分组的好友数量
 */
int C_UI_BuddyList::GetBuddyCount(int nTeamIndex)
{
    C_UI_BuddyTeamInfo* lpBuddyTeamInfo = GetBuddyTeam(nTeamIndex);
    if (lpBuddyTeamInfo != NULL)
    {
        return lpBuddyTeamInfo->GetBuddyCount();
    }  
    else
    {
        return 0;
    }
}


/**
 * @brief 根据分组ID获取分组的在线好友数
 * 
 * @param nTeamIndex 分组ID
 * @return int 分组的在线好友数
 */
int C_UI_BuddyList::GetOnlineBuddyCount(int nTeamIndex)
{
    C_UI_BuddyTeamInfo* lpBuddyTeamInfo = GetBuddyTeam(nTeamIndex);
    if (lpBuddyTeamInfo != NULL)
    {
        return lpBuddyTeamInfo->GetOnlineBuddyCount();
    }   
    else
    {
        return 0;
    } 
}

/**
 * @brief 根据分组索引和好友索引获取好友信息
 * 
 * @param nTeamIndex 分组索引
 * @param nIndex 好友索引
 * @return C_UI_BuddyInfo* 好友信息
 */
C_UI_BuddyInfo* C_UI_BuddyList::GetBuddy(int nTeamIndex, int nIndex)
{
    C_UI_BuddyTeamInfo* lpBuddyTeamInfo = GetBuddyTeam(nTeamIndex);
    if (lpBuddyTeamInfo != NULL)
    {
        return lpBuddyTeamInfo->GetBuddy(nIndex);
    } 
    else
    {
        return NULL;
    }
}

/**
 * @brief 获取好友信息
 * 
 * @param nUTalkUin 
 * @return C_UI_BuddyInfo* 
 */
//C_UI_BuddyInfo* C_UI_BuddyList::GetBuddy(UINT nUTalkUin)
//{
//    for (int i = 0; i < (int)m_arrBuddyTeamInfo.size(); i++)
//    {
//        C_UI_BuddyTeamInfo* lpBuddyTeamInfo = m_arrBuddyTeamInfo[i];
//        if (lpBuddyTeamInfo != NULL)
//        {
//            for (int j = 0; j < (int)lpBuddyTeamInfo->m_arrBuddyInfo.size(); j++)
//            {
//                C_UI_BuddyInfo* lpBuddyInfo = lpBuddyTeamInfo->m_arrBuddyInfo[j];
//                //if (lpBuddyInfo != NULL && lpBuddyInfo->m_nUTalkUin == nUTalkUin)
//                //if (lpBuddyInfo != NULL && lpBuddyInfo->m_uUserID == nUTalkUin)
//                //{
//                //    return lpBuddyInfo;
//                //}
//            }
//        }
//    }
//    return NULL;
//}


/**
 * @brief 排序好友列表的分组
 * 
 */
void C_UI_BuddyList::SortBuddyTeam()
{
    C_UI_BuddyTeamInfo* lpBuddyTeamInfo1,*lpBuddyTeamInfo2;
    BOOL bExchange;
    int nCount;

    nCount = (int)m_arrBuddyTeamInfo.size();
    for (int i = 0; i < nCount - 1; i++)
    {
        bExchange = FALSE;
        for (int j = nCount - 1; j > i; j--)
        {
            lpBuddyTeamInfo1 = m_arrBuddyTeamInfo[j - 1];
            lpBuddyTeamInfo2 = m_arrBuddyTeamInfo[j];
            if (lpBuddyTeamInfo1 != NULL && lpBuddyTeamInfo2 != NULL)
            {
                if (lpBuddyTeamInfo2->m_nSort < lpBuddyTeamInfo1->m_nSort)
                {
                    m_arrBuddyTeamInfo[j - 1] = lpBuddyTeamInfo2;
                    m_arrBuddyTeamInfo[j] = lpBuddyTeamInfo1;
                    bExchange = TRUE;
                }
            }
        }
        if (!bExchange)
        {
            break;
        } 
    }
}

/**
 * @brief 对好友排序
 * 
 */
void C_UI_BuddyList::SortBuddy()
{
    for (int i = 0; i < (int)m_arrBuddyTeamInfo.size(); i++)
    {
        C_UI_BuddyTeamInfo* lpTeamInfo = m_arrBuddyTeamInfo[i];
        if (lpTeamInfo != NULL)
        {
            lpTeamInfo->Sort();
        }
    }
}

/**
 * @brief 增加好友分组
 * 
 * @param lpBuddyTeamInfo 好友分组
 * @return BOOL 
 */
BOOL C_UI_BuddyList::AddBuddyTeam(C_UI_BuddyTeamInfo* lpBuddyTeamInfo)
{
    if (NULL == lpBuddyTeamInfo)
    {
        return FALSE;
    } 
    lpBuddyTeamInfo->m_nIndex = m_arrBuddyTeamInfo.size();
    m_arrBuddyTeamInfo.push_back(lpBuddyTeamInfo);
    return TRUE;
}



/**
 * @brief 判断某个名称的分组是否已经存在
 * 
 * @param pszTeamName 分组名称
 * @return BOOL 是否存在
 */
BOOL C_UI_BuddyList::IsTeamNameExist(PCTSTR pszTeamName)
{
    C_UI_BuddyTeamInfo* pTeamInfo = NULL;
    size_t nTeamCount = m_arrBuddyTeamInfo.size();
    for (size_t i = 0; i<nTeamCount; ++i)
    {
        pTeamInfo = m_arrBuddyTeamInfo[i];
        if (pTeamInfo == NULL)
        {
            continue;
        }

        if (pTeamInfo->m_strName == pszTeamName)
        {
            return TRUE;
        } 
    }
    return FALSE;
}


/**
 * @brief Construct a new C_UI_GroupInfo::C_UI_GroupInfo object
 * 
 */
C_UI_GroupInfo::C_UI_GroupInfo(void)
{
    Reset();
}


/**
 * @brief Destroy the C_UI_GroupInfo::C_UI_GroupInfo object
 * 
 */
C_UI_GroupInfo::~C_UI_GroupInfo(void)
{
    Reset();
}


/**
 * @brief 重置群组信息
 * 
 */
void C_UI_GroupInfo::Reset()
{
    //m_nGroupCode = 0;
    //m_nGroupId = 0;
    //m_nGroupNumber = 0;
    m_strName = _T("");
    m_strMemo = _T("");
    m_strFingerMemo = _T("");
    //m_nOwnerUin = 0;
    m_nCreateTime = 0;
    m_nFace = 0;
    m_nLevel = 0;
    m_nClass = 0;
    m_nOption = 0;
    m_nFlag = 0;
    m_bHasGroupNumber = m_bHasGroupInfo = FALSE;

    DelAllMember();
}


/**
 * @brief 获取群成员总人数
 * 
 * @return int 总人数
 */
int C_UI_GroupInfo::GetMemberCount()
{
    return static_cast<int>(m_arrMember.size());
}


/**
 * @brief 获取群成员在线人数
 * 
 * @return int 在线人数
 */
int C_UI_GroupInfo::GetOnlineMemberCount()
{
    int nCount = 0;
    for (int i = 0; i < (int)m_arrMember.size(); i++)
    {
        C_UI_BuddyInfo* lpBuddyInfo = m_arrMember[i];
        if (lpBuddyInfo != NULL && lpBuddyInfo->m_nStatus != E_UI_ONLINE_STATUS::STATUS_OFFLINE)
        {
            nCount++;
        }
    }
    return nCount;
}


/**
 * @brief 根据索引获取群成员信息
 * 
 * @param nIndex 索引
 * @return C_UI_BuddyInfo* 成员信息
 */
C_UI_BuddyInfo* C_UI_GroupInfo::GetMember(int nIndex)
{
    if (nIndex < 0 || nIndex >= (int)m_arrMember.size())
    {
        return NULL;
    }  

    return m_arrMember[nIndex];
}


/**
 * @brief 根据UTalkUin获取群成员信息
 * 
 * @param nUTalkUin 
 * @return C_UI_BuddyInfo* 成员信息
 */
//C_UI_BuddyInfo* C_UI_GroupInfo::GetMemberByUin(UINT nUTalkUin)
//{
//    //for (int i = 0; i < (int)m_arrMember.size(); i++)
//    //{
//    //    C_UI_BuddyInfo* lpBuddyInfo = m_arrMember[i];
//    //    if (lpBuddyInfo != NULL && lpBuddyInfo->m_uUserID == nUTalkUin)
//    //    {
//    //        return lpBuddyInfo;
//    //    } 
//    //}
//    return NULL;
//}


/**
 * @brief 根据UTalkNum获取群成员信息
 * 
 * @param pszAccountName 
 * @return C_UI_BuddyInfo* 群成员信息
 */
C_UI_BuddyInfo* C_UI_GroupInfo::GetMemberByAccount(PCTSTR pszAccountName)
{
    for (int i = 0; i < (int)m_arrMember.size(); i++)
    {
        C_UI_BuddyInfo* lpBuddyInfo = m_arrMember[i];
        if (lpBuddyInfo != NULL && lpBuddyInfo->m_strAccount == pszAccountName)
        {
            return lpBuddyInfo;
        }
    }
    return NULL;
}


/**
 * @brief 对群成员列表按在线状态进行排序
 * 
 */
void C_UI_GroupInfo::Sort()
{
    C_UI_BuddyInfo* lpBuddyInfo1=nullptr;
    C_UI_BuddyInfo* lpBuddyInfo2=nullptr;
    BOOL bExchange;
    int nCount;

    nCount = (int)m_arrMember.size();
    for (int i = 0; i < nCount - 1; i++)
    {
        bExchange = FALSE;
        for (int j = nCount - 1; j > i; j--)
        {
            lpBuddyInfo1 = m_arrMember[j - 1];
            lpBuddyInfo2 = m_arrMember[j];
            if (lpBuddyInfo1 != NULL && lpBuddyInfo2 != NULL)
            {
                if (lpBuddyInfo2->m_nStatus > lpBuddyInfo1->m_nStatus)
                {
                    m_arrMember[j - 1] = lpBuddyInfo2;
                    m_arrMember[j] = lpBuddyInfo1;
                    bExchange = TRUE;
                }
            }
        }
        if (!bExchange)
        {
            break;
        }
    }
}


/**
 * @brief 添加群成员
 * 
 * @param lpBuddyInfo 群成员
 * @return BOOL 
 */
BOOL C_UI_GroupInfo::AddMember(C_UI_BuddyInfo* lpBuddyInfo)
{
    if (NULL == lpBuddyInfo)
    {
        return FALSE;
    }
    m_arrMember.push_back(lpBuddyInfo);
    return TRUE;
}


/**
 * @brief 删除所有群成员
 * 
 * @return BOOL 
 */
BOOL C_UI_GroupInfo::DelAllMember()
{
    for (int i = 0; i < (int)m_arrMember.size(); i++)
    {
        C_UI_BuddyInfo* lpBuddyInfo = m_arrMember[i];
        if (lpBuddyInfo != NULL)
        {
            delete lpBuddyInfo;
        }
    }
    m_arrMember.clear();
    return TRUE;
}

/**
 * @brief 是否有群组号码
 * 
 * @return BOOL 
 */
BOOL C_UI_GroupInfo::IsHasGroupNumber()
{
    return m_bHasGroupNumber;
}

/**
 * @brief 是否有群组信息
 * 
 * @return BOOL 
 */
BOOL C_UI_GroupInfo::IsHasGroupInfo()
{
    return m_bHasGroupInfo;
}

/**
 * @brief 是否是群组成员
 * 
 * @param uAccountID 
 * @return BOOL 
 */
BOOL C_UI_GroupInfo::IsMember(UINT uAccountID)
{
    //for (std::vector<C_UI_BuddyInfo*>::iterator iter = m_arrMember.begin(); iter != m_arrMember.end(); ++iter)
    //{
    //    if ((*iter)->m_uUserID == uAccountID)
    //    {
    //        return TRUE;
    //    }
    //}

    return FALSE;
}

/**
 * @brief Construct a new CGroupList::CGroupList object
 * 
 */
CGroupList::CGroupList(void)
{
    Reset();
}


/**
 * @brief Destroy the CGroupList::CGroupList object
 * 
 */
CGroupList::~CGroupList(void)
{
    Reset();
}

/**
 * @brief 重置群组列表
 * 
 */
void CGroupList::Reset()
{
    for (int i = 0; i < (int)m_arrGroupInfo.size(); i++)
    {
        C_UI_GroupInfo* lpGroupInfo = m_arrGroupInfo[i];
        if (lpGroupInfo != NULL)
        {
            delete lpGroupInfo;
        }
    }
    m_arrGroupInfo.clear();
}


/**
 * @brief 获取群总数
 * 
 * @return int 
 */
int CGroupList::GetGroupCount()
{
    return static_cast<int>(m_arrGroupInfo.size());
}


/**
 * @brief 获取群信息(根据索引)
 * 
 * @param nIndex 索引
 * @return C_UI_GroupInfo* 群信息
 */
C_UI_GroupInfo* CGroupList::GetGroup(int nIndex)
{
	if (nIndex >= 0 && nIndex < (int)m_arrGroupInfo.size())
	{
		return m_arrGroupInfo[nIndex];
	}
	else
	{
		return NULL;
	}
}


/**
 * @brief 获取群信息(根据群代码)
 * 
 * @param nGroupCode 群代码
 * @return C_UI_GroupInfo* 群信息
 */
//C_UI_GroupInfo* CGroupList::GetGroupByCode(UINT nGroupCode)
//{
//    for (int i = 0; i < (int)m_arrGroupInfo.size(); i++)
//    {
//        C_UI_GroupInfo* lpGroupInfo = m_arrGroupInfo[i];
//        if (lpGroupInfo != NULL)
//        {
//            return lpGroupInfo;
//        }
//    }
//    return NULL;
//}


/**
 * @brief 获取群信息(根据群Id)
 * 
 * @param nGroupId 群Id
 * @return C_UI_GroupInfo* 群信息
 */
C_UI_GroupInfo* CGroupList::GetGroupById(const std::string strGroupId)
{
    for (int i = 0; i < static_cast<int>(m_arrGroupInfo.size()); i++)
    {
        C_UI_GroupInfo* lpGroupInfo = m_arrGroupInfo[i];
        if (lpGroupInfo != NULL && lpGroupInfo->m_strGroupId == strGroupId)
        {
            return lpGroupInfo;
        }
    }
    return NULL;
}


/**
 * @brief 根据群代码和群成员UTalkUin获取群成员信息
 * 
 * @param nGroupCode 群代码
 * @param nUTalkUin 群成员UTalkUin
 * @return C_UI_BuddyInfo* 成员信息
 */
//C_UI_BuddyInfo* CGroupList::GetGroupMemberByCode(UINT nGroupCode, UINT nUTalkUin)
//{
//    C_UI_GroupInfo* lpGroupInfo = GetGroupByCode(nGroupCode);
//    if (lpGroupInfo != NULL)
//    {
//        return lpGroupInfo->GetMemberByUin(nUTalkUin);
//    }
//    else
//    {
//        return NULL;
//    } 
//}


/**
 * @brief 根据群Id和群成员UTalkUin获取群成员信息
 * 
 * @param nGroupId 群Id
 * @param nUTalkUin 群成员UTalkUin
 * @return C_UI_BuddyInfo* 成员信息
 */
//C_UI_BuddyInfo* CGroupList::GetGroupMemberById(UINT nGroupId, UINT nUTalkUin)
//{
//    C_UI_GroupInfo* lpGroupInfo = GetGroupById(nGroupId);
//    if (lpGroupInfo != NULL)
//    {
//        return lpGroupInfo->GetMemberByUin(nUTalkUin);
//    }
//    else
//    {
//        return NULL;
//    }
//}


/**
 * @brief 添加群
 * 
 * @param lpGroupInfo 群信息
 * @return BOOL 成功或者失败
 */
BOOL CGroupList::AddGroup(C_UI_GroupInfo* lpGroupInfo)
{
    if (NULL == lpGroupInfo)
    {
        return FALSE;
    } 
    m_arrGroupInfo.push_back(lpGroupInfo);
    return TRUE;
}


/**
 * @brief 由群Id获取群代码
 * 
 * @param nGroupId 群Id
 * @return UINT 群代码
 */
//UINT CGroupList::GetGroupCodeById(UINT nGroupId)
//{
//    C_UI_GroupInfo* lpGroupInfo = GetGroupById(nGroupId);
//	return 0;
//    //return ((lpGroupInfo != NULL) ? lpGroupInfo->m_nGroupCode : 0);
//}


/**
 * @brief 由群代码获取群Id
 * 
 * @param nGroupCode 群代码
 * @return UINT 群Id
 */
//UINT CGroupList::GetGroupIdByCode(UINT nGroupCode)
//{
//    C_UI_GroupInfo* lpGroupInfo = GetGroupByCode(nGroupCode);
//	return 0;
//    //return ((lpGroupInfo != NULL) ? lpGroupInfo->m_nGroupId : 0);
//}


/**
 * @brief Construct a new C_UI_RecentInfo::C_UI_RecentInfo object
 * 
 */
C_UI_RecentInfo::C_UI_RecentInfo(void)
{
    Reset();
}

/**
 * @brief Destroy the C_UI_RecentInfo::C_UI_RecentInfo object
 * 
 */
C_UI_RecentInfo::~C_UI_RecentInfo(void)
{

}

/**
 * @brief 重置最近信息
 * 
 */
void C_UI_RecentInfo::Reset()
{
    m_nType = E_UI_CHAT_MSG_TYPE::FMG_MSG_TYPE_BUDDY;
    m_uUserID = 0;
    m_uFaceID = 0;
    m_MsgTime = 0;
    memset(m_szName, 0, sizeof(m_szName));
    memset(m_szLastMsgText, 0, sizeof(m_szLastMsgText));
}

/**
 * @brief Construct a new C_UI_RecentList::C_UI_RecentList object
 * 
 */
C_UI_RecentList::C_UI_RecentList(void)
{
    Reset();
}

/**
 * @brief Destroy the C_UI_RecentList::C_UI_RecentList object
 * 
 */
C_UI_RecentList::~C_UI_RecentList(void)
{
    Reset();
}

/**
 * @brief 
 * 
 */
void C_UI_RecentList::Reset()
{
    m_nRetCode = 0;

    for (int i = 0; i < (int)m_arrRecentInfo.size(); i++)
    {
        if (m_arrRecentInfo[i] != NULL)
        {
            delete m_arrRecentInfo[i];
        }
    }
    m_arrRecentInfo.clear();
}

/**
 * @brief 删除最近的某个项
 * 
 * @param uAccountID 
 * @return BOOL 
 */
BOOL C_UI_RecentList::DeleteRecentItem(UINT uAccountID)
{
    for (std::vector<C_UI_RecentInfo*>::const_iterator iter = m_arrRecentInfo.begin(); iter != m_arrRecentInfo.end(); ++iter)
    {
        C_UI_RecentInfo* pRecentInfo =*iter;
        if (pRecentInfo != NULL && pRecentInfo->m_uUserID == uAccountID)
        {
            delete pRecentInfo;
            m_arrRecentInfo.erase(iter);
            return TRUE;
        }
    }

    return FALSE;
}

/**
 * @brief 添加最近项
 * 
 * @param lpRecentInfo 
 * @return BOOL 
 */
BOOL C_UI_RecentList::AddRecent(C_UI_RecentInfo* lpRecentInfo)
{
    if (NULL == lpRecentInfo)
    {
        return FALSE;
    }

    //先删除原来的最近联系人信息
    for (std::vector<C_UI_RecentInfo*>::iterator iter = m_arrRecentInfo.begin(); iter != m_arrRecentInfo.end(); ++iter)
    {
        //TODO: 这里有问题
        if (*iter == NULL)
        {
            continue;
        } 

        if ((*iter)->m_uUserID == lpRecentInfo->m_uUserID)
        {
            m_arrRecentInfo.erase(iter);
            break;
        }
    }


    //再将最新的存入进去
    m_arrRecentInfo.push_back(lpRecentInfo);

    //最多五十项
    //TODO 
    if (m_arrRecentInfo.size() >= 50)
    {
        C_UI_RecentInfo* pLast = m_arrRecentInfo[m_arrRecentInfo.size() - 1];
        if (pLast != NULL)
        {
            m_arrRecentInfo.erase(m_arrRecentInfo.end() - 1);
            delete pLast;
        }
    }

    return TRUE;
}

/**
 * @brief 获取最近的总数
 * 
 * @return int 
 */
int C_UI_RecentList::GetRecentCount()
{
    return static_cast<int>(m_arrRecentInfo.size());
}

/**
 * @brief 根据索引获取某个最近的好友信息
 * 
 * @param nIndex 
 * @return C_UI_RecentInfo* 
 */
C_UI_RecentInfo* C_UI_RecentList::GetRecent(int nIndex)
{
    if ( (nIndex >= 0) && 
         (nIndex < static_cast<int>(m_arrRecentInfo.size()) ) )
    {
        return m_arrRecentInfo[nIndex];
    }
    else
    {
        return NULL;
    }
}

/**
 * @brief Construct a new C_UI_BuddyMessage::C_UI_BuddyMessage object
 * 
 */
C_UI_BuddyMessage::C_UI_BuddyMessage(void)
{
    m_nMsgType = E_UI_CONTENT_TYPE::CONTENT_TYPE_UNKNOWN;
    m_nMsgId = 0;
    m_nMsgId2 = 0;
    m_nFromUin = 0;
    m_nToUin = 0;
    m_nReplyIp = 0;
    m_nTime = 0;
    m_hwndFrom = NULL;
}

/**
 * @brief Destroy the C_UI_BuddyMessage::C_UI_BuddyMessage object
 * 
 */
C_UI_BuddyMessage::~C_UI_BuddyMessage(void)
{
    Reset();
}

/**
 * @brief 
 * 
 * @param lpType 
 * @return BOOL 
 */
BOOL C_UI_BuddyMessage::IsType(LPCTSTR lpType)
{
    return (_tcsicmp(lpType, _T("message")) == 0);
}

/**
 * @brief 好友信息重置
 * 
 */
void C_UI_BuddyMessage::Reset()
{
    m_nMsgId = 0;
    m_nMsgId2 = 0;
    m_nFromUin = 0;
    m_nToUin = 0;
    m_nMsgType = E_UI_CONTENT_TYPE::CONTENT_TYPE_UNKNOWN;
    m_nReplyIp = 0;
    m_nTime = 0;

    for (int i = 0; i < (int)m_arrContent.size(); i++)
    {
        C_UI_Content* lpContent = m_arrContent[i];
        if (lpContent != NULL)
        {
            delete lpContent;
        }
    }
    m_arrContent.clear();
}

/**
 * @brief Construct a new C_WND_MSG_BuddyTextMessage::C_WND_MSG_BuddyTextMessage object
 * 
 */
C_WND_MSG_BuddyTextMessage::C_WND_MSG_BuddyTextMessage()
{
	m_uType = E_UI_NET_DATA_TYPE::NET_DATA_FRIEND_RECV_TEXT_REQ;
}
//BOOL C_UI_BuddyMessage::Parse(Json::Value& JsonValue)
//{
//    std::string strValue;
//    int nCount;
//
//    if (JsonValue["msgType"].isNull() || !JsonValue["msgType"].isInt())
//        return FALSE;
//
//    m_nMsgType = (CONTENT_TYPE)JsonValue["msgType"].asInt();
//
//    if (JsonValue["time"].isNull())
//        return false;
//
//
//    m_nTime = JsonValue["time"].asUInt();
//
//    CString strTmp;
//
//    C_UI_Content* pContent = new C_UI_Content;
//    //先解析字体信息
//    //字体名
//    if (!JsonValue["font"][(UINT)0].isNull())
//    {
//        strValue = JsonValue["font"][(UINT)0].asString();
//        EncodeUtil::Utf8ToUnicode(strValue.data(), strTmp.GetBuffer(strValue.length()* 2), strValue.length()* 2);
//        strTmp.ReleaseBuffer();
//        pContent->m_FontInfo.m_strName = strTmp.GetString();
//    }
//    else
//        pContent->m_FontInfo.m_strName = _T("微软雅黑");
//
//    //字体大小
//    if (!JsonValue["font"][(UINT)1].isNull())
//        pContent->m_FontInfo.m_nSize = JsonValue["font"][(UINT)1].asUInt();
//    else
//        pContent->m_FontInfo.m_nSize = 12;
//
//    //字体颜色
//    if (!JsonValue["font"][(UINT)2].isNull())
//        pContent->m_FontInfo.m_clrText = JsonValue["font"][(UINT)2].asUInt();
//    else
//        pContent->m_FontInfo.m_clrText = RGB(0, 0, 0);
//
//    //是否粗体
//    if (!JsonValue["font"][(UINT)3].isNull())
//        pContent->m_FontInfo.m_bBold = JsonValue["font"][(UINT)3].asInt() != 0 ? TRUE : FALSE;
//    else
//        pContent->m_FontInfo.m_bBold = FALSE;
//
//    //是否斜体
//    if (!JsonValue["font"][(UINT)4].isNull())
//        pContent->m_FontInfo.m_bItalic = JsonValue["font"][(UINT)4].asInt() != 0 ? TRUE : FALSE;
//    else
//        pContent->m_FontInfo.m_bItalic = FALSE;
//
//    //是否下划线
//    if (!JsonValue["font"][(UINT)5].isNull())
//        pContent->m_FontInfo.m_bUnderLine = JsonValue["font"][(UINT)5].asInt() != 0 ? TRUE : FALSE;
//    else
//        pContent->m_FontInfo.m_bUnderLine = FALSE;
//
//    pContent->m_nType = CONTENT_TYPE_FONT_INFO;
//    m_arrContent.push_back(pContent);
//
//    std::string strNodeValue;
//    nCount = (int)JsonValue["content"].size();
//    for (int i = 0; i < nCount; ++i)
//    {
//        if (JsonValue["content"][i].isNull())
//            continue;
//
//        if (!JsonValue["content"][i]["msgText"].isNull())
//        {
//            pContent = new C_UI_Content();
//            pContent->m_nType = CONTENT_TYPE_TEXT;
//            strNodeValue = JsonValue["content"][i]["msgText"].asString();
//            EncodeUtil::Utf8ToUnicode(strNodeValue.data(), strTmp.GetBuffer(strNodeValue.length()* 2), strNodeValue.length()* 2);
//            strTmp.ReleaseBuffer();
//            pContent->m_strText = strTmp;
//            m_arrContent.push_back(pContent);
//        }
//        else if (!JsonValue["content"][i]["faceID"].isNull())
//        {
//            pContent = new C_UI_Content();
//            pContent->m_nType = CONTENT_TYPE_FACE;
//            pContent->m_nFaceId = JsonValue["content"][i]["faceID"].asInt();
//            m_arrContent.push_back(pContent);
//        }
//        else if (!JsonValue["content"][i]["shake"].isNull())
//        {
//            pContent = new C_UI_Content();
//            pContent->m_nType = CONTENT_TYPE_SHAKE_WINDOW;
//            pContent->m_nShakeTimes = JsonValue["content"][i]["shake"].asInt();
//            m_arrContent.push_back(pContent);
//        }
//        else if (!JsonValue["content"][i]["pic"].isNull())
//        {
//            pContent = new C_UI_Content();
//            pContent->m_nType = CONTENT_TYPE_CHAT_IMAGE;
//            strNodeValue = JsonValue["content"][i]["pic"][(UINT)0].asString();;
//            EncodeUtil::Utf8ToUnicode(strNodeValue.data(), strTmp.GetBuffer(strNodeValue.length()* 2), strNodeValue.length()* 2);
//            strTmp.ReleaseBuffer();
//            pContent->m_CFaceInfo.m_strFileName = strTmp;
//
//            strNodeValue = JsonValue["content"][i]["pic"][(UINT)1].asString();;
//            if (!strNodeValue.empty())
//            {
//                EncodeUtil::Utf8ToUnicode(strNodeValue.data(), strTmp.GetBuffer(strNodeValue.length()* 2), strNodeValue.length()* 2);
//                strTmp.ReleaseBuffer();
//                pContent->m_CFaceInfo.m_strFilePath = strTmp;
//            }
//
//            pContent->m_CFaceInfo.m_dwFileSize = JsonValue["content"][i]["pic"][(UINT)2].asUInt();
//            m_arrContent.push_back(pContent);
//        }
//        else if (!JsonValue["content"][i]["file"].isNull())
//        {
//            pContent = new C_UI_Content();
//            pContent->m_nType = CONTENT_TYPE_FILE;
//            strNodeValue = JsonValue["content"][i]["file"][(UINT)0].asString();;
//            EncodeUtil::Utf8ToUnicode(strNodeValue.data(), strTmp.GetBuffer(strNodeValue.length()* 2), strNodeValue.length()* 2);
//            strTmp.ReleaseBuffer();
//            pContent->m_CFaceInfo.m_strFileName = strTmp;
//
//            strNodeValue = JsonValue["content"][i]["file"][(UINT)1].asString();;
//            EncodeUtil::Utf8ToUnicode(strNodeValue.data(), strTmp.GetBuffer(strNodeValue.length()* 2), strNodeValue.length()* 2);
//            strTmp.ReleaseBuffer();
//            pContent->m_CFaceInfo.m_strFilePath = strTmp;
//
//            pContent->m_CFaceInfo.m_dwFileSize = JsonValue["content"][i]["file"][2].asUInt();
//
//            pContent->m_CFaceInfo.m_bOnline = JsonValue["content"][i]["file"][3].asUInt();
//            m_arrContent.push_back(pContent);
//        }
//        else if (!JsonValue["content"][i]["remotedesktop"].isNull())
//        {
//            pContent = new C_UI_Content();
//            pContent->m_nType = CONTENT_TYPE_REMOTE_DESKTOP;
//            m_arrContent.push_back(pContent);
//        }
//
//    }
//
//    return TRUE;
//}


/**
 * @brief 判断是否是窗口抖动消息
 * 
 * @return BOOL 
 */
BOOL C_UI_BuddyMessage::IsShakeWindowMsg()
{
    long nSize = m_arrContent.size();
    if (nSize <= 0)
    {
        return FALSE;
    }

    for (long i = nSize - 1; i >= 0; --i)
    {
        if (m_arrContent[i]->m_nType == E_UI_CONTENT_TYPE::CONTENT_TYPE_SHAKE_WINDOW)
        {
            return TRUE;
        } 
    }

    return FALSE;
}


/**
 * @brief Construct a new C_UI_GroupMessage::C_UI_GroupMessage object
 * 
 */
C_UI_GroupMessage::C_UI_GroupMessage(void)
{
    Reset();
}

/**
 * @brief Destroy the C_UI_GroupMessage::C_UI_GroupMessage object
 * 
 */
C_UI_GroupMessage::~C_UI_GroupMessage(void)
{

}

/**
 * @brief 判断是否是群组消息
 * 
 * @param lpType 
 * @return BOOL 
 */
BOOL C_UI_GroupMessage::IsType(LPCTSTR lpType)
{
    return (_tcsicmp(lpType, _T("group_message")) == 0);
}

/**
 * @brief 群组消息重置
 * 
 */
void C_UI_GroupMessage::Reset()
{
}

//BOOL C_UI_GroupMessage::Parse(Json::Value& JsonValue)
//{
//    std::string strValue;
//    int nCount, nCount2;
//
//    Reset();
//
//    if (!JsonValue["msg_id"].isNull())
//        m_nMsgId = JsonValue["msg_id"].asUInt();
//
//    if (!JsonValue["msg_id2"].isNull())
//        m_nMsgId2 = JsonValue["msg_id2"].asUInt();
//
//    if (!JsonValue["from_uin"].isNull())
//        m_nFromUin = JsonValue["from_uin"].asUInt();
//
//    if (!JsonValue["to_uin"].isNull())
//        m_nToUin = JsonValue["to_uin"].asUInt();
//
//    if (!JsonValue["msg_type"].isNull())
//        m_nMsgType = JsonValue["msg_type"].asUInt();
//
//    if (!JsonValue["reply_ip"].isNull())
//        m_nReplyIp = JsonValue["reply_ip"].asUInt();
//
//    if (!JsonValue["time"].isNull())
//        m_nTime = JsonValue["time"].asUInt();
//
//    if (!JsonValue["group_code"].isNull())
//        m_nGroupCode = JsonValue["group_code"].asUInt();
//
//    if (!JsonValue["send_uin"].isNull())
//        m_nSendUin = JsonValue["send_uin"].asUInt();
//
//    if (!JsonValue["seq"].isNull())
//        m_nSeq = JsonValue["seq"].asUInt();
//
//    if (!JsonValue["info_seq"].isNull())
//        m_nInfoSeq = JsonValue["info_seq"].asUInt();
//
//    nCount = (int)JsonValue["content"].size();
//    for (int i = 0; i < nCount; ++i)
//    {
//        if (JsonValue["content"][i].isNull())
//            continue;
//
//        if (JsonValue["content"][i].isString())		// 纯文本
//        {
//            C_UI_Content* lpContent = new C_UI_Content;
//            if (NULL == lpContent)
//                continue;
//
//            strValue = JsonValue["content"][i].asString();
//
//            if (i == nCount - 1)					// 去除消息最后多余的一个空格
//            {
//                nCount2 = strValue.size();
//                if (nCount2 > 0 && strValue[nCount2 - 1] == ' ')
//                    strValue.erase(strValue.end() - 1);
//            }
//
//            Replace(strValue, "\r", "\r\n");
//
//            lpContent->m_nType = CONTENT_TYPE_TEXT;
//            lpContent->m_strText = EncodeUtil::Utf8ToUnicode(strValue);
//            m_arrContent.push_back(lpContent);
//        }
//        else if (JsonValue["content"][i].isArray())
//        {
//            if (JsonValue["content"][i][(UINT)0].isNull())
//                continue;
//
//            strValue = JsonValue["content"][i][(UINT)0].asString();	// 内容类型
//            if (strValue == "font")			// 字体信息
//            {
//                C_UI_Content* lpContent = new C_UI_Content;
//                if (NULL == lpContent)
//                    continue;
//
//                lpContent->m_nType = CONTENT_TYPE_FONT_INFO;
//
//                if (!JsonValue["content"][i][(UINT)1]["size"].isNull())				// 字体大小
//                    lpContent->m_FontInfo.m_nSize = JsonValue["content"][i][(UINT)1]["size"].asInt();
//
//                if (!JsonValue["content"][i][(UINT)1]["color"].isNull())			// 字体颜色
//                {
//                    strValue = JsonValue["content"][i][(UINT)1]["color"].asString();
//                    lpContent->m_FontInfo.m_clrText = HexStrToRGB(EncodeUtil::Utf8ToUnicode(strValue).c_str());
//                }
//
//                if (!JsonValue["content"][i][(UINT)1]["style"][(UINT)0].isNull())	// 字体风格(加粗)
//                    lpContent->m_FontInfo.m_bBold = JsonValue["content"][i][(UINT)1]["style"][(UINT)0].asBool();
//
//                if (!JsonValue["content"][i][(UINT)1]["style"][(UINT)1].isNull())	// 字体风格(倾斜)
//                    lpContent->m_FontInfo.m_bItalic = JsonValue["content"][i][(UINT)1]["style"][(UINT)1].asBool();
//
//                if (!JsonValue["content"][i][(UINT)1]["style"][(UINT)2].isNull())	// 字体风格(下划线)
//                    lpContent->m_FontInfo.m_bUnderLine = JsonValue["content"][i][(UINT)1]["style"][(UINT)2].asBool();
//
//                if (!JsonValue["content"][i][(UINT)1]["name"].isNull())				// 字体名称
//                {
//                    strValue = JsonValue["content"][i][(UINT)1]["name"].asString();
//                    lpContent->m_FontInfo.m_strName = EncodeUtil::Utf8ToUnicode(strValue);
//                }
//
//                m_arrContent.push_back(lpContent);
//            }
//            else if (strValue == "face")	// 系统表情
//            {
//                C_UI_Content* lpContent = new C_UI_Content;
//                if (NULL == lpContent)
//                    continue;
//                lpContent->m_nType = CONTENT_TYPE_FACE;
//                if (!JsonValue["content"][i][(UINT)1].isNull())
//                    lpContent->m_nFaceId = JsonValue["content"][i][(UINT)1].asInt();
//                m_arrContent.push_back(lpContent);
//            }
//            else if (strValue == "cface")	// 自定义表情
//            {
//                C_UI_Content* lpContent = new C_UI_Content;
//                if (NULL == lpContent)
//                    continue;
//                lpContent->m_nType = CONTENT_TYPE_CHAT_IMAGE;
//                if (!JsonValue["content"][i][(UINT)1].isNull())
//                {
//                    if (!JsonValue["content"][i][(UINT)1]["name"].isNull())
//                    {
//                        strValue = JsonValue["content"][i][(UINT)1]["name"].asString();
//                        lpContent->m_CFaceInfo.m_strName = EncodeUtil::Utf8ToUnicode(strValue);
//                    }
//
//                    if (!JsonValue["content"][i][(UINT)1]["file_id"].isNull())
//                        lpContent->m_CFaceInfo.m_nFileId = JsonValue["content"][i][(UINT)1]["file_id"].asUInt();
//
//                    if (!JsonValue["content"][i][(UINT)1]["key"].isNull())
//                    {
//                        strValue = JsonValue["content"][i][(UINT)1]["key"].asString();
//                        lpContent->m_CFaceInfo.m_strKey = EncodeUtil::Utf8ToUnicode(strValue);
//                    }
//
//                    if (!JsonValue["content"][i][(UINT)1]["server"].isNull())
//                    {
//                        strValue = JsonValue["content"][i][(UINT)1]["server"].asString();
//                        lpContent->m_CFaceInfo.m_strServer = EncodeUtil::Utf8ToUnicode(strValue);
//                    }
//                }
//                m_arrContent.push_back(lpContent);
//            }
//            else if (strValue == "cface_idx")	// 未知
//            {
//
//            }
//        }
//    }
//
//    return TRUE;
//}

/**
 * @brief Construct a new C_UI_SessMessage::C_UI_SessMessage object
 * 
 */
C_UI_SessMessage::C_UI_SessMessage(void)
{
    Reset();
}

/**
 * @brief Destroy the C_UI_SessMessage::C_UI_SessMessage object
 * 
 */
C_UI_SessMessage::~C_UI_SessMessage(void)
{
    Reset();
}

/**
 * @brief 判断是否是临时会话消息
 * 
 * @param lpType 
 * @return BOOL 
 */
BOOL C_UI_SessMessage::IsType(LPCTSTR lpType)
{
    return (_tcsicmp(lpType, _T("sess_message")) == 0);
}

/**
 * @brief 重置消息
 * 
 */
void C_UI_SessMessage::Reset()
{
    m_nMsgId = 0;
    m_nMsgId2 = 0;
    m_nFromUin = 0;
    m_nToUin = 0;
    m_nMsgType = 0;
    m_nReplyIp = 0;
    m_nTime = 0;
    m_nGroupId = 0;
    m_nUTalkNum = 0;
    m_nServiceType = 0;
    m_nFlags = 0;

    for (int i = 0; i < (int)m_arrContent.size(); i++)
    {
        C_UI_Content* lpContent = m_arrContent[i];
        if (lpContent != NULL)
        {
            delete lpContent;
        }
    }
    m_arrContent.clear();
}

//BOOL C_UI_SessMessage::Parse(Json::Value& JsonValue)
//{
//    std::string strValue;
//    int nCount, nCount2;
//
//    Reset();
//
//    if (!JsonValue["msg_id"].isNull())
//        m_nMsgId = JsonValue["msg_id"].asUInt();
//
//    if (!JsonValue["msg_id2"].isNull())
//        m_nMsgId2 = JsonValue["msg_id2"].asUInt();
//
//    if (!JsonValue["from_uin"].isNull())
//        m_nFromUin = JsonValue["from_uin"].asUInt();
//
//    if (!JsonValue["to_uin"].isNull())
//        m_nToUin = JsonValue["to_uin"].asUInt();
//
//    if (!JsonValue["msg_type"].isNull())
//        m_nMsgType = JsonValue["msg_type"].asUInt();
//
//    if (!JsonValue["reply_ip"].isNull())
//        m_nReplyIp = JsonValue["reply_ip"].asUInt();
//
//    if (!JsonValue["time"].isNull())
//        m_nTime = JsonValue["time"].asUInt();
//
//    if (!JsonValue["id"].isNull())
//        m_nGroupId = JsonValue["id"].asUInt();
//
//    if (!JsonValue["ruin"].isNull())
//        m_nUTalkNum = JsonValue["ruin"].asUInt();
//
//    if (!JsonValue["service_type"].isNull())
//        m_nServiceType = JsonValue["service_type"].asUInt();
//
//    m_nFlags = 0;	// 待解析("flags":{"text":1,"pic":1,"file":1,"audio":1,"video":1})
//
//    nCount = (int)JsonValue["content"].size();
//    for (int i = 0; i < nCount; ++i)
//    {
//        if (JsonValue["content"][i].isNull())
//            continue;
//
//        if (JsonValue["content"][i].isString())		// 纯文本
//        {
//            C_UI_Content* lpContent = new C_UI_Content;
//            if (NULL == lpContent)
//                continue;
//
//            strValue = JsonValue["content"][i].asString();
//
//            if (i == nCount - 1)					// 去除消息最后多余的一个空格
//            {
//                nCount2 = strValue.size();
//                if (nCount2 > 0 && (strValue[nCount2 - 1] == ' '))
//                    strValue.erase(strValue.end() - 1);
//            }
//
//            Replace(strValue, "\r", "\r\n");
//
//            lpContent->m_nType = CONTENT_TYPE_TEXT;
//            lpContent->m_strText = EncodeUtil::Utf8ToUnicode(strValue);
//            m_arrContent.push_back(lpContent);
//        }
//        else if (JsonValue["content"][i].isArray())
//        {
//            if (JsonValue["content"][i][(UINT)0].isNull())
//                continue;
//
//            strValue = JsonValue["content"][i][(UINT)0].asString();	// 内容类型
//            if (strValue == "font")		// 字体信息
//            {
//                C_UI_Content* lpContent = new C_UI_Content;
//                if (NULL == lpContent)
//                    continue;
//
//                lpContent->m_nType = CONTENT_TYPE_FONT_INFO;
//
//                if (!JsonValue["content"][i][(UINT)1]["size"].isNull())				// 字体大小
//                    lpContent->m_FontInfo.m_nSize = JsonValue["content"][i][(UINT)1]["size"].asInt();
//
//                if (!JsonValue["content"][i][(UINT)1]["color"].isNull())			// 字体颜色
//                {
//                    strValue = JsonValue["content"][i][(UINT)1]["color"].asString();
//                    lpContent->m_FontInfo.m_clrText = HexStrToRGB(EncodeUtil::Utf8ToUnicode(strValue).c_str());
//                }
//
//                if (!JsonValue["content"][i][(UINT)1]["style"][(UINT)0].isNull())	// 字体风格(加粗)
//                    lpContent->m_FontInfo.m_bBold = JsonValue["content"][i][(UINT)1]["style"][(UINT)0].asBool();
//
//                if (!JsonValue["content"][i][(UINT)1]["style"][(UINT)1].isNull())	// 字体风格(倾斜)
//                    lpContent->m_FontInfo.m_bItalic = JsonValue["content"][i][(UINT)1]["style"][(UINT)1].asBool();
//
//                if (!JsonValue["content"][i][(UINT)1]["style"][(UINT)2].isNull())	// 字体风格(下划线)
//                    lpContent->m_FontInfo.m_bUnderLine = JsonValue["content"][i][(UINT)1]["style"][(UINT)2].asBool();
//
//                if (!JsonValue["content"][i][(UINT)1]["name"].isNull())				// 字体名称
//                {
//                    strValue = JsonValue["content"][i][(UINT)1]["name"].asString();
//                    lpContent->m_FontInfo.m_strName = EncodeUtil::Utf8ToUnicode(strValue);
//                }
//                m_arrContent.push_back(lpContent);
//            }
//            else if (strValue == "face")	// 系统表情
//            {
//                C_UI_Content* lpContent = new C_UI_Content;
//                if (NULL == lpContent)
//                    continue;
//                lpContent->m_nType = CONTENT_TYPE_FACE;
//                if (!JsonValue["content"][i][(UINT)1].isNull())
//                    lpContent->m_nFaceId = JsonValue["content"][i][(UINT)1].asInt();
//                m_arrContent.push_back(lpContent);
//            }
//        }
//    }
//
//    return TRUE;
//}

/**
 * @brief Construct a new C_UI_StatusChangeMessage::C_UI_StatusChangeMessage object
 * 
 */
C_UI_StatusChangeMessage::C_UI_StatusChangeMessage(void)
{
    Reset();
}


/**
 * @brief Destroy the C_UI_StatusChangeMessage::C_UI_StatusChangeMessage object
 * 
 */
C_UI_StatusChangeMessage::~C_UI_StatusChangeMessage(void)
{

}

/**
 * @brief 判断是否是状态改变消息
 * 
 * @param lpType 
 * @return BOOL 
 */
BOOL C_UI_StatusChangeMessage::IsType(LPCTSTR lpType)
{
    return (_tcsicmp(lpType, _T("buddies_status_change")) == 0);
}

/**
 * @brief 重置消息
 * 
 */
void C_UI_StatusChangeMessage::Reset()
{
    m_nUTalkUin = 0;
    m_nStatus = E_UI_ONLINE_STATUS::STATUS_OFFLINE;
    m_nClientType = 0;
}

//BOOL C_UI_StatusChangeMessage::Parse(Json::Value& JsonValue)
//{
//    std::string strValue;
//
//    Reset();
//
//    if (!JsonValue["uin"].isNull())
//        m_nUTalkUin = JsonValue["uin"].asUInt();
//
//    if (!JsonValue["status"].isNull())
//    {
//        strValue = JsonValue["status"].asString();
//        /*m_nStatus = ConvertToUTalkStatus(EncodeUtil::Utf8ToUnicode(strValue).c_str());*/
//    }
//
//    if (!JsonValue["client_type"].isNull())
//        m_nClientType = JsonValue["client_type"].asUInt();
//
//    return TRUE;
//}

/**
 * @brief Construct a new C_UI_KickMessage::C_UI_KickMessage object
 * 
 */
C_UI_KickMessage::C_UI_KickMessage(void)
{
    Reset();
}

/**
 * @brief Destroy the C_UI_KickMessage::C_UI_KickMessage object
 * 
 */
C_UI_KickMessage::~C_UI_KickMessage(void)
{

}

/**
 * @brief 
 * 
 * @param lpType 
 * @return BOOL 
 */
BOOL C_UI_KickMessage::IsType(LPCTSTR lpType)
{
    return (_tcsicmp(lpType, _T("kick_message")) == 0);
}


/**
 * @brief 重置踢出消息
 * 
 */
void C_UI_KickMessage::Reset()
{
    m_nMsgId = 0;
    m_nMsgId2 = 0;
    m_nFromUin = 0;
    m_nToUin = 0;
    m_nMsgType = 0;
    m_nReplyIp = 0;
    m_bShowReason = FALSE;
    m_strReason = _T("");
}

//BOOL C_UI_KickMessage::Parse(Json::Value& JsonValue)
//{
//    std::string strValue;
//
//    Reset();
//
//    if (!JsonValue["msg_id"].isNull())
//        m_nMsgId = JsonValue["msg_id"].asUInt();
//
//    if (!JsonValue["from_uin"].isNull())
//        m_nFromUin = JsonValue["from_uin"].asUInt();
//
//    if (!JsonValue["to_uin"].isNull())
//        m_nToUin = JsonValue["to_uin"].asUInt();
//
//    if (!JsonValue["msg_id2"].isNull())
//        m_nMsgId2 = JsonValue["msg_id2"].asUInt();
//
//    if (!JsonValue["msg_type"].isNull())
//        m_nMsgType = JsonValue["msg_type"].asUInt();
//
//    if (!JsonValue["reply_ip"].isNull())
//        m_nReplyIp = JsonValue["reply_ip"].asUInt();
//
//    if (!JsonValue["show_reason"].isNull())
//        m_bShowReason = JsonValue["show_reason"].asBool();
//
//    if (!JsonValue["reason"].isNull())
//    {
//        strValue = JsonValue["reason"].asString();
//        m_strReason = EncodeUtil::Utf8ToUnicode(strValue);
//    }
//
//    return TRUE;
//}

/**
 * @brief Construct a new C_UI_SysGroupMessage::C_UI_SysGroupMessage object
 * 
 */
C_UI_SysGroupMessage::C_UI_SysGroupMessage(void)
{
    Reset();
}


/**
 * @brief Destroy the C_UI_SysGroupMessage::C_UI_SysGroupMessage object
 * 
 */
C_UI_SysGroupMessage::~C_UI_SysGroupMessage(void)
{

}

/**
 * @brief 是否是群组系统的消息
 * 
 * @param lpType 
 * @return BOOL 
 */
BOOL C_UI_SysGroupMessage::IsType(LPCTSTR lpType)
{
    return (_tcsicmp(lpType, _T("sys_g_msg")) == 0);
}

/**
 * @brief 重置系统的群组消息
 * 
 */
void C_UI_SysGroupMessage::Reset()
{
    m_nMsgId = 0;
    m_nMsgId2 = 0;
    m_nFromUin = 0;
    m_nToUin = 0;
    m_nMsgType = 0;
    m_nReplyIp = 0;
    m_strSubType = _T("");
    m_nGroupCode = 0;
    m_nGroupNumber = 0;
    m_nAdminUin = 0;
    m_strMsg = _T("");
    m_nOpType = 0;
    m_nOldMember = 0;
    m_strOldMember = _T("");
    m_strAdminUin = _T("");
    m_strAdminNickName = _T("");
}

//BOOL C_UI_SysGroupMessage::Parse(Json::Value& JsonValue)
//{
//    std::string strValue;
//
//    Reset();
//
//    if (!JsonValue["msg_id"].isNull())
//        m_nMsgId = JsonValue["msg_id"].asUInt();
//
//    if (!JsonValue["from_uin"].isNull())
//        m_nFromUin = JsonValue["from_uin"].asUInt();
//
//    if (!JsonValue["to_uin"].isNull())
//        m_nToUin = JsonValue["to_uin"].asUInt();
//
//    if (!JsonValue["msg_id2"].isNull())
//        m_nMsgId2 = JsonValue["msg_id2"].asUInt();
//
//    if (!JsonValue["msg_type"].isNull())
//        m_nMsgType = JsonValue["msg_type"].asUInt();
//
//    if (!JsonValue["reply_ip"].isNull())
//        m_nReplyIp = JsonValue["reply_ip"].asUInt();
//
//    if (!JsonValue["type"].isNull())
//    {
//        strValue = JsonValue["type"].asString();
//        m_strSubType = EncodeUtil::Utf8ToUnicode(strValue);
//    }
//
//    if (m_strSubType == _T("group_request_join_agree")
//        || m_strSubType == _T("group_request_join_deny"))		// 加群同意通知或拒绝加群通知
//    {
//        if (!JsonValue["gcode"].isNull())
//            m_nGroupCode = JsonValue["gcode"].asUInt();
//
//        if (!JsonValue["t_gcode"].isNull())
//            m_nGroupNumber = JsonValue["t_gcode"].asUInt();
//
//        if (!JsonValue["admin_uin"].isNull())
//            m_nAdminUin = JsonValue["admin_uin"].asUInt();
//
//        if (!JsonValue["msg"].isNull())
//        {
//            strValue = JsonValue["msg"].asString();
//            m_strMsg = EncodeUtil::Utf8ToUnicode(strValue);
//        }
//        return TRUE;
//    }
//    else if (m_strSubType == _T("group_leave"))	// 被移出群通知
//    {
//        if (!JsonValue["gcode"].isNull())
//            m_nGroupCode = JsonValue["gcode"].asUInt();
//
//        if (!JsonValue["t_gcode"].isNull())
//            m_nGroupNumber = JsonValue["t_gcode"].asUInt();
//
//        if (!JsonValue["op_type"].isNull())
//            m_nOpType = JsonValue["op_type"].asUInt();
//
//        if (!JsonValue["old_member"].isNull())
//            m_nOldMember = JsonValue["old_member"].asUInt();
//
//        if (!JsonValue["t_old_member"].isNull())
//        {
//            strValue = JsonValue["t_old_member"].asString();
//            m_strOldMember = EncodeUtil::Utf8ToUnicode(strValue);
//        }
//
//        if (!JsonValue["admin_uin"].isNull())
//            m_nAdminUin = JsonValue["admin_uin"].asUInt();
//
//        if (!JsonValue["t_admin_uin"].isNull())
//        {
//            strValue = JsonValue["t_admin_uin"].asString();
//            m_strAdminUin = EncodeUtil::Utf8ToUnicode(strValue);
//        }
//
//        if (!JsonValue["admin_nick"].isNull())
//        {
//            strValue = JsonValue["admin_nick"].asString();
//            m_strAdminNickName = EncodeUtil::Utf8ToUnicode(strValue);
//        }
//        return TRUE;
//    }
//    else
//    {
//        return FALSE;
//    }
//}

/**
 * @brief Construct a new C_UI_UpdateFriendInfoMsg::C_UI_UpdateFriendInfoMsg object
 * 
 */
C_UI_UpdateFriendInfoMsg::C_UI_UpdateFriendInfoMsg()
{
    Reset();
}


/**
 * @brief Destroy the C_UI_UpdateFriendInfoMsg::C_UI_UpdateFriendInfoMsg object
 * 
 */
C_UI_UpdateFriendInfoMsg::~C_UI_UpdateFriendInfoMsg()
{

}

/**
 * @brief 
 * 
 * @param lpType 
 * @return BOOL 
 */
BOOL C_UI_UpdateFriendInfoMsg::IsType(LPCTSTR lpType)
{
    return (_tcsicmp(lpType, _T("update_friend_info_msg")) == 0);
}

/**
 * @brief 
 * 
 */
void C_UI_UpdateFriendInfoMsg::Reset()
{
    m_uMsgType = 0;
    m_uMsgId = 0;
    m_uMsgId2 = 0;
    m_uFromUin = 0;
    m_uToUin = 0;
    m_uClientType = E_UI_CLIENT_TYPE::CLIENT_TYPE_PC;
    m_uTime = 0;
}

//BOOL C_UI_UpdateFriendInfoMsg::Parse(Json::Value& JsonValue)
//{
//    Reset();
//
//    if (!JsonValue["msgType"].isNull())
//        m_uMsgType = JsonValue["msgType"].asUInt();
//
//    if (!JsonValue["clientType"].isNull())
//        m_uClientType = JsonValue["clientType"].asUInt();
//
//    if (!JsonValue["msgID"].isNull())
//        m_uMsgId = JsonValue["msgID"].asUInt();
//
//    m_uMsgId2 = m_uMsgId;
//
//    if (!JsonValue["from"].isNull())
//        m_uFromUin = JsonValue["from"].asUInt();
//
//    if (!JsonValue["to"].isNull())
//        m_uToUin = JsonValue["to"].asUInt();
//
//    if (!JsonValue["time"].isNull())
//        m_uTime = JsonValue["time"].asUInt();
//
//    return TRUE;
//}


/**
 * @brief Construct a new C_UI_MessageSender::C_UI_MessageSender object
 * 
 * @param nType 
 * @param nSenderId 
 * @param nGroupCode 
 */
C_UI_MessageSender::C_UI_MessageSender(E_UI_CHAT_MSG_TYPE nType, UINT nSenderId, UINT nGroupCode)
{
    m_nType = nType;
    m_nSenderId = nSenderId;
    m_nGroupCode = nGroupCode;
}

/**
 * @brief Destroy the C_UI_MessageSender::C_UI_MessageSender object
 * 
 */
C_UI_MessageSender::~C_UI_MessageSender(void)
{
    DelAllMsg();
}


/**
 * @brief 
 * 
 * @param lpMsg 
 * @return BOOL 
 */
BOOL C_UI_MessageSender::AddMsg(void* lpMsg)
{
    if (NULL == lpMsg)
    {
        return FALSE;
    } 
    m_arrMsg.push_back(lpMsg);
    return TRUE;
}


/**
 * @brief 删除消息(根据索引)
 * 
 * @param nIndex 
 * @return BOOL 
 */
BOOL C_UI_MessageSender::DelMsg(int nIndex)
{
    if (nIndex >= 0 && nIndex < (int)m_arrMsg.size())
    {
        m_arrMsg.erase(m_arrMsg.begin() + nIndex);
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

// 删除消息(根据消息Id)
BOOL C_UI_MessageSender::DelMsgById(UINT nMsgId)
{
    for (int i = 0; i < (int)m_arrMsg.size(); i++)
    {
        switch (m_nType)
        {
        case E_UI_CHAT_MSG_TYPE::FMG_MSG_TYPE_BUDDY:
        {
            C_UI_BuddyMessage* lpMsg = (C_UI_BuddyMessage*)m_arrMsg[i];
            if (lpMsg != NULL && lpMsg->m_nMsgId == nMsgId)
            {
                delete lpMsg;
                m_arrMsg.erase(m_arrMsg.begin() + i);
                return TRUE;
            }
        }break;
        case E_UI_CHAT_MSG_TYPE::FMG_MSG_TYPE_GROUP:
        {
            //C_UI_GroupMessage* lpMsg = (C_UI_GroupMessage*)m_arrMsg[i];
            C_UI_BuddyMessage* lpMsg = (C_UI_BuddyMessage*)m_arrMsg[i];
            if (lpMsg != NULL && lpMsg->m_nMsgId == nMsgId)
            {
                delete lpMsg;
                m_arrMsg.erase(m_arrMsg.begin() + i);
                return TRUE;
            }
        }break;
        case E_UI_CHAT_MSG_TYPE::FMG_MSG_TYPE_SESS:
        {
            C_UI_SessMessage* lpMsg = (C_UI_SessMessage*)m_arrMsg[i];
            if (lpMsg != NULL && lpMsg->m_nMsgId == nMsgId)
            {
                delete lpMsg;
                m_arrMsg.erase(m_arrMsg.begin() + i);
                return TRUE;
            }
        }break;
        default:{

        }break;
        }
    }
    return FALSE;
}


/**
 * @brief 删除所有消息
 * 
 * @return BOOL 
 */
BOOL C_UI_MessageSender::DelAllMsg()
{
    for (int i = 0; i < (int)m_arrMsg.size(); i++)
    {
        void* lpMsg = m_arrMsg[i];
        if (lpMsg != NULL)
        {
            switch (m_nType)
            {
                case E_UI_CHAT_MSG_TYPE::FMG_MSG_TYPE_BUDDY:
                {
                    delete (C_UI_BuddyMessage*)lpMsg;
                }break;
                case E_UI_CHAT_MSG_TYPE::FMG_MSG_TYPE_GROUP:
                {
                    delete (C_UI_BuddyMessage*)lpMsg;
                }break;
                case E_UI_CHAT_MSG_TYPE::FMG_MSG_TYPE_SESS:
                {
                    delete (C_UI_SessMessage*)lpMsg;
                }break;
                default:
                {

                }break;
            }
        }
    }
    m_arrMsg.clear();
    return TRUE;
}


/**
 * @brief 获取消息类型
 * 
 * @return long 
 */
E_UI_CHAT_MSG_TYPE C_UI_MessageSender::GetMsgType()
{
    return m_nType;
}


/**
 * @brief 获取发送者Id
 * 
 * @return UINT 
 */
UINT C_UI_MessageSender::GetSenderId()
{
    return m_nSenderId;
}


/**
 * @brief 获取消息总数
 * 
 * @return int 
 */
int C_UI_MessageSender::GetDisplayMsgCount()
{
    C_UI_BuddyMessage* pBuddyMessage = NULL;
    long nMsgCount = 0;
    size_t nCount = m_arrMsg.size();
    for (size_t i = 0; i<nCount; ++i)
    {
        pBuddyMessage = (C_UI_BuddyMessage*)m_arrMsg[i];
        if (pBuddyMessage != NULL && pBuddyMessage->m_nMsgType != E_UI_CONTENT_TYPE::CONTENT_TYPE_IMAGE_CONFIRM)
        {
            ++nMsgCount;
        }

    }
    return nMsgCount;
}


/**
 * @brief 获取消息总数
 * 
 * @return int 
 */
int C_UI_MessageSender::GetMsgCount()
{
    return (int)m_arrMsg.size();
}


/**
 * @brief 获取群代码
 * 
 * @return UINT 
 */
UINT C_UI_MessageSender::GetGroupCode()
{
    return m_nGroupCode;
}


/**
 * @brief 获取好友消息(根据索引)
 * 
 * @param nIndex 索引
 * @return C_UI_BuddyMessage* 好友消息
 */
C_UI_BuddyMessage* C_UI_MessageSender::GetBuddyMsg(int nIndex)
{
    if (m_nType != E_UI_CHAT_MSG_TYPE::FMG_MSG_TYPE_BUDDY)
    {
        return NULL;
    }

    if ( (nIndex >= 0) && 
         ( nIndex < static_cast<int>(m_arrMsg.size()) ) )
    {
         return (C_UI_BuddyMessage*)m_arrMsg[nIndex];
    }   
    else
    {
        return NULL;
    }
}


/**
 * @brief 获取群消息(根据索引)
 * 
 * @param nIndex 
 * @return C_UI_BuddyMessage* 
 */
C_UI_BuddyMessage* C_UI_MessageSender::GetGroupMsg(int nIndex)
{
    if (m_nType != E_UI_CHAT_MSG_TYPE::FMG_MSG_TYPE_GROUP)
    {
        return NULL;
    }

    if ( (nIndex >= 0) && 
         (nIndex < (int)m_arrMsg.size()) )
    {
        return (C_UI_BuddyMessage*)m_arrMsg[nIndex];
    }
    else
    {
        return NULL;
    }
}


/**
 * @brief 获取临时会话消息(根据索引)
 * 
 * @param nIndex 
 * @return C_UI_SessMessage* 
 */
C_UI_SessMessage* C_UI_MessageSender::GetSessMsg(int nIndex)
{
    if (m_nType != E_UI_CHAT_MSG_TYPE::FMG_MSG_TYPE_SESS)
    {
        return NULL;
    }
  

    if ( (nIndex >= 0) && 
         (nIndex < (int)m_arrMsg.size()) )
    {
        return (C_UI_SessMessage*)m_arrMsg[nIndex];
    }
    else
    {
        return NULL;
    }
}

/**
 * @brief 获取好友消息(根据消息Id)
 * 
 * @param nMsgId 
 * @return C_UI_BuddyMessage* 
 */
C_UI_BuddyMessage* C_UI_MessageSender::GetBuddyMsgById(UINT nMsgId)
{
    if (m_nType != E_UI_CHAT_MSG_TYPE::FMG_MSG_TYPE_BUDDY)
    {
        return NULL;
    }

    for (int i = 0; i < (int)m_arrMsg.size(); i++)
    {
        C_UI_BuddyMessage* lpMsg = (C_UI_BuddyMessage*)m_arrMsg[i];
        if (lpMsg != NULL && lpMsg->m_nMsgId == nMsgId)
        {
            return lpMsg;
        }
    }

    return NULL;
}


/**
 * @brief 获取群消息(根据消息Id)
 * 
 * @param nMsgId 
 * @return C_UI_BuddyMessage* 
 */
C_UI_BuddyMessage* C_UI_MessageSender::GetGroupMsgById(UINT nMsgId)
{
    if (m_nType != E_UI_CHAT_MSG_TYPE::FMG_MSG_TYPE_GROUP)
    {
        return NULL;
    }

    for (int i = 0; i < (int)m_arrMsg.size(); i++)
    {
        C_UI_BuddyMessage* lpMsg = (C_UI_BuddyMessage*)m_arrMsg[i];
        if (lpMsg != NULL && lpMsg->m_nMsgId == nMsgId)
        {
            return lpMsg;
        }
    }

    return NULL;
}


/**
 * @brief 获取临时会话消息(根据消息Id)
 * 
 * @param nMsgId 
 * @return C_UI_SessMessage* 
 */
C_UI_SessMessage* C_UI_MessageSender::GetSessMsgById(UINT nMsgId)
{
    if (m_nType != E_UI_CHAT_MSG_TYPE::FMG_MSG_TYPE_SESS)
    {
        return NULL;
    }

    for (int i = 0; i < (int)m_arrMsg.size(); i++)
    {
        C_UI_SessMessage* lpMsg = (C_UI_SessMessage*)m_arrMsg[i];
        if (lpMsg != NULL && lpMsg->m_nMsgId == nMsgId)
        {
            return lpMsg;  
        } 
    }

    return NULL;
}


/**
 * @brief 获取群系统消息(根据消息Id)
 * 
 * @param nMsgId 
 * @return C_UI_SysGroupMessage* 
 */
C_UI_SysGroupMessage* C_UI_MessageSender::GetSysGroupMsgById(UINT nMsgId)
{
    //if (m_nType != FMG_MSG_TYPE_SYSGROUP)
    //	return NULL;

    //for (int i = 0; i < (int)m_arrMsg.size(); i++)
    //{
    //	C_UI_SysGroupMessage* lpMsg = (C_UI_SysGroupMessage*)m_arrMsg[i];
    //	if (lpMsg != NULL && lpMsg->m_nMsgId == nMsgId)
    //		return lpMsg;
    //}

    return NULL;
}


/**
 * @brief Construct a new C_UI_MessageList::C_UI_MessageList object
 * 
 */
C_UI_MessageList::C_UI_MessageList(void)
{

}

/**
 * @brief Destroy the C_UI_MessageList::C_UI_MessageList object
 * 
 */
C_UI_MessageList::~C_UI_MessageList(void)
{
    DelAllMsgSender();
}


/**
 * @brief 添加消息
 * 
 * @param nType 
 * @param nSenderId 
 * @param nGroupCode 
 * @param lpMsg 
 * @return BOOL 
 */
BOOL C_UI_MessageList::AddMsg(E_UI_CHAT_MSG_TYPE nType, UINT nSenderId,
    UINT nGroupCode, void* lpMsg)
{
    if (NULL == lpMsg)
    {
        return FALSE;
    }

    C_UI_MessageSender* lpMsgSender = GetMsgSender(nType, nSenderId);
    if (NULL == lpMsgSender)
    {
        lpMsgSender = new C_UI_MessageSender(nType, nSenderId, nGroupCode);
        if (NULL == lpMsgSender)
        {
            return FALSE;
        } 

        m_arrMsgSender.push_back(lpMsgSender);
    }
    AddMsgIdentity(nType, nSenderId);
    return lpMsgSender->AddMsg(lpMsg);
}

// 
/**
 * @brief 删除消息
 * 
 * @param nType 
 * @param nMsgId 
 * @return BOOL 
 */
BOOL C_UI_MessageList::DelMsg(E_UI_CHAT_MSG_TYPE nType, UINT nMsgId)
{
    for (int i = 0; i < (int)m_arrMsgSender.size(); i++)
    {
        C_UI_MessageSender* lpMsgSender = m_arrMsgSender[i];
        if (lpMsgSender != NULL && lpMsgSender->GetMsgType() == nType)
        {
            if (lpMsgSender->DelMsgById(nMsgId))
            {
                return TRUE;
            }
        }
    }
    return FALSE;
}


/**
 * @brief 删除消息发送者(根据索引)
 * 
 * @param nIndex 
 * @return BOOL 
 */
BOOL C_UI_MessageList::DelMsgSender(int nIndex)
{
    if (nIndex >= 0 && nIndex < (int)m_arrMsgSender.size())
    {
        C_UI_MessageSender* lpMsgSender = m_arrMsgSender[nIndex];
        if (lpMsgSender != NULL)
        {
            DelMsgIdentity(lpMsgSender->GetMsgType(), lpMsgSender->GetSenderId());
            delete lpMsgSender;
        }
        m_arrMsgSender.erase(m_arrMsgSender.begin() + nIndex);
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}


/**
 * @brief 删除消息发送者(根据消息类型和发送者Id)
 * 
 * @param nType 
 * @param nSenderId 
 * @return BOOL 
 */
BOOL C_UI_MessageList::DelMsgSender(E_UI_CHAT_MSG_TYPE nType, UINT nSenderId)
{
    for (int i = 0; i < (int)m_arrMsgSender.size(); i++)
    {
        C_UI_MessageSender* lpMsgSender = m_arrMsgSender[i];
        if (lpMsgSender != NULL && lpMsgSender->GetMsgType() == nType
            && lpMsgSender->GetSenderId() == nSenderId)
        {
            DelMsgIdentity(nType, nSenderId);
            delete lpMsgSender;
            m_arrMsgSender.erase(m_arrMsgSender.begin() + i);
            return TRUE;
        }
    }
    return FALSE;
}


/**
 * @brief 删除所有消息发送者
 * 
 * @return BOOL 
 */
BOOL C_UI_MessageList::DelAllMsgSender()
{
    for (int i = 0; i < (int)m_arrMsgSender.size(); i++)
    {
        C_UI_MessageSender* lpMsgSender = m_arrMsgSender[i];
        if (lpMsgSender != NULL)
        {
            DelMsgIdentity(lpMsgSender->GetMsgType(), lpMsgSender->GetSenderId());
            delete lpMsgSender;
        }
    }
    m_arrMsgSender.clear();
    return TRUE;
}


/**
 * @brief 获取消息发送者总数
 * 
 * @return int 
 */
int C_UI_MessageList::GetMsgSenderCount()
{
    return static_cast<int>(m_arrMsgSender.size());
}


/**
 * @brief 获取消息发送者(根据索引)
 * 
 * @param nIndex 
 * @return C_UI_MessageSender* 
 */
C_UI_MessageSender* C_UI_MessageList::GetMsgSender(int nIndex)
{
    if (nIndex >= 0 && nIndex < (int)m_arrMsgSender.size())
    {
        return m_arrMsgSender[nIndex];
    } 
    else
    {
        return NULL;
    }  
}


/**
 * @brief 获取消息发送者(根据消息类型和发送者Id)
 * 
 * @param nType 
 * @param nSenderId 
 * @return C_UI_MessageSender* 
 */
C_UI_MessageSender* C_UI_MessageList::GetMsgSender(E_UI_CHAT_MSG_TYPE nType, UINT nSenderId)
{
    for (int i = 0; i < (int)m_arrMsgSender.size(); i++)
    {
        C_UI_MessageSender* lpMsgSender = m_arrMsgSender[i];
        if (lpMsgSender != NULL && lpMsgSender->GetMsgType() == nType
            && lpMsgSender->GetSenderId() == nSenderId)
        {
            return lpMsgSender;
        }
    }
    return NULL;
}


/**
 * @brief 获取最后一个发送消息过来的发送者
 * 
 * @return C_UI_MessageSender* 
 */
C_UI_MessageSender* C_UI_MessageList::GetLastMsgSender()
{
    int nCount = m_arrMsgIdentity.size();
    if (nCount <= 0)
    {
        return NULL;
    }

    MSG_IDENTITY stMsgId;
    stMsgId = m_arrMsgIdentity[nCount - 1];
    return GetMsgSender(stMsgId.nType, stMsgId.nSenderId);
}


/**
 * @brief 获取好友消息(根据消息Id)
 * 
 * @param nMsgId 
 * @return C_UI_BuddyMessage* 
 */
C_UI_BuddyMessage* C_UI_MessageList::GetBuddyMsg(UINT nMsgId)
{
    for (int i = 0; i < (int)m_arrMsgSender.size(); i++)
    {
        C_UI_MessageSender* lpMsgSender = m_arrMsgSender[i];
        if (  (lpMsgSender != NULL) && 
              (lpMsgSender->GetMsgType() == E_UI_CHAT_MSG_TYPE::FMG_MSG_TYPE_BUDDY) )
        {
            C_UI_BuddyMessage* lpMsg = lpMsgSender->GetBuddyMsgById(nMsgId);
            if (lpMsg != NULL)
            {
                return lpMsg;
            }
        }
    }
    return NULL;
}


/**
 * @brief 获取群消息(根据消息Id)
 * 
 * @param nMsgId 
 * @return C_UI_BuddyMessage* 
 */
C_UI_BuddyMessage* C_UI_MessageList::GetGroupMsg(UINT nMsgId)
{
    for (int i = 0; i < (int)m_arrMsgSender.size(); i++)
    {
        C_UI_MessageSender* lpMsgSender = m_arrMsgSender[i];
        if (  (lpMsgSender != NULL) && 
              (lpMsgSender->GetMsgType() == E_UI_CHAT_MSG_TYPE::FMG_MSG_TYPE_GROUP) )
        {
            C_UI_BuddyMessage* lpMsg = lpMsgSender->GetGroupMsgById(nMsgId);
            if (lpMsg != NULL)
            {
                return lpMsg;
            }
        }
    }
    return NULL;
}


/**
 * @brief 获取临时会话消息(根据消息Id)
 * 
 * @param nMsgId 
 * @return C_UI_SessMessage* 
 */
C_UI_SessMessage* C_UI_MessageList::GetSessMsg(UINT nMsgId)
{
    for (int i = 0; i < (int)m_arrMsgSender.size(); i++)
    {
        C_UI_MessageSender* lpMsgSender = m_arrMsgSender[i];
        if (lpMsgSender != NULL && lpMsgSender->GetMsgType() == E_UI_CHAT_MSG_TYPE::FMG_MSG_TYPE_SESS)
        {
            C_UI_SessMessage* lpMsg = lpMsgSender->GetSessMsgById(nMsgId);
            if (lpMsg != NULL)
            {
                return lpMsg;
            }
        }
    }
    return NULL;
}


/**
 * @brief 获取好友消息(根据发送者Id和消息Id)
 * 
 * @param nUTalkUin 
 * @param nMsgId 
 * @return C_UI_BuddyMessage* 
 */
C_UI_BuddyMessage* C_UI_MessageList::GetBuddyMsg(UINT nUTalkUin, UINT nMsgId)
{
    C_UI_MessageSender* lpMsgSender = GetMsgSender(E_UI_CHAT_MSG_TYPE::FMG_MSG_TYPE_BUDDY, nUTalkUin);
    if (lpMsgSender != NULL)
    {
        return lpMsgSender->GetBuddyMsgById(nMsgId);
    }
    else
    {
        return NULL;
    }
}


/**
 * @brief 获取群消息(根据发送者Id和消息Id)
 * 
 * @param nGroupCode 
 * @param nMsgId 
 * @return C_UI_BuddyMessage* 
 */
C_UI_BuddyMessage* C_UI_MessageList::GetGroupMsg(UINT nGroupCode, UINT nMsgId)
{
    C_UI_MessageSender* lpMsgSender = GetMsgSender(E_UI_CHAT_MSG_TYPE::FMG_MSG_TYPE_GROUP, nGroupCode);
    if (lpMsgSender != NULL)
    {
        return lpMsgSender->GetGroupMsgById(nMsgId);
    } 
    else
    {
        return NULL;
    } 
}


/**
 * @brief 获取临时会话消息(根据发送者Id和消息Id)
 * 
 * @param nUTalkUin 
 * @param nMsgId 
 * @return C_UI_SessMessage* 
 */
C_UI_SessMessage* C_UI_MessageList::GetSessMsg(UINT nUTalkUin, UINT nMsgId)
{
    C_UI_MessageSender* lpMsgSender = GetMsgSender(E_UI_CHAT_MSG_TYPE::FMG_MSG_TYPE_SESS, nUTalkUin);
    if (lpMsgSender != NULL)
    {
        return lpMsgSender->GetSessMsgById(nMsgId);
    }
    else
    {
        return NULL;
    }
}

/**
 * @brief 添加消息标识
 * 
 * @param nType 
 * @param nSenderId 
 */
void C_UI_MessageList::AddMsgIdentity(E_UI_CHAT_MSG_TYPE nType, UINT nSenderId)
{
    DelMsgIdentity(nType, nSenderId);

    MSG_IDENTITY stMsgId;
    stMsgId.nType = nType;
    stMsgId.nSenderId = nSenderId;
    m_arrMsgIdentity.push_back(stMsgId);
}

/**
 * @brief 删除消息标识
 * 
 * @param nType 
 * @param nSenderId 
 */
void C_UI_MessageList::DelMsgIdentity(E_UI_CHAT_MSG_TYPE nType, UINT nSenderId)
{
    for (int i = 0; i < (int)m_arrMsgIdentity.size(); i++)
    {
        if ( (m_arrMsgIdentity[i].nType == nType)  && 
             (m_arrMsgIdentity[i].nSenderId == nSenderId) )
        {
            m_arrMsgIdentity.erase(m_arrMsgIdentity.begin() + i);
            break;
        }
    }
}

