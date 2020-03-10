#include "stdafx.h"
#include "IUProtocolData.h"

//class NetData
CWndMsgBase::CWndMsgBase()
{
	m_uType = E_UI_NET_DATA_TYPE::NET_DATA_UNKNOWN;
	m_nRetryTimes = 0;
}

CWndMsgBase::~CWndMsgBase()
{
}


//class C_WND_MSG_RegisterRequest
C_WND_MSG_RegisterRequest::C_WND_MSG_RegisterRequest()
{
	m_uType = E_UI_NET_DATA_TYPE::NET_DATA_REGISTER;
	m_nResultCode = E_UI_REGISTER_RESULT_CODE::REGISTER_FAILED;
	memset(m_szMobile, 0, sizeof(m_szMobile));
	memset(m_szAccount, 0, sizeof(m_szAccount));
}

C_WND_MSG_RegisterRequest::~C_WND_MSG_RegisterRequest()
{

}



//class C_WND_MSG_LoginResult
C_WND_MSG_LoginResult::C_WND_MSG_LoginResult()
{
	m_uType = E_UI_NET_DATA_TYPE::NET_DATA_LOGIN;
	m_LoginResultCode = E_UI_LOGIN_RESULT_CODE::LOGIN_FAILED;
	m_uAccountID = 0;	
	memset(m_szAccountName, 0, sizeof(m_szAccountName));
	memset(m_szNickName, 0, sizeof(m_szNickName));
	memset(m_szSignature, 0, sizeof(m_szSignature));
	m_nFace = 0;
    memset(m_szCustomFace, 0, sizeof(m_szCustomFace));
    m_nGender = 0;
    m_nBirthday = 19900101;
    memset(m_szAddress, 0, sizeof(m_szAddress));
    memset(m_szPhoneNumber, 0, sizeof(m_szPhoneNumber));
    memset(m_szMail, 0, sizeof(m_szMail));
	m_nStatus = E_UI_ONLINE_STATUS::STATUS_OFFLINE;
}

C_WND_MSG_LoginResult::~C_WND_MSG_LoginResult()
{

}


C_WND_MSG_FindFriendResult::C_WND_MSG_FindFriendResult()
{
	m_uType = E_UI_NET_DATA_TYPE::NET_DATA_FIND_FRIEND;
	m_nResultCode = E_UI_ADD_FRIEND_RESULT::FIND_FRIEND_FAILED;
	memset(m_szAccountId, 0, sizeof(m_szAccountId));
	memset(m_szAccountName, 0, sizeof(m_szAccountName));
	memset(m_szNickName, 0, sizeof(m_szNickName));
}

C_WND_MSG_FindFriendResult::~C_WND_MSG_FindFriendResult()
{

}

//class C_WND_MSG_OperateFriendResult
C_WND_MSG_OperateFriendResult::C_WND_MSG_OperateFriendResult()
{
	m_uType = E_UI_NET_DATA_TYPE::NET_DATA_OPERATE_FRIEND;
	m_uAccountID = 0;
	m_uCmd = E_UI_OPERATE_FRIEND::UnKnown;
	memset(m_szAccountName, 0, sizeof(m_szAccountName));
	memset(m_szNickName, 0, sizeof(m_szNickName));
}

C_WND_MSG_OperateFriendResult::~C_WND_MSG_OperateFriendResult()
{

}

C_WND_MSG_AddFriendNotifyRequest::C_WND_MSG_AddFriendNotifyRequest()
{
	m_uType = E_UI_NET_DATA_TYPE::NET_DATA_ADD_FRIEND_NOTIFY_REQ;
	memset(m_szMsgId, 0, sizeof(m_szMsgId));
	memset(m_szUserName, 0, sizeof(m_szUserName));
	memset(m_szFriendName, 0, sizeof(m_szFriendName));
	memset(m_szFriendNickName, 0, sizeof(m_szFriendNickName));
}

C_WND_MSG_AddFriendNotifyRequest::~C_WND_MSG_AddFriendNotifyRequest()
{

}


//class C_WND_MSG_SentChatConfirmImageMessage
C_WND_MSG_SentChatConfirmImageMessage::C_WND_MSG_SentChatConfirmImageMessage()
{
	m_uType = E_UI_NET_DATA_TYPE::NET_DATA_CHAT_CONFIRM_IMAGE_MSG;
	m_nType = E_UI_CHAT_CONFIRM_TYPE::CHAT_CONFIRM_TYPE_SINGLE;
	m_hwndChat = NULL;
	m_pszConfirmBody = NULL;
	m_uConfirmBodySize = 0;
	m_uSenderID = 0;
}

C_WND_MSG_SentChatConfirmImageMessage::~C_WND_MSG_SentChatConfirmImageMessage()
{

}



//class CCreateGroupResult
C_WND_MSG_CreateNewGroupResult::C_WND_MSG_CreateNewGroupResult()
{
	m_uType = E_UI_NET_DATA_TYPE::NET_DATA_CREATE_NEW_GROUP;
	m_uError = 0;
	memset(m_szGroupName, 0, sizeof(m_szGroupName));
	memset(m_szAccount, 0, sizeof(m_szAccount));
	memset(m_szGroupId, 0, sizeof(m_szGroupId));
}

C_WND_MSG_CreateNewGroupResult::~C_WND_MSG_CreateNewGroupResult()
{
}



//class C_WND_MSG_FileItemRequest
C_WND_MSG_FileItemRequest::C_WND_MSG_FileItemRequest()
{
	m_uType = E_UI_NET_DATA_TYPE::NET_DATA_FILE;

	m_nID = 0;
	memset(m_szUtfFilePath, 0, sizeof(m_szUtfFilePath));
	memset(m_szFilePath, 0, sizeof(m_szFilePath));

	m_hwndReflection = NULL;
	m_hCancelEvent = NULL;

	m_uSenderID = 0;

	m_nFileType = E_UI_FILE_ITEM_TYPE::FILE_ITEM_UNKNOWN;

	m_pBuddyMsg = NULL;

	m_bPending = TRUE;

	m_uAccountID = 0;
}

C_WND_MSG_FileItemRequest::~C_WND_MSG_FileItemRequest()
{
	if(m_hCancelEvent != NULL)
		::CloseHandle(m_hCancelEvent);
}

// class CUploadFileResult
CUploadFileResult::CUploadFileResult()
{
	m_nFileType = E_UI_FILE_ITEM_TYPE::FILE_ITEM_UNKNOWN;
	m_bSuccessful = FALSE;
    m_nFileSize = 0;
	memset(m_szLocalName, 0, sizeof(m_szLocalName));

	memset(m_szMd5, 0, sizeof(m_szMd5));
	memset(m_szRemoteName, 0, sizeof(m_szRemoteName));

	m_uSenderID = 0;

	m_hwndReflection = NULL;
}

CUploadFileResult::~CUploadFileResult()
{

}

void CUploadFileResult::Clone(const CUploadFileResult* pSource)
{
	if(pSource == NULL)
		return;

	m_nFileType = pSource->m_nFileType;
	m_bSuccessful = pSource->m_bSuccessful;
	m_nFileSize = pSource->m_nFileSize;

	_tcscpy_s(m_szLocalName, ARRAYSIZE(m_szLocalName), pSource->m_szLocalName);

	strcpy_s(m_szMd5, ARRAYSIZE(m_szMd5), pSource->m_szMd5);
	strcpy_s(m_szRemoteName, ARRAYSIZE(m_szRemoteName), pSource->m_szRemoteName);

	m_uSenderID = pSource->m_uSenderID;
	m_setTargetIDs = pSource->m_setTargetIDs;

	m_hwndReflection = pSource->m_hwndReflection;
}


C_WND_MSG_FileRecvReq::C_WND_MSG_FileRecvReq()
{
	memset(m_szMsgId, 0, sizeof(m_szMsgId));
	memset(m_szUserId, 0, sizeof(m_szUserId));
	memset(m_szFriendId, 0, sizeof(m_szFriendId));
	memset(m_szFileName, 0, sizeof(m_szFileName));
}

C_WND_MSG_FileRecvReq::~C_WND_MSG_FileRecvReq()
{

}


C_WND_MSG_FileSendRsp::C_WND_MSG_FileSendRsp()
{
	memset(m_szMsgId, 0, sizeof(m_szMsgId));
	memset(m_szUserId, 0, sizeof(m_szUserId));
	memset(m_szFriendId, 0, sizeof(m_szFriendId));
	memset(m_szFileName, 0, sizeof(m_szFileName));
}

C_WND_MSG_FileSendRsp::~C_WND_MSG_FileSendRsp()
{

}



C_WND_MSG_FileNotifyReq::C_WND_MSG_FileNotifyReq()
{
	memset(m_szMsgId, 0, sizeof(m_szMsgId));
	memset(m_szUserId, 0, sizeof(m_szUserId));
	memset(m_szFriendId, 0, sizeof(m_szFriendId));
	memset(m_szFileName, 0, sizeof(m_szFileName));
}

C_WND_MSG_FileNotifyReq::~C_WND_MSG_FileNotifyReq()
{

}

